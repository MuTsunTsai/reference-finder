
#include "refBase.h"
#include "global/global.h"
#include "math/paper.h"
#include "math/xypt.h"
#include "refDgmr.h"
#include "refLine/refLine.h"
#include "refLine/refLineC2PC2P.h"
#include "refLine/refLineL2L.h"
#include "refLine/refLineL2LC2P.h"
#include "refLine/refLineL2LP2L.h"
#include "refLine/refLineOriginal.h"
#include "refLine/refLineP2LC2P.h"
#include "refLine/refLineP2LP2L.h"
#include "refLine/refLineP2P.h"
#include "refMark/refMark.h"
#include "refMark/refMarkOriginal.h"
#include "refMark/refMarkIntersection.h"
#include "json/jsonArray.h"

#include <algorithm>

using namespace std;

/**********
class RefBase - base class for a mark or line.
**********/

/*****
RefBase static member initialization
*****/
vector<RefBase *> RefBase::sSequence;
vector<RefBase::DgmInfo> RefBase::sDgms;
const std::array<RefBaseLogic *, 10> RefBase::logics = {
	new RefLine_OriginalLogic(),
	new RefLine_C2P_C2P_Logic(),
	new RefLine_P2P_Logic(),
	new RefLine_L2L_Logic(),
	new RefLine_L2L_C2P_Logic(),
	new RefLine_P2L_C2P_Logic(),
	new RefLine_P2L_P2L_Logic(),
	new RefLine_L2L_P2L_Logic(),
	new RefMark_OriginalLogic(),
	new RefMark_IntersectionLogic()
};

