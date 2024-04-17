
#ifndef _REF_LINE_P2L_C2P_H_
#define _REF_LINE_P2L_C2P_H_

#include "../refMark/refMark.h"
#include "refLine.h"

/**********
class RefLine_P2L_C2P - Huzita-Hatori Axiom O5.
Bring point p1 to line l1 so that the crease passes through point p2.
**********/
class RefLine_P2L_C2P : public RefLine {
public:
  RefMark* rm1;       // bring a point...
  RefLine* rl1;       // to a line...
  RefMark* rm2;       // so that the crease runs through another point.
  
private:
  enum WhoMoves {
    WHOMOVES_P1,
    WHOMOVES_L1
  };  
  WhoMoves mWhoMoves;
  
public:
  RefLine_P2L_C2P(RefMark* arm1, RefLine* arl1, RefMark* arm2, short iroot);
  
  bool UsesImmediate(RefBase* rb) const;
  void SequencePushSelf();
  bool PutHowto(std::ostream& os) const;
  void DrawSelf(RefStyle rstyle, short ipass) const;
  static void MakeAll(rank_t arank);
};

#endif