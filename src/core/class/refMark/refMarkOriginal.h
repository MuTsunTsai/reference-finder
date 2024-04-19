
#ifndef _REF_MARK_ORIGINAL_H_
#define _REF_MARK_ORIGINAL_H_

#include "refMark.h"

#include <string>

/**********
class RefMark_Original - Specialization of RefMark that represents a named mark
(like a corner).
**********/
class RefMark_Original : public RefMark {
  private:
	std::string mName; // name of the mark

  public:
	RefMark_Original(const XYPt &ap, rank_t arank, std::string aName);

	const char GetLabel() const;
	bool PutName(std::ostream &os) const;
	void DrawSelf(RefStyle rstyle, short ipass) const;

  protected:
	virtual bool IsDerived() const;
	void SetIndex();
};

#endif
