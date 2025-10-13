
#include "../refDgmr.h"
#include "ReferenceFinder.h"
#include "math/paper.h"

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

RefLine_L2L::RefLine_L2L(RefLine *arl1, RefLine *arl2, unsigned char iroot)
	: RefLine(RefType::LINE_L2L), rl1(arl1), rl2(arl2) {

	mRoot = iroot;
	mScore = rl1->mScore + rl2->mScore + Shared::sAxiomWeights[2];

	// Get references to lines
	XYLine &l1 = rl1->l;
	XYPt &u1 = l1.u;
	double &d1 = l1.d;
	XYLine &l2 = rl2->l;
	XYPt &u2 = l2.u;
	double &d2 = l2.d;

	// Parallel lines get handled specially. There's only one solution; we arbitrarily make
	// it the iroot=0 solution.
	if(l1.IsParallelTo(l2)) {
		if(iroot == 0) {
			l.u = u1;
			l.d = .5 * (d1 + d2 * u2.Dot(u1));
		} else
			// NOLINTNEXTLINE(clang-analyzer-optin.cplusplus.UninitializedObject)
			return; // iroot = 1 for parallel lines isn't a valid solution.
	} else {		// nonparallel lines

		// Construct the direction vector for the bisector, depending on the value of iroot.
		if(iroot == 0)
			l.u = (u1 + u2).Normalize();
		else
			l.u = (u1 - u2).Normalize();

		l.d = Intersection(l1, l2).Dot(l.u);
	};

	// If the paper doesn't overlap the fold line, we're not valid.
	if(!Shared::sPaper.InteriorOverlaps(l)) return;

	// Check visibility
	bool l1edge = arl1->IsOnEdge();
	bool l2edge = arl2->IsOnEdge();

	if(Shared::sVisibilityMatters) {
		if(l1edge)
			mWhoMoves = RefLine_L2L_Logic::WHOMOVES_L1;
		else if(l2edge)
			mWhoMoves = RefLine_L2L_Logic::WHOMOVES_L2;
		else {
			XYPt lp1;
			XYPt lp2;
			Shared::sPaper.ClipLine(l1, lp1, lp2);
			if(Shared::sPaper.Encloses(l.Fold(lp1)) &&
			   Shared::sPaper.Encloses(l.Fold(lp2)))
				mWhoMoves = RefLine_L2L_Logic::WHOMOVES_L1;
			else {
				Shared::sPaper.ClipLine(l2, lp1, lp2);
				if(Shared::sPaper.Encloses(l.Fold(lp1)) &&
				   Shared::sPaper.Encloses(l.Fold(lp2)))
					mWhoMoves = RefLine_L2L_Logic::WHOMOVES_L2;
				else
					return;
			}
		}
	} else {
		mWhoMoves = RefLine_L2L_Logic::WHOMOVES_L1;
	};

	// If this line creates a skinny flap, we won't use it.
	if(Shared::sPaper.MakesSkinnyFlap(l)) return;

	// Set the key.
	FinishConstructor();
}

/*****
Go through existing lines and marks and create RefLine_L2Ls with rank equal to
arank up to a cumulative total of sMaxLines.
*****/
void RefLine_L2L_Logic::MakeAll(rank_t arank) const {
	for(rank_t irank = 0; irank <= (arank - 1) / 2; irank++) {
		rank_t jrank = arank - irank - 1;
		bool sameRank = (irank == jrank);
		auto &imap = ReferenceFinder::sBasisLines.ranks[irank];
		for(auto li = imap.begin() + (sameRank ? 1 : 0); li != imap.end(); li++) {
			auto &jmap = ReferenceFinder::sBasisLines.ranks[jrank];
			for(auto lj = jmap.begin(); lj != (sameRank ? li : jmap.end()); lj++) {
				if(ReferenceFinder::GetNumLines() >= Shared::sMaxLines) return;
				RefLine_L2L rls1(*li, *lj, 0);
				ReferenceFinder::sBasisLines.AddCopyIfValidAndUnique(rls1);
				if(ReferenceFinder::GetNumLines() >= Shared::sMaxLines) return;
				RefLine_L2L rls2(*li, *lj, 1);
				ReferenceFinder::sBasisLines.AddCopyIfValidAndUnique(rls2);
			};
		}
	}
}

rank_t RefLine_L2L_Logic::GetRank(const RefBase *self) const {
	const auto *s = static_cast<const RefLine_L2L *>(self);
	return 1 + s->rl1->GetRank() + s->rl2->GetRank();
}

/*****
Return true if this line uses rb for immediate reference.
*****/
bool RefLine_L2L_Logic::UsesImmediate(const RefBase *self, RefBase *rb) const {
	const auto *s = static_cast<const RefLine_L2L *>(self);
	return (rb == s->rl1 || rb == s->rl2);
}

