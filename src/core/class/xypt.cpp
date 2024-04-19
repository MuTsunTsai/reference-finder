
#include "xypt.h"

using namespace std;

/**********
class XYPt - a 2-vector that represents a point or 2-vector (direction vector).
It is a lightweight mathematical object. Most operations are defined in the
header, intended to be inlined for speed.
**********/

/*****
Stream I/O for a XYPt spits out the point as a parenthesis-enclosed,
comma-delimited pair.
*****/
ostream &operator<<(ostream &os, const XYPt &p) {
	return os << "(" << (p.x) << ", " << (p.y) << ")";
}
