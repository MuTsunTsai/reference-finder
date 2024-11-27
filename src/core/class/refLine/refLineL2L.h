
#pragma once

#include "../refMark/refMark.h"
#include "refLine.h"
#include "json/jsonArray.h"

/**********
class RefLine_L2L - Huzita-Hatori Axiom O3
Bring line l1 to line l2.
**********/
class RefLine_L2L : public RefLine {
  public:
	RefLine *rl1; // make a crease by bringing one line...
	RefLine *rl2; // to another line

  private:
	enum WhoMoves {
		WHOMOVES_L1,
		WHOMOVES_L2
	};
	WhoMoves mWhoMoves;
	unsigned char mRoot;

  public:
	RefLine_L2L(RefLine *arl1, RefLine *arl2, unsigned char iroot);

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
