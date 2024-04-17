
#include "../ReferenceFinder.h"

#include "refContainer.h"
#include "refLine/refLine.h"
#include "refLine/refLineL2L.h"
#include "refLine/refLineP2P.h"
#include "refLine/refLineL2LP2L.h"
#include "refLine/refLineP2LP2L.h"
#include "refLine/refLineP2LC2P.h"
#include "refLine/refLineL2LC2P.h"
#include "refLine/refLineC2PC2P.h"
#include "refMark/refMark.h"
#include "refMark/refMarkIntersection.h"

template class RefContainer<RefLine>;
template RefContainer<RefLine>::RefContainer();
template void RefContainer<RefLine>::FlushBuffer();
template void RefContainer<RefLine>::Rebuild();
template void RefContainer<RefLine>::Add(RefLine*);
template void RefContainer<RefLine>::AddCopyIfValidAndUnique<RefLine_L2L>(RefLine_L2L const&);
template void RefContainer<RefLine>::AddCopyIfValidAndUnique<RefLine_P2P>(RefLine_P2P const&);
template void RefContainer<RefLine>::AddCopyIfValidAndUnique<RefLine_L2L_P2L>(RefLine_L2L_P2L const&);
template void RefContainer<RefLine>::AddCopyIfValidAndUnique<RefLine_P2L_P2L>(RefLine_P2L_P2L const&);
template void RefContainer<RefLine>::AddCopyIfValidAndUnique<RefLine_P2L_C2P>(RefLine_P2L_C2P const&);
template void RefContainer<RefLine>::AddCopyIfValidAndUnique<RefLine_L2L_C2P>(RefLine_L2L_C2P const&);
template void RefContainer<RefLine>::AddCopyIfValidAndUnique<RefLine_C2P_C2P>(RefLine_C2P_C2P const&);
template void RefContainer<RefLine>::ClearMaps();

template class RefContainer<RefMark>;
template RefContainer<RefMark>::RefContainer();
template void RefContainer<RefMark>::FlushBuffer();
template void RefContainer<RefMark>::Rebuild();
template void RefContainer<RefMark>::AddCopyIfValidAndUnique<RefMark_Intersection>(RefMark_Intersection const&);
template void RefContainer<RefMark>::ClearMaps();

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
RefContainer<R>::RefContainer() : rcsz(0), rcbz(0)
{
  // expand our map array to hold all the ranks that we will create
  maps.resize(1 + ReferenceFinder::sMaxRank);
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
void RefContainer<R>::AddCopyIfValidAndUnique(const Rs& ars)
{
  // The ref is valid (fully constructed) if its key is something other than 0.
  // It's unique if the container doesn't already have one with the same key in
  // one of the rank maps.
  if (ars.mKey != 0 && !Contains(&ars)) Add(new Rs(ars));
  // ReferenceFinder::CheckDatabaseStatus();  // report progress if appropriate
}


/*****
Rebuild all arrays and related counters.
*****/
template <class R>
void RefContainer<R>::Rebuild()
{
  rcsz = 0;
  rcbz = 0;
  this->resize(0);
  maps.resize(0);
  maps.resize(1 + ReferenceFinder::sMaxRank);
}


/*****
Return true if the container (or the buffer) contain an equivalent object.
*****/
template <class R>
bool RefContainer<R>::Contains(const R* ar) const
{
  // go through each rank and look for an object with the same key. If we find one,
  // return true.
  for (size_t ir = 0; ir < maps.size(); ir++) {
    if (maps[ir].count(ar->mKey)) return true;
  }

  // Also check the buffer.
  if (buffer.count(ar->mKey)) return true;

  // Still here? then we didn't find it.
  return false;
}


/*****
Add an element to the array; to be called only if an equivalent element isn't
already present anywhere. To avoid corrupting iterators, we add the new element
to the buffer; it will get added to the main container on the next call to
FlushBuffer().
*****/
template<class R>
void RefContainer<R>::Add(R* ar)
{
  // Add it to the buffer and increment the buffer size.
  buffer.insert(typename map_t::value_type(ar->mKey, ar));
  rcbz++;
}


/*****
Put the contents of the buffer into the main container.
*****/
template <class R>
void RefContainer<R>::FlushBuffer()
{
  // Make room for the buffer in the sortable list.
  // Tsai: Add this
  this->reserve(this->size() + rcbz);

  // Go through the buffer and add each element to the appropriate rank in the main container.

  rank_iterator bi = buffer.begin();
  while (bi != buffer.end()) {
    R*& rr = bi->second;        // get pointer to each new element
    maps[rr->mRank].insert(*bi);    // add to the map of the appropriate rank
	// Tsai: Add this
    this->push_back(rr);            // also add to our sortable list
    rcsz++;               // increment our size counter
    bi++;               // increment the buffer iterator
  };
  buffer.clear();             // clear the buffer
  rcbz = 0;
}


/*****
Clear the map arrays. Called when they're no longer needed.
*****/
template <class R>
void RefContainer<R>::ClearMaps()
{
  for (size_t ir = 0; ir < maps.size(); ir++) maps[ir].clear();
}
