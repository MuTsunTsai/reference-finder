/******************************************************************************
File:         ReferenceFinder_console.cpp
Project:      ReferenceFinder 4.x
Purpose:      Implementation for command-line version of ReferenceFinder
Author:       Robert J. Lang
Modified by:  Mu-Tsun Tsai
Created:      2006-04-22
Copyright:    ©1999-2006 Robert J. Lang. All Rights Reserved.
******************************************************************************/

#include "RFVersion.h"
#include "ReferenceFinder.h"
#include "class/jsonStreamDgmr.h"
#include "class/paper.h"
#include "utils.h"

#include <sstream>

using namespace std;

static int intervalReport;
static int intervalCheckStop;
static long startTime;
static JsonArray progress;

/*****
Get a number from the user via standard input cin.
*****/
static double ReadNumber() {
	return emscripten_utils_get_double();
}

/*****
Callback routine to show progress by reporting information to the console.
*****/
void ConsoleDatabaseProgress(ReferenceFinder::DatabaseInfo info, void *, bool &) {
	switch (info.mStatus) {
	case ReferenceFinder::DATABASE_INITIALIZING:
		// Called at beginning of initialization
		cout << "Initializing using";
		if (ReferenceFinder::sUseRefLine_C2P_C2P) cout << " O1,";
		if (ReferenceFinder::sUseRefLine_P2P) cout << " O2,";
		if (ReferenceFinder::sUseRefLine_L2L) cout << " O3,";
		if (ReferenceFinder::sUseRefLine_L2L_C2P) cout << " O4,";
		if (ReferenceFinder::sUseRefLine_P2L_C2P) cout << " O5,";
		if (ReferenceFinder::sUseRefLine_P2L_P2L) cout << " O6,";
		if (ReferenceFinder::sUseRefLine_L2L_P2L) cout << " O7,";
		cout << " vis=";
		if (ReferenceFinder::sVisibilityMatters) cout << "true";
		else cout << "false";
		cout << " wce=";
		if (ReferenceFinder::sLineWorstCaseError) cout << "true";
		else cout << "false";
		cout << endl;
		break;

	// Initialization progress report
	case ReferenceFinder::DATABASE_WORKING:
	case ReferenceFinder::DATABASE_RANK_COMPLETE:
		cout << "{\"rank\": " << info.mRank << ", \"lines\": " << info.mNumLines
			 << ", \"marks\": " << info.mNumMarks << "}" << endl;
		break;

	default:
		break;
	}
}

/*****
Callback routine for statistics function
*****/
void ConsoleStatisticsProgress(ReferenceFinder::StatisticsInfo info, void *, bool &cancel) {
	switch (info.mStatus) {
	case ReferenceFinder::STATISTICS_BEGIN: {
		// record start time
		startTime = emscripten_utils_get_now();
		break;
	}
	case ReferenceFinder::STATISTICS_WORKING: {
		progress.add(info.mError);

		// Use the execution time of the first trial to estimate intervals
		int index = (int)info.mIndex + 1;
		if (index == 1) {
			long lap = emscripten_utils_get_now() - startTime;
			intervalReport = 250 / lap;
			intervalCheckStop = 1000 / lap;

			// Just in case one lap is VERY slow
			if (intervalReport == 0) intervalReport = 1;
			if (intervalCheckStop == 0) intervalCheckStop = 1;
		}

		// Print progress
		if (index % intervalReport == 0) {
			JsonObject json;
			json.add("total", ReferenceFinder::sNumTrials);
			json.add("progress", progress);
			progress.clear();
			cout << json << endl;
		}

		// Check if the user requested stopping
		if (index % intervalCheckStop == 0) {
			cancel = emscripten_utils_check_cancel();
		}
		break;
	}
	case ReferenceFinder::STATISTICS_DONE: {
		// Print last progress
		JsonObject json;
		json.add("total", ReferenceFinder::sNumTrials);
		json.add("progress", progress);
		json.add("done", "yes");
		progress.clear();
		cout << json << endl;
		break;
	}
	}
}

/******************************
Read settings related to database generating
******************************/
void readDbSettings() {
	double width = ReadNumber();
	double height = ReadNumber();
	ReferenceFinder::sPaper = Paper(width, height);
	ReferenceFinder::sMaxRank = ReadNumber();
	ReferenceFinder::sMaxLines = ReadNumber();
	ReferenceFinder::sMaxMarks = ReadNumber();
	ReferenceFinder::sUseRefLine_C2P_C2P = ReadNumber();
	ReferenceFinder::sUseRefLine_P2P = ReadNumber();
	ReferenceFinder::sUseRefLine_L2L = ReadNumber();
	ReferenceFinder::sUseRefLine_L2L_C2P = ReadNumber();
	ReferenceFinder::sUseRefLine_P2L_C2P = ReadNumber();
	ReferenceFinder::sUseRefLine_P2L_P2L = ReadNumber();
	ReferenceFinder::sUseRefLine_L2L_P2L = ReadNumber();
	ReferenceFinder::sNumX = ReadNumber();
	ReferenceFinder::sNumY = ReadNumber();
	ReferenceFinder::sNumA = ReadNumber();
	ReferenceFinder::sNumD = ReadNumber();
	ReferenceFinder::sMinAspectRatio = ReadNumber();
	ReferenceFinder::sMinAngleSine = ReadNumber();
	ReferenceFinder::sVisibilityMatters = ReadNumber();
}

/******************************
Read settings related to search only
******************************/
int readSearchSettings() {
	ReferenceFinder::sGoodEnoughError = ReadNumber();
	int count = ReadNumber();
	ReferenceFinder::sLineWorstCaseError = ReadNumber();
	return count;
}

/******************************
Main program loop
******************************/
int main() {
	cout << APP_V_M_B_NAME_STR << " (build " << BUILD_CODE_STR << ")" << endl;
	cout << "Copyright (c)1999-2006 by Robert J. Lang and (c)2024 by Mu-Tsun "
			"Tsai. All rights reserved."
		 << endl;

	readDbSettings();

	JsonStreamDgmr jsonDgmr(cout);
	ReferenceFinder::SetDatabaseFn(&ConsoleDatabaseProgress);
	ReferenceFinder::SetStatisticsFn(&ConsoleStatisticsProgress);
	ReferenceFinder::MakeAllMarksAndLines();

	//  Loop forever until the user quits from the menu.
	while (true) {
		emscripten_utils_clear();

		cout << "Ready" << endl;
		int ns = ReadNumber();

		switch (ns) {
		case 0: {
			exit(1);
			break;
		}
		case 1: {
			int count = readSearchSettings();
			XYPt pp;
			pp.x = ReadNumber();
			pp.y = ReadNumber();
			string err;
			if (ReferenceFinder::ValidateMark(pp, err)) {
				vector<RefMark *> marks;
				ReferenceFinder::FindBestMarks(pp, marks, count);

				jsonDgmr.PutMarkList(pp, marks);
			} else {
				cerr << err << endl;
			}
			break;
		}
		case 2: {
			int count = readSearchSettings();
			XYPt p1, p2;
			p1.x = ReadNumber();
			p1.y = ReadNumber();
			p2.x = ReadNumber();
			p2.y = ReadNumber();
			string err;
			if (ReferenceFinder::ValidateLine(p1, p2, err)) {
				XYLine ll(p1, p2);
				vector<RefLine *> lines;
				ReferenceFinder::FindBestLines(ll, lines, count);

				jsonDgmr.PutLineList(ll, lines);
			} else {
				cerr << err << endl;
			}
			break;
		}
		case 99: {
			ReferenceFinder::sNumTrials = ReadNumber();
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
