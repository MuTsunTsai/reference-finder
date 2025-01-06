
#pragma once

#include "../refBase.h"
#include "math/xyline.h"
#include "json/jsonObject.h"

#include <array>

/**********
class RefLine - base class for a reference line.
**********/
class RefLine: public RefBase {
  public:
	using bare_t = XYLine; // type of bare object that a RefLine represents
	bare_t l;			   // the line this contains

	/* A none-null value indicating that this line is used only for creating an intersection. */
	RefBase *mForMark;

	size_t hash() const override;
	bool operator==(const RefLine &other) const;

  private:
	static index_t sCount;					   // class index, used for numbering sequences of lines
	static std::array<const char, 10> sLabels; // labels for lines, indexed by sCount

	static void moveCloser(XYPt &from, const XYPt &to, double dist);

  public:
	RefLine(): mForMark(nullptr) {}
	RefLine(const XYLine &al): l(al), mForMark(nullptr) {}

	void FinishConstructor();
	double DistanceTo(const XYLine &al) const;
	double DistanceTo(const RefLine *ref) const;
	bool IsOnEdge() const;
	bool IsLine() const override;
	bool IsActionLine() const override;

	char GetLabel() const override;
	void PutName(char const *key, JsonObject &obj) const override;
	void PutDistanceAndRank(JsonObject &solution, const XYLine &al) const;
	void DrawSelf(RefStyle rstyle, short ipass) const override;

  protected:
	void SetIndex() override;

  private:
	static void ResetCount();
	friend class RefBase;
};
