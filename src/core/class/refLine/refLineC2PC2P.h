
#pragma once

#include "../refMark/refMark.h"
#include "refLine.h"

/**********
class RefLine_C2P_C2P - Huzita-Hatori Axiom O1
Make a crease through two points p1 and p2.
**********/
class RefLine_C2P_C2P: public RefLine {
  public:
	RefMark *rm1; // make a crease from one mark...
	RefMark *rm2; // to another mark

	RefLine_C2P_C2P(RefMark *arm1, RefMark *arm2);

	type_t GetType() const override;
	rank_t GetRank() const override;

	bool UsesImmediate(RefBase *rb) const override;
	void SequencePushSelf() override;
	JsonObject Serialize() const override;
	void DrawSelf(RefStyle rstyle, short ipass) const override;
	void Export(BinaryOutputStream &os) const override;

	static RefLine *Import(BinaryInputStream &is);
	static void MakeAll(rank_t arank);
};
