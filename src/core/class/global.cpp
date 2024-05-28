
#include "global.h"
#include "math/paper.h"

/*****
Accessible static member initialization -- variables that you might want to
change (because they alter the behavior of the program) are given default values here.
*****/

Shared::CheckDatabaseStatusFn Shared::CheckDatabaseStatus = 0;

// Use unit square paper
Paper Shared::sPaper(1.0, 1.0);

// The axioms to use, in the order of priority. A value of 0 means skipping.
int Shared::sAxioms[7] = {2, 3, 7, 6, 5, 4, 1};

// The score weight for each axiom
int Shared::sAxiomWeights[7] = {0, 0, 0, 0, 0, 0, 0};

// Maximum rank and number of marks and lines to collect. These can be tweaked
// up or down to trade off accuracy versus memory and initialization time.
rank_t Shared::sMaxRank = 6;
size_t Shared::sMaxLines = 500000;
size_t Shared::sMaxMarks = 500000;

// constants that quantify the discretization of marks and lines in forming
// keys. The maximum key has the value (sNumX * sNumY) for marks, (sNumA * sNumD)
// for lines. These numbers set a limit on the accuracy, since we won't create
// more than one object for a given key.
int Shared::sNumX = 5000;
int Shared::sNumY = 5000;
int Shared::sNumA = 5000;
int Shared::sNumD = 5000;

// Defines "good enough" for a mark. For marks with errors better than this, we
// give priority to lower-rank marks.
double Shared::sGoodEnoughError = .005;

// Minimum allowable aspect ratio for a flap. Too skinny of a flap can't be
// folded accurately.
double Shared::sMinAspectRatio = 0.100;

// Sine of minimum intersection angle between creases that define a new mark.
// If the lines are close to parallel, the mark is imprecise.
double Shared::sMinAngleSine = 0.342; // = sin(20 degrees)

// If sVisibilityMatters == true, we don't construct alignments that can't be
// made with opaque paper. Otherwise, we allow alignments that can be done with
// translucent paper.
bool Shared::sVisibilityMatters = true;

// If sLineWorstCaseError == true, we use worst-case error to sort lines rather
// than Pythagorean error. The former is more accurate, but slows searches.
bool Shared::sLineWorstCaseError = true;

// We make a call to our show progress callback routine every sDatabaseStatusSkip
// attempts.
int Shared::sDatabaseStatusSkip = 800000;

// Variables used when we calculate statistics on the database
int Shared::sNumTrials = 1000; // number of test cases total