/*****
Build the folding sequence that constructs this object.
*****/
void RefLine_L2L_Logic::SequencePushSelf(RefBase *self) const {
	const auto *s = static_cast<const RefLine_L2L *>(self);
	switch(s->mWhoMoves) {
	case WHOMOVES_L1:
		s->rl2->SequencePushSelf();
		s->rl1->SequencePushSelf();
		break;

	case WHOMOVES_L2:
		s->rl1->SequencePushSelf();
		s->rl2->SequencePushSelf();
		break;
	};
	RefBaseLogic::SequencePushSelf(self);
}

/*****
Export the construction of this line.
*****/
JsonObject RefLine_L2L_Logic::Serialize(const RefBase *self) const {
	const auto *s = static_cast<const RefLine_L2L *>(self);
	JsonObject step;
	step.add("axiom", 3);
	switch(s->mWhoMoves) {
	case WHOMOVES_L1:
		s->rl1->PutName("l0", step);
		s->rl2->PutName("l1", step);
		break;

	case WHOMOVES_L2:
		s->rl2->PutName("l0", step);
		s->rl1->PutName("l1", step);
		break;
	};
	PutName(self, "x", step);

	// Now we need to specify which of the two bisectors this is, which we do
	// by specifying a point where the bisector hits the edge of the square.
	XYPt p;
	s->rl1->l.Intersects(s->rl2->l, p); // get the intersection of the two bisectors

	XYPt pa;
	XYPt pb;
	Shared::sPaper.ClipLine(s->l, pa, pb); // find where our fold line hits the paper.

	// Return the first point of intersection between the fold line and the edge of the
	// paper that _isn't_ the intersection of the two bisectors.
	if(p.equals(pa)) {
		step.add("p0", pb.Chop());
	} else {
		step.add("p0", pa.Chop());
	}

	if(s->mForMark != nullptr) step.add("pinch", 1);
#ifdef _DEBUG_DB_
	PutDebug(step);
#endif
	return step;
}

/*****
Draw this line, adding arrows if appropriate
*****/
void RefLine_L2L_Logic::DrawSelf(const RefBase *self, RefStyle rstyle, short ipass) const {
	const auto *s = static_cast<const RefLine_L2L *>(self);
	// Call inherited method to draw the lines
	RefLineLogic::DrawSelf(self, rstyle, ipass);

	// If we're moving, we need an arrow that brings two points from one line to
	// two points on the other line. We need to pick points that are within the
	// paper for both lines.
	if((ipass == PASS_ARROWS) && (rstyle == REFSTYLE_ACTION)) {

		XYLine &l1 = s->rl1->l;
		XYLine &l2 = s->rl2->l;
		XYPt p;
		bool isParallel = !l1.Intersects(l2, p); // intersection
		XYPt p1a;
		XYPt p1b;
		Shared::sPaper.ClipLine(l1, p1a, p1b); // endpoints of l1
		XYPt p2a;
		XYPt p2b;
		Shared::sPaper.ClipLine(l2, p2a, p2b); // endpoints of l2
		p2a = s->l.Fold(p2a);				   // flop l2 points onto l1
		p2b = s->l.Fold(p2b);
		XYPt du1 = l1.d * l1.u;				   // a point on l1
		XYPt up1 = l1.u.Rotate90();			   // a tangent to l1
		vector<double> tvals;				   // holds parameterizations of the 4 points
		tvals.push_back((p1a - du1).Dot(up1)); // parameterize p1a along l1
		tvals.push_back((p1b - du1).Dot(up1)); // parameterize p1b along l1
		tvals.push_back((p2a - du1).Dot(up1)); // parameterize p2a along l1
		tvals.push_back((p2b - du1).Dot(up1)); // parameterize p2b along l1
		sort(tvals.begin(), tvals.end());	   // sort them in order; we want the middle 2

		// Place the arrow closer to tvals[2] if the resulting arrow is too small
		XYPt p1c;
		XYPt p2c;
		int weight = 1;
		do {
			double offset = (tvals[1] + weight * tvals[2]) / (1 + weight);
			p1c = du1 + offset * up1;
			p2c = s->l.Fold(p1c);
			weight++;
		} while(!isParallel && (p1c - p2c).Mag() < 0.3 && weight < 5);

		XYPt *around = isParallel ? nullptr : &p; // Specify arrow orientation for L2L
		switch(s->mWhoMoves) {
		case WHOMOVES_L1:
			sDgmr->DrawArrow(p1c, p2c, around);
			break;
		case WHOMOVES_L2:
			sDgmr->DrawArrow(p1c, p2c, around);
			break;
		}
	}
}

RefBase *RefLine_L2L_Logic::Import(BinaryInputStream &is) const {
	size_t id1;
	size_t id2;
	unsigned char root;
	is.read(id1).read(id2).read(root);
	RefLine *rl1 = ReferenceFinder::sBasisLines[id1];
	RefLine *rl2 = ReferenceFinder::sBasisLines[id2];
	return new RefLine_L2L(rl1, rl2, root);
}

void RefLine_L2L_Logic::Export(const RefBase *self, BinaryOutputStream &os) const {
	const auto *s = static_cast<const RefLine_L2L *>(self);
	RefBaseLogic::Export(self, os);
	os << s->rl1->id << s->rl2->id << s->mRoot;
}
