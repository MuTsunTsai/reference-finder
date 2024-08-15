
#include "refMarkOriginal.h"
#include "../refDgmr.h"

using namespace std;

/**********
class RefMark_Original - Specialization of RefMark that represents a named mark
(e.g., a corner).
**********/

/*****
Constructor.
*****/
RefMark_Original::RefMark_Original(const XYPt &ap, rank_t arank, string aName)
	: RefMark(ap), mRank(arank), mName(aName) {
	FinishConstructor();
}

RefBase::type_t RefMark_Original::GetType() const {
	return RefType::MARK_ORIGINAL;
}

rank_t RefMark_Original::GetRank() const {
	return mRank;
}

/*****
Return the label for this mark.
*****/
const char RefMark_Original::GetLabel() const {
	return 0; // originals get no labels
}

/*****
Put the name of this mark to a stream. Override to put the string name (rather
than a letter).
*****/

void RefMark_Original::PutName(char const *key, JsonObject &obj) const {
	obj.add(key, mName);
}

/*****
Draw this mark
*****/
void RefMark_Original::DrawSelf(RefStyle rstyle, short ipass) const {
	// Override the default because original marks don't get labels and are only
	// drawn when they are highlighted or action (in which case we still draw them
	// highlighted).
	if ((ipass == PASS_POINTS) &&
		(rstyle == REFSTYLE_HILITE || rstyle == REFSTYLE_ACTION))
		sDgmr->DrawPt(p, RefDgmr::POINTSTYLE_HILITE);
}

/*****
Return false, since this is original
*****/
bool RefMark_Original::IsDerived() const {
	return false;
}

/*****
Overridden because we don't use an index for named marks.
*****/
void RefMark_Original::SetIndex() {}

void RefMark_Original::Export(BinaryOutputStream &os) const {
	RefBase::Export(os);
	os << p << mRank << mName;
}

RefMark *RefMark_Original::Import(BinaryInputStream &is) {
	double x, y;
	rank_t rank;
	string name;
	is.read(x).read(y).read(rank).read(name);
	XYPt p(x, y);
	return new RefMark_Original(p, rank, name);
}
