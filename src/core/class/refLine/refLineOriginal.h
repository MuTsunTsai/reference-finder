
#ifndef _REF_LINE_ORIGINAL_H_
#define _REF_LINE_ORIGINAL_H_

#include "refLine.h"

/**********
class RefLine_Original - Specialization of RefLine that represents a line that
is the edge of the paper or an initial crease (like the diagonal).
**********/
class RefLine_Original : public RefLine {
private:
  std::string mName;  // name of the line
  
public:
  RefLine_Original(const XYLine& al, rank_t arank, std::string aName);

  bool IsActionLine() const;
  const char GetLabel() const;
  bool PutName(std::ostream& os) const;
  void DrawSelf(RefStyle rstyle, short ipass) const;

protected:
  virtual bool IsDerived() const;
  void SetIndex();
};

#endif