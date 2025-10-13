
#pragma once

#include "../refMark/refMark.h"
#include "refLine.h"
#include "json/jsonArray.h"

/**********
struct RefLine_L2L_C2P - Huzita-Hatori Axiom O4.
Bring line l1 to itself so that the crease passes through point p1.
**********/
struct RefLine_L2L_C2P: RefLine {
	RefLine *rl1; // bring line l1 to itself
	RefMark *rm1; // so that the crease runs through another point.

	RefLine_L2L_C2P(RefLine *arl1, RefMark *arm1);
};

class RefLine_L2L_C2P_Logic: public RefLineLogic {
  public:
	void MakeAll(rank_t arank) const override;
	rank_t GetRank(const RefBase *self) const override;
	bool UsesImmediate(const RefBase *self, RefBase *rb) const override;
	void SequencePushSelf(RefBase *self) const override;
	JsonObject Serialize(const RefBase *self) const override;
	void DrawSelf(const RefBase *self, RefStyle rstyle, short ipass) const override;
	RefBase *Import(BinaryInputStream &is) const override;
	void Export(const RefBase *self, BinaryOutputStream &os) const override;
};
