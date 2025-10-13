
#include "../refDgmr.h"
#include "ReferenceFinder.h"
#include "math/paper.h"

#include "refLineOriginal.h"

using namespace std;

/**********
class RefLine_Original - Specialization of RefLine that represents a line that
is the edge of the paper or an initial crease (like the diagonal).
**********/

/*****
Constructor.
*****/
RefLine_Original::RefLine_Original(const XYLine &al, rank_t arank, string aName)
	: RefLine(al, RefType::LINE_ORIGINAL), mRank(arank), mName(std::move(aName)) {
	FinishConstructor();
}

rank_t RefLine_OriginalLogic::GetRank(const RefBase *self) const {
	const auto *s = static_cast<const RefLine_Original *>(self);
	return s->mRank;
}

/*****
Return the label for this line.
*****/
char RefLine_OriginalLogic::GetLabel(const RefBase *self) const {
	return 0; // originals get no label
}

/*****
Put the name to a stream. Override the default method to use the actual string
name, rather than a letter. Return false since we didn't use a letter.
*****/

void RefLine_OriginalLogic::PutName(const RefBase *self, char const *key, JsonObject &obj) const {
	const auto *s = static_cast<const RefLine_Original *>(self);
	obj.add(key, s->mName);
}

/*****
Draw this line in the appropriate style
*****/
void RefLine_OriginalLogic::DrawSelf(const RefBase *self, RefStyle rstyle, short ipass) const {
	const auto *s = static_cast<const RefLine_Original *>(self);
	// RefLine_Originals don't get labels, and they are REFSTYLE_ACTION, we
	// still draw them highlighted.
	XYPt p1;
	XYPt p2;
	Shared::sPaper.ClipLine(s->l, p1, p2);
	switch(ipass) {
	case PASS_LINES:
		switch(rstyle) {
		case REFSTYLE_NORMAL:
			sDgmr->DrawLine(p1, p2, RefDgmr::LINESTYLE_CREASE);
			break;
		default:; // keep compiler happy
		}
		break;
	case PASS_HLINES:
		switch(rstyle) {
		case REFSTYLE_HILITE:
		case REFSTYLE_ACTION:
			sDgmr->DrawLine(p1, p2, RefDgmr::LINESTYLE_HILITE);
			break;
		default:; // keep compiler happy
		}
		break;
	default:; // keep compiler happy about unhandled cases
	}
}

RefBase *RefLine_OriginalLogic::Import(BinaryInputStream &is) const {
	double d;
	double x;
	double y;
	rank_t rank;
	string name;
	is.read(d).read(x).read(y).read(rank).read(name);
	XYPt p(x, y);
	XYLine l(d, p);
	return new RefLine_Original(l, rank, name);
}

void RefLine_OriginalLogic::Export(const RefBase *self, BinaryOutputStream &os) const {
	const auto *s = static_cast<const RefLine_Original *>(self);
	RefBaseLogic::Export(self, os);
	os << s->l << s->mRank << s->mName;
}

/*****
Return false, since this is original
*****/
bool RefLine_OriginalLogic::IsDerived(const RefBase *self) const {
	return false;
}

/*****
Set the index of this line. Override the default method to use an index of
zero, since we've already got a name.
*****/

void RefLine_OriginalLogic::SetIndex(const RefBase *self) const {}
