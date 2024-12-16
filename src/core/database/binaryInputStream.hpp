
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

	BinaryInputStream &read(size_t &value) {
		value = 0;
		is_.read(reinterpret_cast<char *>(&value), Shared::sizeBytes);
		return *this;
	}

	BinaryInputStream &read(std::string &str) {
		size_t size;
		read(size);
		str.resize(size);
		is_.read(&str[0], size);
		return *this;
	}

  private:
	std::istream &is_;
};