/*  Notes on Sequences.
A ref (RefMark or RefLine) is typically defined in terms of other refs, going
all the way back to RefMark_Original and RefLine_Original. The routine
aRef->BuildAndNumberSequence() constructs an ordered list of all the refs that
make up aRef. The ordering is such that ancestor refs come earlier in the list
than refs derived from them. In addition to constructing the list,
BuildAndNumberSequence also sets the sIndices of each non-original ref,
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
	logics[mType]->SequencePushSelf(this);
}

/*****
Build a sequence (in sSequence) of all of the references that are needed to
define this one; also set sIndices for each reference so that the relevant
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
	RefBaseLogic::sIndices.clear();
	for(auto &i: sSequence) i->SetIndex();
}

void RefBase::PutName(char const *key, JsonObject &obj) const {
	logics[mType]->PutName(this, key, obj);
}

/*****
Create a JSON object about how to make this mark from its constituents.
Overridden by most subclasses. Default behavior is returning an empty object.
*****/
JsonObject RefBase::Serialize() const {
	return logics[mType]->Serialize(this);
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
	step_t ss = sSequence.size();
	for(step_t i = 0; i < ss; i++) {
		if(sSequence[i]->IsActionLine()) {
			sDgms.emplace_back(i, i);

			// For each line, check if it is only used to create one intersection and nothing more.
			// If so, it suffices to pinch only near the corresponding mark, and not the entire line.
			RefBase *mark = nullptr;
			for(step_t j = 0; j < ss; j++) {
				if(sSequence[j]->UsesImmediate(sSequence[i])) {
					if(sSequence[j]->IsLine() || mark != nullptr) {
						mark = nullptr;
						break;
					}
					mark = sSequence[j];
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
	if(sDgms.empty()) sDgms.emplace_back(0, 0);

	// And we make sure we have a diagram for the last ref in the sequence (which
	// might not be the case if we ended with a RefMark or an original).
	if(sDgms[sDgms.size() - 1].iact < ss - 1) sDgms.emplace_back(0, ss - 1);

	// Now we go through and set the idef fields of each DgmInfo record.
	step_t id = 0;
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
	RefBaseLogic::sDgmr->DrawPoly(corners, RefDgmr::POLYSTYLE_WHITE);
}

/*****
Draw the given diagram using the RefDgmr aDgmr.
*****/
void RefBase::DrawDiagram(RefDgmr &aDgmr, const DgmInfo &aDgm) {
	// Set the current RefDgmr to be aDgmr.
	RefBaseLogic::sDgmr = &aDgmr;
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
	for(short ipass = 0; ipass < RefBaseLogic::NUM_PASSES; ipass++) {
		for(step_t i = 0; i < act; i++) {
			RefBase *rb = sSequence[i];
			bool shouldHighlight = (i >= aDgm.idef && rb->IsDerived()) || ral->UsesImmediate(rb);
			RefBaseLogic::RefStyle style = shouldHighlight ? RefBaseLogic::REFSTYLE_HILITE : RefBaseLogic::REFSTYLE_NORMAL;
			rb->DrawSelf(style, ipass);
		};
		if(act < ss) {
			sSequence[act]->DrawSelf(RefBaseLogic::REFSTYLE_ACTION, ipass);

			// When the next thing in the sequence is a mark, we also include it in the current diagram.
			if(
				act < ss - 2 &&				  // unless it's the very last one, which will be a standalone diagram.
				!sSequence[act + 1]->IsLine() // the next one is a mark
			) {
				sSequence[act + 1]->DrawSelf(RefBaseLogic::REFSTYLE_ACTION, ipass);
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
bool RefBase::UsesImmediate(RefBase *rb) const {
	return logics[mType]->UsesImmediate(this, rb);
}

/*****
Return true if this is a derived (rather than an original) mark or line. Note
that this is not the same as mRank!=0, because the diagonals have mRank==1 but
are still considered original. Default is true since most objects will be
derived.
*****/
bool RefBase::IsDerived() const {
	return logics[mType]->IsDerived(this);
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
	logics[mType]->Export(this, os);
}

size_t RefBase::simple_hash() const {
	return std::hash<key_t>()(mKey);
}

bool RefBase::simple_equals(const RefBase *other) const {
	return mKey == other->mKey;
}

rank_t RefBase::GetRank() const {
	return logics[mType]->GetRank(this);
}
size_t RefBase::hash() const {
	return logics[mType]->hash(this);
}
bool RefBase::equals(const RefBase *other) const {
	return logics[mType]->equals(this, other);
}
bool RefBase::IsLine() const {
	return mType < RefType::MARK_ORIGINAL;
}
bool RefBase::IsActionLine() const {
	return RefType::LINE_ORIGINAL < mType && mType < RefType::MARK_ORIGINAL;
}
void RefBase::SetIndex() const {
	logics[mType]->SetIndex(this);
}
void RefBase::DrawSelf(RefBaseLogic::RefStyle rstyle, short ipass) const {
	logics[mType]->DrawSelf(this, rstyle, ipass);
}

#ifdef _DEBUG_DB_
void RefBase::PutDebug(JsonObject &step) const {
	step.add("key", mKey);
	step.add("score", mScore);
}
#endif

size_t (RefBase::*ptrToHash)() const = &RefBase::simple_hash;
bool (RefBase::*ptrToEquals)(const RefBase *other) const = &RefBase::simple_equals;

/**********
class RefBaseLogic
**********/

RefDgmr *RefBaseLogic::sDgmr;
unordered_map<const RefBase *, step_t> RefBaseLogic::sIndices;

void RefBaseLogic::MakeAll(rank_t arank) const {
	// Do nothing by default
}

void RefBaseLogic::SequencePushSelf(RefBase *self) const {
	RefBase::SequencePushUnique(self);
}

void RefBaseLogic::PutName(const RefBase *self, char const *key, JsonObject &obj) const {
	// Do nothing by default
}

JsonObject RefBaseLogic::Serialize(const RefBase *self) const {
	JsonObject step;
	return step;
}

void RefBaseLogic::Export(const RefBase *self, BinaryOutputStream &os) const {
	os << self->mType;
}

bool RefBaseLogic::UsesImmediate(const RefBase *self, RefBase *rb) const {
	return false;
}

bool RefBaseLogic::IsDerived(const RefBase *self) const {
	return true;
}
