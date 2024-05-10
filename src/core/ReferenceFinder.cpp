/******************************************************************************
File:         ReferenceFinder.cpp
Project:      ReferenceFinder 4.x
Purpose:      Implementation for ReferenceFinder generic model
Author:       Robert J. Lang
Modified by:  Mu-Tsun Tsai
Created:      2006-04-22
Copyright:    ©1999-2007 Robert J. Lang. All Rights Reserved.
******************************************************************************/

#include "ReferenceFinder.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

#include "class/xyline.h"
#include "class/xypt.h"
#include "class/xyrect.h"

#include "class/paper.h"
#include "class/refLine/refLineOriginal.h"
#include "class/refMark/refMarkIntersection.h"
#include "class/refMark/refMarkOriginal.h"

#include "class/refLine/refLineC2PC2P.h"
#include "class/refLine/refLineL2L.h"
#include "class/refLine/refLineL2LC2P.h"
#include "class/refLine/refLineL2LP2L.h"
#include "class/refLine/refLineP2LC2P.h"
#include "class/refLine/refLineP2LP2L.h"
#include "class/refLine/refLineP2P.h"

using namespace std;

/**********
class ReferenceFinder - object that builds and maintains collections of marks
and lines and can search through the collection for marks and lines close to a
target mark or line. This class is the primary interface to the ReferenceFinder
database.
**********/

/*****
Accessible static member initialization -- variables that you might want to
change (because they alter the behavior of the program) are given default
values here. Less-accessible static member variables are defined lower down.
*****/

// Use unit square paper
Paper ReferenceFinder::sPaper(1.0, 1.0);

// These are switches by which we can turn on and off the use of different
// types of alignments. Default is to use all possible.
bool ReferenceFinder::sUseRefLine_C2P_C2P = true;
bool ReferenceFinder::sUseRefLine_P2P = true;
bool ReferenceFinder::sUseRefLine_L2L = true;
bool ReferenceFinder::sUseRefLine_L2L_C2P = true;
bool ReferenceFinder::sUseRefLine_P2L_C2P = true;
bool ReferenceFinder::sUseRefLine_P2L_P2L = true;
bool ReferenceFinder::sUseRefLine_L2L_P2L = true;

// Maximum rank and number of marks and lines to collect. These can be tweaked
// up or down to trade off accuracy versus memory and initialization time.
rank_t ReferenceFinder::sMaxRank = 6;
size_t ReferenceFinder::sMaxLines = 500000;
size_t ReferenceFinder::sMaxMarks = 500000;

// constants that quantify the discretization of marks and lines in forming
// keys. The maximum key has the value (sNumX * sNumY) for marks, (sNumA * sNumD)
// for lines. These numbers set a limit on the accuracy, since we won't create
// more than one object for a given key.
int ReferenceFinder::sNumX = 5000;
int ReferenceFinder::sNumY = 5000;
int ReferenceFinder::sNumA = 5000;
int ReferenceFinder::sNumD = 5000;

// Defines "good enough" for a mark. For marks with errors better than this, we
// give priority to lower-rank marks.
double ReferenceFinder::sGoodEnoughError = .005;

// Minimum allowable aspect ratio for a flap. Too skinny of a flap can't be
// folded accurately.
double ReferenceFinder::sMinAspectRatio = 0.100;

// Sine of minimum intersection angle between creases that define a new mark.
// If the lines are close to parallel, the mark is imprecise.
double ReferenceFinder::sMinAngleSine = 0.342; // = sin(20 degrees)

// If sVisibilityMatters == true, we don't construct alignments that can't be
// made with opaque paper. Otherwise, we allow alignments that can be done with
// translucent paper.
bool ReferenceFinder::sVisibilityMatters = true;

// If sLineWorstCaseError == true, we use worst-case error to sort lines rather
// than Pythagorean error. The former is more accurate, but slows searches.
bool ReferenceFinder::sLineWorstCaseError = true;

// We make a call to our show progress callback routine every sDatabaseStatusSkip
// attempts.
int ReferenceFinder::sDatabaseStatusSkip = 400000;

// Variables used when we calculate statistics on the database
int ReferenceFinder::sNumTrials = 1000; // number of test cases total

// Letters that are used for labels for marks and lines.
char RefLine::sLabels[] = "ABCDEFGHIJ";
char RefMark::sLabels[] = "PQRSTUVWXYZ";

/*****
private static member initialization
*****/
RefContainer<RefLine> ReferenceFinder::sBasisLines;
RefContainer<RefMark> ReferenceFinder::sBasisMarks;
ReferenceFinder::DatabaseFn ReferenceFinder::sDatabaseFn = 0;
void *ReferenceFinder::sDatabaseUserData = 0;
ReferenceFinder::StatisticsFn ReferenceFinder::sStatisticsFn = 0;
void *ReferenceFinder::sStatisticsUserData = 0;
int ReferenceFinder::sStatusCount = 0;
rank_t ReferenceFinder::sCurRank = 0;

