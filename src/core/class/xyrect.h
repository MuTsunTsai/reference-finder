
#ifndef _XYRECT_H_
#define _XYRECT_H_

#include "xypt.h"

/**********
class XYRect - a class for representing rectangles by two points, the bottom
left and top right corners.
**********/
class XYRect {
  public:
	XYPt bl; // bottom left corner
	XYPt tr; // top right corner

	// Constructors
	XYRect(const XYPt &ap) : bl(ap), tr(ap) {}
	XYRect(const XYPt &abl, const XYPt &atr) : bl(abl), tr(atr) {}
	XYRect(double ablx, double ably, double atrx, double atry) : bl(ablx, ably), tr(atrx, atry) {}

	// Dimensional queries
	double GetWidth() const { return tr.x - bl.x; }
	double GetHeight() const { return tr.y - bl.y; }
	double GetAspectRatio() const;

	// Boolean queries
	bool IsValid() const {
		// Return true if bl is below and to the left of tr
		return (bl.x <= tr.x) && (bl.y <= tr.y);
	}
	bool IsEmpty() const {
		// Return true if the rectangle is a line or point
		return (std::abs(bl.x - tr.x) < EPS) || (std::abs(bl.y - tr.y) < EPS);
	}
	bool Encloses(const XYPt &ap) const {
		// Return true if pt falls within this rectangle, padded by EPS
		return (ap.x >= bl.x - EPS) && (ap.x <= tr.x + EPS) &&
			   (ap.y >= bl.y - EPS) && (ap.y <= tr.y + EPS);
	}
	bool Encloses(const XYPt &ap1, XYPt &ap2) const {
		// Return true if both pts fall within the rectangle.
		return Encloses(ap1) && Encloses(ap2);
	}

	// Include(p) stretches the coordinates so that this rect encloses the point.
	// Returns a reference so multiple calls can be chained.
	XYRect &Include(const XYPt &p) {
		if (bl.x > p.x) bl.x = p.x;
		if (bl.y > p.y) bl.y = p.y;
		if (tr.x < p.x) tr.x = p.x;
		if (tr.y < p.y) tr.y = p.y;
		return *this;
	}

	// Stream I/O
	friend std::ostream &operator<<(std::ostream &os, const XYRect &r);
};

/*****
Return an XYRect that encloses all of the points passed as parameters.
*****/
inline const XYRect GetBoundingBox(const XYPt &p1, const XYPt &p2) {
	return XYRect(p1).Include(p2);
}
inline const XYRect GetBoundingBox(const XYPt &p1, const XYPt &p2,
								   const XYPt &p3) {
	return XYRect(p1).Include(p2).Include(p3);
}

#endif
