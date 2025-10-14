
#pragma once

#include <array>
#include <limits>
#include <vector>

// Uncomment the next line to include debug info in the JSON data.
// #define _DEBUG_DB_

/**********
Globals
**********/

const double EPS = 1.0e-12; // used for equality of XYPts, parallelness of XYLines

/** Type for step indices (we won't have a lot of them) */
using step_t = short;

/**
 * Type for indices of the Refs in their container.
 * We limit this to `unsigned int` (max 4,294,967,295),
 * which is very much enough.
 */
using index_t = unsigned int;

using rank_t = unsigned short;
using key_t = unsigned int;

/*****
Utilities
*****/
template <class T>
inline T min_val(T t1, T t2) { return (t1 < t2) ? t1 : t2; };
template <class T>
inline T max_val(T t1, T t2) { return (t1 > t2) ? t1 : t2; };

class Paper;
class BinaryOutputStream;
class XYPt;
class XYLine;

class Shared {
  public:
	// Publicly accessible settings. Users can set these directly before calling
	// BuildAndExportDatabase().
	static Paper sPaper; // dimensions of the paper

	static bool useDatabase;  // Save the database to device
	static bool forceRebuild; // Rebuild the database regardless of the saved one

	/**
	 * The number of bytes to import/export for `index_t` values.
	 * Its value is dynamically decided based on the total number of refs,
	 * in order to fully optimize storage.
	 */
	static int indexBytes;

	static BinaryOutputStream *dbStream;

	static std::array<int, 7> sAxioms;
	static std::array<int, 7> sAxiomWeights;

	static rank_t sMaxRank;		  // maximum rank to create
	static index_t sMaxLines; // maximum number of lines to create
	static index_t sMaxMarks; // maximum number of marks to create

	static bool use_division;
	static key_t sNumX;
	static key_t sNumY;
	static key_t sNumA;
	static key_t sNumD;

	static double sGoodEnoughError;	 // tolerable error in a mark or line
	static double sMinAspectRatio;	 // minimum aspect ratio for a triangular flap
	static double sMinAngleSine;	 // minimum line intersection for defining a mark
	static bool sVisibilityMatters;	 // restrict to what can be made w/ opaque paper
	static bool sLineWorstCaseError; // true = use worst-case error vs Pythagorean
	static int sDatabaseStatusSkip;	 // frequency that sDatabaseFn gets called

	static int sNumTrials; // number of test cases total

	// Check key sizes against type size
	static bool LineKeySizeOK() {
		return sNumA < std::numeric_limits<key_t>::max() / sNumD;
	};
	static bool MarkKeySizeOK() {
		return sNumX < std::numeric_limits<key_t>::max() / sNumY;
	};

	using CheckDatabaseStatusFn = void (*)();

	static CheckDatabaseStatusFn CheckDatabaseStatus;

	// Existing marks and lines
	static std::vector<XYPt> existingMarks;
	static std::vector<XYLine> existingLines;
};
