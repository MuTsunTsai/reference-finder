
#pragma once

#include "../refBase.h"
#include "math/xyline.h"
#include "json/jsonObject.h"

#include <array>

/**********
struct RefLine - base struct for a reference line.
**********/
struct RefLine: RefBase {
  public:
	using bare_t = XYLine; // type of bare object that a RefLine represents
	bare_t l;			   // the line this contains

  private:
	static RefBaseLogic::index_t sCount;	   // class index, used for numbering sequences of lines
	static std::array<const char, 10> sLabels; // labels for lines, indexed by sCount

  public:
	RefLine() = delete;
	RefLine(const RefType atype): RefBase(atype) {}
	RefLine(const XYLine &al, const RefType atype): RefBase(atype), l(al) {}

	void FinishConstructor();
	double DistanceTo(const XYLine &al) const;
	double DistanceTo(const RefLine *ref) const;
	bool IsOnEdge() const;
	void PutDistanceAndRank(JsonObject &solution, const XYLine &al) const;

  private:
	static void ResetCount();
	friend struct RefBase;
	friend class RefLineLogic;
};

class RefLineLogic: public RefBaseLogic {
  public:
	size_t hash(const RefBase *self) const override;
	bool equals(const RefBase *self, const RefBase *other) const override;
	char GetLabel(const RefBase *self) const override;
	void PutName(const RefBase *self, char const *key, JsonObject &obj) const override;
	void DrawSelf(const RefBase *self, RefStyle rstyle, short ipass) const override;
	void SetIndex(const RefBase *self) const override;

  private:
	static void moveCloser(XYPt &from, const XYPt &to, double dist);
};
