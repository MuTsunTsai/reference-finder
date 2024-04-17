/******************************************************************************
This module is not used in WASM build, but we keep it for reference.
******************************************************************************/

#ifndef _PS_STREAM_DGMR_H_
#define _PS_STREAM_DGMR_H_

#include "refDgmr.h"
#include "xyline.h"
#include "xyrect.h"
#include "refMark/refMark.h"
#include "refLine/refLine.h"

/**********
class PSStreamDgmr - a subclass of RefDgmr that writes a PostScript stream of
diagrams.
**********/
class PSStreamDgmr : public RefDgmr {
public:
  static double sPSUnit;          // size in points of a unit square
  static const XYRect sPSPageSize;// printable area on the page in PS units

  PSStreamDgmr(std::ostream& os);

  // Write to stream
  void PutMarkList(const XYPt& pp, std::vector<RefMark*>& vm);
  void PutLineList(const XYLine& ll, std::vector<RefLine*>& vl);

private:
  std::ostream* mStream;
  XYPt mPSOrigin;          // current loc of the origin in PS units
  int mPSPageCount;          // for page breaks
    
  class PSPt {
    public:
      double px;
      double py;
      PSPt(double x, double y) : px(x), py(y) {};
  };
  friend std::ostream& operator<<(std::ostream& os, const PSPt& pp);
  
  PSPt ToPS(const XYPt& pt);

  // Overridden functions from ancestor class RefDgmr
  void DrawPt(const XYPt& aPt, PointStyle pstyle);
  void DrawLine(const XYPt& fromPt, const XYPt& toPt, LineStyle lstyle);
  void DrawArc(const XYPt& ctr, double rad, double fromAngle,
    double toAngle, bool ccw, LineStyle lstyle);
  void DrawPoly(const std::vector<XYPt>& poly, PolyStyle pstyle);
  void DrawLabel(const XYPt& aPt, const std::string& aString, LabelStyle lstyle);
  
  // PSStreamDgmr - specific stuff
  void SetPointStyle(PointStyle pstyle);
  void SetLineStyle(LineStyle lstyle);
  void SetPolyStyle(PolyStyle pstyle);
  void SetLabelStyle(LabelStyle lstyle);

  void DecrementOrigin(double d);
  template <class R>
  void PutRefList(const typename R::bare_t& ar, std::vector<R*>& vr);
};

#endif