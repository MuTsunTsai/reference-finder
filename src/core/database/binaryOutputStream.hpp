
#pragma once

#include "global/global.h"

#include <iostream>

class BinaryOutputStream {
  public:
	BinaryOutputStream(std::ostream &os) : os_(os) {}

	template <typename T>
	BinaryOutputStream &operator<<(const T &value) {
		os_.write(reinterpret_cast<const char *>(&value), sizeof(T));
		return *this;
	}

	BinaryOutputStream &operator<<(const size_t &value) {
		os_.write(reinterpret_cast<const char *>(&value), Shared::sizeBytes);
		return *this;
	}

	BinaryOutputStream &operator<<(const std::string &str) {
		size_t size = str.size();
		*this << size;
		os_.write(str.data(), size);
		return *this;
	}

  private:
	std::ostream &os_;
};
