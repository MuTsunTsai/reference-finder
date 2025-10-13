
#pragma once

#include "../refMark/refMark.h"
#include "refLine.h"
#include "json/jsonArray.h"

/**********
struct RefLine_P2P - Huzita-Hatori Axiom O2
Bring p1 to p2.
**********/
struct RefLine_P2P: RefLine {
	RefMark *rm1; // bring one mark...
	RefMark *rm2; // to another mark, and form a crease.

	RefLine_P2P(RefMark *arm1, RefMark *arm2);
};

class RefLine_P2P_Logic: public RefLineLogic {
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
		WHOMOVES_P2
	};
};
