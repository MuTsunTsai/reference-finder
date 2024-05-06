
#include "xyline.h"

using namespace std;

/**********
class XYLine - a class for representing a line by a scalar d and the unit
normal to the line, u, where the point d*u is the point on the line closest to
the origin. Note that (d,u) and (-d,-u) represent the same line. If u isn't a
unit vector, many things will break, so when in doubt, use Normalize() or
NormalizeSelf().
**********/

/*****
Stream I/O for a XYLine spits out the values of d and u as a
parenthesis-enclosed, comma-delimited pair, e.g., "(0,(1,0))".
*****/
ostream &operator<<(ostream &os, const XYLine &l) {
	return os << "[" << (l.d) << "," << (l.u) << "]";
}
