
#ifndef _REF_LINE_L2L_P2L_H_
#define _REF_LINE_L2L_P2L_H_

#include "../refMark/refMark.h"
#include "refLine.h"

/**********
class RefLine_L2L_P2L - Huzita-Hatori Axiom O7 (Hatori's Axiom).
Bring line l1 to itself so that the point p1 goes on line l2.
**********/
class RefLine_L2L_P2L : public RefLine {
  public:
	RefLine *rl1; // bring line l1 onto itself
	RefMark *rm1; // so that point p1
	RefLine *rl2; // falls on line l2.
  private:
	enum WhoMoves {
		WHOMOVES_P1,
		WHOMOVES_L1
	};
	WhoMoves mWhoMoves;

  public:
	RefLine_L2L_P2L(RefLine *arl1, RefMark *arm1, RefLine *arl2);

	bool UsesImmediate(RefBase *rb) const;
	void SequencePushSelf();
	bool PutHowto(std::ostream &os) const;
	void DrawSelf(RefStyle rstyle, short ipass) const;
	static void MakeAll(rank_t arank);
};

#endif
