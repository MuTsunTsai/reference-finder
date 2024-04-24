
#include "../../ReferenceFinder.h"
#include "../paper.h"
#include "../refDgmr.h"

#include "refLineOriginal.h"

using namespace std;

/**********
class RefLine_Original - Specialization of RefLine that represents a line that
is the edge of the paper or an initial crease (like the diagonal).
**********/

/*****
Constructor.
*****/
RefLine_Original::RefLine_Original(const XYLine &al, rank_t arank, string aName) : RefLine(al, arank), mName(aName) {
	FinishConstructor();
}

/*****
Return false because RefLine_Originals aren't actions, they're present from the
beginning.
*****/
bool RefLine_Original::IsActionLine() const {
	return false;
}

/*****
Return the label for this line.
*****/
const char RefLine_Original::GetLabel() const {
	return 0; // originals get no label
}

/*****
Put the name to a stream. Override the default method to use the actual string
name, rather than a letter. Return false since we didn't use a letter.
*****/

bool RefLine_Original::PutName(ostream &os) const {
	os << mName;
	return false;
}

/*****
Draw this line in the appropriate style
*****/
void RefLine_Original::DrawSelf(RefStyle rstyle, short ipass) const {
	// RefLine_Originals don't get labels, and they are REFSTYLE_ACTION, we
	// still draw them highlighted.
	XYPt p1, p2;
	ReferenceFinder::sPaper.ClipLine(l, p1, p2);
	switch (ipass) {
	case PASS_LINES:
		switch (rstyle) {
		case REFSTYLE_NORMAL:
			sDgmr->DrawLine(p1, p2, RefDgmr::LINESTYLE_CREASE);
			break;
		default:; // keep compiler happy
		}
		break;
	case PASS_HLINES:
		switch (rstyle) {
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

/*****
Return false, since this is original
*****/
bool RefLine_Original::IsDerived() const {
	return false;
}

/*****
Set the index of this line. Override the default method to use an index of
zero, since we've already got a name.
*****/

void RefLine_Original::SetIndex() {
	mIndex = 0;
}
