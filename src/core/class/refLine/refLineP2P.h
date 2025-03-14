
#pragma once

#include "../refMark/refMark.h"
#include "refLine.h"
#include "json/jsonArray.h"

/**********
class RefLine_P2P - Huzita-Hatori Axiom O2
Bring p1 to p2.
**********/
class RefLine_P2P: public RefLine {
  public:
	RefMark *rm1; // bring one mark...
	RefMark *rm2; // to another mark, and form a crease.

  private:
	enum WhoMoves : std::uint8_t {
		WHOMOVES_P1,
		WHOMOVES_P2
	};

  public:
	RefLine_P2P(RefMark *arm1, RefMark *arm2);

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
