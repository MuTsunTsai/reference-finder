
#ifndef _REF_MARK_INTERSECTION_H_
#define _REF_MARK_INTERSECTION_H_

#include "refMark.h"

class RefLine; // forward declaration needed by RefMark_Intersection

/**********
class RefMark_Intersection - Specialization of a RefMark for a mark defined by
the intersection of 2 lines.
**********/
class RefMark_Intersection : public RefMark {
  public:
	RefLine *rl1; // first line
	RefLine *rl2; // second line

	RefMark_Intersection(RefLine *al1, RefLine *al2);

	bool UsesImmediate(RefBase *rb) const;
	void SequencePushSelf();
	bool PutHowto(std::ostream &os) const;
	static void MakeAll(rank_t arank);
};

#endif
