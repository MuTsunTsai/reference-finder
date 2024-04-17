
#include "../refDgmr.h"
#include "refMarkOriginal.h"

using namespace std;

/**********
class RefMark_Original - Specialization of RefMark that represents a named mark
(e.g., a corner).
**********/

/*****
Constructor.
*****/
RefMark_Original::RefMark_Original(const XYPt& ap, rank_t arank, string aName) : 
  RefMark(ap, arank), mName(aName)
{
  FinishConstructor();
}


/*****
Return the label for this mark.
*****/
const char RefMark_Original::GetLabel() const
{
  return 0; // originals get no labels
}


/*****
Put the name of this mark to a stream. Override to put the string name (rather
than a letter).
*****/

bool RefMark_Original::PutName(ostream& os) const
{
  os << mName;  // return the string
  return false;
}


/*****
Draw this mark
*****/
void RefMark_Original::DrawSelf(RefStyle rstyle, short ipass) const
{
  // Override the default because original marks don't get labels and are only
  // drawn when they are hilited or action (in which case we still draw them
  // hilited).
  if ((ipass == PASS_POINTS) && 
    (rstyle == REFSTYLE_HILITE || rstyle == REFSTYLE_ACTION)) 
    sDgmr->DrawPt(p, RefDgmr::POINTSTYLE_HILITE);
} 


/*****
Return false, since this is original
*****/
bool RefMark_Original::IsDerived() const
{
  return false;
}


/*****
Overridden because we don't use an index for named marks.
*****/
void RefMark_Original::SetIndex()
{
  mIndex = 0;
};
