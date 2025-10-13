
#pragma once

#include "../refMark/refMark.h"
#include "refLine.h"
#include "json/jsonArray.h"

/**********
struct RefLine_P2L_P2L - Huzita-Hatori Axiom O6 (the cubic!)
Bring point p1 to line l1 and point p2 to line l2
**********/
struct RefLine_P2L_P2L: RefLine {
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

  public:
	RefLine_P2L_P2L(RefMark *arm1, RefLine *arl1, RefMark *arm2, RefLine *arl2, unsigned char iroot);
};

class RefLine_P2L_P2L_Logic: public RefLineLogic {
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
		WHOMOVES_P1P2,
		WHOMOVES_L1L2,
		WHOMOVES_P1L2,
		WHOMOVES_P2L1
	};

  private:
	static void MakeAllCore(rank_t irank, rank_t jrank, rank_t krank, rank_t lrank);
};
