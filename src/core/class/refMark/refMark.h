
#ifndef _REF_MARK_H_
#define _REF_MARK_H_

#include "../refBase.h"
#include "math/xypt.h"
#include "json/jsonObject.h"

/**********
class RefMark - base class for a mark on the paper.
**********/
class RefMark : public RefBase {
  public:
	typedef XYPt bare_t; // type of bare object a RefMark represents
	bare_t p;			 // coordinates of the mark
  private:
	static index_t sCount; // class index, used for numbering sequences of marks
	static char sLabels[]; // labels for marks, indexed by sCount

  public:
	RefMark(type_t atype, rank_t arank) : RefBase(atype, arank) {}
	RefMark(type_t atype, const XYPt &ap, rank_t arank) : RefBase(atype, arank), p(ap) {}

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
	static rank_t CalcMarkRank(const RefBase *ar1, const RefBase *ar2) {
		return ar1->mRank + ar2->mRank;
	}
	void SetIndex() override;

  private:
	static void ResetCount();
	friend class RefBase;
};

#endif
