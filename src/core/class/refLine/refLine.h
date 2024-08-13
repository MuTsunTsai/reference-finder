
#ifndef _REF_LINE_H_
#define _REF_LINE_H_

#include "json/jsonObject.h"
#include "math/xyline.h"
#include "../refBase.h"

/**********
class RefLine - base class for a reference line.
**********/
class RefLine : public RefBase {
  public:
	typedef XYLine bare_t; // type of bare object that a RefLine represents
	bare_t l;			   // the line this contains
  private:
	static index_t sCount; // class index, used for numbering sequences of lines
	static char sLabels[]; // labels for lines, indexed by sCount

	static void moveCloser(XYPt &from, const XYPt &to, double dist);

  public:
	RefLine(type_t atype, rank_t arank) : RefBase(atype, arank) {}
	RefLine(type_t atype, const XYLine &al, rank_t arank) : RefBase(atype, arank), l(al) {}

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
	static rank_t CalcLineRank(const RefBase *ar1, const RefBase *ar2) {
		return 1 + ar1->mRank + ar2->mRank;
	}
	static rank_t CalcLineRank(const RefBase *ar1, const RefBase *ar2,
							   const RefBase *ar3) { return 1 + ar1->mRank + ar2->mRank + ar3->mRank; }
	static rank_t CalcLineRank(const RefBase *ar1, const RefBase *ar2,
							   const RefBase *ar3, const RefBase *ar4) {
		return 1 + ar1->mRank + ar2->mRank + ar3->mRank + ar4->mRank;
	}
	void SetIndex() override;

  private:
	static void ResetCount();
	friend class RefBase;
};

#endif
