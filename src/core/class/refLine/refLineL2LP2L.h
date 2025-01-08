
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
  private:
	enum WhoMoves : std::uint8_t {
		WHOMOVES_P1,
		WHOMOVES_L1
	};

  public:
	RefLine_L2L_P2L(RefLine *arl1, RefMark *arm1, RefLine *arl2);

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
