
#ifndef _JSON_OBJECT_H_
#define _JSON_OBJECT_H_

#include "math/xypt.h"
#include "math/xyline.h"

#include <sstream>

class JsonArray;

class JsonObject {
  public:
	JsonObject();
	void add(char const *key, const std::string &value);
	void add(char const *key, const char value);
	void add(char const *key, const int value);
	void add(char const *key, const double value);
	void add(char const *key, const XYPt &pt);
	void add(char const *key, const XYLine &pt);
	void add(char const *key, const JsonObject &obj);
	void add(char const *key, const JsonArray &array);

  private:
	std::stringstream mStream;
	bool mStarted{false};

	void addKey(char const *key);

	friend JsonArray;

	friend std::ostream &operator<<(std::ostream &os, const JsonObject &j);
};

#endif
