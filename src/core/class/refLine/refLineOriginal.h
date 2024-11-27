
#pragma once

#include "refLine.h"

/**********
class RefLine_Original - Specialization of RefLine that represents a line that
is the edge of the paper or an initial crease (like the diagonal).
**********/
class RefLine_Original : public RefLine {
  private:
	std::string mName; // name of the line
	rank_t mRank;	   // Rank of this line

  public:
	RefLine_Original(const XYLine &al, rank_t arank, std::string aName);

	type_t GetType() const override;
	rank_t GetRank() const override;

	bool IsActionLine() const override;
	const char GetLabel() const override;
	void PutName(char const *key, JsonObject &obj) const override;
	void DrawSelf(RefStyle rstyle, short ipass) const override;
	void Export(BinaryOutputStream &os) const override;

	static RefLine *Import(BinaryInputStream &is);

  protected:
	bool IsDerived() const override;
	void SetIndex() override;
};
