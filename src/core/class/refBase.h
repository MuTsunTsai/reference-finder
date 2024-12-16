
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
**********/
class RefBase {
  public:
	std::size_t id{0}; // Unique id for refs that are actually added to the DB

	enum RefType {
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

	key_t mKey{0}; // Key used for maps within RefContainers

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
	using index_t = short;										  // type for indices
	static std::unordered_map<const RefBase *, index_t> sIndices; // used to label refs in a folding sequence

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
	RefBase() = default;
	virtual ~RefBase() = default;

	using type_t = const unsigned char;
	virtual type_t GetType() const = 0;

	// Rank of this mark or line.
	// We only need this info in edge cases during CompareRankAndError and in exporting,
	// so calculate its value as needed recursively does not significantly bring down performance,
	// and we can save quite some memory usage by eliminating one field.
	virtual rank_t GetRank() const = 0;

	// routines for building a sequence of refs
	virtual void SequencePushSelf();
	void BuildAndNumberSequence();

	// routine for creating a text description of how to fold a ref
	virtual const char GetLabel() const = 0;
	virtual void PutName(char const *key, JsonObject &obj) const;
	virtual JsonObject Serialize() const;
	virtual void Export(BinaryOutputStream &os) const;
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
