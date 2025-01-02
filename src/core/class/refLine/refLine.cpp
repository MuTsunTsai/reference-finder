
#include "refLine.h"
#include "../refDgmr.h"
#include "../refMark/refMark.h"
#include "global/global.h"
#include "math/paper.h"

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
	if(l.d < 0) {
		l.d = -l.d;
		l.u.x = -l.u.x;
		l.u.y = -l.u.y;
	};

	double fa = (1. + atan2(l.u.y, l.u.x) / (3.14159265358979323)) / 2.0; // fa is between 0 & 1
	const double dmax = sqrt(pow(Shared::sPaper.mWidth, 2) + pow(Shared::sPaper.mHeight, 2));
	const double fd = l.d / dmax; // fd is between 0 and 1

	auto nd = static_cast<key_t>(floor(0.5 + fd * (Shared::sNumD - 1)));
	if(nd == 0) fa = fmod(2 * fa, 1); // for d=0, we map alpha and pi+alpha to the same key
	auto na = static_cast<key_t>(floor(0.5 + fa * (Shared::sNumA - 1)));
	mKey = 1 + na * Shared::sNumD + nd;
}

/*****
Return the "distance" between two lines.
*****/
double RefLine::DistanceTo(const XYLine &al) const {
	if(Shared::sLineWorstCaseError) {
		// Use the worst-case separation between the endpoints of the two lines
		// where they leave the paper.
		XYPt p1a;
		XYPt p1b;
		XYPt p2a;
		XYPt p2b;
		if(Shared::sPaper.ClipLine(l, p1a, p1b) && Shared::sPaper.ClipLine(al, p2a, p2b)) {
			double err1 = max_val((p1a - p2a).Mag(), (p1b - p2b).Mag());
			double err2 = max_val((p1a - p2b).Mag(), (p1b - p2a).Mag());
			return min_val(err1, err2);
		}
		return 1 / EPS; // lines don't intersect the paper, return very large number

	}
	// Use the Pythagorean sum of the distance between the characteristic
	// vectors of the tangent point and angle.
	return sqrt(pow(l.u.Dot(al.u.Rotate90()), 2) + pow(l.d - al.d * l.u.Dot(al.u), 2));
}

double RefLine::DistanceTo(const RefLine *ref) const {
	return DistanceTo(ref->l);
}

/*****
Return true if this RefLine is on the edge of the paper
*****/
bool RefLine::IsOnEdge() const {
	return ((Shared::sPaper.mLeftEdge == l) || (Shared::sPaper.mTopEdge == l) || (Shared::sPaper.mRightEdge == l) || (Shared::sPaper.mBottomEdge == l));
}

bool RefLine::IsLine() const {
	return true;
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
char RefLine::GetLabel() const {
	index_t mIndex = sIndices[this];
	if(mIndex == 0) return ' ';
	return sLabels.at(mIndex - 1);
}

/*****
Put the name of this line to a stream. Default behavior gives this line a
letter. Return true if we used a letter. (We'll return false if we use
something else, i.e., a RefLine_Original).
*****/
void RefLine::PutName(char const *key, JsonObject &obj) const {
	obj.add(key, GetLabel());
}

/*****
Put the distance between this line and line al to a stream along with the rank
*****/
void RefLine::PutDistanceAndRank(JsonObject &solution, const XYLine &al) const {
	solution.add("solution", l);
	solution.add("err", DistanceTo(al));
	solution.add("rank", GetRank());
}

/*****
Draw a line in the given style.
*****/
void RefLine::DrawSelf(RefStyle rstyle, short ipass) const {
	XYPt op1;
	XYPt op2;
	Shared::sPaper.ClipLine(l, op1, op2);
	XYPt p1(op1);
	XYPt p2(op2);
	double pinchLength = 0;

	if(mForMark != nullptr) {
		pinchLength = std::min(Shared::sPaper.mWidth, Shared::sPaper.mHeight) / 10.0;
		auto *mark = (RefMark *)mForMark;
		moveCloser(p1, mark->p, pinchLength);
		moveCloser(p2, mark->p, pinchLength);
	}

	switch(ipass) {
	case PASS_LINES: {
		switch(rstyle) {
		case REFSTYLE_NORMAL:
			sDgmr->DrawLine(p1, p2, RefDgmr::LINESTYLE_CREASE);
			break;
		default:; // keep compiler happy
		}
	};
		break;

	case PASS_HLINES: // highlighted lines and action lines go on top of others
	{
		switch(rstyle) {
		case REFSTYLE_HILITE:
			sDgmr->DrawLine(p1, p2, RefDgmr::LINESTYLE_HILITE);
			break;
		case REFSTYLE_ACTION:
			if(mForMark != nullptr) {
				sDgmr->DrawLine(op1, p1, RefDgmr::LINESTYLE_DOTTED);
				sDgmr->DrawLine(op2, p2, RefDgmr::LINESTYLE_DOTTED);
			}
			sDgmr->DrawLine(p1, p2, mForMark == nullptr ? RefDgmr::LINESTYLE_VALLEY : RefDgmr::LINESTYLE_PINCH);
			break;
		default:; // keep compiler happy
		}
	};
		break;

	case PASS_LABELS: {
		XYPt mp = MidPoint(op1, op2); // label goes at the midpoint of the line

		// We add a tiny offset along the direction of the line,
		// to prevent the label from coinciding with other labels,
		// causing rendering error.
		mp += (op1 - op2) * 1e-10;

		string sl(1, GetLabel());
		switch(rstyle) {
		case REFSTYLE_NORMAL:
			// normal lines don't get labels
			break;
		case REFSTYLE_HILITE:
			if(mForMark != nullptr) {
				auto *mark = (RefMark *)mForMark;
				moveCloser(mp, mark->p, pinchLength * 0.75);
			}
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

void RefLine::moveCloser(XYPt &from, const XYPt &to, double dist) {
	XYPt diff(from - to);
	if(diff.Mag() < dist) return;
	diff.NormalizeSelf();
	from = to + diff * dist;
}

/*****
Most subclasses will use the default method, which sets the index from the
class variable sCount and then bumps up the count.
*****/

void RefLine::SetIndex() {
	sIndices[this] = ++sCount;
}

/*****
Reset the class variable sCount to zero.
*****/
void RefLine::ResetCount() {
	sCount = 0;
}
