
#include "../refDgmr.h"
#include "ReferenceFinder.h"
#include "math/paper.h"

#include "refLineC2PC2P.h"

using namespace std;

/**********
class RefLine_C2P_C2P - Huzita-Hatori Axiom O1
Make a crease through two points p1 and p2.
**********/

/*****
Constructor. Initialize with the two marks that this line connects.
*****/
RefLine_C2P_C2P::RefLine_C2P_C2P(RefMark *arm1, RefMark *arm2): RefLine(RefType::LINE_C2P_C2P), rm1(arm1), rm2(arm2) {

	mScore = rm1->mScore + rm2->mScore + Shared::sAxiomWeights[0];

	const XYPt &p1 = rm1->p;
	const XYPt &p2 = rm2->p;

	// Construct member data
	l.u = (p2 - p1).Rotate90().Normalize();
	l.d = .5 * (p1 + p2).Dot(l.u);

	// Don't need to check visibility because this type is always visible.
	// If this line creates a skinny flap, we won't use it.
	if(Shared::sPaper.MakesSkinnyFlap(l)) return;

	// This type is always valid.
	FinishConstructor();
}

/*****
Go through existing lines and marks and create RefLine_C2P_C2Ps with rank equal
to arank, up to a cumulative total of sMaxLines.
*****/
void RefLine_C2P_C2P_Logic::MakeAll(rank_t arank) const {
	for(rank_t irank = 0; irank <= (arank - 1) / 2; irank++) {
		rank_t jrank = arank - irank - 1;
		bool sameRank = (irank == jrank);
		auto &imap = ReferenceFinder::sBasisMarks.ranks[irank];
		for(auto mi = imap.begin() + (sameRank ? 1 : 0); mi != imap.end(); mi++) {
			auto &jmap = ReferenceFinder::sBasisMarks.ranks[jrank];
			for(auto mj = jmap.begin(); mj != (sameRank ? mi : jmap.end()); mj++) {
				if(ReferenceFinder::GetNumLines() >= Shared::sMaxLines) return;
				RefLine_C2P_C2P rlc(*mi, *mj);
				ReferenceFinder::sBasisLines.AddCopyIfValidAndUnique(rlc);
			};
		}
	}
}

rank_t RefLine_C2P_C2P_Logic::GetRank(const RefBase *self) const {
	const auto *s = static_cast<const RefLine_C2P_C2P *>(self);
	return 1 + s->rm1->GetRank() + s->rm2->GetRank();
}

/*****
Return true if this line uses rb for immediate reference.
*****/
bool RefLine_C2P_C2P_Logic::UsesImmediate(const RefBase *self, RefBase *rb) const {
	const auto *s = static_cast<const RefLine_C2P_C2P *>(self);
	return (rb == s->rm1 || rb == s->rm2);
}

/*****
Build the folding sequence that constructs this object.
*****/
void RefLine_C2P_C2P_Logic::SequencePushSelf(RefBase *self) const {
	const auto *s = static_cast<const RefLine_C2P_C2P *>(self);
	s->rm1->SequencePushSelf();
	s->rm2->SequencePushSelf();
	RefBaseLogic::SequencePushSelf(self);
}

/*****
Export the construction of this line.
*****/
JsonObject RefLine_C2P_C2P_Logic::Serialize(const RefBase *self) const {
	const auto *s = static_cast<const RefLine_C2P_C2P *>(self);
	JsonObject step;
	step.add("axiom", 1);
	s->rm1->PutName("p0", step);
	s->rm2->PutName("p1", step);
	PutName(self, "x", step);

	if(s->mForMark != nullptr) step.add("pinch", 1);

#ifdef _DEBUG_DB_
	PutDebug(step);
#endif
	return step;
}

/*****
Draw this line, adding arrows if appropriate
*****/
void RefLine_C2P_C2P_Logic::DrawSelf(const RefBase *self, RefStyle rstyle, short ipass) const {
	const auto *s = static_cast<const RefLine_C2P_C2P *>(self);
	// Call the inherited method to draw the line
	RefLineLogic::DrawSelf(self, rstyle, ipass);

	// If we're moving, we need arrows.
	if((ipass == PASS_ARROWS) && (rstyle == REFSTYLE_ACTION)) {

		// Get the endpoints of the fold
		const XYPt &p1 = s->rm1->p;
		const XYPt &p2 = s->rm2->p;

		// Get the perpendicular bisector of the fold
		XYPt mp = MidPoint(p1, p2);
		XYLine lb;
		lb.u = s->l.u.Rotate90();
		lb.d = mp.Dot(lb.u);

		// Get the points where the bisector crosses the paper
		XYPt p3;
		XYPt p4;
		Shared::sPaper.ClipLine(lb, p3, p4);

		// Parameterize these points along the bisector. Don't care about sign.
		double t3 = abs((p3 - mp).Dot(s->l.u));
		double t4 = abs((p4 - mp).Dot(s->l.u));

		// Construct a new pair of points that mate when folded and that are
		// guaranteed to lie within the paper.
		XYPt dp;
		if(t3 < t4)
			dp = t3 * s->l.u;
		else
			dp = t4 * s->l.u;
		p3 = mp + dp;
		p4 = mp - dp;

		// Draw an arrow that connects these two points.
		sDgmr->DrawArrow(p3, p4);
	}
}

RefBase *RefLine_C2P_C2P_Logic::Import(BinaryInputStream &is) const {
	size_t id1;
	size_t id2;
	is.read(id1).read(id2);
	RefMark *rm1 = ReferenceFinder::sBasisMarks[id1];
	RefMark *rm2 = ReferenceFinder::sBasisMarks[id2];
	return new RefLine_C2P_C2P(rm1, rm2);
}

void RefLine_C2P_C2P_Logic::Export(const RefBase *self, BinaryOutputStream &os) const {
	const auto *s = static_cast<const RefLine_C2P_C2P *>(self);
	RefBaseLogic::Export(self, os);
	os << s->rm1->id << s->rm2->id;
}
