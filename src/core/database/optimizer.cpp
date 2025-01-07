
#include "optimizer.h"

#include <algorithm>

#include "ReferenceFinder.h"
#include "chebyshev.hpp"
#include "global/global.h"
#include "math/paper.h"

using namespace std;

double Optimizer::sSize = 0;
int Optimizer::sRows = 0;
int Optimizer::sCols = 0;
vector<int> *Optimizer::sIndices = nullptr;

int Optimizer::keyV(double v) {
	return static_cast<int>(floor(v / sSize));
}

int Optimizer::key(RefMark *m) {
	return keyV(m->p.x) + keyV(m->p.y) * sCols;
};

bool Optimizer::operator()(RefMark *m1, RefMark *m2) const {
	return m1->optKey < m2->optKey;
};

void Optimizer::OptimizeMarks() {
	if(sIndices) return;

	auto &paper = Shared::sPaper;
	auto &sBasisMarks = ReferenceFinder::sBasisMarks;
	size_t total = sBasisMarks.size();

	// Make each bucket holding about 100 marks on average
	int bucketCount = static_cast<int>(ceil((double)total / 100));

	sSize = sqrt((paper.mWidth * paper.mHeight) / bucketCount);
	sCols = static_cast<int>(floor(paper.mWidth / sSize)) + 1;
	sRows = static_cast<int>(floor(paper.mHeight / sSize)) + 1;

	// Optimize sBasisMarks
	for(auto *iter: sBasisMarks) iter->optKey = Optimizer::key(iter);
	sort(sBasisMarks.begin(), sBasisMarks.end(), Optimizer());

	// Build index
	sIndices = new vector<int>(sCols * sRows + 1);
	int cursor = -1;
	for(int i = 0; i < sBasisMarks.size(); i++) {
		while(sBasisMarks[i]->optKey > cursor) (*sIndices)[++cursor] = i;
	}
	(*sIndices)[sCols * sRows] = total;
}

double Optimizer::GetBestError(XYPt &testPt) {
	int x = keyV(testPt.x);
	int y = keyV(testPt.y);

	// Step 1: Test the exact bucket where testPt is
	int key = x + y * sCols;
	double error = GetBestErrorAtBucket(testPt, key);

	// Step 2: If the bucket is empty, find the closet nonempty bucket
	int r = 1;
	int sdx = 0;
	int sdy = 0;
	while(error < 0) {
		for(auto iter: Chebyshev(r)) {
			int dx = iter.x;
			int dy = iter.y;
			if(x + dx < 0 || x + dx >= sCols) continue;
			if(y + dy < 0 || y + dy >= sRows) continue;
			key = (x + dx) + (y + dy) * sCols;
			double tempError = GetBestErrorAtBucket(testPt, key);
			if(tempError < 0) continue;
			if(error < 0 || tempError < error) {
				sdx = dx;
				sdy = dy;
				error = tempError;
			}
		}
		r++;
	}
	x += sdx;
	y += sdy;

	// Step 3: Compare neighbor buckets if the error is large
	for(r = 1; r <= 2; r++) {
		for(auto iter: Chebyshev(r)) {
			int sx = iter.x + x;
			int sy = iter.y + y;
			if(sx < 0 || sx >= sCols) continue;
			if(sy < 0 || sy >= sRows) continue;

			// Test if the bucket square intersects the error circle
			double dx = max(sx * sSize, min(testPt.x, (sx + 1) * (sSize))) - testPt.x;
			double dy = max(sy * sSize, min(testPt.y, (sy + 1) * (sSize))) - testPt.y;
			if(dx * dx + dy * dy > error * error) continue;

			key = sx + sy * sCols;
			double tempError = GetBestErrorAtBucket(testPt, key);
			if(tempError > 0 && tempError < error) error = tempError;
		}
	}

	return error;
}

double Optimizer::GetBestErrorAtBucket(XYPt &testPt, int key) {
	int start = (*sIndices)[key];
	int end = (*sIndices)[key + 1];
	if(start == end) return -1; // The bucket is empty

	auto &sBasisMarks = ReferenceFinder::sBasisMarks;
	vector<RefMark *> sortMarks(1); // a vector to do our sorting into

	// Find the mark closest to the test mark.
	partial_sort_copy(
		sBasisMarks.begin() + start, sBasisMarks.begin() + end,
		sortMarks.begin(), sortMarks.end(), CompareError<RefMark>(testPt)
	);

	return (testPt - sortMarks[0]->p).Mag();
}
