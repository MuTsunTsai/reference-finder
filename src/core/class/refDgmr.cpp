
#include "refDgmr.h"
#include "global/global.h"
#include "math/paper.h"

using namespace std;

/**********
class RefDgmr - object that draws folding diagrams of references. Subclasses
specialize to particular drawing environments (print vs screen, multiple GUIs,
platform-specific drawing models, etc.
**********/

/*  Notes on RefDgmr and its descendants.
Subclasses will typically maintain an internal graphics state that describes
the location of the diagram on the canvas and will implement the handful of
virtual drawing methods that render the diagram on the canvas. The base class
does nothing. All points are given in the paper coordinate system. The subclass
implementation of drawing should determine the layout of the diagrams and
offset and scale appropriately to convert to canvas coordinates.
*/


/*****
Calculate all the parameters necessary to draw arrow.
*****/
void RefDgmr::CalcArrow(const XYPt &fromPt, const XYPt &toPt, const XYPt *around, XYPt &ctr, double &rad, double &fromAngle, double &toAngle, bool &ccw, double &ahSize, XYPt &fromDir, XYPt &toDir) {
	const double RADIANS = 57.29577951;
	const double TWO_PI = 6.283185308;
	const double PI = 3.1415926535;

	const double ha = 30 / RADIANS; // half-angle of arc of arrow, in degrees
	const double tana = tan(ha);	// tan of this angle

	XYPt mp = MidPoint(fromPt, toPt);	   // midpoint of arrow line
	XYPt mu = (toPt - fromPt);			   // vector in direction of arrow line
	XYPt mup = 0.5 * mu.Rotate90() / tana; // vector from midpt to center of curvature

	// Compute the center of rotation. There are two possible choices.
	// We'll want the bulge of the arc to always be toward the inside of the square,
	// i.e., closer to the middle of the square (or away from the point around, if specified),
	// so we pick the value of the center that's farther away.
	XYPt target = around == nullptr ? MidPoint(Shared::sPaper.mBotLeft, Shared::sPaper.mTopRight) : mp * 2 - *around;
	XYPt ctr1 = mp + mup;
	XYPt ctr2 = mp - mup;
	ctr = (ctr1 - target).Mag() > (ctr2 - target).Mag() ? ctr1 : ctr2;

	// radius of the arc.
	rad = (toPt - ctr).Mag();

	// Now compute the angles of the lines to the two points.
	XYPt fp = fromPt - ctr;
	fromAngle = atan2(fp.y, fp.x);
	XYPt tp = toPt - ctr;
	toAngle = atan2(tp.y, tp.x);

	// Check direction of rotation.
	double ra = toAngle - fromAngle; // rotation angle
	while(ra < 0) ra += TWO_PI;		 // get it into the right range
	while(ra > TWO_PI) ra -= TWO_PI;
	ccw = (ra < PI); // true == arc goes in ccw direction

	// Compute the size of the arrowheads
	ahSize = Shared::sPaper.mWidth;
	if(ahSize > Shared::sPaper.mHeight) {
		ahSize = Shared::sPaper.mHeight;
	}
	ahSize *= 0.15;
	double ah1 = 0.4 * (toPt - fromPt).Mag();
	if(ahSize > ah1) ahSize = ah1;

	// Compute the direction vectors for the arrowheads
	mu.NormalizeSelf();
	toDir = ccw ? mu.RotateCCW(ha) : mu.RotateCCW(-ha);
	mu *= -1;
	fromDir = ccw ? mu.RotateCCW(-ha) : mu.RotateCCW(ha);
}

/*****
Draw a arrow. fromPt is the moving point, toPt is the destination.
*****/
void RefDgmr::DrawArrow(const XYPt &fromPt, const XYPt &toPt, const XYPt *around) {
	XYPt ctr;
	double rad;
	double fromAngle;
	double toAngle;
	bool ccw;
	double ahSize;
	XYPt fromDir;
	XYPt toDir;
	CalcArrow(fromPt, toPt, around, ctr, rad, fromAngle, toAngle, ccw, ahSize, fromDir, toDir);
	DrawArc(ctr, rad, fromAngle, toAngle, ccw, LINESTYLE_ARROW);
}

void RefDgmr::DrawArrow(const XYPt &fromPt, const XYPt &toPt) {
	DrawArrow(fromPt, toPt, nullptr);
}
