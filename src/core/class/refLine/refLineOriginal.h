
#pragma once

#include "refLine.h"

/**********
class RefLine_Original - Specialization of RefLine that represents a line that
is the edge of the paper or an initial crease (like the diagonal).
**********/
class RefLine_Original: public RefLine {
  private:
	std::string mName; // name of the line
	rank_t mRank;	   // Rank of this line

  public:
	RefLine_Original(const XYLine &al, rank_t arank, std::string aName);

	friend class RefLine_OriginalLogic;
};

class RefLine_OriginalLogic: public RefLineLogic {
  public:
	rank_t GetRank(const RefBase *self) const override;
	char GetLabel(const RefBase *self) const override;
	void PutName(const RefBase *self, char const *key, JsonObject &obj) const override;
	void DrawSelf(const RefBase *self, RefStyle rstyle, short ipass) const override;
	RefBase *Import(BinaryInputStream &is) const override;
	void Export(const RefBase *self, BinaryOutputStream &os) const override;
	bool IsDerived(const RefBase *self) const override;
	void SetIndex(const RefBase *self) const override;
};
