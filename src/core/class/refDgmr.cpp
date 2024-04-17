
#include "../ReferenceFinder.h"

#include "global.h"
#include "paper.h"
#include "refDgmr.h"

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
offset and scale appropriately to convert to canvas coordinates. See
PSStreamDgmr for an example.
*/  

/*****
Draw a point in the given style.
*****/
void RefDgmr::DrawPt(const XYPt& /* aPt */, PointStyle /* pstyle */)
{
}


/*****
Draw a line in the given style.
*****/
void RefDgmr::DrawLine(const XYPt& /* fromPt */, const XYPt& /* toPt */, 
  LineStyle /* lstyle */)
{
}


/*****
Draw an arc in the given style. fromAngle and toAngle are given in radians.
*****/
void RefDgmr::DrawArc(const XYPt& /* ctr */, double /* rad */, 
  double /* fromAngle */, double /* toAngle */, bool /* ccw */, 
  LineStyle /* lstyle */)
{
}


/*****
Draw a polygon in the given style.
*****/
void RefDgmr::DrawPoly(const vector<XYPt>& /* poly */, PolyStyle /* pstyle */)
{
}


/*****
Draw a label at a given point
*****/
void RefDgmr::DrawLabel(const XYPt& /* aPt */, const string& /* aString */, 
  LabelStyle /* lstyle */)
{
}


/*
Class RefDgmr provides a set of routines for drawing arrows that are built on
top of the primitive routines above. These can be overridden if you want to
implement a different style of arrow.
*/

/*****
Draw a valley-fold arrowhead with tip at location loc, direction dir, and size
len.
*****/
void RefDgmr::DrawValleyArrowhead(const XYPt& loc, const XYPt& dir, double len)
{
  DrawLine(loc, loc - len * dir.RotateCCW(.523), LINESTYLE_ARROW);
  DrawLine(loc, loc - len * dir.RotateCCW(-.523), LINESTYLE_ARROW);
}


/*****
Draw a mountain arrowhead with tip at location loc, direction dir, and size len.
*****/
void RefDgmr::DrawMountainArrowhead(const XYPt& loc, const XYPt& dir, double len)
{
  XYPt ldir = len * dir;
  vector<XYPt> poly;
  poly.push_back(loc);
  poly.push_back(loc - ldir.RotateCCW(.523));
  poly.push_back(loc - .8 * ldir);
  DrawPoly(poly, POLYSTYLE_ARROW);
}


/*****
Draw an unfold arrowhead with tip at location loc, direction dir, and size len.
*****/
void RefDgmr::DrawUnfoldArrowhead(const XYPt& loc, const XYPt& dir, double len)
{
  XYPt ldir = len * dir;
  vector<XYPt> poly;
  poly.push_back(loc);
  poly.push_back(loc - ldir.RotateCCW(.523));
  poly.push_back(loc - .8 * ldir);
  poly.push_back(loc - ldir.RotateCCW(-.523));
  DrawPoly(poly, POLYSTYLE_ARROW);
}


