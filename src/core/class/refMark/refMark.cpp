
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
step_t RefMark::sCount = 0; // Initialize class index

/*****
Calculate the key value used for distinguishing RefMarks. This should be called
at the end of every constructor if the mark is valid (and not if it isn't).
*****/
void RefMark::FinishConstructor() {
	if(!Shared::use_division) {
		mKey = 1;
		return;
	}

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

double RefMark::DistanceTo(const RefMark *ref) const {
	return DistanceTo(ref->p);
}

/*****
Return true if this mark is on the edge of the paper
*****/
bool RefMark::IsOnEdge() const {
	return (Shared::sPaper.mLeftEdge.Intersects(p) || Shared::sPaper.mRightEdge.Intersects(p) || Shared::sPaper.mTopEdge.Intersects(p) || Shared::sPaper.mBottomEdge.Intersects(p));
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
Reset the counter used for indexing marks in a sequence.
*****/
void RefMark::ResetCount() {
	sCount = 0;
}

size_t RefMarkLogic::hash(const RefBase *self) const {
	const auto *s = static_cast<const RefMark *>(self);
	return std::hash<double>()(s->p.x) ^ (std::hash<double>()(s->p.y) << 1);
}

bool RefMarkLogic::equals(const RefBase *self, const RefBase *other) const {
	const auto *s = static_cast<const RefMark *>(self);
	const auto *o = static_cast<const RefMark *>(other);
	return s->p.x == o->p.x && s->p.y == o->p.y;
}

/*****
Return the label for this mark.
*****/
char RefMarkLogic::GetLabel(const RefBase *self) const {
	const auto *s = static_cast<const RefMark *>(self);
	step_t mIndex = sIndices[self];
	if(mIndex == 0) return ' ';
	return RefMark::sLabels.at(mIndex - 1);
}

/*****
Put the name of this mark to a stream. Default behavior gives this mark a
letter. Return true if we used a letter, false if something else (i.e., the
name of a RefMark_Original).
*****/
void RefMarkLogic::PutName(const RefBase *self, char const *key, JsonObject &obj) const {
	obj.add(key, GetLabel(self));
}

/*****
Draw a RefMark in the indicated style
*****/
void RefMarkLogic::DrawSelf(const RefBase *self, RefStyle rstyle, short ipass) const {
	const auto *s = static_cast<const RefMark *>(self);
	switch(ipass) {
	case PASS_POINTS: {
		switch(rstyle) {
		case REFSTYLE_NORMAL:
			sDgmr->DrawPt(s->p, RefDgmr::POINTSTYLE_NORMAL);
			break;
		case REFSTYLE_HILITE:
			sDgmr->DrawPt(s->p, RefDgmr::POINTSTYLE_HILITE);
			break;
		case REFSTYLE_ACTION:
			sDgmr->DrawPt(s->p, RefDgmr::POINTSTYLE_ACTION);
			break;
		}
	};
		break;

	case PASS_LABELS: {
		string sm(1, GetLabel(self));
		switch(rstyle) {
		case REFSTYLE_NORMAL:
			// Normal points don't get labels drawn
			break;
		case REFSTYLE_HILITE:
			sDgmr->DrawLabel(s->p, sm, RefDgmr::LABELSTYLE_HILITE);
			break;
		case REFSTYLE_ACTION:
			sDgmr->DrawLabel(s->p, sm, RefDgmr::LABELSTYLE_ACTION);
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
void RefMarkLogic::SetIndex(const RefBase *self) const {
	sIndices[self] = ++RefMark::sCount;
}
