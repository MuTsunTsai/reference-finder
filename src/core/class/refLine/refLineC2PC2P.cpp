
#include "../../ReferenceFinder.h"
#include "../math/paper.h"
#include "../refDgmr.h"

#include "refLineC2PC2P.h"

using namespace std;

/**********
class RefLine_C2P_C2P - Huzita-Hatori Axiom O1
Make a crease through two points p1 and p2.
**********/

/*****
Constructor. Initialize with the two marks that this line connects.
*****/
RefLine_C2P_C2P::RefLine_C2P_C2P(RefMark *arm1, RefMark *arm2) : RefLine(CalcLineRank(arm1, arm2)), rm1(arm1), rm2(arm2) {
	const XYPt &p1 = rm1->p;
	const XYPt &p2 = rm2->p;

	// Construct member data
	l.u = (p2 - p1).Rotate90().Normalize();
	l.d = .5 * (p1 + p2).Dot(l.u);

	// Don't need to check visibility because this type is always visible.
	// If this line creates a skinny flap, we won't use it.
	if (Shared::sPaper.MakesSkinnyFlap(l)) return;

	// This type is always valid.
	FinishConstructor();
}

/*****
Return true if this line uses rb for immediate reference.
*****/
bool RefLine_C2P_C2P::UsesImmediate(RefBase *rb) const {
	return (rb == rm1 || rb == rm2);
}

/*****
Build the folding sequence that constructs this object.
*****/
void RefLine_C2P_C2P::SequencePushSelf() {
	rm1->SequencePushSelf();
	rm2->SequencePushSelf();
	RefBase::SequencePushSelf();
}

/*****
Put the construction of this line to a stream.
*****/
void RefLine_C2P_C2P::PutHowto(JsonArray &steps) const {
	JsonObject step;
	step.add("axiom", 1);
	rm1->PutName("p0", step);
	rm2->PutName("p1", step);
	PutName("x", step);

	if (mForMark != NULL) step.add("pinch", 1);
	steps.add(step);
}

/*****
Draw this line, adding arrows if appropriate
*****/
void RefLine_C2P_C2P::DrawSelf(RefStyle rstyle, short ipass) const {
	// Call the inherited method to draw the line
	RefLine::DrawSelf(rstyle, ipass);

	// If we're moving, we need arrows.
	if ((ipass == PASS_ARROWS) && (rstyle == REFSTYLE_ACTION)) {

		// Get the endpoints of the fold
		const XYPt &p1 = rm1->p;
		const XYPt &p2 = rm2->p;

		// Get the perpendicular bisector of the fold
		XYPt mp = MidPoint(p1, p2);
		XYLine lb;
		lb.u = l.u.Rotate90();
		lb.d = mp.Dot(lb.u);

		// Get the points where the bisector crosses the paper
		XYPt p3, p4;
		Shared::sPaper.ClipLine(lb, p3, p4);

		// Parameterize these points along the bisector. Don't care about sign.
		double t3 = abs((p3 - mp).Dot(l.u));
		double t4 = abs((p4 - mp).Dot(l.u));

		// Construct a new pair of points that mate when folded and that are
		// guaranteed to lie within the paper.
		XYPt dp;
		if (t3 < t4)
			dp = t3 * l.u;
		else
			dp = t4 * l.u;
		p3 = mp + dp;
		p4 = mp - dp;

		// Draw an arrow that connects these two points.
		sDgmr->DrawArrow(p3, p4);
	}
}

/*****
Go through existing lines and marks and create RefLine_C2P_C2Ps with rank equal
to arank, up to a cumulative total of sMaxLines.
*****/
void RefLine_C2P_C2P::MakeAll(rank_t arank) {
	for (rank_t irank = 0; irank <= (arank - 1) / 2; irank++) {
		rank_t jrank = arank - irank - 1;
		bool sameRank = (irank == jrank);
		auto &imap = ReferenceFinder::sBasisMarks.maps[irank];
		for (auto mi = imap.begin() + (sameRank ? 1 : 0); mi != imap.end(); mi++) {
			auto &jmap = ReferenceFinder::sBasisMarks.maps[jrank];
			for (auto mj = jmap.begin(); mj != (sameRank ? mi : jmap.end()); mj++) {
				if (ReferenceFinder::GetNumLines() >= Shared::sMaxLines) return;
				RefLine_C2P_C2P rlc(*mi, *mj);
				ReferenceFinder::sBasisLines.AddCopyIfValidAndUnique(rlc);
			};
		}
	}
}
