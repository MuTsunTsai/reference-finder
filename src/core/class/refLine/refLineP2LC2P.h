
#pragma once

#include "../refMark/refMark.h"
#include "refLine.h"
#include "json/jsonArray.h"

/**********
class RefLine_P2L_C2P - Huzita-Hatori Axiom O5.
Bring point p1 to line l1 so that the crease passes through point p2.
**********/
class RefLine_P2L_C2P : public RefLine {
  public:
	RefMark *rm1; // bring a point...
	RefLine *rl1; // to a line...
	RefMark *rm2; // so that the crease runs through another point.

  private:
	enum WhoMoves {
		WHOMOVES_P1,
		WHOMOVES_L1
	};
	WhoMoves mWhoMoves;
	unsigned char mRoot;

  public:
	RefLine_P2L_C2P(RefMark *arm1, RefLine *arl1, RefMark *arm2, unsigned char iroot);

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
