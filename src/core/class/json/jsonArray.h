
#ifndef _JSON_ARRAY_H_
#define _JSON_ARRAY_H_

#include <sstream>

class JsonObject;

class JsonArray {
  public:
	JsonArray();
	void reset();
	void add(const JsonObject &obj);
	void add(const JsonArray &array);
	void add(const int value);

  private:
	std::stringstream mStream;
	bool mStarted;

	friend std::ostream &operator<<(std::ostream &os, const JsonArray &j);
};

#endif
