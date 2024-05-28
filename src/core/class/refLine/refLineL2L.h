
#ifndef _REF_LINE_L2L_H_
#define _REF_LINE_L2L_H_

#include "../refMark/refMark.h"
#include "../json/jsonArray.h"
#include "refLine.h"

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

  public:
	RefLine_L2L(RefLine *arl1, RefLine *arl2, short iroot);

	bool UsesImmediate(RefBase *rb) const;
	void SequencePushSelf();
	JsonObject Serialize() const;
	void DrawSelf(RefStyle rstyle, short ipass) const;
	static void MakeAll(rank_t arank);
};

#endif
