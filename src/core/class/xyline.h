
#ifndef _XYLINE_H_
#define _XYLINE_H_

#include "xypt.h"

/**********
class XYLine - a class for representing a line by a scalar and the normal to
the line.
**********/
class XYLine {
public:
  double d; // d*u is the point on the line closest to the origin
  XYPt u;   // a unit normal vector to the line
  
  // Constructors
  XYLine(double dd = 0, const XYPt& uu = XYPt(1, 0)) : d(dd), u(uu) {}
  
  XYLine(const XYPt& p1, const XYPt& p2) {
    // line through two points
    u = (p2 - p1).Normalize().Rotate90();
    d = p1.Dot(u);
  }
  
  XYPt Fold(const XYPt& p1) const {
    // Fold a point about the line
    return p1 + 2 * (d - (p1.Dot(u))) * u;
  }
  
  bool IsParallelTo(const XYLine& ll) const {
    // true if lines are parallel
    return std::abs(u.Dot(ll.u.Rotate90())) < EPS;
  }
  
  bool operator==(const XYLine& ll) const {
    // true if lines are same
    return (std::abs(d - ll.d * u.Dot(ll.u)) < EPS) && 
      (std::abs(u.Dot(ll.u.Rotate90())) < EPS);
    }
    
  bool Intersects(const XYPt& pp) const {
    // true if pt on line
    return (std::abs(d - pp.Dot(u)) < EPS);
  }
    
  bool Intersects(const XYLine& ll, XYPt& pp) const {
    // true if lines intersect, intersection goes in pp
    double denom = u.x * ll.u.y - u.y * ll.u.x;
    if (std::abs(denom) < EPS) return false;
    pp.x = (d * ll.u.y - ll.d * u.y) / denom;
    pp.y = (ll.d * u.x - d * ll.u.x) / denom;
    return true;
  }
    
  // Intersection() just returns the intersection point, no error checking
  // for parallel-ness. Use Intersects() when in doubt.
  friend const XYPt Intersection(const XYLine& l1, const XYLine& l2) {
    double denom = l1.u.x * l2.u.y - l1.u.y * l2.u.x;
    return XYPt((l1.d * l2.u.y - l2.d * l1.u.y) / denom, 
      (l2.d * l1.u.x - l1.d * l2.u.x) / denom);
  }

  // Stream I/O
  friend std::ostream& operator<<(std::ostream& os, const XYLine& l);
};    

#endif