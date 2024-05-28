
#include "../../ReferenceFinder.h"

#include "../global.h"
#include "../math/paper.h"
#include "../math/xyline.h"
#include "../refLine/refLine.h"
#include "refMarkIntersection.h"

using namespace std;

/**********
class RefMark_Intersection - Specialization of a RefMark for a mark defined by
the intersection of 2 lines.
**********/

/*****
Constructor.
*****/
RefMark_Intersection::RefMark_Intersection(RefLine *arl1, RefLine *arl2) : RefMark(CalcMarkRank(arl1, arl2)), rl1(arl1), rl2(arl2) {
	// Get references to constituent math types

	mScore = rl1->mScore + rl2->mScore;

	const XYLine &l1 = rl1->l;
	const XYPt &u1 = rl1->l.u;
	//  const double& d1 = rl1->l.d;

	const XYLine &l2 = rl2->l;
	const XYPt &u2 = rl2->l.u;
	//  const double& d2 = rl2->l.d;

	// If the lines don't intersect, it's not a valid point. If they do,
	// assign the intersection to the member variable p.

	if (!l1.Intersects(l2, p)) return;

	// If the intersection point falls outside the square, it's not valid.

	if (!Shared::sPaper.Encloses(p)) return;

	// If the lines intersect at less than a 30 degree angle, we won't keep this
	// point because such intersections are imprecise to use as reference points.

	if (abs(u1.Dot(u2.Rotate90())) < Shared::sMinAngleSine) return;

	FinishConstructor();
}

/*****
Return true if this mark uses rb for immediate reference.
*****/
bool RefMark_Intersection::UsesImmediate(RefBase *rb) const {
	return (rb == rl1 || rb == rl2);
}

/*****
Build the folding sequence that constructs this object.
*****/
void RefMark_Intersection::SequencePushSelf() {
	rl1->SequencePushSelf();
	rl2->SequencePushSelf();
	RefBase::SequencePushSelf();
}

/*****
Export a description of how to construct this mark.
*****/
JsonObject RefMark_Intersection::Serialize() const {
	JsonObject step;
	step.add("axiom", 0);
	rl1->PutName("l0", step);
	rl2->PutName("l1", step);
	PutName("x", step);

#ifdef _DEBUG_DB_
	PutDebug(step);
#endif
	return step;
}

/*****
Go through existing lines and create RefMark_Intersections with rank equal to
arank, up to a cumulative total of sMaxMarks.
*****/
void RefMark_Intersection::MakeAll(rank_t arank) {
	for (rank_t irank = 0; irank <= arank / 2; irank++) {
		rank_t jrank = arank - irank;
		bool sameRank = (irank == jrank);
		auto &imap = ReferenceFinder::sBasisLines.ranks[irank];
		for (auto li = imap.begin() + (sameRank ? 1 : 0); li != imap.end(); li++) {
			auto &jmap = ReferenceFinder::sBasisLines.ranks[jrank];
			for (auto lj = jmap.begin(); lj != (sameRank ? li : jmap.end()); lj++) {
				if (ReferenceFinder::GetNumMarks() >= Shared::sMaxMarks) return;
				RefMark_Intersection rmi(*li, *lj);
				ReferenceFinder::sBasisMarks.AddCopyIfValidAndUnique(rmi);
			}
		}
	}
}
