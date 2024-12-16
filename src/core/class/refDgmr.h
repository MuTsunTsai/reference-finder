
#pragma once

#include <vector>

#include "math/xypt.h"

/**********
class RefDgmr - object that draws folding diagrams of references. Subclasses
specialize to particular drawing environments (print vs screen, multiple GUIs,
platform-specific drawing models, etc.
**********/
class RefDgmr {
  public:
	RefDgmr() = default;
	virtual ~RefDgmr() = default;

	// Subclasses must override these methods to implement
	enum PointStyle {
		POINTSTYLE_NORMAL,
		POINTSTYLE_HILITE,
		POINTSTYLE_ACTION
	};
	/*****
	Draw a point in the given style.
	*****/
	virtual void DrawPt(const XYPt &aPt, PointStyle pstyle) = 0;

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
	/*****
	Draw a line in the given style.
	*****/
	virtual void DrawLine(const XYPt &fromPt, const XYPt &toPt, LineStyle lstyle) = 0;

	/*****
	Draw an arc in the given style. fromAngle and toAngle are given in radians.
	*****/
	virtual void DrawArc(const XYPt &ctr, double rad, double fromAngle, double toAngle, bool ccw, LineStyle lstyle) = 0;

	enum PolyStyle {
		POLYSTYLE_WHITE,
		POLYSTYLE_COLORED,
		POLYSTYLE_ARROW
	};
	/*****
	Draw a polygon in the given style.
	*****/
	virtual void DrawPoly(const std::vector<XYPt> &poly, PolyStyle pstyle) = 0;

	enum LabelStyle {
		LABELSTYLE_NORMAL,
		LABELSTYLE_HILITE,
		LABELSTYLE_ACTION
	};
	/*****
	Draw a label at a given point
	*****/
	virtual void DrawLabel(const XYPt &aPt, const std::string &aString, LabelStyle lstyle) = 0;

	void DrawArrow(const XYPt &fromPt, const XYPt &toPt, const XYPt *around);
	void DrawArrow(const XYPt &fromPt, const XYPt &toPt);

  private:
	void CalcArrow(const XYPt &fromPt, const XYPt &toPt, const XYPt *around, XYPt &ctr, double &rad, double &fromAngle, double &toAngle, bool &ccw, double &ahSize, XYPt &fromDir, XYPt &toDir);
};
