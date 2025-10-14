
#pragma once

#include "database/binaryOutputStream.hpp"
#include "global/global.h"
#include "refBase.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>

// Custom hash
struct RefHash {
	template <typename T>
	size_t operator()(T *ptr) const {
		return (ptr->*ptrToHash)();
	}
};

// Custom equality comparison
struct RefEqual {
	template <typename T>
	bool operator()(T *a, T *b) const {
		return (a->*ptrToEquals)(b);
	}
};

/**********
class RefContainer - Container for marks and lines.
**********/
template <class R>
class RefContainer: public std::vector<R *> {
  public:
	using set_t = std::unordered_set<R *, RefHash, RefEqual>; // typedef for set holding R*
	using vec_t = std::vector<R *>;							  // typedef for vector holding R*

	std::vector<vec_t> ranks; // Holds vectors of objects, one for each rank

	// Used to accumulate new objects.
	// We break it into chunks of size CHUNK_SIZE,
	// to reduce the stress of copying data from the buffer to the main container.
	std::vector<set_t> buffer;

	index_t GetTotalSize() const {
		// Total number of elements, all ranks
		return this->size() + mBufferSize;
	};

	template <class Rs>
	void AddCopyIfValidAndUnique(const Rs &ars); // add a copy of ars if valid and unique

  private:
	static constexpr int CHUNK_SIZE = 1 << 20;

	friend class ReferenceFinder; // only class that gets to use these methods

	RefContainer(); // Constructor

	index_t nextId{0};
	set_t set; // A centralized set for checking duplication
	int mBufferSize{0};
	void Rebuild();					// Re-initialize with new values
	void Add(R *ar);				// Add an element to the array
	void FlushBuffer(rank_t arank); // Add the contents of the buffer to the container
	void ClearMaps();				// Clear the map arrays when no longer needed
};

/**********
class RefContainer - a container for storing RefMark* and RefLine* that
organizes them by rank and within a rank, uses a map<R*> to store them. The
mKey member variable is used as the key in the map; only one object is stored
per key. class R = RefMark or RefLine
**********/

/*****
Constructor. Initialize arrays.
*****/
template <class R>
RefContainer<R>::RefContainer() {
	// expand our map array to hold all the ranks that we will create
	ranks.resize(1 + Shared::sMaxRank);
	buffer.resize(0);
}

/*****
Rebuild all arrays and related counters.
*****/
template <class R>
void RefContainer<R>::Rebuild() {
	this->resize(0);
	set.clear();
	ranks.resize(0);
	ranks.resize(1 + Shared::sMaxRank);
}

/*****
Add an element to the array; to be called only if an equivalent element isn't
already present anywhere. To avoid corrupting iterators, we add the new element
to the buffer; it will get added to the main container on the next call to
FlushBuffer().
*****/
template <class R>
void RefContainer<R>::Add(R *ar) {
	int i = mBufferSize / CHUNK_SIZE;
	if(mBufferSize % CHUNK_SIZE == 0) buffer.resize(i + 1); // Add a new chunk
	buffer[i].insert(ar);									// Add it to the buffer.
	mBufferSize++;
}

/*****
Check the validity and uniqueness of object ars of type Rs (descended from type
R), and if it passes, add it to the container. Used by all the MakeAll()
functions. Note that we use the default (compiler-generated) copy constructor,
which is OK since our objects only contain POD, pointers that we WANT
blind-copied, and/or strings (which know how to copy themselves).

New in v4.7.3: We call `new Rs(ars)` only when needed.
This significantly reduces total memory usage when building database.
*****/
template <class R>
template <class Rs>
void RefContainer<R>::AddCopyIfValidAndUnique(const Rs &ars) {
	if(
		// The ref is valid (fully constructed) if its key is something other than 0.
		ars.mKey != 0 &&
		// If the main set already has one with the same key,
		// it must be of a lower rank and we can safely ignore the current one.
		!set.count((R *)&ars)
	) {
		// See if any buffer has an entry with the same key
		bool found = false;
		for(set_t &buf: buffer) {
			auto iter = buf.find((R *)&ars);
			if(iter != buf.end()) {
				found = true;
				// see if the current one has a lower score (otherwise ignore)
				if(ars.mScore < (*iter)->mScore) {
					delete *iter; // don't forget to release memory
					buf.erase(iter);
					buf.insert(new Rs(ars));
				}
				break;
			}
		}
		// if the buffer doesn't have the same key, add the ref directly
		if(!found) Add(new Rs(ars));
	}
	Shared::CheckDatabaseStatus(); // report progress if appropriate
}

/*****
Put the contents of the buffer into the main container.
*****/
template <class R>
void RefContainer<R>::FlushBuffer(rank_t arank) {
	for(auto &buf: buffer) {
		// Make room for the buffer in the sortable list.
		this->reserve(this->size() + buf.size());

		// Go through the buffer and add each element to the appropriate rank in the main container.
		for(auto *ar: buf) {
			set.insert(ar);				// add to the main set
			ranks[arank].push_back(ar); // add to appropriate rank
			this->push_back(ar);		// also add to our sortable list
			if(Shared::useDatabase) {
				// We do a cast here for intellisense
				auto *ref = static_cast<RefBase *>(ar);
				ref->id = nextId++; // assign a new id
				ref->Export(*Shared::dbStream);
			}
		}
		buf.clear();   // clear the buffer
		buf = set_t(); // force releasing memory
	}
	buffer.resize(0);
	mBufferSize = 0;
}

/*****
Clear the set and rank vectors. Called when they're no longer needed.
*****/
template <class R>
void RefContainer<R>::ClearMaps() {
	set.clear();
	for(auto &rank: ranks) rank.clear();
}
