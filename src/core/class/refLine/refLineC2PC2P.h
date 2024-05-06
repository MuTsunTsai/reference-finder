
#ifndef _REF_LINE_C2P_C2P_H_
#define _REF_LINE_C2P_C2P_H_

#include "../refMark/refMark.h"
#include "refLine.h"

/**********
class RefLine_C2P_C2P - Huzita-Hatori Axiom O1
Make a crease through two points p1 and p2.
**********/
class RefLine_C2P_C2P : public RefLine {
  public:
	RefMark *rm1; // make a crease from one mark...
	RefMark *rm2; // to another mark

	RefLine_C2P_C2P(RefMark *arm1, RefMark *arm2);

	bool UsesImmediate(RefBase *rb) const;
	void SequencePushSelf();
	void PutHowto(JsonArray &steps) const;
	void DrawSelf(RefStyle rstyle, short ipass) const;
	static void MakeAll(rank_t arank);
};

#endif
