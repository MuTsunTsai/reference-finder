
#include "refMark.h"
#include "../refDgmr.h"

#include "global/global.h"
#include "math/paper.h"

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
	const double fx = p.x / Shared::sPaper.mWidth;	// fx is between 0 and 1
	const double fy = p.y / Shared::sPaper.mHeight; // fy is between 0 and 1

	auto nx = static_cast<key_t>(floor(0.5 + fx * (Shared::sNumX - 1)));
	auto ny = static_cast<key_t>(floor(0.5 + fy * (Shared::sNumY - 1)));
	mKey = 1 + nx * Shared::sNumY + ny;
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
	return (Shared::sPaper.mLeftEdge.Intersects(p) || Shared::sPaper.mRightEdge.Intersects(p) || Shared::sPaper.mTopEdge.Intersects(p) || Shared::sPaper.mBottomEdge.Intersects(p));
}

bool RefMark::IsLine() const {
	return false;
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
char RefMark::GetLabel() const {
	index_t mIndex = sIndices[this];
	if(mIndex == 0) return ' ';
	return sLabels.at(mIndex - 1);
}

/*****
Put the name of this mark to a stream. Default behavior gives this mark a
letter. Return true if we used a letter, false if something else (i.e., the
name of a RefMark_Original).
*****/
void RefMark::PutName(char const *key, JsonObject &obj) const {
	obj.add(key, GetLabel());
}

/*****
Put the distance from this mark to point ap to a stream.
*****/
void RefMark::PutDistanceAndRank(JsonObject &solution, const XYPt &ap) const {
	solution.add("solution", p.Chop());
	solution.add("err", DistanceTo(ap));
	solution.add("rank", GetRank());
}

/*****
Draw a RefMark in the indicated style
*****/
void RefMark::DrawSelf(RefStyle rstyle, short ipass) const {
	switch(ipass) {
	case PASS_POINTS: {
		switch(rstyle) {
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
		switch(rstyle) {
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
	sIndices[this] = ++sCount;
}

/*****
Reset the counter used for indexing marks in a sequence.
*****/
void RefMark::ResetCount() {
	sCount = 0;
}
