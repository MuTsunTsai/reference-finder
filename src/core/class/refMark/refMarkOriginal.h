
#pragma once

#include "refMark.h"

#include <string>

/**********
struct RefMark_Original - Specialization of RefMark that represents a named mark
(like a corner).
**********/
struct RefMark_Original: RefMark {
  private:
	std::string mName; // name of the mark
	rank_t mRank;	   // Rank of this mark

  public:
	RefMark_Original(const XYPt &ap, rank_t arank, std::string aName);

	friend class RefMark_OriginalLogic;
};

class RefMark_OriginalLogic: public RefMarkLogic {
	rank_t GetRank(const RefBase* self) const override;
	char GetLabel(const RefBase* self) const override;
	void PutName(const RefBase* self, char const *key, JsonObject &obj) const override;
	void DrawSelf(const RefBase* self, RefStyle rstyle, short ipass) const override;
	RefBase *Import(BinaryInputStream &is) const override;
	void Export(const RefBase* self, BinaryOutputStream &os) const override;
	bool IsDerived(const RefBase* self) const override;
	void SetIndex(const RefBase* self) const override;
};
