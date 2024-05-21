
#ifndef _REF_DGMR_H_
#define _REF_DGMR_H_

#include <vector>

#include "xypt.h"

/**********
class RefDgmr - object that draws folding diagrams of references. Subclasses
specialize to particular drawing environments (print vs screen, multiple GUIs,
platform-specific drawing models, etc.
**********/
class RefDgmr {
  public:
	RefDgmr(){};
	virtual ~RefDgmr(){};

	// Subclasses must override these methods to implement
	enum PointStyle {
		POINTSTYLE_NORMAL,
		POINTSTYLE_HILITE,
		POINTSTYLE_ACTION
	};
	virtual void DrawPt(const XYPt &aPt, PointStyle pstyle);

	enum LineStyle {
		LINESTYLE_CREASE,
		LINESTYLE_EDGE,
		LINESTYLE_HILITE,
		LINESTYLE_VALLEY,
		LINESTYLE_MOUNTAIN, // not in use
		LINESTYLE_ARROW,
		LINESTYLE_DOTTED,
		LINESTYLE_PINCH,
	};
	virtual void DrawLine(const XYPt &fromPt, const XYPt &toPt, LineStyle lstyle);
	virtual void DrawArc(const XYPt &ctr, double rad, double fromAngle,
						 double toAngle, bool ccw, LineStyle lstyle);

	enum PolyStyle {
		POLYSTYLE_WHITE,
		POLYSTYLE_COLORED,
		POLYSTYLE_ARROW
	};
	virtual void DrawPoly(const std::vector<XYPt> &poly, PolyStyle pstyle);

	enum LabelStyle {
		LABELSTYLE_NORMAL,
		LABELSTYLE_HILITE,
		LABELSTYLE_ACTION
	};
	virtual void DrawLabel(const XYPt &aPt, const std::string &aString,
						   LabelStyle lstyle);

	void DrawArrow(const XYPt &fromPt, const XYPt &toPt, const XYPt *around);
	void DrawArrow(const XYPt &fromPt, const XYPt &toPt);

  private:
	void CalcArrow(const XYPt &fromPt, const XYPt &toPt, const XYPt *around,
				   XYPt &ctr, double &rad, double &fromAngle, double &toAngle, bool &ccw,
				   double &ahSize, XYPt &fromDir, XYPt &toDir);
};

#endif
