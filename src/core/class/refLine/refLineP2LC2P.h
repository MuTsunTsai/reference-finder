
#pragma once

#include "../refMark/refMark.h"
#include "refLine.h"
#include "json/jsonArray.h"

/**********
class RefLine_P2L_C2P - Huzita-Hatori Axiom O5.
Bring point p1 to line l1 so that the crease passes through point p2.
**********/
class RefLine_P2L_C2P: public RefLine {
  public:
	RefMark *rm1; // bring a point...
	RefLine *rl1; // to a line...
	RefMark *rm2; // so that the crease runs through another point.

	RefLine_P2L_C2P(RefMark *arm1, RefLine *arl1, RefMark *arm2, unsigned char iroot);
};

class RefLine_P2L_C2P_Logic: public RefLineLogic {
  public:
	void MakeAll(rank_t arank) const override;
	rank_t GetRank(const RefBase *self) const override;
	bool UsesImmediate(const RefBase *self, RefBase *rb) const override;
	void SequencePushSelf(RefBase *self) const override;
	JsonObject Serialize(const RefBase *self) const override;
	void DrawSelf(const RefBase *self, RefStyle rstyle, short ipass) const override;
	RefBase *Import(BinaryInputStream &is) const override;
	void Export(const RefBase *self, BinaryOutputStream &os) const override;

	enum WhoMoves : std::uint8_t {
		WHOMOVES_P1,
		WHOMOVES_L1
	};
};
