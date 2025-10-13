
#include "../refDgmr.h"
#include "ReferenceFinder.h"
#include "math/paper.h"

#include "refLineL2LC2P.h"

using namespace std;

/**********
class RefLine_L2L_C2P - Huzita-Hatori Axiom O4
Bring line l1 to itself so that the crease goes through point p1
**********/

/*****
Constructor.
*****/
RefLine_L2L_C2P::RefLine_L2L_C2P(RefLine *arl1, RefMark *arm1)
	: RefLine(RefType::LINE_L2L_C2P), rl1(arl1), rm1(arm1) {

	mScore = rl1->mScore + rm1->mScore + Shared::sAxiomWeights[3];

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
	if(!Shared::sPaper.Encloses(p1p)) return;

	// Don't need to check visibility, this kind is always visible.
	// If this line creates a skinny flap, we won't use it.
	if(Shared::sPaper.MakesSkinnyFlap(l)) return;

	// Set the key.
	FinishConstructor();
}

/*****
Go through existing lines and marks and create RefLine_L2L_C2Ps with rank equal
to arank up to a cumulative total of sMaxLines.
*****/
void RefLine_L2L_C2P_Logic::MakeAll(rank_t arank) const {
	for(rank_t irank = 0; irank <= (arank - 1); irank++) {
		rank_t jrank = arank - irank - 1;
		for(auto *li: ReferenceFinder::sBasisLines.ranks[irank]) {
			for(auto *mj: ReferenceFinder::sBasisMarks.ranks[jrank]) {
				if(ReferenceFinder::GetNumLines() >= Shared::sMaxLines) return;
				RefLine_L2L_C2P rls1(li, mj);
				ReferenceFinder::sBasisLines.AddCopyIfValidAndUnique(rls1);
			}
		}
	}
}

rank_t RefLine_L2L_C2P_Logic::GetRank(const RefBase *self) const {
	const auto *s = static_cast<const RefLine_L2L_C2P *>(self);
	return 1 + s->rl1->GetRank() + s->rm1->GetRank();
}

/*****
Return true if this line uses rb for immediate reference.
*****/
bool RefLine_L2L_C2P_Logic::UsesImmediate(const RefBase *self, RefBase *rb) const {
	const auto *s = static_cast<const RefLine_L2L_C2P *>(self);
	return (rb == s->rl1 || rb == s->rm1);
}

/*****
Build the folding sequence that constructs this object.
*****/
void RefLine_L2L_C2P_Logic::SequencePushSelf(RefBase *self) const {
	const auto *s = static_cast<const RefLine_L2L_C2P *>(self);
	s->rm1->SequencePushSelf();
	s->rl1->SequencePushSelf();
	RefBaseLogic::SequencePushSelf(self);
}

/*****
Export the construction of this line.
*****/
JsonObject RefLine_L2L_C2P_Logic::Serialize(const RefBase *self) const {
	const auto *s = static_cast<const RefLine_L2L_C2P *>(self);
	JsonObject step;
	step.add("axiom", 4);
	s->rl1->PutName("l0", step);
	PutName(self, "x", step);
	s->rm1->PutName("p0", step);

	if(s->mForMark != nullptr) step.add("pinch", 1);
#ifdef _DEBUG_DB_
	PutDebug(step);
#endif
	return step;
}

/*****
Draw this line, adding arrows if appropriate
*****/
void RefLine_L2L_C2P_Logic::DrawSelf(const RefBase *self, RefStyle rstyle, short ipass) const {
	const auto *s = static_cast<const RefLine_L2L_C2P *>(self);
	// Call inherited method to draw the lines
	RefLineLogic::DrawSelf(self, rstyle, ipass);

	// If we're moving, we need an arrow
	if((ipass == PASS_ARROWS) && (rstyle == REFSTYLE_ACTION)) {
		XYPt p1;
		XYPt p2;
		XYLine &l1 = s->rl1->l;
		Shared::sPaper.ClipLine(l1, p1, p2); // get endpts of the reference line
		XYPt pi = Intersection(s->l, l1);	 // intersection w/ fold line
		XYPt u1p = l1.u.Rotate90();			 // tangent to reference line
		double t1 = abs((p1 - pi).Dot(u1p));
		double t2 = abs((p2 - pi).Dot(u1p));
		double tmin = t1 < t2 ? t1 : t2;
		sDgmr->DrawArrow(pi + tmin * u1p, pi - tmin * u1p);
	}
}

RefBase *RefLine_L2L_C2P_Logic::Import(BinaryInputStream &is) const {
	size_t id1;
	size_t id2;
	is.read(id1).read(id2);
	RefLine *rl1 = ReferenceFinder::sBasisLines[id1];
	RefMark *rm1 = ReferenceFinder::sBasisMarks[id2];
	return new RefLine_L2L_C2P(rl1, rm1);
}

void RefLine_L2L_C2P_Logic::Export(const RefBase *self, BinaryOutputStream &os) const {
	const auto *s = static_cast<const RefLine_L2L_C2P *>(self);
	RefBaseLogic::Export(self, os);
	os << s->rl1->id << s->rm1->id;
}
