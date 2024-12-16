
#include "refBase.h"
#include "global/global.h"
#include "math/paper.h"
#include "math/xypt.h"
#include "refDgmr.h"
#include "refLine/refLine.h"
#include "refMark/refMark.h"
#include "refMark/refMarkIntersection.h"
#include "json/jsonArray.h"

#include <algorithm>

using namespace std;

/**********
class RefBase - base class for a mark or line.
**********/

/*
Non-static member variables are:
mRank - the rank, which is the number of creases that need to be made to define
  it.
mKey - a unique key that is used to efficiently store and search over marks and
  lines. mKey is initialized to 0. If the object has been successfully
  constructed, it will be set to an integer greater than 0, so mKey==0 is used
  as a test for successful construction.
mIndex - a counter used in constructing the verbal sequence; basically, it's
  the order in which the object is created for a given folding sequence.
Any subclasses of RefBase should be fairly lightweight because we'll be
creating a couple hundred thousand of them during program initialization.
*/

/*****
RefBase static member initialization
*****/
RefDgmr *RefBase::sDgmr;
unordered_map<const RefBase *, RefBase::index_t> RefBase::sIndices;
vector<RefBase *> RefBase::sSequence;
vector<RefBase::DgmInfo> RefBase::sDgms;

/*  Notes on Sequences.
A ref (RefMark or RefLine) is typically defined in terms of other refs, going
all the way back to RefMark_Original and RefLine_Original. The routine
aRef->BuildAndNumberSequence() constructs an ordered list of all the refs that
make up aRef. The ordering is such that ancestor refs come earlier in the list
than refs derived from them. In addition to constructing the list,
BuildAndNumberSequence also sets the mIndex variable of each non- original ref,
so that each can be given a unique name (which depends on the index, and is a
letter, A-J for lines, P-Z for points).

This numbering scheme means that there can only be one sequence in existence at
a time. To insure this, the sequence is constructed in the static RefBase
member variable sSequence, which is public, so you can (if you like) read out
this sequence.
*/

/*****
Append to the end of sSequence pointers to all the marks and lines needed to
create this mark or line. Default behavior is to append a pointer to self. A
subclass line or mark made from others should call SequencePushUnique() for
each of the elements that define it, and then call the RefBase method for
itself.
*****/
void RefBase::SequencePushSelf() {
	SequencePushUnique(this);
}

/*****
Build a sequence (in sSequence) of all of the references that are needed to
define this one; also set mIndex for each reference so that the relevant
RefMarks and RefLines are sequentially numbered.
*****/
void RefBase::BuildAndNumberSequence() {
	sSequence.clear();
	SequencePushSelf();

	// Making sure that all original references are at the head of the sequence,
	// as it is possible that some original references are pushed in the middle
	// (especially the diagonal).
	stable_partition(sSequence.begin(), sSequence.end(), [](RefBase *r) {
		return !r->IsDerived();
	});

	RefMark::ResetCount();
	RefLine::ResetCount();
	sIndices.clear();
	for(auto &i: sSequence) i->SetIndex();
}

void RefBase::PutName(char const *key, JsonObject &obj) const {}

/*****
Create a JSON object about how to make this mark from its constituents.
Overridden by most subclasses. Default behavior is returning an empty object.
*****/
JsonObject RefBase::Serialize() const {
	JsonObject step;
	return step;
}

/*****
Send the full how-to sequence to the given stream.
*****/
void RefBase::PutHowtoSequence(JsonObject &solution) {
	JsonArray steps;
	for(auto &i: sSequence) {
		steps.add(i->Serialize());
	}
	solution.add("steps", steps);
}

/*  Notes on diagrams.
A DgmInfo is a very simple object that contains just a couple of bits of
information necessary to construct a complete diagram from the list of refs
contained in sSequence. Any diagram is a subsequence of sSequence, which, in
effect, describes all the refs that are already on the paper as well as the
ref(s) currently being made.

DgmInfo.idef is the index of the first ref in sSequence that is defined in the
current diagram.

DgmInfo.iact is the index of the last ref that is defined in the current
diagram, which is the action line, if this diagram includes an action line.
idef <= iact.
*/

