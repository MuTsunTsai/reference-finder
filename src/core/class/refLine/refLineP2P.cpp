
#include "../../ReferenceFinder.h"
#include "../paper.h"
#include "../refDgmr.h"

#include "refLineP2P.h"

using namespace std;

/**********
class RefLine_P2P - Huzita-Hatori Axiom O2
Bring p1 to p2.
**********/

/*****
Constructor.
*****/
RefLine_P2P::RefLine_P2P(RefMark *arm1, RefMark *arm2) : RefLine(CalcLineRank(arm1, arm2)), rm1(arm1), rm2(arm2) {
	// Get references to points
	XYPt &p1 = rm1->p;
	XYPt &p2 = rm2->p;

	// Construct member data
	l.u = (p2 - p1).Normalize();
	l.d = .5 * (p1 + p2).Dot(l.u);

	// Check visibility
	bool p1edge = arm1->IsOnEdge();
	bool p2edge = arm2->IsOnEdge();

	if (ReferenceFinder::sVisibilityMatters) {
		if (p1edge)
			mWhoMoves = WHOMOVES_P1;
		else if (p2edge)
			mWhoMoves = WHOMOVES_P2;
		else
			return;
	} else {
		mWhoMoves = WHOMOVES_P1;
	};

	// If this line creates a skinny flap, we won't use it.
	if (ReferenceFinder::sPaper.MakesSkinnyFlap(l)) return;

	// Set the key.
	FinishConstructor();
}

/*****
Return true if this line uses rb for immediate reference.
*****/
bool RefLine_P2P::UsesImmediate(RefBase *rb) const {
	return (rb == rm1 || rb == rm2);
}

/*****
Build the folding sequence that constructs this object.
*****/
void RefLine_P2P::SequencePushSelf() {
	switch (mWhoMoves) {
	case WHOMOVES_P1:
		rm2->SequencePushSelf();
		rm1->SequencePushSelf();
		break;

	case WHOMOVES_P2:
		rm1->SequencePushSelf();
		rm2->SequencePushSelf();
		break;
	};
	RefBase::SequencePushSelf();
}

/*****
Put the construction of this line to a stream.
*****/
void RefLine_P2P::PutHowto(JsonArray &steps) const {
	JsonObject step;
	step.add("axiom", 2);
	switch (mWhoMoves) {
	case WHOMOVES_P1:
		rm1->PutName("p0", step);
		rm2->PutName("p1", step);
		break;

	case WHOMOVES_P2:
		rm2->PutName("p0", step);
		rm1->PutName("p1", step);
		break;
	};
	PutName("x", step);

	if (mForMark != NULL) step.add("pinch", 1);
	steps.add(step);
}

/*****
Draw this line, adding arrows if appropriate
*****/
void RefLine_P2P::DrawSelf(RefStyle rstyle, short ipass) const {
	// Call inherited method to draw the lines
	RefLine::DrawSelf(rstyle, ipass);

	// If we're moving, we need an arrow
	if ((ipass == PASS_ARROWS) && (rstyle == REFSTYLE_ACTION)) {
		XYPt &p1 = rm1->p;
		XYPt &p2 = rm2->p;
		switch (mWhoMoves) {
		case WHOMOVES_P1:
			sDgmr->DrawArrow(p1, p2);
			break;
		case WHOMOVES_P2:
			sDgmr->DrawArrow(p2, p1);
			break;
		}
	}
}

/*****
Go through existing lines and marks and create RefLine_P2Ps with rank equal to
arank, up to a cumulative total of sMaxLines.
*****/
void RefLine_P2P::MakeAll(rank_t arank) {
	for (rank_t irank = 0; irank <= (arank - 1) / 2; irank++) {
		rank_t jrank = arank - irank - 1;
		bool sameRank = (irank == jrank);
		auto &imap = ReferenceFinder::sBasisMarks.maps[irank];
		for (auto mi = imap.begin() + (sameRank ? 1 : 0); mi != imap.end(); mi++) {
			auto &jmap = ReferenceFinder::sBasisMarks.maps[jrank];
			for (auto mj = jmap.begin(); mj != (sameRank ? mi : jmap.end()); mj++) {
				if (ReferenceFinder::GetNumLines() >= ReferenceFinder::sMaxLines) return;
				RefLine_P2P rlb(*mi, *mj);
				ReferenceFinder::sBasisLines.AddCopyIfValidAndUnique(rlb);
			};
		}
	}
}
