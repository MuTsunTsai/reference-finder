
#pragma once

#include "global/global.h"

#include <iostream>

class BinaryInputStream {
  public:
	BinaryInputStream(std::istream &is): is_(is) {}

	template <typename T>
	BinaryInputStream &read(T &value) {
		is_.read(reinterpret_cast<char *>(&value), sizeof(T));
		return *this;
	}

	/** Values of type `index_t` are imported in a special way. */
	BinaryInputStream &read(index_t &value) {
		value = 0; // Clear all bytes first
		is_.read(reinterpret_cast<char *>(&value), Shared::indexBytes);
		return *this;
	}

	BinaryInputStream &read(std::string &str) {
		// See BinaryOutputStream for a comment on this.
		index_t size;

		read(size);
		str.resize(size);
		is_.read(str.data(), size);
		return *this;
	}

  private:
	std::istream &is_;
};
