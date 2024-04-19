/******************************************************************************
File:         ReferenceFinder_console.cpp
Project:      ReferenceFinder 4.x
Purpose:      Implementation for command-line version of ReferenceFinder
Author:       Robert J. Lang
Modified by:  Mu-Tsun Tsai
Created:      2006-04-22
Copyright:    �1999-2006 Robert J. Lang. All Rights Reserved.
******************************************************************************/

#include "ReferenceFinder.h"
#include "RFVersion.h"
#include "utils.h"
#include "class/jsonStreamDgmr.h"
#include "class/paper.h"

#include <sstream>

using namespace std;

// #define CALCINPUT to 1 to use the expression evaluator, which accepts
// #symbolic input. If  CALCINPUT is 0, we'll just use the standard console
// #cin, which wants to see decimal values.
#ifndef CALCINPUT
#define CALCINPUT 0
#endif

#if CALCINPUT
#include "parser.h"
Parser parser;
/*****
Get a number from the user via the expression evaluator
*****/
static bool ReadNumber(double &n, bool assumeDefault = true)
{
	// Read a string from console, parse a numerical expression and save result
	// in parameter. Loop until a valid expression is entered. Abort if user ends
	// input. At present, double ReadNumber (void) would be enough; we could
	// generalize it to boolean ReadNumber (double &) and let user abort without
	// entering a valid expression.
	string buffer; // text read here
	emscripten_utils_getline(buffer);
	Parser::Status st = parser.evaluate(buffer, n, assumeDefault);
	if (!st.isOK())
	{
		cerr << st << endl;
		return false;
	}
	return true;
}
#else
/*****
Get a number from the user via standard input cin.
*****/
static bool ReadNumber(double &n, bool assumeDefault = true)
{
	emscripten_utils_get_double(n);
	return true;
}
#endif // CALCINPUT

/*****
Callback routine to show progress by reporting information to the console.
*****/
void ConsoleDatabaseProgress(ReferenceFinder::DatabaseInfo info, void *, bool &)
{
	switch (info.mStatus)
	{
	case ReferenceFinder::DATABASE_INITIALIZING:
		// Called at beginning of initialization
		cout << "Initializing using";
		if (ReferenceFinder::sUseRefLine_C2P_C2P)
			cout << " O1,";
		if (ReferenceFinder::sUseRefLine_P2P)
			cout << " O2,";
		if (ReferenceFinder::sUseRefLine_L2L)
			cout << " O3,";
		if (ReferenceFinder::sUseRefLine_L2L_C2P)
			cout << " O4,";
		if (ReferenceFinder::sUseRefLine_P2L_C2P)
			cout << " O5,";
		if (ReferenceFinder::sUseRefLine_P2L_P2L)
			cout << " O6,";
		if (ReferenceFinder::sUseRefLine_L2L_P2L)
			cout << " O7,";
		cout << " vis=";
		if (ReferenceFinder::sVisibilityMatters)
			cout << "true";
		else
			cout << "false";
		cout << " wce=";
		if (ReferenceFinder::sLineWorstCaseError)
			cout << "true";
		else
			cout << "false";
		cout << endl;
		break;

	// Initialization progress report
	case ReferenceFinder::DATABASE_WORKING:
	case ReferenceFinder::DATABASE_RANK_COMPLETE:
		cout << "{\"rank\": " << info.mRank
			 << ", \"lines\": " << info.mNumLines
			 << ", \"marks\": " << info.mNumMarks
			 << "}" << endl;
		// cout
		// 	<< "There are " << info.mNumLines << " lines and " << info.mNumMarks << " marks of rank <= " << info.mRank << " " << endl;
		break;

	default:
		break;
	}
}

/*****
Callback routine for statistics function
*****/
void ConsoleStatisticsProgress(ReferenceFinder::StatisticsInfo info, void *, bool &)
{
	switch (info.mStatus)
	{
	case ReferenceFinder::STATISTICS_BEGIN:
	{
		// Initialize reporting of each trial
		cout.precision(4);
		cout.setf(ios_base::fixed, ios_base::floatfield);
		cout << "(test #) error" << endl;
		break;
	}
	case ReferenceFinder::STATISTICS_WORKING:
	{
		// Print the trials, 5 to a line
		cout << "(" << info.mIndex + 1 << "/" << ReferenceFinder::sNumTrials << ") " << info.mError << ", ";
		if (fmod(double(info.mIndex) + 1, 5) == 0)
			cout << endl;
		break;
	}
	case ReferenceFinder::STATISTICS_DONE:
	{
		// Report the results
		cout << endl;
		cout << ReferenceFinder::sStatistics << endl;
		cout << endl;
		break;
	}
	}
}

