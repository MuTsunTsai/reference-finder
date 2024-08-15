
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

	type_t GetType() const override;

	const char GetLabel() const override;
	void PutName(char const *key, JsonObject &obj) const override;
	void DrawSelf(RefStyle rstyle, short ipass) const override;
	void Export(BinaryOutputStream &os) const override;

	static RefMark *Import(BinaryInputStream &is);

  protected:
	bool IsDerived() const override;
	void SetIndex() override;
};

#endif
