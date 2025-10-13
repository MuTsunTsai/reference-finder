
#pragma once

#include "../refMark/refMark.h"
#include "refLine.h"

/**********
struct RefLine_C2P_C2P - Huzita-Hatori Axiom O1
Make a crease through two points p1 and p2.
**********/
struct RefLine_C2P_C2P: RefLine {
	RefMark *rm1; // make a crease from one mark...
	RefMark *rm2; // to another mark

	RefLine_C2P_C2P(RefMark *arm1, RefMark *arm2);
};

class RefLine_C2P_C2P_Logic: public RefLineLogic {
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
