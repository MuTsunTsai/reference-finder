
#pragma once

#include "database/binaryInputStream.hpp"
#include "database/binaryOutputStream.hpp"
#include "global/global.h"
#include "json/jsonArray.h"
#include "json/jsonObject.h"

#include <array>
#include <iostream>
#include <unordered_map>
#include <vector>

class BinaryInputStream;
class BinaryOutputStream;
class RefDgmr;
class RefBase;

/**********
class RefBaseLogic - base class for processing logic.

Originally, the methods defined in this class are virtual methods of
the RefBase class. However, doing so adds the vtable pointer (vptr)
to all the hundreds of thousands of RefBase instances, thereby wasting
significant amount of memory. Since v4.8, we separate the processing
logic to their associated logic classes, eliminating vptr from the
RefBase and its derived classes. There will be a very slight overhead
for looking up the methods, but in practice the overhead can be ignored.
**********/
class RefBaseLogic {
  public:
	enum RefStyle : std::uint8_t {
		REFSTYLE_NORMAL,
		REFSTYLE_HILITE,
		REFSTYLE_ACTION
	};

	virtual void MakeAll(rank_t arank) const;
	virtual rank_t GetRank(const RefBase *self) const = 0;
	virtual size_t hash(const RefBase *self) const = 0;
	virtual bool equals(const RefBase *self, const RefBase *other) const = 0;
	virtual void SequencePushSelf(RefBase *self) const;
	virtual char GetLabel(const RefBase *self) const = 0;
	virtual void PutName(const RefBase *self, char const *key, JsonObject &obj) const;
	virtual JsonObject Serialize(const RefBase *self) const;
	virtual RefBase *Import(BinaryInputStream &is) const = 0;
	virtual void Export(const RefBase *self, BinaryOutputStream &os) const;
	virtual bool UsesImmediate(const RefBase *self, RefBase *rb) const;
	virtual bool IsDerived(const RefBase *self) const;
	virtual void SetIndex(const RefBase *self) const = 0;
	virtual void DrawSelf(const RefBase *self, RefStyle rstyle, short ipass) const = 0;

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
};

/**********
class RefBase - base class for a mark or line.

Any subclasses of RefBase should be fairly lightweight because we'll be
creating a couple hundred thousand of them during program initialization.
**********/
class RefBase {
  public:
	static const std::array<RefBaseLogic *, 10> logics;

	friend class RefBaseLogic;

	// We squeeze a few fields into a union to save memory.
	union {
		/**
		 * Unique id for refs that are actually added to the DB.
		 * Used after leaving buffer, and before initialization complete.
		 */
		std::size_t id;

		/**
		 * Key for optimization. Used only for RefMark and after initialization.
		 * Technically we could use `unsigned int` for this,
		 * but since we may need to manipulate it with negative numbers,
		 * we use `int` instead.
		 */
		int optKey;

		/**
		 * A none-null value indicating that this line is used only for creating an intersection.
		 * Used only for RefLine and after initialization.
		 */
		RefBase *mForMark{nullptr};
	};

	/**
	 * Used to decide whether to override existing refs.
	 * This is used throughout the initialization, so we cannot put this in the union.
	 */
	int mScore{0};

	/**
	 * A unique key that is used to efficiently store and search over marks and lines.
	 * mKey is initialized to 0. If the object has been successfully constructed,
	 * it will be set to an integer greater than 0,
	 * so `mKey == 0` is used as a test for successful construction.
	 *
	 * Note that this is used even after the ref left the buffer,
	 * so currently there's no way to reuse this memory space.
	 */
	key_t mKey{0};

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

  private:
	const RefType mType;

  public:
	// These two fields are used only in some RefLine classes.
	// Since we have space left in RefBase, we squeeze them here.
	// This can save us 8 bytes for those RefLine classes.
	unsigned char mWhoMoves;
	unsigned char mRoot;

	static std::vector<RefBase *> sSequence; // a sequence of refs that fully define a ref

	struct DgmInfo {	  // information that encodes a diagram description
		std::size_t idef; // first ref that's defined in this diagram
		std::size_t iact; // ref that terminates this diagram
		DgmInfo(std::size_t adef, std::size_t aact): idef(adef), iact(aact) {}
	};
	static std::vector<DgmInfo> sDgms; // a list of diagrams that describe a given ref

	RefBase() = delete;
	RefBase(const RefType atype): mType(atype) {};

	// Rank of this mark or line, which is the number of creases that need to be made to define it.
	// We only need this info in edge cases during CompareRankAndError and in exporting,
	// so calculate its value as needed recursively does not significantly bring down performance,
	// and we can save quite some memory usage by eliminating one field.
	rank_t GetRank() const;

	size_t simple_hash() const;
	size_t hash() const;
	bool simple_equals(const RefBase *other) const;
	bool equals(const RefBase *other) const;

	// routines for building a sequence of refs
	void SequencePushSelf();
	void BuildAndNumberSequence();

	// routine for creating a text description of how to fold a ref
	void PutName(char const *key, JsonObject &obj) const;
	JsonObject Serialize() const;
	void Export(BinaryOutputStream &os) const;
	static void PutHowtoSequence(JsonObject &solution);

	// routines for drawing diagrams
	static void BuildDiagrams();
	static void DrawPaper();
	static void DrawDiagram(RefDgmr &aDgmr, const DgmInfo &aDgm);

	bool IsLine() const;

  protected:
	bool UsesImmediate(RefBase *rb) const;
	bool IsActionLine() const;
	bool IsDerived() const;
	void SetIndex() const;
	static void SequencePushUnique(RefBase *rb);
	void DrawSelf(RefBaseLogic::RefStyle rstyle, short ipass) const;

#ifdef _DEBUG_DB_
	void PutDebug(JsonObject &step) const;
#endif
};

extern size_t (RefBase::*ptrToHash)() const;
extern bool (RefBase::*ptrToEquals)(const RefBase *other) const;
