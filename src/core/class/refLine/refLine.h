
#ifndef _REF_LINE_H_
#define _REF_LINE_H_

#include "../refBase.h"
#include "math/xyline.h"
#include "json/jsonObject.h"

/**********
class RefLine - base class for a reference line.
**********/
class RefLine : public RefBase {
  public:
	using bare_t = XYLine; // type of bare object that a RefLine represents
	bare_t l;			   // the line this contains

	/* A none-null value indicating that this line is used only for creating an intersection. */
	RefBase *mForMark;

  private:
	static index_t sCount; // class index, used for numbering sequences of lines
	static char sLabels[]; // labels for lines, indexed by sCount

	static void moveCloser(XYPt &from, const XYPt &to, double dist);

  public:
	RefLine() : RefBase(), mForMark(nullptr) {}
	RefLine(const XYLine &al) : RefBase(), l(al), mForMark(nullptr) {}

	void FinishConstructor();
	double DistanceTo(const XYLine &al) const;
	bool IsOnEdge() const;
	bool IsLine() const override;
	bool IsActionLine() const override;

	const char GetLabel() const override;
	void PutName(char const *key, JsonObject &obj) const override;
	void PutDistanceAndRank(JsonObject &solution, const XYLine &al) const;
	void DrawSelf(RefStyle rstyle, short ipass) const override;

  protected:
	void SetIndex() override;

  private:
	static void ResetCount();
	friend class RefBase;
};

#endif
