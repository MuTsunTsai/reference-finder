
#include "ReferenceFinder.h"

#include "../refLine/refLine.h"

#include "global/global.h"
#include "math/paper.h"
#include "math/xyline.h"
#include "refMarkIntersection.h"

using namespace std;

/**********
class RefMark_Intersection - Specialization of a RefMark for a mark defined by
the intersection of 2 lines.
**********/

/*****
Constructor.
*****/
RefMark_Intersection::RefMark_Intersection(RefLine *arl1, RefLine *arl2): RefMark(RefType::MARK_INTERSECTION), rl1(arl1), rl2(arl2) {
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

	if(!l1.Intersects(l2, p)) return;

	// If the intersection point falls outside the square, it's not valid.

	if(!Shared::sPaper.Encloses(p)) return;

	// If the lines intersect at less than a 30 degree angle, we won't keep this
	// point because such intersections are imprecise to use as reference points.

	if(abs(u1.Dot(u2.Rotate90())) < Shared::sMinAngleSine) return;

	FinishConstructor();
}

/*****
Go through existing lines and create RefMark_Intersections with rank equal to
arank, up to a cumulative total of sMaxMarks.
*****/
void RefMark_IntersectionLogic::MakeAll(rank_t arank) const {
	for(rank_t irank = 0; irank <= arank / 2; irank++) {
		rank_t jrank = arank - irank;
		bool sameRank = (irank == jrank);
		auto &imap = ReferenceFinder::sBasisLines.ranks[irank];
		for(auto li = imap.begin() + (sameRank ? 1 : 0); li != imap.end(); li++) {
			auto &jmap = ReferenceFinder::sBasisLines.ranks[jrank];
			for(auto lj = jmap.begin(); lj != (sameRank ? li : jmap.end()); lj++) {
				if(ReferenceFinder::GetNumMarks() >= Shared::sMaxMarks) return;
				RefMark_Intersection rmi(*li, *lj);
				ReferenceFinder::sBasisMarks.AddCopyIfValidAndUnique(rmi);
			}
		}
	}
}

rank_t RefMark_IntersectionLogic::GetRank(const RefBase *self) const {
	const auto *s = static_cast<const RefMark_Intersection *>(self);
	return s->rl1->GetRank() + s->rl2->GetRank();
}

/*****
Return true if this mark uses rb for immediate reference.
*****/
bool RefMark_IntersectionLogic::UsesImmediate(const RefBase *self, RefBase *rb) const {
	const auto *s = static_cast<const RefMark_Intersection *>(self);
	return (rb == s->rl1 || rb == s->rl2);
}

/*****
Build the folding sequence that constructs this object.
*****/
void RefMark_IntersectionLogic::SequencePushSelf(RefBase *self) const {
	const auto *s = static_cast<const RefMark_Intersection *>(self);
	s->rl1->SequencePushSelf();
	s->rl2->SequencePushSelf();
	RefBaseLogic::SequencePushSelf(self);
}

/*****
Export a description of how to construct this mark.
*****/
JsonObject RefMark_IntersectionLogic::Serialize(const RefBase *self) const {
	const auto *s = static_cast<const RefMark_Intersection *>(self);
	JsonObject step;
	step.add("axiom", 0);
	s->rl1->PutName("l0", step);
	s->rl2->PutName("l1", step);
	PutName(self, "x", step);

#ifdef _DEBUG_DB_
	PutDebug(step);
#endif
	return step;
}

RefBase *RefMark_IntersectionLogic::Import(BinaryInputStream &is) const {
	size_t id1;
	size_t id2;
	is.read(id1).read(id2);
	// auto size = ReferenceFinder::sBasisLines.size();
	// if (id1 >= size || id2 >= size) cout << "lines: " << size << ", id1: " << id1 << ", id2: " << id2 << ", marks: " << ReferenceFinder::sBasisMarks.size() << endl;
	RefLine *rl1 = ReferenceFinder::sBasisLines[id1];
	RefLine *rl2 = ReferenceFinder::sBasisLines[id2];
	return new RefMark_Intersection(rl1, rl2);
}

void RefMark_IntersectionLogic::Export(const RefBase *self, BinaryOutputStream &os) const {
	RefBaseLogic::Export(self, os);
	const auto *s = static_cast<const RefMark_Intersection *>(self);
	os << s->rl1->id << s->rl2->id;
}