void readSettings()
{
	double width, height, rank, lines, marks, o[7], num[7];
	ReadNumber(width);
	ReadNumber(height);
	ReferenceFinder::sPaper = Paper(width, height);
	ReadNumber(rank);
	ReadNumber(lines);
	ReadNumber(marks);
	for (int i = 0; i < 7; i++)
		ReadNumber(o[i]);
	ReferenceFinder::sMaxRank = rank;
	ReferenceFinder::sMaxLines = lines;
	ReferenceFinder::sMaxMarks = marks;
	ReferenceFinder::sUseRefLine_C2P_C2P = o[0];
	ReferenceFinder::sUseRefLine_P2P = o[1];
	ReferenceFinder::sUseRefLine_L2L = o[2];
	ReferenceFinder::sUseRefLine_L2L_C2P = o[3];
	ReferenceFinder::sUseRefLine_P2L_C2P = o[4];
	ReferenceFinder::sUseRefLine_P2L_P2L = o[5];
	ReferenceFinder::sUseRefLine_L2L_P2L = o[6];
	for (int i = 0; i < 7; i++)
		ReadNumber(num[i]);
	ReferenceFinder::sNumX = num[0];
	ReferenceFinder::sNumY = num[1];
	ReferenceFinder::sNumA = num[2];
	ReferenceFinder::sNumD = num[3];
	ReferenceFinder::sGoodEnoughError = num[4];
	ReferenceFinder::sMinAspectRatio = num[5];
	ReferenceFinder::sMinAngleSine = num[6];
}

/******************************
Main program loop
******************************/
int main()
{
	cout << APP_V_M_B_NAME_STR << " (build " << BUILD_CODE_STR << ")" << endl;
	cout << "Copyright (c)1999-2006 by Robert J. Lang and (c)2024 by Mu-Tsun Tsai. All rights reserved." << endl;

	readSettings();

	JsonStreamDgmr jsonDgmr(cout);
	ReferenceFinder::SetDatabaseFn(&ConsoleDatabaseProgress);
	ReferenceFinder::SetStatisticsFn(&ConsoleStatisticsProgress);
	ReferenceFinder::MakeAllMarksAndLines();

	//  Loop forever until the user quits from the menu.
	while (true)
	{
		emscripten_utils_clear();

		// cout << "0 = exit, 1 = find mark, 2 = find line : " << endl;
		cout << "Ready" << endl;
		double ns;
		if (!ReadNumber(ns, false))
			continue;

		switch (int(ns))
		{
		case 0:
		{
			exit(1);
			break;
		}
		case 1:
		{
			XYPt pp(0, 0);
			// cout << "Enter x coordinate: " << endl;
			if (!ReadNumber(pp.x))
				continue;
			// cout << "Enter y coordinate: " << endl;
			if (!ReadNumber(pp.y))
				continue;
			string err;
			if (ReferenceFinder::ValidateMark(pp, err))
			{
				vector<RefMark *> vm;
				ReferenceFinder::FindBestMarks(pp, vm, 5);

				jsonDgmr.PutMarkList(pp, vm);
			}
			else
				cerr << err << endl;
			break;
		}
		case 2:
		{
			XYPt p1, p2;
			// cout << "Enter p1 x coordinate: " << endl;
			if (!ReadNumber(p1.x))
				continue;
			// cout << "Enter p1 y coordinate: " << endl;
			if (!ReadNumber(p1.y))
				continue;
			// cout << "Enter p2 x coordinate: " << endl;
			if (!ReadNumber(p2.x))
				continue;
			// cout << "Enter p2 y coordinate: " << endl;
			if (!ReadNumber(p2.y))
				continue;
			string err;
			if (ReferenceFinder::ValidateLine(p1, p2, err))
			{
				XYLine ll(p1, p2);
				vector<RefLine *> vl;
				ReferenceFinder::FindBestLines(ll, vl, 5);

				jsonDgmr.PutLineList(ll, vl);
			}
			else
				cerr << err << endl;
			break;
		}
		case 99:
		{
			// hidden command to calculate statistics on marks & report results
			ReferenceFinder::CalcStatistics();
			break;
		}
		default:
			cerr << "Enter just 0, 1 or 2, please.\n\n";
		}
	};
	return 0;
}
