
#pragma once

#include "../refMark/refMark.h"
#include "refLine.h"
#include "json/jsonArray.h"

/**********
struct RefLine_L2L - Huzita-Hatori Axiom O3
Bring line l1 to line l2.
**********/
struct RefLine_L2L: RefLine {
	RefLine *rl1; // make a crease by bringing one line...
	RefLine *rl2; // to another line

	RefLine_L2L(RefLine *arl1, RefLine *arl2, unsigned char iroot);
};

class RefLine_L2L_Logic: public RefLineLogic {
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
		WHOMOVES_L1,
		WHOMOVES_L2
	};
};
