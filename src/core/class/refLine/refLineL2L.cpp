
#include "../../ReferenceFinder.h"
#include "../paper.h"
#include "../refDgmr.h"

#include "refLineL2L.h"

#include <algorithm>

using namespace std;

/**********
class RefLine_L2L - Huzita-Hatori Axiom O3
Bring line l1 to line l2.
**********/

/*****
Constructor. iroot = 0 or 1.
*****/

RefLine_L2L::RefLine_L2L(RefLine *arl1, RefLine *arl2, short iroot) : RefLine(CalcLineRank(arl1, arl2)), rl1(arl1), rl2(arl2) {
	// Get references to lines
	XYLine &l1 = rl1->l;
	XYPt &u1 = l1.u;
	double &d1 = l1.d;
	XYLine &l2 = rl2->l;
	XYPt &u2 = l2.u;
	double &d2 = l2.d;

	// Parallel lines get handled specially. There's only one solution; we arbitrarily make
	// it the iroot=0 solution.
	if (l1.IsParallelTo(l2)) {
		if (iroot == 0) {
			l.u = u1;
			l.d = .5 * (d1 + d2 * u2.Dot(u1));
		} else
			return; // iroot = 1 for parallel lines isn't a valid solution.
	} else {		// nonparallel lines

		// Construct the direction vector for the bisector, depending on the value of iroot.
		if (iroot == 0)
			l.u = (u1 + u2).Normalize();
		else
			l.u = (u1 - u2).Normalize();

		l.d = Intersection(l1, l2).Dot(l.u);
	};

	// If the paper doesn't overlap the fold line, we're not valid.
	if (!ReferenceFinder::sPaper.InteriorOverlaps(l)) return;

	// Check visibility
	bool l1edge = arl1->IsOnEdge();
	bool l2edge = arl2->IsOnEdge();

	if (ReferenceFinder::sVisibilityMatters) {
		if (l1edge)
			mWhoMoves = WHOMOVES_L1;
		else if (l2edge)
			mWhoMoves = WHOMOVES_L2;
		else {
			XYPt lp1, lp2;
			ReferenceFinder::sPaper.ClipLine(l1, lp1, lp2);
			if (ReferenceFinder::sPaper.Encloses(l.Fold(lp1)) &&
				ReferenceFinder::sPaper.Encloses(l.Fold(lp2)))
				mWhoMoves = WHOMOVES_L1;
			else {
				ReferenceFinder::sPaper.ClipLine(l2, lp1, lp2);
				if (ReferenceFinder::sPaper.Encloses(l.Fold(lp1)) &&
					ReferenceFinder::sPaper.Encloses(l.Fold(lp2)))
					mWhoMoves = WHOMOVES_L2;
				else
					return;
			}
		}
	} else {
		mWhoMoves = WHOMOVES_L1;
	};

	// If this line creates a skinny flap, we won't use it.
	if (ReferenceFinder::sPaper.MakesSkinnyFlap(l)) return;

	// Set the key.
	FinishConstructor();
}

/*****
Return true if this line uses rb for immediate reference.
*****/
bool RefLine_L2L::UsesImmediate(RefBase *rb) const {
	return (rb == rl1 || rb == rl2);
}

/*****
Build the folding sequence that constructs this object.
*****/
void RefLine_L2L::SequencePushSelf() {
	switch (mWhoMoves) {
	case WHOMOVES_L1:
		rl2->SequencePushSelf();
		rl1->SequencePushSelf();
		break;

	case WHOMOVES_L2:
		rl1->SequencePushSelf();
		rl2->SequencePushSelf();
		break;
	};
	RefBase::SequencePushSelf();
}

