
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <limits>

// Uncomment the next line to include debug info in the JSON data.
// #define _DEBUG_DB_

/**********
Globals
**********/

const double EPS = 1.0e-8; // used for equality of XYPts, parallelness of XYLines

using rank_t = unsigned short;
using key_t = int;

/*****
Utilities
*****/
template <class T>
inline T min_val(T t1, T t2) { return (t1 < t2) ? t1 : t2; };
template <class T>
inline T max_val(T t1, T t2) { return (t1 > t2) ? t1 : t2; };

class Paper;
class BinaryOutputStream;

class Shared {
  public:
	// Publicly accessible settings. Users can set these directly before calling
	// BuildAndExportDatabase().
	static Paper sPaper; // dimensions of the paper

	static bool useDatabase;
	static bool forceRebuild;
	static int sizeBytes;
	static BinaryOutputStream *dbStream;

	static int sAxioms[7];
	static int sAxiomWeights[7];

	static rank_t sMaxRank;		  // maximum rank to create
	static std::size_t sMaxLines; // maximum number of lines to create
	static std::size_t sMaxMarks; // maximum number of marks to create

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
};

#endif
