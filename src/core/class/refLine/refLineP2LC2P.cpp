
#include "../../ReferenceFinder.h"
#include "../math/paper.h"
#include "../refDgmr.h"

#include "refLineP2LC2P.h"

using namespace std;

/**********
class RefLine_P2L_C2P - Huzita-Hatori Axiom O5.
Bring point p1 to line l1 so that the crease passes through point p2.
**********/

/*****
Constructor. iroot can be 0 or 1.
*****/
RefLine_P2L_C2P::RefLine_P2L_C2P(RefMark *arm1, RefLine *arl1, RefMark *arm2, short iroot) : RefLine(CalcLineRank(arm1, arl1, arm2)), rm1(arm1), rl1(arl1), rm2(arm2) {

	mScore = rm1->mScore + rl1->mScore + rm2->mScore + Shared::sAxiomWeights[4];

	// Get references to the points and lines.
	XYPt &p1 = rm1->p;
	XYLine &l1 = rl1->l;
	XYPt &u1 = l1.u;
	double &d1 = l1.d;
	XYPt &p2 = rm2->p;

	// If either point is already on the line, then this isn't interesting, i.e., it's
	// a trivial Haga construction.
	if (l1.Intersects(p1) || l1.Intersects(p2)) return;

	// Construct the line.
	double a = d1 - p2.Dot(u1);
	double b2 = (p2 - p1).Mag2() - a * a;

	if (b2 < 0) return; // no solution for negative b2 (implies imaginary b)

	double b = sqrt(b2);
	if ((b < EPS) && (iroot == 1)) return; // degenerate case, there's only one solution

	// Construct the image of p1 (p1p), which depends on which root we're after.
	XYPt u1p = u1.Rotate90();
	XYPt p1p = p2 + a * u1;
	if (iroot == 0)
		p1p += b * u1p;
	else
		p1p -= b * u1p;

	// Validate; the point of incidence must lie within the square.
	if (!Shared::sPaper.Encloses(p1p)) return;

	// Construct member data.
	l.u = (p1p - p1).Normalize();
	l.d = p2.Dot(l.u);

	// Check visibility.
	bool p1edge = arm1->IsOnEdge();
	bool l1edge = arl1->IsOnEdge();

	if (Shared::sVisibilityMatters) {
		if (p1edge)
			mWhoMoves = WHOMOVES_P1;
		else if (l1edge)
			mWhoMoves = WHOMOVES_L1;
		else
			return;
	} else {
		mWhoMoves = WHOMOVES_P1;
	};

	// If this line creates a skinny flap, we won't use it.
	if (Shared::sPaper.MakesSkinnyFlap(l)) return;

	// Set the key.
	FinishConstructor();
}

/*****
Return true if this line uses rb for immediate reference.
*****/
bool RefLine_P2L_C2P::UsesImmediate(RefBase *rb) const {
	return (rb == rl1 || rb == rm1 || rb == rm2);
}

/*****
Build the folding sequence that constructs this object.
*****/
void RefLine_P2L_C2P::SequencePushSelf() {
	rm2->SequencePushSelf();
	switch (mWhoMoves) {
	case WHOMOVES_P1:
		rl1->SequencePushSelf();
		rm1->SequencePushSelf();
		break;

	case WHOMOVES_L1:
		rm1->SequencePushSelf();
		rl1->SequencePushSelf();

		break;
	};
	RefBase::SequencePushSelf();
}

/*****
Export the construction of this line.
*****/
JsonObject RefLine_P2L_C2P::Serialize() const {
	JsonObject step;
	step.add("axiom", 5);
	switch (mWhoMoves) {
	case WHOMOVES_P1:
		rm1->PutName("p0", step);
		rl1->PutName("l0", step);
		break;

	case WHOMOVES_L1:
		rl1->PutName("p0", step);
		rm1->PutName("l0", step);
		break;
	};
	rm2->PutName("p1", step);
	PutName("x", step);

	if (mForMark != NULL) step.add("pinch", 1);
#ifdef _DEBUG_DB_
	PutDebug(step);
#endif
	return step;
}

/*****
Draw this line, adding arrows if appropriate
*****/
void RefLine_P2L_C2P::DrawSelf(RefStyle rstyle, short ipass) const {
	// Call inherited method to draw the lines

	RefLine::DrawSelf(rstyle, ipass);

	// If we're moving, we need an arrow

	if ((ipass == PASS_ARROWS) && (rstyle == REFSTYLE_ACTION)) {

		XYPt &p1 = rm1->p;
		XYPt p1f = l.Fold(p1);
		switch (mWhoMoves) {
		case WHOMOVES_P1:
			sDgmr->DrawArrow(p1, p1f);
			break;
		case WHOMOVES_L1:
			sDgmr->DrawArrow(p1f, p1);
			break;
		}
	}
}

/*****
Go through existing lines and marks and create RefLine_P2L_C2Ps with rank equal
arank up to a cumulative total of sMaxLines.
*****/
void RefLine_P2L_C2P::MakeAll(rank_t arank) {
	for (rank_t irank = 0; irank <= (arank - 1); irank++)
		for (rank_t jrank = 0; jrank <= (arank - 1 - irank); jrank++) {
			rank_t krank = arank - irank - jrank - 1;
			for (auto mi : ReferenceFinder::sBasisMarks.ranks[irank]) {
				for (auto lj : ReferenceFinder::sBasisLines.ranks[jrank]) {
					for (auto mk : ReferenceFinder::sBasisMarks.ranks[krank]) {
						if ((irank != krank) || (mi != mk)) { // only cmpr iterators if same container
							if (ReferenceFinder::GetNumLines() >= Shared::sMaxLines) return;
							RefLine_P2L_C2P rlh1(mi, lj, mk, 0);
							ReferenceFinder::sBasisLines.AddCopyIfValidAndUnique(rlh1);
							if (ReferenceFinder::GetNumLines() >= Shared::sMaxLines) return;
							RefLine_P2L_C2P rlh2(mi, lj, mk, 1);
							ReferenceFinder::sBasisLines.AddCopyIfValidAndUnique(rlh2);
						};
					}
				}
			}
		}
}