/*****
Calculate all the parameters necessary to draw any type of arrow (valley,
mountain, unfold, fold-and-unfold).
*****/
void RefDgmr::CalcArrow(const XYPt& fromPt, const XYPt& toPt,
  XYPt& ctr, double& rad, double& fromAngle, double& toAngle, bool& ccw,
  double& ahSize, XYPt& fromDir, XYPt& toDir)
{
  const double RADIANS = 57.29577951;
  const double TWO_PI = 6.283185308;
  const double PI = 3.1415926535;
  
  const double ha = 30 / RADIANS;     // half-angle of arc of arrow, in degrees
  const double tana = tan(ha);      // tan of this angle
  
  XYPt mp = MidPoint(fromPt, toPt);   // midpoint of arrow line
  XYPt mu = (toPt - fromPt);        // vector in direction of arrow line
  XYPt mup = 0.5 * mu.Rotate90() / tana;  // vector from midpt to center of curvature
  
  // Compute the center of rotation. There are two possible choices.
  // We'll want the bulge of the arc to always be toward the inside of the square,
  // i.e., closer to the middle of the square, so we pick the value of the center
  // that's farther away.
  XYPt sqmp = MidPoint(ReferenceFinder::sPaper.mBotLeft, 
    ReferenceFinder::sPaper.mTopRight);
  XYPt ctr1 = mp + mup;
  XYPt ctr2 = mp - mup;
  ctr = (ctr1 - sqmp).Mag() > (ctr2 - sqmp).Mag() ? ctr1 : ctr2;

  // radius of the arc.
  rad = (toPt - ctr).Mag();
  
  // Now compute the angles of the lines to the two points.
  XYPt fp = fromPt - ctr;
  fromAngle = atan2(fp.y, fp.x);
  XYPt tp = toPt - ctr;
  toAngle = atan2(tp.y, tp.x);
  
  // Check direction of rotation.
  double ra = toAngle - fromAngle;  // rotation angle
  while (ra < 0) ra += TWO_PI;    // get it into the right range
  while (ra > TWO_PI) ra -= TWO_PI; 
  ccw = (ra < PI);          // true == arc goes in ccw direction
  
  // Compute the size of the arrowheads
  ahSize = ReferenceFinder::sPaper.mWidth;
  if (ahSize > ReferenceFinder::sPaper.mHeight) {
    ahSize = ReferenceFinder::sPaper.mHeight;
  }
  ahSize *= 0.15;
  double ah1 = 0.4 * (toPt - fromPt).Mag();
  if (ahSize > ah1) ahSize = ah1;
  
  // Compute the direction vectors for the arrowheads
  mu.NormalizeSelf();
  toDir = ccw ? mu.RotateCCW(ha) : mu.RotateCCW(-ha);
  mu *= -1;
  fromDir = ccw ? mu.RotateCCW(-ha) : mu.RotateCCW(ha);
}


/*****
Draw a valley-fold arrow. fromPt is the moving point, toPt is the destination.
*****/
void RefDgmr::DrawValleyArrow(const XYPt& fromPt, const XYPt& toPt)
{
  XYPt ctr;
  double rad;
  double fromAngle;
  double toAngle;
  bool ccw;
  double ahSize;
  XYPt fromDir;
  XYPt toDir;
  CalcArrow(fromPt, toPt, ctr, rad, fromAngle, toAngle, ccw, ahSize, fromDir, 
    toDir);
  DrawArc(ctr, rad, fromAngle, toAngle, ccw, LINESTYLE_ARROW);
  DrawValleyArrowhead(toPt, toDir, ahSize);
}


/*****
Draw a mountain-fold arrow. fromPt is the moving point, toPt is the
destination.
*****/
void RefDgmr::DrawMountainArrow(const XYPt& fromPt, const XYPt& toPt)
{
  XYPt ctr;
  double rad;
  double fromAngle;
  double toAngle;
  bool ccw;
  double ahSize;
  XYPt fromDir;
  XYPt toDir;
  CalcArrow(fromPt, toPt, ctr, rad, fromAngle, toAngle, ccw, ahSize, fromDir, 
    toDir);
  DrawArc(ctr, rad, fromAngle, toAngle, ccw, LINESTYLE_ARROW);
  DrawMountainArrowhead(toPt, toDir, ahSize);
}


/*****
Draw an unfold arrow. fromPt is the moving point, toPt is the destination.
*****/
void RefDgmr::DrawUnfoldArrow(const XYPt& fromPt, const XYPt& toPt)
{
  XYPt ctr;
  double rad;
  double fromAngle;
  double toAngle;
  bool ccw;
  double ahSize;
  XYPt fromDir;
  XYPt toDir;
  CalcArrow(fromPt, toPt, ctr, rad, fromAngle, toAngle, ccw, ahSize, fromDir, 
    toDir);
  DrawArc(ctr, rad, fromAngle, toAngle, ccw, LINESTYLE_ARROW);
  DrawUnfoldArrowhead(toPt, toDir, ahSize);
}


/*****
Draw a fold-and-unfold arrow. fromPt is the moving point, toPt is the
destination.
*****/
void RefDgmr::DrawFoldAndUnfoldArrow(const XYPt& fromPt, const XYPt& toPt)
{
  XYPt ctr;
  double rad;
  double fromAngle;
  double toAngle;
  bool ccw;
  double ahSize;
  XYPt fromDir;
  XYPt toDir;
  CalcArrow(fromPt, toPt, ctr, rad, fromAngle, toAngle, ccw, ahSize, fromDir, 
    toDir);
  DrawArc(ctr, rad, fromAngle, toAngle, ccw, LINESTYLE_ARROW);
  DrawValleyArrowhead(toPt, toDir, ahSize);
  DrawUnfoldArrowhead(fromPt, fromDir, ahSize);
}
