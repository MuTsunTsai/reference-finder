
#ifndef _REF_LINE_P2L_P2L_H_
#define _REF_LINE_P2L_P2L_H_

#include "../refMark/refMark.h"
#include "refLine.h"
#include "json/jsonArray.h"

/**********
class RefLine_P2L_P2L - Huzita-Hatori Axiom O6 (the cubic!)
Bring point p1 to line l1 and point p2 to line l2
**********/
class RefLine_P2L_P2L : public RefLine {
  public:
	RefMark *rm1; // bring a point...
	RefLine *rl1; // to a line...
	RefMark *rm2; // and another point...
	RefLine *rl2; // to another line.

  private:
	static short order;	   // the order of the equation
	static short irootMax; // maximum value of iroot, = ((# of roots) - 1)

	static double q1; // used for quadratic equation solutions
	static double q2;

	static double S; // used for cubic equation solutions
	static double Sr;
	static double Si;
	static double U;

	enum WhoMoves {
		WHOMOVES_P1P2,
		WHOMOVES_L1L2,
		WHOMOVES_P1L2,
		WHOMOVES_P2L1
	};
	WhoMoves mWhoMoves;
	unsigned char mRoot;

  public:
	RefLine_P2L_P2L(RefMark *arm1, RefLine *arl1, RefMark *arm2, RefLine *arl2, unsigned char iroot);

	bool UsesImmediate(RefBase *rb) const override;
	void SequencePushSelf() override;
	JsonObject Serialize() const override;
	void DrawSelf(RefStyle rstyle, short ipass) const override;
	void Export(BinaryOutputStream &os) const override;

	static RefLine *Import(BinaryInputStream &is);
	static void MakeAll(rank_t arank);

  private:
	static void MakeAllCore(rank_t irank, rank_t jrank, rank_t krank, rank_t lrank);
};

#endif
