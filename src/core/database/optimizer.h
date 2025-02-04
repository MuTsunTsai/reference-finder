
#pragma once

#include "class/refMark/refMark.h"

class Optimizer {
  public:
	bool operator()(RefMark *m1, RefMark *m2) const;

	static void OptimizeMarks();
	static double GetBestError(XYPt &testPt);

  private:
	// Length of each side of each bucket
	static double sSize;
	static int sCols;
	static int sRows;

	static std::vector<int> *sIndices;

	static int keyV(double v);
	static int key(RefMark *m);

	static double GetBestErrorAtBucket(XYPt &testPt, int key);
};