/*****
Put the construction of this line to a stream.
*****/
bool RefLine_L2L::PutHowto(ostream &os) const {
	os << "{\"axiom\": 3, \"l0\": \"";
	switch (mWhoMoves) {
	case WHOMOVES_L1:
		rl1->PutName(os);
		os << "\", \"l1\": \"";
		rl2->PutName(os);
		break;

	case WHOMOVES_L2:
		rl2->PutName(os);
		os << "\", \"l1\": \"";
		rl1->PutName(os);
		break;
	};
	os << "\", \"x\": \"";
	PutName(os);
	os << "\", \"p0\": \"";

	// Now we need to specify which of the two bisectors this is, which we do
	// by specifying a point where the bisector hits the edge of the square.
	XYPt p;
	rl1->l.Intersects(rl2->l, p); // get the intersection of the two bisectors

	XYPt pa, pb;
	ReferenceFinder::sPaper.ClipLine(l, pa, pb); // find where our fold line hits the paper.

	// Return the first point of intersection between the fold line and the edge of the
	// paper that _isn't_ the intersection of the two bisectors.
	os.precision(2);
	os.setf(ios_base::fixed, ios_base::floatfield);
	if (p == pa) {
		os << pb.Chop();
	} else {
		os << pa.Chop();
	}

	os << "\"}";
	return true;
}

/*****
Draw this line, adding arrows if appropriate
*****/
void RefLine_L2L::DrawSelf(RefStyle rstyle, short ipass) const {
	// Call inherited method to draw the lines
	RefLine::DrawSelf(rstyle, ipass);

	// If we're moving, we need an arrow that brings two points from one line to
	// two points on the other line. We need to pick points that are within the
	// paper for both lines.
	if ((ipass == PASS_ARROWS) && (rstyle == REFSTYLE_ACTION)) {

		XYLine &l1 = rl1->l;
		XYLine &l2 = rl2->l;
		XYPt p1a, p1b;
		ReferenceFinder::sPaper.ClipLine(l1, p1a, p1b); // endpoints of l1
		XYPt p2a, p2b;
		ReferenceFinder::sPaper.ClipLine(l2, p2a, p2b); // endpoints of l2
		p2a = l.Fold(p2a);								// flop l2 points onto l1
		p2b = l.Fold(p2b);
		XYPt du1 = l1.d * l1.u;				   // a point on l1
		XYPt up1 = l1.u.Rotate90();			   // a tangent to l1
		vector<double> tvals;				   // holds parameterizations of the 4 points
		tvals.push_back((p1a - du1).Dot(up1)); // parameterize p1a along l1
		tvals.push_back((p1b - du1).Dot(up1)); // parameterize p1b along l1
		tvals.push_back((p2a - du1).Dot(up1)); // parameterize p2a along l1
		tvals.push_back((p2b - du1).Dot(up1)); // parameterize p2b along l1
		sort(tvals.begin(), tvals.end());	   // sort them in order; we want the middle 2
		XYPt p1c = du1 + 0.5 * (tvals[1] + tvals[2]) * up1;
		XYPt p2c = l.Fold(p1c);
		switch (mWhoMoves) {
		case WHOMOVES_L1:
			sDgmr->DrawFoldAndUnfoldArrow(p1c, p2c);
			break;
		case WHOMOVES_L2:
			sDgmr->DrawFoldAndUnfoldArrow(p1c, p2c);
			break;
		}
	}
}

/*****
Go through existing lines and marks and create RefLine_L2Ls with rank equal to
arank up to a cumulative total of sMaxLines.
*****/
void RefLine_L2L::MakeAll(rank_t arank) {
	for (rank_t irank = 0; irank <= (arank - 1) / 2; irank++) {
		rank_t jrank = arank - irank - 1;
		bool sameRank = (irank == jrank);
		RefContainer<RefLine>::rank_iterator li =
			ReferenceFinder::sBasisLines.maps[irank].begin();
		if (sameRank) li++;
		while (li != ReferenceFinder::sBasisLines.maps[irank].end()) {
			RefContainer<RefLine>::rank_iterator lj =
				ReferenceFinder::sBasisLines.maps[jrank].begin();
			while (lj != (sameRank ? li : ReferenceFinder::sBasisLines.maps[jrank].end())) {
				if (ReferenceFinder::GetNumLines() >= ReferenceFinder::sMaxLines) return;
				RefLine_L2L rls1(li->second, lj->second, 0);
				ReferenceFinder::sBasisLines.AddCopyIfValidAndUnique(rls1);
				if (ReferenceFinder::GetNumLines() >= ReferenceFinder::sMaxLines) return;
				RefLine_L2L rls2(li->second, lj->second, 1);
				ReferenceFinder::sBasisLines.AddCopyIfValidAndUnique(rls2);
				lj++;
			};
			li++;
		}
	}
}
