
#pragma once

#include "math/xyline.h"
#include "math/xypt.h"

#include <sstream>

class JsonArray;

class JsonObject {
  public:
	JsonObject();
	void add(char const *key, const std::string &value);
	void add(char const *key, char value);
	void add(char const *key, int value);
	void add(char const *key, double value);
	void add(char const *key, const XYPt &pt);
	void add(char const *key, const XYLine &pt);
	void add(char const *key, const JsonObject &obj);
	void add(char const *key, const JsonArray &array);

#ifdef _DEBUG_DB_
	void add(char const *key, unsigned int value);
#endif

  private:
	std::stringstream mStream;
	bool mStarted{false};

	void addKey(char const *key);

	friend JsonArray;

	friend std::ostream &operator<<(std::ostream &os, const JsonObject &j);
};
