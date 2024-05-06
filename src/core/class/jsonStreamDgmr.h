
#ifndef _VERBAL_STREAM_DGMR_H_
#define _VERBAL_STREAM_DGMR_H_

#include "refDgmr.h"
#include "refLine/refLine.h"
#include "refMark/refMark.h"
#include "xyline.h"
#include "xyrect.h"
#include "json/jsonArray.h"
#include "json/jsonObject.h"

/**********
class JsonStreamDgmr is a variation of the original VerbalStreamDgmr.
**********/
class JsonStreamDgmr : public RefDgmr {
  public:
	JsonStreamDgmr(std::ostream &aStream);

	void PutMarkList(const XYPt &pp, std::vector<RefMark *> &vm);
	void PutLineList(const XYLine &ll, std::vector<RefLine *> &vl);

  private:
	std::ostream *mStream;
	JsonArray *mArray;
	template <class R>
	void PutRefList(const typename R::bare_t &ar, std::vector<R *> &vr);

	// Overridden functions from ancestor class RefDgmr
	void DrawPt(const XYPt &aPt, PointStyle pstyle);
	void DrawLine(const XYPt &fromPt, const XYPt &toPt, LineStyle lstyle);
	void DrawArc(const XYPt &ctr, double rad, double fromAngle,
				 double toAngle, bool ccw, LineStyle lstyle);
	void DrawPoly(const std::vector<XYPt> &poly, PolyStyle pstyle);
	void DrawLabel(const XYPt &aPt, const std::string &aString, LabelStyle lstyle);
};

#endif
