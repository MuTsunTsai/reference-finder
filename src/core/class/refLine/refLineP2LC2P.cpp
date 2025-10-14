
#include "../refDgmr.h"
#include "ReferenceFinder.h"
#include "math/paper.h"

#include "refLineP2LC2P.h"

using namespace std;

/**********
class RefLine_P2L_C2P - Huzita-Hatori Axiom O5.
Bring point p1 to line l1 so that the crease passes through point p2.
**********/

/*****
Constructor. iroot can be 0 or 1.
*****/
RefLine_P2L_C2P::RefLine_P2L_C2P(RefMark *arm1, RefLine *arl1, RefMark *arm2, unsigned char iroot)
	: RefLine(RefType::LINE_P2L_C2P), rm1(arm1), rl1(arl1), rm2(arm2) {

	mRoot = iroot;
	mScore = rm1->mScore + rl1->mScore + rm2->mScore + Shared::sAxiomWeights[4];

	// Get references to the points and lines.
	XYPt &p1 = rm1->p;
	XYLine &l1 = rl1->l;
	XYPt &u1 = l1.u;
	double &d1 = l1.d;
	XYPt &p2 = rm2->p;

	// If either point is already on the line, then this isn't interesting, i.e., it's
	// a trivial Haga construction.
	// NOLINTNEXTLINE(clang-analyzer-optin.cplusplus.UninitializedObject)
	if(l1.Intersects(p1) || l1.Intersects(p2)) return;

	// Construct the line.
	double a = d1 - p2.Dot(u1);
	double b2 = (p2 - p1).Mag2() - a * a;

	if(b2 < 0) return; // no solution for negative b2 (implies imaginary b)

	double b = sqrt(b2);
	if((b < EPS) && (iroot == 1)) return; // degenerate case, there's only one solution

	// Construct the image of p1 (p1p), which depends on which root we're after.
	XYPt u1p = u1.Rotate90();
	XYPt p1p = p2 + a * u1;
	if(iroot == 0)
		p1p += b * u1p;
	else
		p1p -= b * u1p;

	// Validate; the point of incidence must lie within the square.
	if(!Shared::sPaper.Encloses(p1p)) return;

	// Construct member data.
	l.u = (p1p - p1).Normalize();
	l.d = p2.Dot(l.u);

	// Check visibility.
	bool p1edge = arm1->IsOnEdge();
	bool l1edge = arl1->IsOnEdge();

	if(Shared::sVisibilityMatters) {
		if(p1edge)
			mWhoMoves = RefLine_P2L_C2P_Logic::WHOMOVES_P1;
		else if(l1edge)
			mWhoMoves = RefLine_P2L_C2P_Logic::WHOMOVES_L1;
		else
			return;
	} else {
		mWhoMoves = RefLine_P2L_C2P_Logic::WHOMOVES_P1;
	};

	// If this line creates a skinny flap, we won't use it.
	if(Shared::sPaper.MakesSkinnyFlap(l)) return;

	// Set the key.
	FinishConstructor();
}

/*****
Go through existing lines and marks and create RefLine_P2L_C2Ps with rank equal
arank up to a cumulative total of sMaxLines.
*****/
void RefLine_P2L_C2P_Logic::MakeAll(rank_t arank) const {
	for(rank_t irank = 0; irank <= (arank - 1); irank++)
		for(rank_t jrank = 0; jrank <= (arank - 1 - irank); jrank++) {
			rank_t krank = arank - irank - jrank - 1;
			for(auto *mi: ReferenceFinder::sBasisMarks.ranks[irank]) {
				for(auto *lj: ReferenceFinder::sBasisLines.ranks[jrank]) {
					for(auto *mk: ReferenceFinder::sBasisMarks.ranks[krank]) {
						if((irank != krank) || (mi != mk)) { // only cmpr iterators if same container
							if(ReferenceFinder::GetNumLines() >= Shared::sMaxLines) return;
							RefLine_P2L_C2P rlh1(mi, lj, mk, 0);
							ReferenceFinder::sBasisLines.AddCopyIfValidAndUnique(rlh1);
							if(ReferenceFinder::GetNumLines() >= Shared::sMaxLines) return;
							RefLine_P2L_C2P rlh2(mi, lj, mk, 1);
							ReferenceFinder::sBasisLines.AddCopyIfValidAndUnique(rlh2);
						};
					}
				}
			}
		}
}

