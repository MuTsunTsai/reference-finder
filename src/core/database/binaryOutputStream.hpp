
#pragma once

#include "global/global.h"

#include <iostream>

class BinaryOutputStream {
  public:
	BinaryOutputStream(std::ostream &os): os_(os) {}

	template <typename T>
	BinaryOutputStream &operator<<(const T &value) {
		os_.write(reinterpret_cast<const char *>(&value), sizeof(T));
		return *this;
	}

	/** Values of type `index_t` are exported in a special way. */
	BinaryOutputStream &operator<<(const index_t &value) {
		os_.write(reinterpret_cast<const char *>(&value), Shared::indexBytes);
		return *this;
	}

	BinaryOutputStream &operator<<(const std::string &str) {
		// Here the string size is exported in variable byte size,
		// while it really should have been exported in fixed size.
		// This is a historical mistake, and changing it will result
		// in creating a new DB schema, which doesn't seem necessary at this point.
		// If, in the future we really need a new schema, we can fix this then.
		index_t size = str.size();

		*this << size;
		os_.write(str.data(), size);
		return *this;
	}

  private:
	std::ostream &os_;
};
