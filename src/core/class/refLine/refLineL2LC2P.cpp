
#include "../../ReferenceFinder.h"
#include "../paper.h"
#include "../refDgmr.h"

#include "refLineL2LC2P.h"

using namespace std;

/**********
class RefLine_L2L_C2P - Huzita-Hatori Axiom O4
Bring line l1 to itself so that the crease goes through point p1
**********/

/*****
Constructor.
*****/
RefLine_L2L_C2P::RefLine_L2L_C2P(RefLine *arl1, RefMark *arm1) : RefLine(CalcLineRank(arl1, arm1)), rl1(arl1), rm1(arm1) {
	// Get references to line and mark
	XYPt &u1 = rl1->l.u;
	double &d1 = rl1->l.d;
	XYPt &p1 = rm1->p;

	// Construct the direction vector and distance scalar for the fold line.
	l.u = u1.Rotate90();
	l.d = p1.Dot(l.u);

	// The intersection of the fold line with line l1 must be enclosed in the paper.
	// That point is the projection of p1 onto line l1.
	XYPt p1p = p1 + (d1 - (p1.Dot(u1))) * u1;
	if (!ReferenceFinder::sPaper.Encloses(p1p)) return;

	// Don't need to check visibility, this kind is always visible.
	// If this line creates a skinny flap, we won't use it.
	if (ReferenceFinder::sPaper.MakesSkinnyFlap(l)) return;

	// Set the key.
	FinishConstructor();
}

/*****
Return true if this line uses rb for immediate reference.
*****/
bool RefLine_L2L_C2P::UsesImmediate(RefBase *rb) const {
	return (rb == rl1 || rb == rm1);
}

/*****
Build the folding sequence that constructs this object.
*****/
void RefLine_L2L_C2P::SequencePushSelf() {
	rm1->SequencePushSelf();
	rl1->SequencePushSelf();
	RefBase::SequencePushSelf();
}

/*****
Put the construction of this line to a stream.
*****/
void RefLine_L2L_C2P::PutHowto(JsonArray &steps) const {
	JsonObject step;
	step.add("axiom", 4);
	rl1->PutName("l0", step);
	PutName("x", step);
	rm1->PutName("p0", step);
	steps.add(step);
}

/*****
Draw this line, adding arrows if appropriate
*****/
void RefLine_L2L_C2P::DrawSelf(RefStyle rstyle, short ipass) const {
	// Call inherited method to draw the lines
	RefLine::DrawSelf(rstyle, ipass);

	// If we're moving, we need an arrow
	if ((ipass == PASS_ARROWS) && (rstyle == REFSTYLE_ACTION)) {

		XYPt p1, p2;
		XYLine &l1 = rl1->l;
		ReferenceFinder::sPaper.ClipLine(l1, p1, p2); // get endpts of the reference line
		XYPt pi = Intersection(l, l1);				  // intersection w/ fold line
		XYPt u1p = l1.u.Rotate90();					  // tangent to reference line
		double t1 = abs((p1 - pi).Dot(u1p));
		double t2 = abs((p2 - pi).Dot(u1p));
		double tmin = t1 < t2 ? t1 : t2;
		sDgmr->DrawFoldAndUnfoldArrow(pi + tmin * u1p, pi - tmin * u1p);
	}
}

/*****
Go through existing lines and marks and create RefLine_L2L_C2Ps with rank equal
to arank up to a cumulative total of sMaxLines.
*****/
void RefLine_L2L_C2P::MakeAll(rank_t arank) {
	for (rank_t irank = 0; irank <= (arank - 1); irank++) {
		rank_t jrank = arank - irank - 1;
		RefContainer<RefLine>::rank_iterator li =
			ReferenceFinder::sBasisLines.maps[irank].begin();
		while (li != ReferenceFinder::sBasisLines.maps[irank].end()) {
			RefContainer<RefMark>::rank_iterator mj =
				ReferenceFinder::sBasisMarks.maps[jrank].begin();
			while (mj != ReferenceFinder::sBasisMarks.maps[jrank].end()) {
				if (ReferenceFinder::GetNumLines() >= ReferenceFinder::sMaxLines) return;
				RefLine_L2L_C2P rls1(*li, *mj);
				ReferenceFinder::sBasisLines.AddCopyIfValidAndUnique(rls1);
				mj++;
			};
			li++;
		}
	}
}
