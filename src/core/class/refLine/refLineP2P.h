
#ifndef _REF_LINE_P2P_H_
#define _REF_LINE_P2P_H_

#include "../refMark/refMark.h"
#include "refLine.h"

/**********
class RefLine_P2P - Huzita-Hatori Axiom O2
Bring p1 to p2.
**********/
class RefLine_P2P : public RefLine {
public:
  RefMark* rm1;       // bring one mark...
  RefMark* rm2;       // to another mark, and form a crease.
  
private:
  enum WhoMoves {
    WHOMOVES_P1,
    WHOMOVES_P2
  };  
  WhoMoves mWhoMoves;

public:
  RefLine_P2P(RefMark* arm1, RefMark* arm2);
  
  bool UsesImmediate(RefBase* rb) const;
  void SequencePushSelf();
  bool PutHowto(std::ostream& os) const;
  void DrawSelf(RefStyle rstyle, short ipass) const;
  static void MakeAll(rank_t arank);
};

#endif