/*****
Routine called by RefContainer<R> to report progress during the time-consuming
process of initialization. This routine updates our private counter each time
it's called and only occasionally passes on a full call to sDatabaseFn. Clients
can adjust the frequency of calling by changing the variable
sDatabaseStatusSkip. If the client DatabaseFn sets the value of haltFlag to
true, we immediately terminate construction of references.
*****/
void ReferenceFinder::CheckDatabaseStatus() {
	if (sStatusCount < sDatabaseStatusSkip)
		sStatusCount++;
	else {
		bool haltFlag = false;
		if (sDatabaseFn) (*sDatabaseFn)(
			DatabaseInfo(DATABASE_WORKING, sCurRank, GetNumLines(), GetNumMarks()),
			sDatabaseUserData, haltFlag);
		if (haltFlag) throw EXC_HALT();
		sStatusCount = 0;
	}
}

/*****
Create all marks and lines of a given rank.
*****/
void ReferenceFinder::MakeAllMarksAndLinesOfRank(rank_t arank) {
	sCurRank = arank;

	// Construct all types of lines of the given rank. Note that the order in
	// which we call the MakeAll() functions determines which types of RefLine
	// get built, since the first object with a given key to be constructed gets
	// the key slot.

	// We give first preference to lines that don't involve making creases
	// through points, because these are the hardest to do accurately in practice.
	if (sUseRefLine_L2L) RefLine_L2L::MakeAll(arank);
	if (sUseRefLine_P2P) RefLine_P2P::MakeAll(arank);
	if (sUseRefLine_L2L_P2L) RefLine_L2L_P2L::MakeAll(arank);
	if (sUseRefLine_P2L_P2L) RefLine_P2L_P2L::MakeAll(arank);

	// Next, we'll make lines that put a crease through a single point.
	if (sUseRefLine_P2L_C2P) RefLine_P2L_C2P::MakeAll(arank);
	if (sUseRefLine_L2L_C2P) RefLine_L2L_C2P::MakeAll(arank);

	// Finally, we'll do lines that put a crease through both points.
	if (sUseRefLine_C2P_C2P) RefLine_C2P_C2P::MakeAll(arank);

	// Having constructed all lines in the buffer, add them to the main collection.
	sBasisLines.FlushBuffer();

	// construct all types of marks of the given rank
	RefMark_Intersection::MakeAll(arank);
	sBasisMarks.FlushBuffer();

	// if we're reporting status, say how many we constructed.
	bool haltFlag = false;
	if (sDatabaseFn) (*sDatabaseFn)(
		DatabaseInfo(DATABASE_RANK_COMPLETE, arank, GetNumLines(), GetNumMarks()),
		sDatabaseUserData, haltFlag);
	if (haltFlag) throw EXC_HALT();
}

/*****
Create all marks and lines sequentially. you should have previously verified
that LineKeySizeOK() and MarkKeySizeOK() return true.
*****/
void ReferenceFinder::MakeAllMarksAndLines() {
	// Start by clearing out any old marks or lines; this is so we can restart if
	// we want.
	sBasisLines.Rebuild();
	sBasisMarks.Rebuild();

	// Let the user know that we're initializing and what operations we're using.
	bool haltFlag = false;
	if (sDatabaseFn) (*sDatabaseFn)(
		DatabaseInfo(DATABASE_INITIALIZING, 0, GetNumLines(), GetNumMarks()),
		sDatabaseUserData, haltFlag);

	// Build a bunch of marks of successively higher rank. Note that building
	// lines up to rank 4 and marks up to rank 8 with no limits would result in
	// 4185 lines and 1,090,203 marks, which would take about 60 MB of memory.

	// Rank 0: Construct the four edges of the square.
	sBasisLines.Add(new RefLine_Original(sPaper.mBottomEdge, 0, string("s")));
	sBasisLines.Add(new RefLine_Original(sPaper.mLeftEdge, 0, string("w")));
	sBasisLines.Add(new RefLine_Original(sPaper.mRightEdge, 0, string("e")));
	sBasisLines.Add(new RefLine_Original(sPaper.mTopEdge, 0, string("n")));

	// Rank 0: Construct the four corners of the square.
	sBasisMarks.Add(new RefMark_Original(sPaper.mBotLeft, 0, string("sw")));
	sBasisMarks.Add(new RefMark_Original(sPaper.mBotRight, 0, string("se")));
	sBasisMarks.Add(new RefMark_Original(sPaper.mTopLeft, 0, string("nw")));
	sBasisMarks.Add(new RefMark_Original(sPaper.mTopRight, 0, string("ne")));

	// Report our status for rank 0.
	if (sDatabaseFn) (*sDatabaseFn)(
		DatabaseInfo(DATABASE_RANK_COMPLETE, 0, GetNumLines(), GetNumMarks()),
		sDatabaseUserData, haltFlag);

	// Rank 1: Construct the two diagonals.
	sBasisLines.Add(new RefLine_Original(sPaper.mUpwardDiagonal, 1, string("sw_ne")));
	sBasisLines.Add(new RefLine_Original(sPaper.mDownwardDiagonal, 1, string("nw_se")));

	// Flush the buffers.
	sBasisLines.FlushBuffer();
	sBasisMarks.FlushBuffer();

	// Now build the rest, one rank at a time, starting with rank 1. This can
	// be terminated by a EXC_HALT if the user cancelled during the callback.
	try {
		for (rank_t irank = 1; irank <= sMaxRank; irank++) {
			MakeAllMarksAndLinesOfRank(irank);
		}
	} catch (EXC_HALT) {
		sBasisLines.FlushBuffer();
		sBasisMarks.FlushBuffer();
	}

	// Once that's done, all the objects are in the sortable arrays and we can
	// free up the memory used by the maps.
	sBasisLines.ClearMaps();
	sBasisMarks.ClearMaps();

	// And perform a final update of progress.
	if (sDatabaseFn) (*sDatabaseFn)(
		DatabaseInfo(DATABASE_READY, sCurRank, GetNumLines(), GetNumMarks()),
		sDatabaseUserData, haltFlag);
}

