
#include "../../ReferenceFinder.h"

#include "../global.h"
#include "../paper.h"
#include "../refDgmr.h"
#include "refMark.h"

using namespace std;

/**********
class RefMark - base class for a mark on the paper.
**********/

/*****
RefMark static member initialization
*****/
RefBase::index_t RefMark::sCount = 0; // Initialize class index

/*****
Calculate the key value used for distinguishing RefMarks. This should be called
at the end of every constructor if the mark is valid (and not if it isn't).
*****/
void RefMark::FinishConstructor() {
	const double fx = p.x / ReferenceFinder::sPaper.mWidth;	 // fx is between 0 and 1
	const double fy = p.y / ReferenceFinder::sPaper.mHeight; // fy is between 0 and 1

	key_t nx = static_cast<key_t>(floor(0.5 + fx * ReferenceFinder::sNumX));
	key_t ny = static_cast<key_t>(floor(0.5 + fy * ReferenceFinder::sNumY));
	mKey = 1 + nx * ReferenceFinder::sNumY + ny;
}

/*****
Return the distance to a point ap. This is used when sorting marks by their
distance from a given mark.
*****/
double RefMark::DistanceTo(const XYPt &ap) const {
	return (p - ap).Mag();
}

/*****
Return true if this mark is on the edge of the paper
*****/
bool RefMark::IsOnEdge() const {
	return (ReferenceFinder::sPaper.mLeftEdge.Intersects(p) ||
			ReferenceFinder::sPaper.mRightEdge.Intersects(p) ||
			ReferenceFinder::sPaper.mTopEdge.Intersects(p) ||
			ReferenceFinder::sPaper.mBottomEdge.Intersects(p));
}

/*****
Return false, since marks can never be actions
*****/
bool RefMark::IsActionLine() const {
	return false;
}

/*****
Return the label for this mark.
*****/
const char RefMark::GetLabel() const {
	return sLabels[mIndex - 1];
}

/*****
Put the name of this mark to a stream. Default behavior gives this mark a
letter. Return true if we used a letter, false if something else (i.e., the
name of a RefMark_Original).
*****/
bool RefMark::PutName(ostream &os) const {
	os << GetLabel()
#ifdef RF_PUT_KEY_IN_TEXT
	   << "[" << mKey << "]"
#endif // RF_PUT_KEY_IN_TEXT
		;
	return true;
}

/*****
Put the distance from this mark to point ap to a stream.
*****/
void RefMark::PutDistanceAndRank(ostream &os, const XYPt &ap) const {
	os.precision(4);
	os.setf(ios_base::fixed, ios_base::floatfield);
	os << "{\"solution\": " << p.Chop() << ", \"err\": " << DistanceTo(ap) << ", \"rank\": " << mRank << ", \"steps\": [";
}

/*****
Draw a RefMark in the indicated style
*****/
void RefMark::DrawSelf(RefStyle rstyle, short ipass) const {
	switch (ipass) {
	case PASS_POINTS: {
		switch (rstyle) {
		case REFSTYLE_NORMAL:
			sDgmr->DrawPt(p, RefDgmr::POINTSTYLE_NORMAL);
			break;
		case REFSTYLE_HILITE:
			sDgmr->DrawPt(p, RefDgmr::POINTSTYLE_HILITE);
			break;
		case REFSTYLE_ACTION:
			sDgmr->DrawPt(p, RefDgmr::POINTSTYLE_ACTION);
			break;
		}
	};
		break;

	case PASS_LABELS: {
		string sm(1, GetLabel());
		switch (rstyle) {
		case REFSTYLE_NORMAL:
			// Normal points don't get labels drawn
			break;
		case REFSTYLE_HILITE:
			sDgmr->DrawLabel(p, sm, RefDgmr::LABELSTYLE_HILITE);
			break;
		case REFSTYLE_ACTION:
			sDgmr->DrawLabel(p, sm, RefDgmr::LABELSTYLE_ACTION);
			break;
		};
		break;
	}
	}
}

/*****
Most types of RefMark use the default method, which gives the mark an index
from the class variable sCount and then bumps up the class variable.
*****/
void RefMark::SetIndex() {
	mIndex = ++sCount;
}

/*****
Reset the counter used for indexing marks in a sequence.
*****/
void RefMark::ResetCount() {
	sCount = 0;
}
