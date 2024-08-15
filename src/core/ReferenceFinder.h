/******************************************************************************
File:         ReferenceFinder.h
Project:      ReferenceFinder 4.x
Purpose:      Header for ReferenceFinder generic model
Author:       Robert J. Lang
Modified by:  Mu-Tsun Tsai
Created:      2006-04-22
Copyright:    ©1999-2007 Robert J. Lang. All Rights Reserved.
******************************************************************************/

#ifndef _REFERENCEFINDER_H_
#define _REFERENCEFINDER_H_

#include <sstream>
#include <vector>

// If this symbol is defined, adds the mark key as part of the label, which can
// be helpful in debugging.
// #define RF_PUT_KEY_IN_TEXT

#include "class/refContainer.h"
#include "database/binaryOutputStream.hpp"
#include "global/global.h"

// forward declarations
class XYPt;
class XYLine;
class Paper;
class RefMark;
class RefLine;

/**********
class ReferenceFinder - class that builds and maintains collections of marks
and lines and can search through the collection for marks and lines close to a
target mark or line.
**********/
class ReferenceFinder {
  public:
	// Getters
	static std::size_t GetNumLines() {
		return sBasisLines.GetTotalSize();
	};
	static std::size_t GetNumMarks() {
		return sBasisMarks.GetTotalSize();
	};

	// Support for a callback function to show progress during initialization
	enum DatabaseStatus {
		DATABASE_EMPTY,
		DATABASE_INITIALIZING,
		DATABASE_WORKING,
		DATABASE_RANK_COMPLETE,
		DATABASE_READY
	};
	struct DatabaseInfo {
		DatabaseStatus mStatus;
		rank_t mRank;
		std::size_t mNumLines;
		std::size_t mNumMarks;

		DatabaseInfo(DatabaseStatus status = DATABASE_EMPTY, rank_t rank = 0, std::size_t numLines = 0, std::size_t numMarks = 0)
			: mStatus(status), mRank(rank), mNumLines(numLines), mNumMarks(numMarks) {};
		bool operator==(const DatabaseInfo &info) const {
			return mStatus == info.mStatus && mRank == info.mRank &&
				   mNumLines == info.mNumLines && mNumMarks == info.mNumMarks;
		};
		bool operator!=(const DatabaseInfo &info) const {
			return !operator==(info);
		};
	};
	typedef void (*DatabaseFn)(DatabaseInfo info, void *userData, bool &cancel);
	static void SetDatabaseFn(DatabaseFn databaseFn, void *userData = 0) {
		sDatabaseFn = databaseFn;
		sDatabaseUserData = userData;
	};

	// Complete reinitialization of the database
	static void BuildAndExportDatabase();

	// Import database previously exported
	static bool ImportDatabase();

	// Functions for searching for the best marks and/or lines
	static void FindBestMarks(const XYPt &ap, std::vector<RefMark *> &vm, short numMarks);
	static void FindBestLines(const XYLine &al, std::vector<RefLine *> &vl, short numLines);

	// Utility routines for validating user input
	static bool ValidateMark(const XYPt &ap, std::string &err);
	static bool ValidateLine(const XYPt &ap1, const XYPt &ap2, std::string &err);

	// Support for a callback function to show progress during statistics
	enum StatisticsStatus {
		STATISTICS_BEGIN,
		STATISTICS_WORKING,
		STATISTICS_DONE
	};
	struct StatisticsInfo {
		StatisticsStatus mStatus;
		std::size_t mIndex;
		double mError;

		StatisticsInfo(StatisticsStatus status = STATISTICS_DONE, std::size_t index = std::size_t(-1), double error = 0.0)
			: mStatus(status), mIndex(index), mError(error) {};
		bool operator==(const StatisticsInfo &info) const {
			return mStatus == info.mStatus && mIndex == info.mIndex &&
				   mError == info.mError;
		};
		bool operator!=(const StatisticsInfo &info) const {
			return !operator==(info);
		};
	};
	typedef void (*StatisticsFn)(StatisticsInfo info, void *userData, bool &cancel);
	static void SetStatisticsFn(StatisticsFn statisticsFn, void *userData = 0) {
		sStatisticsFn = statisticsFn;
		sStatisticsUserData = userData;
	};

	// Routine for calculating statistics on marks for a random set of trial points
	static void CalcStatistics();

  private:
	static RefContainer<RefLine> sBasisLines; // all lines
	static RefContainer<RefMark> sBasisMarks; // all marks

	class EXC_HALT {};				// exception for user cancellation
	static rank_t sCurRank;			// the rank that we're currently working on
	static DatabaseFn sDatabaseFn;	// the show-status function callback
	static void *sDatabaseUserData; // ptr to user data in callback
	static int sStatusCount;		// number of attempts since last callback
	static StatisticsFn sStatisticsFn;
	static void *sStatisticsUserData;

	static bool ShowProgress(DatabaseStatus status, rank_t rank);
	static void CheckDatabaseStatus(); // called by RefContainer<>
	static void MakeAllMarksAndLinesOfRank(rank_t arank);

	// You should never create an instance of this class
	ReferenceFinder();
	ReferenceFinder(const ReferenceFinder &);

	friend class Optimizer;
	friend class RefBase;
	friend class RefMark;
	friend class RefMark_Intersection;
	friend class RefLine;
	friend class RefLine_C2P_C2P;
	friend class RefLine_P2P;
	friend class RefLine_L2L;
	friend class RefLine_L2L_C2P;
	friend class RefLine_P2L_C2P;
	friend class RefLine_P2L_P2L;
	friend class RefLine_L2L_P2L;

	friend class RefContainer<RefLine>;
	friend class RefContainer<RefMark>;
};

/**********
class CompareError - function object for comparing two refs of class R
according to their distance from a target value of class R::bare_t.
**********/
template <class R>
class CompareError {
  public:
	typename R::bare_t mTarget;
	CompareError(const typename R::bare_t &target) : mTarget(target) {};
	bool operator()(R *r1, R *r2) const {
		// Compare the distances from the stored target If the distances are
		// equal, then compare the refs by their rank.
		double d1 = r1->DistanceTo(mTarget);
		double d2 = r2->DistanceTo(mTarget);
		if (d1 == d2) return r1->GetRank() < r2->GetRank();
		else return d1 < d2;
	};
};

/**********
class CompareRankAndError - function object for comparing two refs of class R
according to their distance from a target value of class R::bare_t, but for
close points, letting rank win out
**********/
template <class R>
class CompareRankAndError {
  public:
	typename R::bare_t mTarget; // point that we're comparing to
	CompareRankAndError(const typename R::bare_t &target) : mTarget(target) {};
	bool operator()(R *r1, R *r2) const {
		// Compare the distances from the stored target. If both distances are less
		// than or equal to sGoodEnoughError, compare the refs by their rank.
		double d1 = r1->DistanceTo(mTarget);
		double d2 = r2->DistanceTo(mTarget);

		bool notBothGood = d1 > Shared::sGoodEnoughError || d2 > Shared::sGoodEnoughError;
		if (notBothGood && d1 != d2) return d1 < d2;

		rank_t rank1 = r1->GetRank();
		rank_t rank2 = r2->GetRank();
		if (notBothGood) return rank1 < rank2; // we must have d1 == d2 here
		else if (rank1 == rank2) return d1 < d2;
		else return rank1 < rank2;
	};
};

#endif
