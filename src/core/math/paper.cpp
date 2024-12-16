
#include "paper.h"

#include <array>

/**********
class Paper - specialization of XYRect, used for representing the paper
**********/

/*****
Constructor
*****/
Paper::Paper(double aWidth, double aHeight): XYRect(aWidth, aHeight) {
	SetSize(aWidth, aHeight);
}

/*****
Set the size of the paper and all of the incidental data
*****/
void Paper::SetSize(double aWidth, double aHeight) {
	bl = XYPt(0, 0);
	tr = XYPt(aWidth, aHeight);
	mWidth = aWidth;
	mHeight = aHeight;
	mBotLeft = XYPt(0, 0);
	mBotRight = XYPt(aWidth, 0);
	mTopLeft = XYPt(0, aHeight);
	mTopRight = XYPt(aWidth, aHeight);
	mTopEdge = XYLine(mTopLeft, mTopRight);
	mLeftEdge = XYLine(mBotLeft, mTopLeft);
	mRightEdge = XYLine(mBotRight, mTopRight);
	mBottomEdge = XYLine(mBotLeft, mBotRight);
	mUpwardDiagonal = XYLine(mBotLeft, mTopRight);
	mDownwardDiagonal = XYLine(mTopLeft, mBotRight);
}

/*****
Clip the line al to the paper, returning the endpoints of the clipped segment
in ap1 and ap2. If the line misses the paper entirely, return false and leave
ap1 and ap2 unchanged.
*****/
bool Paper::ClipLine(const XYLine &al, XYPt &ap1, XYPt &ap2) const {
	// Start by collecting all points of intersection between the line and the
	// four sides of the paper.

	unsigned npts = 0;		  // counter for number of points of intersection
	std::array<XYPt, 4> ipts; // list of points of intersection
	XYPt p;					  // a scratch pad for intersection points

	if(mTopEdge.Intersects(al, p) && Encloses(p)) ipts[npts++] = p;
	if(mLeftEdge.Intersects(al, p) && Encloses(p)) ipts[npts++] = p;
	if(mRightEdge.Intersects(al, p) && Encloses(p)) ipts[npts++] = p;
	if(mBottomEdge.Intersects(al, p) && Encloses(p)) ipts[npts++] = p;

	if(npts == 0) return false; // line entirely misses the paper

	// Now parameterize all four points along the line, recording the minimum
	// and maximum parameter values.

	XYPt pt = al.d * al.u;	   // a point on the line
	XYPt up = al.u.Rotate90(); // a tangent to the line
	double tmin = (ipts[0] - pt).Dot(up);
	double tmax = tmin;
	for(unsigned i = 1; i < npts; i++) {
		double tt = (ipts[i] - pt).Dot(up);
		if(tmin < tt) tmin = tt;
		if(tmax > tt) tmax = tt;
	};

	// Compute the endpoints from our parameter ranges.

	ap1 = pt + tmin * up;
	ap2 = pt + tmax * up;
	return true;
}

/*****
Return true if line al overlaps the paper in its interior. Return false if the
line misses the paper entirely, only hits a corner, or only runs along an edge.
*****/
bool Paper::InteriorOverlaps(const XYLine &al) const {
	XYPt pa;
	XYPt pb;								// endpoints of the fold line
	if(!ClipLine(al, pa, pb)) return false; // the line completely misses the paper

	if((pa - pb).Mag() < EPS) return false; // line hits at a single point (a corner)

	if(!GetBoundingBox(pa, pb).IsEmpty()) return true; // bounding box has positive volume

	// If still here, then the bounding box must be a line, either the edge of the paper
	// or a vertical or horizontal line in the interior. We can test for the latter by
	// seeing if the midpoint of the line lies fully in the interior of the paper.

	XYPt mp = MidPoint(pa, pb);

	return !(mTopEdge.Intersects(mp) || mBottomEdge.Intersects(mp) || mLeftEdge.Intersects(mp) || mRightEdge.Intersects(mp));
}

/*****
The line al divides the paper into two portions. Return true if either of the
two qualifies as a skinny flap. "Skinny" means a triangle (or quad) whose
aspect ratio falls below a minimum size.
*****/
bool Paper::MakesSkinnyFlap(const XYLine &al) const {
	// Since "true" = "bad", we'll return true for any failures along the way due to bad
	// input parameters.

	XYPt p1;
	XYPt p2;			  // endpoints of the line al
	ClipLine(al, p1, p2); // get the endpoints of the fold line on the paper

	XYLine lb; // perpendicular bisector of line segment p1-p2
	lb.u = al.u.Rotate90();
	lb.d = MidPoint(p1, p2).Dot(lb.u);
	XYPt bp1;
	XYPt bp2;				// endpoints of the bisector
	ClipLine(lb, bp1, bp2); // get the endpoints of the bisector

	// Get the bounding box that contains the fold line and a point on either side of the
	// fold line. If this bounding box is below the minimum aspect ratio, then it contains
	// a flap that falls below the minimum aspect ratio, so we return true.

	if(abs(GetBoundingBox(p1, p2, bp1).GetAspectRatio()) < Shared::sMinAspectRatio)
		return true;
	if(abs(GetBoundingBox(p1, p2, bp2).GetAspectRatio()) < Shared::sMinAspectRatio)
		return true;

	// If we're still here, we didn't create any skinny flaps, so we're cool.

	return false;
}
