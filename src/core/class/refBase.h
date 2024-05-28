
#ifndef _REF_BASE_H_
#define _REF_BASE_H_

#include "global.h"
#include "json/jsonArray.h"
#include "json/jsonObject.h"

#include <iostream>
#include <vector>

class RefDgmr;

/**********
class RefBase - base class for a mark or line.
**********/
class RefBase {
  public:
	rank_t mRank; // Rank of this mark or line
	key_t mKey;	  // Key used for maps within RefContainers
	int mScore;	  // Used to decide whether to override existing refs

	/* A none-null value indicating that this ref (line in fact) is used only for creating an intersection. */
	RefBase *mForMark;

	static std::vector<RefBase *> sSequence; // a sequence of refs that fully define a ref

	struct DgmInfo {	  // information that encodes a diagram description
		std::size_t idef; // first ref that's defined in this diagram
		std::size_t iact; // ref that terminates this diagram
		DgmInfo(std::size_t adef, std::size_t aact) : idef(adef), iact(aact) {}
	};
	static std::vector<DgmInfo> sDgms; // a list of diagrams that describe a given ref

  protected:
	typedef short index_t; // type for indices
	index_t mIndex;		   // used to label this ref in a folding sequence

	static RefDgmr *sDgmr; // object that draws diagrams
	enum {
		// Drawing happens in multiple passes to get the stacking order correct
		PASS_LINES,
		PASS_HLINES,
		PASS_POINTS,
		PASS_ARROWS,
		PASS_LABELS,
		NUM_PASSES
	}; // drawing order

  public:
	RefBase(rank_t arank = 0) : mRank(arank), mKey(0), mIndex(0), mScore(0), mForMark(NULL) {}
	virtual ~RefBase() {}

	// routines for building a sequence of refs
	virtual void SequencePushSelf();
	void BuildAndNumberSequence();

	// routine for creating a text description of how to fold a ref
	virtual const char GetLabel() const = 0;
	virtual void PutName(char const *key, JsonObject &obj) const;
	virtual JsonObject Serialize() const;
	void PutHowtoSequence(JsonObject &solution);

	// routines for drawing diagrams
	void BuildDiagrams();
	static void DrawPaper();
	static void DrawDiagram(RefDgmr &aDgmr, const DgmInfo &aDgm);

  protected:
	virtual bool UsesImmediate(RefBase *rb) const;
	virtual bool IsLine() const = 0;
	virtual bool IsActionLine() const = 0;
	virtual bool IsDerived() const;
	virtual void SetIndex() = 0;
	static void SequencePushUnique(RefBase *rb);
	enum RefStyle {
		REFSTYLE_NORMAL,
		REFSTYLE_HILITE,
		REFSTYLE_ACTION
	};
	virtual void DrawSelf(RefStyle rstyle, short ipass) const = 0;

#ifdef _DEBUG_DB_
	void PutDebug(JsonObject &step) const;
#endif
};

#endif
