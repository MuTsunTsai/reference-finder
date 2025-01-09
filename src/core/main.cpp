/******************************************************************************
File:         ReferenceFinder_console.cpp
Project:      ReferenceFinder 4.x
Purpose:      Implementation for command-line version of ReferenceFinder
Author:       Robert J. Lang
Modified by:  Mu-Tsun Tsai, Omri Shavit
Created:      2006-04-22
Copyright:    ©1999-2006 Robert J. Lang. All Rights Reserved.
******************************************************************************/

#include "RFVersion.h"
#include "ReferenceFinder.h"
#include "class/jsonStreamDgmr.h"
#include "math/paper.h"
#include "utils.h"

#include "class/refMark/refMarkIntersection.h"

#include <chrono>
#include <sstream>

using namespace std;
using namespace std::chrono;

static int lapToTime;
static int intervalReport;
static int intervalCheckStop;
static system_clock::time_point startTime;
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
void ConsoleDatabaseProgress(ReferenceFinder::DatabaseInfo info, void *_, bool &__) {
	switch(info.mStatus) {
	case ReferenceFinder::DATABASE_INITIALIZING:
		// Called at beginning of initialization
		cout << "Initializing using";
		for(int &sAxiom: Shared::sAxioms) {
			if(sAxiom > 0) cout << " O" << sAxiom << ",";
		}
		cout << " vis=";
		if(Shared::sVisibilityMatters) cout << "true";
		else cout << "false";
		cout << endl;
		break;

	// Initialization progress report
	case ReferenceFinder::DATABASE_READY:
	case ReferenceFinder::DATABASE_WORKING:
	case ReferenceFinder::DATABASE_RANK_COMPLETE:
		cout << "{\"rank\": " << info.mRank << ", \"lines\": " << info.mNumLines
			 << ", \"marks\": " << info.mNumMarks << "}" << endl;
		break;

	default:
		break;
	}
}

void updateInterval(double laps) {
	std::chrono::duration<double> duration = system_clock::now() - startTime;
	double lap = duration.count() / laps;
	intervalReport = 0.25 / lap;
	intervalCheckStop = 1 / lap;

	// Just in case one lap is VERY slow
	if(intervalReport == 0) intervalReport = 1;
	if(intervalCheckStop == 0) intervalCheckStop = 1;
}

/*****
Callback routine for statistics function
*****/
void ConsoleStatisticsProgress(ReferenceFinder::StatisticsInfo info, void *_, bool &cancel) {
	switch(info.mStatus) {
	case ReferenceFinder::STATISTICS_BEGIN: {
		// record start time
		lapToTime = 1;
		startTime = system_clock::now();
		break;
	}
	case ReferenceFinder::STATISTICS_WORKING: {
		progress.add(info.mError);
		int index = (int)info.mIndex + 1;

		// Use the execution time of the first trial to estimate intervals.
		if(index == lapToTime) {
			updateInterval(index);
			lapToTime *= 10; // Just to be sure, we re-estimate again on lap 10, 100, and so on.
		}

		// Print progress
		if(index % intervalReport == 0) {
			JsonObject json;
			json.add("total", Shared::sNumTrials);
			json.add("progress", progress);
			progress.clear();
			cout << json << endl;
		}

		// Check if the user requested stopping
		if(index % intervalCheckStop == 0) {
			cancel = emscripten_utils_check_cancel();
		}
		break;
	}
	case ReferenceFinder::STATISTICS_DONE: {
		// Print last progress
		JsonObject json;
		json.add("total", Shared::sNumTrials);
		json.add("progress", progress);
		json.add("done", "yes");
		progress.clear();
		cout << json << endl;
		break;
	}
	}
}

/******************************
Reads existing marks and lines from the console,
and returns them via existingMarks and existingLines respectively
******************************/
void readExistingPointsAndLines(vector<XYPt>* existingMarks, vector<XYLine>* existingLines) {
	// Read existing marks
	int existingMarksCount = ReadNumber();
	XYPt p;
	for(int i = 0; i < existingMarksCount; i++) {
		p = XYPt(ReadNumber(), ReadNumber());
		existingMarks->push_back(p);
	}

	// Read existing lines
	int existingLinesCount = ReadNumber();
	XYPt p1;
	XYPt p2;
	XYLine line;
	for(int i = 0; i < existingLinesCount; i++) {
		p1 = XYPt(ReadNumber(), ReadNumber());
		p2 = XYPt(ReadNumber(), ReadNumber());
		line = XYLine(p1, p2);
		existingLines->push_back(line);
	}
}

