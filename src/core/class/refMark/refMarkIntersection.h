
#pragma once

#include "refMark.h"
#include "json/jsonArray.h"

class RefLine; // forward declaration needed by RefMark_Intersection

/**********
class RefMark_Intersection - Specialization of a RefMark for a mark defined by
the intersection of 2 lines.
**********/
class RefMark_Intersection: public RefMark {
  public:
	RefLine *rl1; // first line
	RefLine *rl2; // second line

	RefMark_Intersection(RefLine *arl1, RefLine *arl2);

	type_t GetType() const override;
	rank_t GetRank() const override;

	bool UsesImmediate(RefBase *rb) const override;
	void SequencePushSelf() override;
	JsonObject Serialize() const override;
	void Export(BinaryOutputStream &os) const override;

	static RefMark *Import(BinaryInputStream &is);

	static void MakeAll(rank_t arank);
};
