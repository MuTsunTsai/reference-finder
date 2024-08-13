
#include "../refDgmr.h"
#include "ReferenceFinder.h"
#include "math/paper.h"

#include "refLineL2LP2L.h"

using namespace std;

/**********
class RefLine_L2L_P2L - Huzita-Hatori Axiom O7 (Hatori's Axiom).
Bring line l1 onto itself so that point p1 falls on line l2.
**********/

/*****
Constructor. iroot can be 0 or 1.
*****/
RefLine_L2L_P2L::RefLine_L2L_P2L(RefLine *arl1, RefMark *arm1, RefLine *arl2)
	: RefLine(RefType::LINE_L2L_P2L, CalcLineRank(arl1, arm1, arl2)), rl1(arl1), rm1(arm1), rl2(arl2) {

	mScore = rl1->mScore + rm1->mScore + rl2->mScore + Shared::sAxiomWeights[6];

	// Get references
	XYLine &l1 = rl1->l;
	XYPt &u1 = l1.u;
	double &d1 = l1.d;
	XYPt &p1 = rm1->p;
	XYLine &l2 = rl2->l;
	XYPt &u2 = l2.u;

	// Construct direction vector and distance scalar
	l.u = u2.Rotate90();

	double uf1 = l.u.Dot(u1);
	if (abs(uf1) < EPS) return; // parallel lines, no solution

	l.d = (d1 + 2 * p1.Dot(l.u) * uf1 - p1.Dot(u1)) / (2 * uf1);

	// Make sure point of intersection of fold with l2 lies within the paper.
	XYPt pt = Intersection(l, l2);
	if (!Shared::sPaper.Encloses(pt)) return;

	// Make sure point of incidence of p1 on l1 lies within the paper.
	XYPt p1p = l.Fold(p1);
	if (!Shared::sPaper.Encloses(p1p)) return;

	// Make sure p1 isn't already on l1 (in which case the alignment is ill-defined).
	if (l1.Intersects(p1)) return;

	// Check visibility.
	bool p1edge = arm1->IsOnEdge();
	bool l1edge = arl1->IsOnEdge();

	if (Shared::sVisibilityMatters) {
		XYPt lp1, lp2;
		Shared::sPaper.ClipLine(l, lp1, lp2);
		double t1 = (lp1 - pt).Dot(l.u);
		double t2 = (lp2 - pt).Dot(l.u);
		double tp = (p1 - pt).Dot(l.u);
		if ((t1 * tp) < 0) {
			double ti = t2;
			t2 = t1;
			t1 = ti;
		}; // now t1 is the parameter for the endpoint on the p1 side of l2.
		if (p1edge && (abs(t1) <= abs(t2)))
			mWhoMoves = WHOMOVES_P1;
		else if (l1edge && (abs(t1) >= abs(t2)))
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
bool RefLine_L2L_P2L::UsesImmediate(RefBase *rb) const {
	return (rb == rl1 || rb == rm1 || rb == rl2);
}

/*****
Build the folding sequence that constructs this object.
*****/
void RefLine_L2L_P2L::SequencePushSelf() {
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
	rl2->SequencePushSelf();
	RefBase::SequencePushSelf();
}

/*****
Export the construction of this line.
*****/
JsonObject RefLine_L2L_P2L::Serialize() const {
	JsonObject step;
	step.add("axiom", 7);
	rl2->PutName("l1", step);
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
void RefLine_L2L_P2L::DrawSelf(RefStyle rstyle, short ipass) const {
	// Call inherited method to draw the lines
	RefLine::DrawSelf(rstyle, ipass);

	// If we're moving, we need an arrow
	if ((ipass == PASS_ARROWS) && (rstyle == REFSTYLE_ACTION)) {

		// Draw line-to-itself arrow
		XYPt p1, p2;
		XYLine &l2 = rl2->l;
		Shared::sPaper.ClipLine(l2, p1, p2); // get endpts of the reference line
		XYPt pi = Intersection(l, l2);		 // intersection w/ fold line
		XYPt u1p = l2.u.Rotate90();			 // tangent to reference line
		double t1 = abs((p1 - pi).Dot(u1p));
		double t2 = abs((p2 - pi).Dot(u1p));
		double tmin = t1 < t2 ? t1 : t2;
		sDgmr->DrawArrow(pi + tmin * u1p, pi - tmin * u1p);

		// Draw point-to-line arrow
		XYPt &p3 = rm1->p;
		XYPt p3p = l.Fold(p3);
		switch (mWhoMoves) {
		case WHOMOVES_P1:
			sDgmr->DrawArrow(p3, p3p);
			break;
		case WHOMOVES_L1:
			sDgmr->DrawArrow(p3p, p3);
			break;
		}
	}
}

/*****
Go through existing lines and marks and create RefLine_L2L_P2Ls with rank equal
arank up to a cumulative total of sMaxLines.
*****/
void RefLine_L2L_P2L::MakeAll(rank_t arank) {
	for (rank_t irank = 0; irank <= (arank - 1); irank++)
		for (rank_t jrank = 0; jrank <= (arank - 1 - irank); jrank++) {
			rank_t krank = arank - irank - jrank - 1;
			for (auto li : ReferenceFinder::sBasisLines.ranks[irank]) {
				for (auto mj : ReferenceFinder::sBasisMarks.ranks[jrank]) {
					for (auto lk : ReferenceFinder::sBasisLines.ranks[krank]) {
						if ((irank != krank) || (li != lk)) {
							if (ReferenceFinder::GetNumLines() >= Shared::sMaxLines) return;
							RefLine_L2L_P2L rlh1(li, mj, lk);
							ReferenceFinder::sBasisLines.AddCopyIfValidAndUnique(rlh1);
						};
					}
				}
			}
		}
}

void RefLine_L2L_P2L::Export(BinaryOutputStream &os) const {
	RefBase::Export(os);
	os << rl1->id << rm1->id << rl2->id;
}

RefLine *RefLine_L2L_P2L::Import(BinaryInputStream &is) {
	size_t id1, id2, id3;
	is.read(id1).read(id2).read(id3);
	RefLine *rl1 = ReferenceFinder::sBasisLines[id1];
	RefMark *rm1 = ReferenceFinder::sBasisMarks[id2];
	RefLine *rl2 = ReferenceFinder::sBasisLines[id3];
	return new RefLine_L2L_P2L(rl1, rm1, rl2);
}
