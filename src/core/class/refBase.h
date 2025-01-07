
#pragma once

#include "database/binaryInputStream.hpp"
#include "database/binaryOutputStream.hpp"
#include "global/global.h"
#include "json/jsonArray.h"
#include "json/jsonObject.h"

#include <iostream>
#include <unordered_map>
#include <vector>

class BinaryInputStream;
class BinaryOutputStream;
class RefDgmr;

/**********
class RefBase - base class for a mark or line.

Any subclasses of RefBase should be fairly lightweight because we'll be
creating a couple hundred thousand of them during program initialization.
**********/
class RefBase {
  public:
	std::size_t id{0}; // Unique id for refs that are actually added to the DB

	enum RefType : std::uint8_t {
		LINE_ORIGINAL,
		LINE_C2P_C2P,
		LINE_P2P,
		LINE_L2L,
		LINE_L2L_C2P,
		LINE_P2L_C2P,
		LINE_P2L_P2L,
		LINE_L2L_P2L,
		MARK_ORIGINAL,
		MARK_INTERSECTION
	};

	// A unique key that is used to efficiently store and search over marks and lines.
	// mKey is initialized to 0. If the object has been successfully constructed,
	// it will be set to an integer greater than 0,
	// so `mKey == 0` is used as a test for successful construction.
	key_t mKey{0};

	// Used to decide whether to override existing refs.
	// After initialization, this field is re-used for optimization.
	int mScore{0};

	static std::vector<RefBase *> sSequence; // a sequence of refs that fully define a ref

	struct DgmInfo {	  // information that encodes a diagram description
		std::size_t idef; // first ref that's defined in this diagram
		std::size_t iact; // ref that terminates this diagram
		DgmInfo(std::size_t adef, std::size_t aact): idef(adef), iact(aact) {}
	};
	static std::vector<DgmInfo> sDgms; // a list of diagrams that describe a given ref

  protected:
	using index_t = short; // type for indices

	// A counter used in constructing the verbal sequence;
	// basically, it's the order in which the object is created for a given folding sequence.
	static std::unordered_map<const RefBase *, index_t> sIndices;

	static RefDgmr *sDgmr; // object that draws diagrams
	enum : std::uint8_t {
		// Drawing happens in multiple passes to get the stacking order correct
		PASS_LINES,
		PASS_HLINES,
		PASS_POINTS,
		PASS_ARROWS,
		PASS_LABELS,
		NUM_PASSES
	}; // drawing order

  public:
	RefBase() = default;
	virtual ~RefBase() = default;

	using type_t = unsigned char;
	virtual type_t GetType() const = 0;

	// Rank of this mark or line, which is the number of creases that need to be made to define it.
	// We only need this info in edge cases during CompareRankAndError and in exporting,
	// so calculate its value as needed recursively does not significantly bring down performance,
	// and we can save quite some memory usage by eliminating one field.
	virtual rank_t GetRank() const = 0;

	size_t simple_hash() const;
	virtual size_t hash() const = 0;
	bool simple_equals(const RefBase *other) const;
	virtual bool equals(const RefBase *other) const = 0;

	// routines for building a sequence of refs
	virtual void SequencePushSelf();
	void BuildAndNumberSequence();

	// routine for creating a text description of how to fold a ref
	virtual char GetLabel() const = 0;
	virtual void PutName(char const *key, JsonObject &obj) const;
	virtual JsonObject Serialize() const;
	virtual void Export(BinaryOutputStream &os) const;
	static void PutHowtoSequence(JsonObject &solution);

	// routines for drawing diagrams
	static void BuildDiagrams();
	static void DrawPaper();
	static void DrawDiagram(RefDgmr &aDgmr, const DgmInfo &aDgm);

  protected:
	virtual bool UsesImmediate(RefBase *rb) const;
	virtual bool IsLine() const = 0;
	virtual bool IsActionLine() const = 0;
	virtual bool IsDerived() const;
	virtual void SetIndex() = 0;
	static void SequencePushUnique(RefBase *rb);
	enum RefStyle : std::uint8_t {
		REFSTYLE_NORMAL,
		REFSTYLE_HILITE,
		REFSTYLE_ACTION
	};
	virtual void DrawSelf(RefStyle rstyle, short ipass) const = 0;

#ifdef _DEBUG_DB_
	void PutDebug(JsonObject &step) const;
#endif
};

extern size_t (RefBase::*ptrToHash)() const;
extern bool (RefBase::*ptrToEquals)(const RefBase *other) const;