/*****
Build a set of diagrams that describe how to fold this reference, by
constructing a list of DgmInfo records (which refer to elements and
subsequences of sSequence).
*****/
void RefBase::BuildDiagrams() {
	sDgms.clear();

	// Now, we need to note which elements of the sequence are action lines;
	// there will be a diagram for each one of these.
	size_t ss = sSequence.size();
	for(size_t i = 0; i < ss; i++) {
		if(sSequence[i]->IsActionLine()) {
			sDgms.emplace_back(i, i);

			// For each line, check if it is only used to create one intersection and nothing more.
			// If so, it suffices to pinch only near the corresponding mark, and not the entire line.
			RefBase *mark = nullptr;
			for(size_t j = 0; j < ss; j++) {
				if(sSequence[j]->UsesImmediate(sSequence[i])) {
					if(sSequence[j]->IsLine() || mark != nullptr) {
						mark = nullptr;
						break;
					} else {
						mark = sSequence[j];
					}
				}
			}
			((RefLine *)sSequence[i])->mForMark = mark;
		}
		if(!sSequence[i]->IsLine() && sSequence[i]->IsDerived()) {
			auto *mark = (RefMark_Intersection *)sSequence[i];
			// It won't make sense to have both lines as pinches.
			if(mark->rl2->mForMark != nullptr) mark->rl1->mForMark = nullptr;
		}
	}

	// We should always have at least one diagram, even if there was only one ref
	// in sSequence (which happens if the ref was a RefMark_Original or
	// RefLine_Original).
	if(sDgms.size() == 0) sDgms.emplace_back(0, 0);

	// And we make sure we have a diagram for the last ref in the sequence (which
	// might not be the case if we ended with a RefMark or an original).
	if(sDgms[sDgms.size() - 1].iact < ss - 1) sDgms.emplace_back(0, ss - 1);

	// Now we go through and set the idef fields of each DgmInfo record.
	size_t id = 0;
	for(auto &sDgm: sDgms) {
		sDgm.idef = id;
		id = sDgm.iact + 1;
	}
}

/*****
Draw the paper
*****/
void RefBase::DrawPaper() {
	vector<XYPt> corners;
	corners.push_back(Shared::sPaper.mBotLeft);
	corners.push_back(Shared::sPaper.mBotRight);
	corners.push_back(Shared::sPaper.mTopRight);
	corners.push_back(Shared::sPaper.mTopLeft);
	sDgmr->DrawPoly(corners, RefDgmr::POLYSTYLE_WHITE);
}

/*****
Draw the given diagram using the RefDgmr aDgmr.
*****/
void RefBase::DrawDiagram(RefDgmr &aDgmr, const DgmInfo &aDgm) {
	// Set the current RefDgmr to be aDgmr.
	sDgmr = &aDgmr;
	const int act = aDgm.iact;

	// always draw the paper
	DrawPaper();

	// Make a note of the action line ref
	RefBase *ral = sSequence[act];
	int ss = sSequence.size();

	// draw all refs specified by the DgmInfo. Most get drawn in normal style.
	// The ref that is the action line (and all subsequent refs) get drawn in
	// action style. Any refs that are used immediately by the action line get
	// drawn in hilite style. Drawing for each diagram is done in multiple passes
	// so that, for examples, labels end up on top of everything else.
	for(short ipass = 0; ipass < NUM_PASSES; ipass++) {
		for(size_t i = 0; i < act; i++) {
			RefBase *rb = sSequence[i];
			bool shouldHighlight = (i >= aDgm.idef && rb->IsDerived()) || ral->UsesImmediate(rb);
			RefStyle style = shouldHighlight ? REFSTYLE_HILITE : REFSTYLE_NORMAL;
			rb->DrawSelf(style, ipass);
		};
		if(act < ss) {
			sSequence[act]->DrawSelf(REFSTYLE_ACTION, ipass);

			// When the next thing in the sequence is a mark, we also include it in the current diagram.
			if(
				act < ss - 2 &&				  // unless it's the very last one, which will be a standalone diagram.
				!sSequence[act + 1]->IsLine() // the next one is a mark
			) {
				sSequence[act + 1]->DrawSelf(REFSTYLE_ACTION, ipass);
			}
		}
	}
}

/*****
Return true if this mark or line uses rb for immediate reference. "Immediate"
means that if A uses B and B uses C, A->UsesImmediate(B) returns true but
A->UsesImmediate(C) returns false. Default returns false; this will be used by
original marks and lines.
*****/
bool RefBase::UsesImmediate(RefBase * /* rb */) const {
	return false;
}

/*****
Return true if this is a derived (rather than an original) mark or line. Note
that this is not the same as mRank!=0, because the diagonals have mRank==1 but
are still considered original. Default is true since most objects will be
derived.
*****/
bool RefBase::IsDerived() const {
	return true;
}

/*****
Utility used by subclasses when they implement SequencePushSelf(). This insures
that a given mark only gets a single label.
*****/
void RefBase::SequencePushUnique(RefBase *rb) {
	if(find(sSequence.begin(), sSequence.end(), rb) == sSequence.end())
		sSequence.push_back(rb);
}

void RefBase::Export(BinaryOutputStream &os) const {
	os << GetType();
}

#ifdef _DEBUG_DB_
void RefBase::PutDebug(JsonObject &step) const {
	step.add("key", mKey);
	step.add("score", mScore);
	step.add("rank", mRank);
}
#endif
