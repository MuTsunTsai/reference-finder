
#include "../../ReferenceFinder.h"

#include "../global.h"
#include "../paper.h"
#include "../refDgmr.h"
#include "refLine.h"

using namespace std;

/**********
class RefLine - base class for a reference line.
**********/

/*****
RefLine static member initialization
*****/
RefBase::index_t RefLine::sCount = 0; // Initialize class index

/*****
Calculate the key values used for sorting RefLines. Like its RefMark
counterpart, this should be called at the end of every successfully constructed
object, and it sets mKey to the proper value.
*****/
void RefLine::FinishConstructor() {
	// resolve the ambiguity in line orientation by requiring d>=0.
	if (l.d < 0) {
		l.d = -l.d;
		l.u.x = -l.u.x;
		l.u.y = -l.u.y;
	};

	double fa = (1. + atan2(l.u.y, l.u.x) / (3.14159265358979323)) / 2.0; // fa is between 0 & 1
	const double dmax = sqrt(pow(ReferenceFinder::sPaper.mWidth, 2) +
							 pow(ReferenceFinder::sPaper.mHeight, 2));
	const double fd = l.d / dmax; // fd is between 0 and 1

	key_t nd = static_cast<key_t>(floor(0.5 + fd * ReferenceFinder::sNumD));
	if (nd == 0) fa = fmod(2 * fa, 1); // for d=0, we map alpha and pi+alpha to the same key
	key_t na = static_cast<key_t>(floor(0.5 + fa * ReferenceFinder::sNumA));
	mKey = 1 + na * ReferenceFinder::sNumD + nd;
}

/*****
Return the "distance" between two lines.
*****/
double RefLine::DistanceTo(const XYLine &al) const {
	if (ReferenceFinder::sLineWorstCaseError) {
		// Use the worst-case separation between the endpoints of the two lines
		// where they leave the paper.
		XYPt p1a, p1b, p2a, p2b;
		if (ReferenceFinder::sPaper.ClipLine(l, p1a, p1b) &&
			ReferenceFinder::sPaper.ClipLine(al, p2a, p2b)) {
			double err1 = max_val((p1a - p2a).Mag(), (p1b - p2b).Mag());
			double err2 = max_val((p1a - p2b).Mag(), (p1b - p2a).Mag());
			return min_val(err1, err2);
		} else {
			return 1 / EPS; // lines don't intersect the paper, return very large number
		}
	} else {
		// Use the Pythagorean sum of the distance between the characteristic
		// vectors of the tangent point and angle.
		return sqrt(pow(l.u.Dot(al.u.Rotate90()), 2) +
					pow(l.d - al.d * l.u.Dot(al.u), 2));
	}
}

/*****
Return true if this RefLine is on the edge of the paper
*****/
bool RefLine::IsOnEdge() const {
	return ((ReferenceFinder::sPaper.mLeftEdge == l) ||
			(ReferenceFinder::sPaper.mTopEdge == l) ||
			(ReferenceFinder::sPaper.mRightEdge == l) ||
			(ReferenceFinder::sPaper.mBottomEdge == l));
}

/*****
Return true, since MOST Reflines are actions
*****/
bool RefLine::IsActionLine() const {
	return true;
}

/*****
Return the label for this line.
*****/
const char RefLine::GetLabel() const {
	return sLabels[mIndex - 1];
}

/*****
Put the name of this line to a stream. Default behavior gives this line a
letter. Return true if we used a letter. (We'll return false if we use
something else, i.e., a RefLine_Original).
*****/
bool RefLine::PutName(ostream &os) const {
	os << GetLabel()
#ifdef RF_PUT_KEY_IN_TEXT
	   << "[" << mKey << "]"
#endif // RF_PUT_KEY_IN_TEXT
		;
	return true;
}

/*****
Put the distance between this line and line al to a stream along with the rank
*****/
void RefLine::PutDistanceAndRank(ostream &os, const XYLine &al) const {
	os.precision(4);
	os.setf(ios_base::fixed, ios_base::floatfield);
	os << "{\"solution\": \"" << l << "\", \"err\": " << DistanceTo(al) << ", \"rank\": " << mRank << ", \"steps\": [";
}

/*****
Draw a line in the given style.
*****/
void RefLine::DrawSelf(RefStyle rstyle, short ipass) const {
	XYPt p1, p2;
	ReferenceFinder::sPaper.ClipLine(l, p1, p2);

	switch (ipass) {
	case PASS_LINES: {
		switch (rstyle) {
		case REFSTYLE_NORMAL:
			sDgmr->DrawLine(p1, p2, RefDgmr::LINESTYLE_CREASE);
			break;
		default:; // keep compiler happy
		}
	};
		break;

	case PASS_HLINES: // hilited lines and action lines go on top of others
	{
		switch (rstyle) {
		case REFSTYLE_HILITE:
			sDgmr->DrawLine(p1, p2, RefDgmr::LINESTYLE_HILITE);
			break;
		case REFSTYLE_ACTION:
			sDgmr->DrawLine(p1, p2, RefDgmr::LINESTYLE_VALLEY);
			break;
		default:; // keep compiler happy
		}
	};
		break;

	case PASS_LABELS: {
		XYPt mp = MidPoint(p1, p2); // label goes at the midpoint of the line
		string sl(1, GetLabel());
		switch (rstyle) {
		case REFSTYLE_NORMAL:
			// normal lines don't get labels
			break;
		case REFSTYLE_HILITE:
			sDgmr->DrawLabel(mp, sl, RefDgmr::LABELSTYLE_HILITE);
			break;
		case REFSTYLE_ACTION:
			sDgmr->DrawLabel(mp, sl, RefDgmr::LABELSTYLE_ACTION);
			break;
		default:; // keep compiler happy
		}
	};
		break;
	default:; // keep compiler happy
	}
	// Subclasses will add arrows for REFSTYLE_ACTION
}

/*****
Most subclasses will use the default method, which sets the index from the
class variable sCount and then bumps up the count.
*****/

void RefLine::SetIndex() {
	mIndex = ++sCount;
}

/*****
Reset the class variable sCount to zero.
*****/
void RefLine::ResetCount() {
	sCount = 0;
}
