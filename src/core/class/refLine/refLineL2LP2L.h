
#pragma once

#include "../refMark/refMark.h"
#include "refLine.h"
#include "json/jsonArray.h"

/**********
class RefLine_L2L_P2L - Huzita-Hatori Axiom O7 (Hatori's Axiom).
Bring line l1 to itself so that the point p1 goes on line l2.
**********/
class RefLine_L2L_P2L: public RefLine {
  public:
	RefLine *rl1; // bring line l1 onto itself
	RefMark *rm1; // so that point p1
	RefLine *rl2; // falls on line l2.

	RefLine_L2L_P2L(RefLine *arl1, RefMark *arm1, RefLine *arl2);
};

class RefLine_L2L_P2L_Logic: public RefLineLogic {
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
