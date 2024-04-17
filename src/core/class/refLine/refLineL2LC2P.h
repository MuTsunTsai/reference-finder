
#ifndef _REF_LINE_L2L_C2P_H_
#define _REF_LINE_L2L_C2P_H_

#include "../refMark/refMark.h"
#include "refLine.h"

/**********
class RefLine_L2L_C2P - Huzita-Hatori Axiom O4.
Bring line l1 to itself so that the crease passes through point p1.
**********/
class RefLine_L2L_C2P : public RefLine {
public:
  RefLine* rl1;       // bring line l1 to itself
  RefMark* rm1;       // so that the crease runs through another point.
  
  RefLine_L2L_C2P(RefLine* arl1, RefMark* arm1);
  
  bool UsesImmediate(RefBase* rb) const;
  void SequencePushSelf();
  bool PutHowto(std::ostream& os) const;
  void DrawSelf(RefStyle rstyle, short ipass) const;
  static void MakeAll(rank_t arank);
};


#endif