rank_t RefLine_P2L_C2P_Logic::GetRank(const RefBase *self) const {
	const auto *s = static_cast<const RefLine_P2L_C2P *>(self);
	return 1 + s->rm1->GetRank() + s->rl1->GetRank() + s->rm2->GetRank();
}

/*****
Return true if this line uses rb for immediate reference.
*****/
bool RefLine_P2L_C2P_Logic::UsesImmediate(const RefBase *self, RefBase *rb) const {
	const auto *s = static_cast<const RefLine_P2L_C2P *>(self);
	return (rb == s->rl1 || rb == s->rm1 || rb == s->rm2);
}

/*****
Build the folding sequence that constructs this object.
*****/
void RefLine_P2L_C2P_Logic::SequencePushSelf(RefBase *self) const {
	const auto *s = static_cast<const RefLine_P2L_C2P *>(self);
	s->rm2->SequencePushSelf();
	switch(s->mWhoMoves) {
	case WHOMOVES_P1:
		s->rl1->SequencePushSelf();
		s->rm1->SequencePushSelf();
		break;

	case WHOMOVES_L1:
		s->rm1->SequencePushSelf();
		s->rl1->SequencePushSelf();

		break;
	};
	RefBaseLogic::SequencePushSelf(self);
}

/*****
Export the construction of this line.
*****/
JsonObject RefLine_P2L_C2P_Logic::Serialize(const RefBase *self) const {
	const auto *s = static_cast<const RefLine_P2L_C2P *>(self);
	JsonObject step;
	step.add("axiom", 5);
	s->rm1->PutName("p0", step);
	s->rl1->PutName("l0", step);
	s->rm2->PutName("p1", step);
	switch(s->mWhoMoves) {
	case WHOMOVES_P1:
		step.add("order", "p0,l0");
		break;
	case WHOMOVES_L1:
		step.add("order", "l0,p0");
		break;
	};
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
void RefLine_P2L_C2P_Logic::DrawSelf(const RefBase *self, RefStyle rstyle, short ipass) const {
	const auto *s = static_cast<const RefLine_P2L_C2P *>(self);
	// Call inherited method to draw the lines

	RefLineLogic::DrawSelf(self, rstyle, ipass);

	// If we're moving, we need an arrow

	if((ipass == PASS_ARROWS) && (rstyle == REFSTYLE_ACTION)) {

		XYPt &p1 = s->rm1->p;
		XYPt p1f = s->l.Fold(p1);
		switch(s->mWhoMoves) {
		case WHOMOVES_P1:
			sDgmr->DrawArrow(p1, p1f);
			break;
		case WHOMOVES_L1:
			sDgmr->DrawArrow(p1f, p1);
			break;
		}
	}
}

RefBase *RefLine_P2L_C2P_Logic::Import(BinaryInputStream &is) const {
	index_t id1;
	index_t id2;
	index_t id3;
	unsigned char root;
	is.read(id1).read(id2).read(id3).read(root);
	RefMark *rm1 = ReferenceFinder::sBasisMarks[id1];
	RefLine *rl1 = ReferenceFinder::sBasisLines[id2];
	RefMark *rm2 = ReferenceFinder::sBasisMarks[id3];
	return new RefLine_P2L_C2P(rm1, rl1, rm2, root);
}

void RefLine_P2L_C2P_Logic::Export(const RefBase *self, BinaryOutputStream &os) const {
	const auto *s = static_cast<const RefLine_P2L_C2P *>(self);
	RefBaseLogic::Export(self, os);
	os << s->rm1->id << s->rl1->id << s->rm2->id << s->mRoot;
}
