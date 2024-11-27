
#pragma once

#include <sstream>

class JsonObject;

class JsonArray {
  public:
	JsonArray();
	void clear();
	void add(const JsonObject &obj);
	void add(const JsonArray &array);
	void add(const int value);
	void add(const double value);

  private:
	std::stringstream mStream;
	bool mStarted{false};

	friend std::ostream &operator<<(std::ostream &os, const JsonArray &j);
};
