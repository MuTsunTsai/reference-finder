
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
	: RefMark(ap, RefType::MARK_ORIGINAL), mRank(arank), mName(std::move(aName)) {
	FinishConstructor();
}

rank_t RefMark_OriginalLogic::GetRank(const RefBase* self) const {
	const auto *s = static_cast<const RefMark_Original *>(self);
	return s->mRank;
}

/*****
Return the label for this mark.
*****/
char RefMark_OriginalLogic::GetLabel(const RefBase* self) const {
	return 0; // originals get no labels
}

/*****
Put the name of this mark to a stream. Override to put the string name (rather
than a letter).
*****/

void RefMark_OriginalLogic::PutName(const RefBase* self, char const *key, JsonObject &obj) const {
	const auto *s = static_cast<const RefMark_Original *>(self);
	obj.add(key, s->mName);
}

/*****
Draw this mark
*****/
void RefMark_OriginalLogic::DrawSelf(const RefBase* self, RefStyle rstyle, short ipass) const {
	const auto *s = static_cast<const RefMark_Original *>(self);
	// Override the default because original marks don't get labels and are only
	// drawn when they are highlighted or action (in which case we still draw them
	// highlighted).
	if((ipass == PASS_POINTS) &&
	   (rstyle == REFSTYLE_HILITE || rstyle == REFSTYLE_ACTION))
		sDgmr->DrawPt(s->p, RefDgmr::POINTSTYLE_HILITE);
}

/*****
Return false, since this is original
*****/
bool RefMark_OriginalLogic::IsDerived(const RefBase* self) const {
	return false;
}

/*****
Overridden because we don't use an index for named marks.
*****/
void RefMark_OriginalLogic::SetIndex(const RefBase* self) const {}

RefBase *RefMark_OriginalLogic::Import(BinaryInputStream &is) const {
	double x;
	double y;
	rank_t rank;
	string name;
	is.read(x).read(y).read(rank).read(name);
	XYPt p(x, y);
	return new RefMark_Original(p, rank, name);
}

void RefMark_OriginalLogic::Export(const RefBase* self, BinaryOutputStream &os) const {
	const auto *s = static_cast<const RefMark_Original *>(self);
	RefBaseLogic::Export(self, os);
	os << s->p << s->mRank << s->mName;
}