/******************************
Read settings related to database generating
******************************/
void readDbSettings() {
	Shared::useDatabase = ReadNumber();
	Shared::forceRebuild = ReadNumber();
	double width = ReadNumber();
	double height = ReadNumber();
	Shared::sPaper = Paper(width, height);
	Shared::sMaxRank = ReadNumber();
	Shared::sMaxLines = ReadNumber();
	Shared::sMaxMarks = ReadNumber();

	int b = 1;
	while(b < 4) {
		size_t t = 1 << (8 * b);
		if(t > Shared::sMaxLines && t > Shared::sMaxMarks) break;
		b++;
	}
	Shared::sizeBytes = b;

	for(int i = 0, j = 0; i < 7; i++) {
		int a = ReadNumber();
		Shared::sAxioms[i] = a;
		if(a > 0) Shared::sAxiomWeights[a - 1] = 1 << (j++);
	}

	Shared::use_division = ReadNumber();
	if(Shared::sMaxRank > 5) Shared::use_division = true; // fool-proof

	Shared::sNumX = ReadNumber();
	Shared::sNumY = ReadNumber();
	Shared::sNumA = ReadNumber();
	Shared::sNumD = ReadNumber();
	Shared::sMinAspectRatio = ReadNumber();
	Shared::sMinAngleSine = ReadNumber();
	Shared::sVisibilityMatters = ReadNumber();

	// Read existing marks and lines
	vector<XYPt> existingMarks;
	vector<XYLine> existingLines;
	readExistingPointsAndLines(&existingMarks, &existingLines);
	Shared::existingMarks = existingMarks;
	Shared::existingLines = existingLines;
}

/******************************
Read settings related to search only
******************************/
int readSearchSettings() {
	Shared::sGoodEnoughError = ReadNumber();
	int count = ReadNumber();
	Shared::sLineWorstCaseError = ReadNumber();
	return count;
}

/******************************
Main program loop
******************************/
int main() {
	cout << APP_V_M_B_NAME_STR << " (build " << BUILD_CODE_STR << ")" << endl;
	cout << "Copyright (c)1999-2006 by Robert J. Lang and (c)2024-2025 by Mu-Tsun Tsai (c)2024-2025 by Omri Shavit. All rights reserved." << endl;

	readDbSettings();

	// // print out existing marks and lines that the core recieved (uncomment for debugging)
	// cout << "existing marks: " << endl;
	// for(const auto &mark: Shared::existingMarks) {
	// 	cout << "    " << mark << endl;
	// }
	// cout << "existing lines: " << endl;
	// for(const auto &line: Shared::existingLines) {
	// 	cout << "    " << line << endl;
	// }

	JsonStreamDgmr jsonDgmr(cout);
	ReferenceFinder::SetDatabaseFn(&ConsoleDatabaseProgress);
	ReferenceFinder::SetStatisticsFn(&ConsoleStatisticsProgress);

	Shared::useDatabase = Shared::useDatabase && emscripten_utils_mount_fs();
	if(!ReferenceFinder::ImportDatabase()) ReferenceFinder::BuildAndExportDatabase();
	if(Shared::useDatabase) emscripten_utils_sync_fs();

	while(true) {
		emscripten_utils_clear();

		cout << "Ready" << endl;
		int ns = ReadNumber();

		switch(ns) {
		case 1: { // reference point
			int count = readSearchSettings();
			XYPt pp(ReadNumber(), ReadNumber());
			string err;
			if(ReferenceFinder::ValidateMark(pp, err)) {
				vector<RefMark *> marks;
				ReferenceFinder::FindBestMarks(pp, marks, count);
				jsonDgmr.PutMarkList(pp, marks);
			} else {
				cerr << err << endl;
			}
			break;
		}
		case 2: { // reference line
			int count = readSearchSettings();
			XYPt p1(ReadNumber(), ReadNumber());
			XYPt p2(ReadNumber(), ReadNumber());
			string err;
			if(ReferenceFinder::ValidateLine(p1, p2, err)) {
				XYLine ll(p1, p2);
				vector<RefLine *> lines;
				ReferenceFinder::FindBestLines(ll, lines, count);
				jsonDgmr.PutLineList(ll, lines);
			} else {
				cerr << err << endl;
			}
			break;
		}
		case 99: { // calculate statistics
			Shared::sNumTrials = ReadNumber();
			unsigned int seed = ReadNumber();
			srand(seed);
			ReferenceFinder::CalcStatistics();
			break;
		}
		default:
			cerr << "Unknown command " << ns << endl;
		}
	};

	// Technically the loop never ends until the user closes the web app.
	return 0;
}
