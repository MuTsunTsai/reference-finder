
#pragma once

#include "../refBase.h"
#include "math/xypt.h"
#include "json/jsonObject.h"

#include <array>

/**********
struct RefMark - base class for a mark on the paper.
**********/
struct RefMark: RefBase {
  public:
	using bare_t = XYPt; // type of bare object a RefMark represents
	bare_t p;			 // coordinates of the mark

  private:
	static RefBaseLogic::index_t sCount;					   // class index, used for numbering sequences of marks
	static std::array<const char, 11> sLabels; // labels for marks, indexed by sCount

  public:
	RefMark() = delete;
	RefMark(const RefType atype): RefBase(atype) {}
	RefMark(const XYPt &ap, const RefType atype): RefBase(atype), p(ap) {}

	void FinishConstructor();

	double DistanceTo(const XYPt &ap) const;
	double DistanceTo(const RefMark *ref) const;
	bool IsOnEdge() const;

	void PutDistanceAndRank(JsonObject &solution, const XYPt &ap) const;

  private:
	static void ResetCount();
	friend class RefBase;
	friend class RefMarkLogic;
};

class RefMarkLogic: public RefBaseLogic {
  public:
	size_t hash(const RefBase *self) const override;
	bool equals(const RefBase *self, const RefBase *other) const override;
	char GetLabel(const RefBase *self) const override;
	void PutName(const RefBase *self, char const *key, JsonObject &obj) const override;
	void DrawSelf(const RefBase *self, RefStyle rstyle, short ipass) const override;
	void SetIndex(const RefBase *self) const override;
};
