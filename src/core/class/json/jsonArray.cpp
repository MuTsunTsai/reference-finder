
#include "jsonArray.h"
#include "jsonObject.h"

using namespace std;

JsonArray::JsonArray() {}

void JsonArray::add(const JsonObject &obj) {
	if (mStarted) mStream << ",";
	mStarted = true;
	mStream << obj;
}

void JsonArray::add(const JsonArray &array) {
	if (mStarted) mStream << ",";
	mStarted = true;
	mStream << array;
}

ostream &operator<<(ostream &os, const JsonArray &j) {
	return os << "[" << (j.mStream.str()) << "]";
}
