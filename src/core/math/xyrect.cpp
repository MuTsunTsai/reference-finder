
#include "xyrect.h"

using namespace std;

/**********
class XYRect - a class for representing rectangles by two points, the bottom
left and top right corners.
**********/

/*****
Return the aspect ratio of the rectangle, defined to be the smaller dimension
divided by the larger dimension. If it's empty, return zero. If the rectangles
is improperly defined, the aspect ratio may be negative.
*****/
double XYRect::GetAspectRatio() const {
	double wd = GetWidth();
	double ht = GetHeight();
	if(abs(wd) < EPS && abs(ht) < EPS) return 0;
	if(abs(wd) <= abs(ht)) return wd / ht;
	return ht / wd;
}

/*****
Stream I/O for a XYRect spits out the values of bl and tr as a
parenthesis-enclosed, comma-delimited pair, e.g., "((0,0),(1,1))".
*****/
ostream &operator<<(ostream &os, const XYRect &r) {
	return os << "(" << (r.bl) << "," << (r.tr) << ")";
}