/*****
Find the best marks closest to a given point ap, storing the results in the
vector vm.
*****/
void ReferenceFinder::FindBestMarks(const XYPt &ap, vector<RefMark *> &vm, short numMarks) {
	vm.resize(numMarks);
	partial_sort_copy(sBasisMarks.begin(), sBasisMarks.end(), vm.begin(), vm.end(), CompareRankAndError<RefMark>(ap));
}

/*****
Find the best lines closest to a given line al, storing the results in the
vector vl.
*****/
void ReferenceFinder::FindBestLines(const XYLine &al, vector<RefLine *> &vl, short numLines) {
	vl.resize(numLines);
	partial_sort_copy(sBasisLines.begin(), sBasisLines.end(), vl.begin(), vl.end(), CompareRankAndError<RefLine>(al));
}

/*****
Return true if ap is a valid mark. Return an error message if it isn't.
*****/
bool ReferenceFinder::ValidateMark(const XYPt &ap, string &err) {
	if (ap.x < 0 || ap.x > sPaper.mWidth) {
		stringstream ss;
		ss << "x coordinate should lie between 0 and " << sPaper.mWidth;
		err = ss.str();
		return false;
	}

	if (ap.y < 0 || ap.y > sPaper.mHeight) {
		stringstream ss;
		ss << "y coordinate should lie between 0 and " << sPaper.mHeight;
		err = ss.str();
		return false;
	}
	return true;
}

/*****
Validate the two entered points that define the line. Return an error message
if they aren't distinct.
*****/
bool ReferenceFinder::ValidateLine(const XYPt &ap1, const XYPt &ap2, string &err) {
	if ((ap1 - ap2).Mag() > EPS) return true;
	stringstream ss;
	ss.precision(10);
	ss << "The two points must be distinct (separated by at least " << EPS << ").";
	err = ss.str();
	return false;
}

/*****
Compute statistics on the accuracy of the current set of marks for a randomly
chosen set of points and pass the results in our static string variable.
*****/
void ReferenceFinder::CalcStatistics() {
	bool cancel = false;
	if (sStatisticsFn) {
		sStatisticsFn(StatisticsInfo(STATISTICS_BEGIN), sStatisticsUserData, cancel);
	}

	vector<RefMark *> sortMarks(1); // a vector to do our sorting into

	// Run a bunch of test cases on random points.
	int actNumTrials = sNumTrials;
	for (size_t i = 0; i < size_t(sNumTrials); i++) {
		XYPt testPt((double(rand()) / (RAND_MAX * sPaper.mWidth)), double(rand()) / (RAND_MAX * sPaper.mHeight));

		// Find the mark closest to the test mark.
		partial_sort_copy(sBasisMarks.begin(), sBasisMarks.end(), sortMarks.begin(), sortMarks.end(), CompareError<RefMark>(testPt));

		// note how close we were
		double error = (testPt - sortMarks[0]->p).Mag();

		// Report progress, and check for early termination from user
		if (sStatisticsFn) {
			sStatisticsFn(StatisticsInfo(STATISTICS_WORKING, i, error), sStatisticsUserData, cancel);
			if (cancel) {
				actNumTrials = 1 + int(i);
				break;
			}
		}
	}

	// Call the callback for the final time, passing the string containing the
	// results.
	if (sStatisticsFn) {
		sStatisticsFn(StatisticsInfo(STATISTICS_DONE), sStatisticsUserData, cancel);
	}
}
