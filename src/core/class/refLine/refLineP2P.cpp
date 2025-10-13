
#include "../refDgmr.h"
#include "ReferenceFinder.h"
#include "math/paper.h"

#include "refLineP2P.h"

using namespace std;

/**********
class RefLine_P2P - Huzita-Hatori Axiom O2
Bring p1 to p2.
**********/

/*****
Constructor.
*****/
RefLine_P2P::RefLine_P2P(RefMark *arm1, RefMark *arm2): RefLine(RefType::LINE_P2P), rm1(arm1), rm2(arm2) {

	mScore = rm1->mScore + rm2->mScore + Shared::sAxiomWeights[1];

	// Get references to points
	XYPt &p1 = rm1->p;
	XYPt &p2 = rm2->p;

	// Construct member data
	l.u = (p2 - p1).Normalize();
	l.d = .5 * (p1 + p2).Dot(l.u);

	// Check visibility
	bool p1edge = arm1->IsOnEdge();
	bool p2edge = arm2->IsOnEdge();

	if(Shared::sVisibilityMatters) {
		if(p1edge)
			mWhoMoves = RefLine_P2P_Logic::WHOMOVES_P1;
		else if(p2edge)
			mWhoMoves = RefLine_P2P_Logic::WHOMOVES_P2;
		else
			// NOLINTNEXTLINE(clang-analyzer-optin.cplusplus.UninitializedObject)
			return;
	} else {
		mWhoMoves = RefLine_P2P_Logic::WHOMOVES_P1;
	};

	// If this line creates a skinny flap, we won't use it.
	if(Shared::sPaper.MakesSkinnyFlap(l)) return;

	// Set the key.
	FinishConstructor();
}

/*****
Go through existing lines and marks and create RefLine_P2Ps with rank equal to
arank, up to a cumulative total of sMaxLines.
*****/
void RefLine_P2P_Logic::MakeAll(rank_t arank) const {
	for(rank_t irank = 0; irank <= (arank - 1) / 2; irank++) {
		rank_t jrank = arank - irank - 1;
		bool sameRank = (irank == jrank);
		auto &imap = ReferenceFinder::sBasisMarks.ranks[irank];
		for(auto mi = imap.begin() + (sameRank ? 1 : 0); mi != imap.end(); mi++) {
			auto &jmap = ReferenceFinder::sBasisMarks.ranks[jrank];
			for(auto mj = jmap.begin(); mj != (sameRank ? mi : jmap.end()); mj++) {
				if(ReferenceFinder::GetNumLines() >= Shared::sMaxLines) return;
				RefLine_P2P rlb(*mi, *mj);
				ReferenceFinder::sBasisLines.AddCopyIfValidAndUnique(rlb);
			};
		}
	}
}

rank_t RefLine_P2P_Logic::GetRank(const RefBase *self) const {
	const auto *s = static_cast<const RefLine_P2P *>(self);
	return 1 + s->rm1->GetRank() + s->rm2->GetRank();
}

/*****
Return true if this line uses rb for immediate reference.
*****/
bool RefLine_P2P_Logic::UsesImmediate(const RefBase *self, RefBase *rb) const {
	const auto *s = static_cast<const RefLine_P2P *>(self);
	return (rb == s->rm1 || rb == s->rm2);
}

/*****
Build the folding sequence that constructs this object.
*****/
void RefLine_P2P_Logic::SequencePushSelf(RefBase *self) const {
	const auto *s = static_cast<const RefLine_P2P *>(self);
	switch(s->mWhoMoves) {
	case WHOMOVES_P1:
		s->rm2->SequencePushSelf();
		s->rm1->SequencePushSelf();
		break;

	case WHOMOVES_P2:
		s->rm1->SequencePushSelf();
		s->rm2->SequencePushSelf();
		break;
	};
	RefBaseLogic::SequencePushSelf(self);
}

/*****
Export the construction of this line.
*****/
JsonObject RefLine_P2P_Logic::Serialize(const RefBase *self) const {
	const auto *s = static_cast<const RefLine_P2P *>(self);
	JsonObject step;
	step.add("axiom", 2);
	switch(s->mWhoMoves) {
	case WHOMOVES_P1:
		s->rm1->PutName("p0", step);
		s->rm2->PutName("p1", step);
		break;

	case WHOMOVES_P2:
		s->rm2->PutName("p0", step);
		s->rm1->PutName("p1", step);
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
void RefLine_P2P_Logic::DrawSelf(const RefBase *self, RefStyle rstyle, short ipass) const {
	const auto *s = static_cast<const RefLine_P2P *>(self);
	// Call inherited method to draw the lines
	RefLineLogic::DrawSelf(self, rstyle, ipass);

	// If we're moving, we need an arrow
	if((ipass == PASS_ARROWS) && (rstyle == REFSTYLE_ACTION)) {
		XYPt &p1 = s->rm1->p;
		XYPt &p2 = s->rm2->p;
		switch(s->mWhoMoves) {
		case WHOMOVES_P1:
			sDgmr->DrawArrow(p1, p2);
			break;
		case WHOMOVES_P2:
			sDgmr->DrawArrow(p2, p1);
			break;
		}
	}
}

RefBase *RefLine_P2P_Logic::Import(BinaryInputStream &is) const {
	size_t id1;
	size_t id2;
	is.read(id1).read(id2);
	RefMark *rm1 = ReferenceFinder::sBasisMarks[id1];
	RefMark *rm2 = ReferenceFinder::sBasisMarks[id2];
	return new RefLine_P2P(rm1, rm2);
}

void RefLine_P2P_Logic::Export(const RefBase *self, BinaryOutputStream &os) const {
	const auto *s = static_cast<const RefLine_P2P *>(self);
	RefBaseLogic::Export(self, os);
	os << s->rm1->id << s->rm2->id;
}
