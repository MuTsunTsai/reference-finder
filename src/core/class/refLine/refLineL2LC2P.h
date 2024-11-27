
#pragma once

#include "../refMark/refMark.h"
#include "refLine.h"
#include "json/jsonArray.h"

/**********
class RefLine_L2L_C2P - Huzita-Hatori Axiom O4.
Bring line l1 to itself so that the crease passes through point p1.
**********/
class RefLine_L2L_C2P : public RefLine {
  public:
	RefLine *rl1; // bring line l1 to itself
	RefMark *rm1; // so that the crease runs through another point.

	RefLine_L2L_C2P(RefLine *arl1, RefMark *arm1);

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
