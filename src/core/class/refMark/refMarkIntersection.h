
#pragma once

#include "refMark.h"
#include "json/jsonArray.h"

struct RefLine; // forward declaration needed by RefMark_Intersection

/**********
struct RefMark_Intersection - Specialization of a RefMark for a mark defined by
the intersection of 2 lines.
**********/
struct RefMark_Intersection: RefMark {
  public:
	RefLine *rl1; // first line
	RefLine *rl2; // second line

	RefMark_Intersection(RefLine *arl1, RefLine *arl2);
};

class RefMark_IntersectionLogic: public RefMarkLogic {
  public:
	void MakeAll(rank_t arank) const override;
	rank_t GetRank(const RefBase *self) const override;
	bool UsesImmediate(const RefBase *self, RefBase *rb) const override;
	void SequencePushSelf(RefBase *self) const override;
	JsonObject Serialize(const RefBase *self) const override;
	RefBase *Import(BinaryInputStream &is) const override;
	void Export(const RefBase *self, BinaryOutputStream &os) const override;
};
