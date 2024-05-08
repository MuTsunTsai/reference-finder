
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
	std::vector<map_t> maps;					  // Holds maps of objects, one for each rank
	map_t buffer;								  // used to accumulate new objects

	typedef typename map_t::iterator rank_iterator; // for iterating through individual ranks

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

#endif
