
#pragma once

#include "../refBase.h"
#include "math/xypt.h"
#include "json/jsonObject.h"

#include <array>

/**********
class RefMark - base class for a mark on the paper.
**********/
class RefMark: public RefBase {
  public:
	using bare_t = XYPt; // type of bare object a RefMark represents
	bare_t p;			 // coordinates of the mark

  private:
	static index_t sCount;					   // class index, used for numbering sequences of marks
	static std::array<const char, 11> sLabels; // labels for marks, indexed by sCount

  public:
	RefMark(): RefBase() {}
	RefMark(const XYPt &ap): RefBase(), p(ap) {}

	void FinishConstructor();

	double DistanceTo(const XYPt &ap) const;
	bool IsOnEdge() const;
	bool IsLine() const override;
	bool IsActionLine() const override;

	const char GetLabel() const override;
	void PutName(char const *key, JsonObject &obj) const override;
	void PutDistanceAndRank(JsonObject &solution, const XYPt &ap) const;
	void DrawSelf(RefStyle rstyle, short ipass) const override;

  protected:
	void SetIndex() override;

  private:
	static void ResetCount();
	friend class RefBase;
};
