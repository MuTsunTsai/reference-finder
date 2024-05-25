
#ifndef _REF_CONTAINER_H_
#define _REF_CONTAINER_H_

#include "global.h"

#include <unordered_map>
#include <vector>

/**********
class RefContainer - Container for marks and lines.
**********/
template <class R>
class RefContainer : public std::vector<R *> {
  public:
	// For our use case, using std::unordered_map can improve performance by about 12% comparing to std::map
	typedef std::unordered_map<key_t, R *> map_t; // typedef for map holding R*
	typedef std::vector<R *> vec_t;				  // typedef for vector holding R*
	map_t map;									  // A centralized map for checking duplication
	std::vector<vec_t> maps;					  // Holds vectors of objects, one for each rank
	vec_t buffer;								  // used to accumulate new objects (to avoid corrupting the main iterator)

  public:
	std::size_t GetTotalSize() const {
		// Total number of elements, all ranks
		return this->size() + buffer.size();
	};

	template <class Rs>
	void AddCopyIfValidAndUnique(const Rs &ars); // add a copy of ars if valid and unique

  private:
	friend class ReferenceFinder; // only class that gets to use these methods

	RefContainer(); // Constructor

	void Rebuild();					  // Re-initialize with new values
	bool Contains(const R *ar) const; // True if an equivalent element already exists
	void Add(R *ar);				  // Add an element to the array
	void FlushBuffer();				  // Add the contents of the buffer to the container
	void ClearMaps();				  // Clear the map arrays when no longer needed
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
	maps.resize(1 + Shared::sMaxRank);
}

/*****
Rebuild all arrays and related counters.
*****/
template <class R>
void RefContainer<R>::Rebuild() {
	this->resize(0);
	map.clear();
	maps.resize(0);
	maps.resize(1 + Shared::sMaxRank);
}

/*****
Add an element to the array; to be called only if an equivalent element isn't
already present anywhere. To avoid corrupting iterators, we add the new element
to the buffer; it will get added to the main container on the next call to
FlushBuffer().
*****/
template <class R>
void RefContainer<R>::Add(R *ar) {
	buffer.push_back(ar);								  // Add it to the buffer.
	map.insert(typename map_t::value_type(ar->mKey, ar)); // Also add it to the map immediately.
}

/*****
Check the validity and uniqueness of object ars of type Rs (descended from type
R), and if it passes, add it to the container. Used by all the MakeAll()
functions. Note that we use the default (compiler-generated) copy constructor,
which is OK since our objects only contain POD, pointers that we WANT
blind-copied, and/or strings (which know how to copy themselves).
*****/
template <class R>
template <class Rs>
void RefContainer<R>::AddCopyIfValidAndUnique(const Rs &ars) {
	// The ref is valid (fully constructed) if its key is something other than 0.
	// It's unique if the container doesn't already have one with the same key in
	// one of the rank maps.
	if (ars.mKey != 0 && !Contains(&ars)) Add(new Rs(ars));
	Shared::CheckDatabaseStatus(); // report progress if appropriate
}

/*****
Return true if the container (including the buffer) contain an equivalent object.
*****/
template <class R>
bool RefContainer<R>::Contains(const R *ar) const {
	return map.count(ar->mKey);
}

/*****
Put the contents of the buffer into the main container.
*****/
template <class R>
void RefContainer<R>::FlushBuffer() {
	// Make room for the buffer in the sortable list.
	this->reserve(this->size() + buffer.size());

	// Go through the buffer and add each element to the appropriate rank in the main container.
	for (auto bi : buffer) {
		maps[bi->mRank].push_back(bi); // add to the map of the appropriate rank
		this->push_back(bi);		   // also add to our sortable list
	}
	buffer.clear(); // clear the buffer
}

/*****
Clear the map arrays. Called when they're no longer needed.
*****/
template <class R>
void RefContainer<R>::ClearMaps() {
	map.clear();
	for (size_t ir = 0; ir < maps.size(); ir++) maps[ir].clear();
}

#endif
