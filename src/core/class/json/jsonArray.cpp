
#include "jsonArray.h"
#include "jsonObject.h"

using namespace std;

JsonArray::JsonArray() : mStarted(false) {}

void JsonArray::clear() {
	mStarted = false;
	mStream.str(""); // clear the stream
}

void JsonArray::add(const JsonObject &obj) {
	if (!obj.mStarted) return; // ignore empty object
	if (mStarted) mStream << ",";
	mStarted = true;
	mStream << obj;
}

void JsonArray::add(const JsonArray &array) {
	if (mStarted) mStream << ",";
	mStarted = true;
	mStream << array;
}

void JsonArray::add(const int value) {
	if (mStarted) mStream << ",";
	mStarted = true;
	mStream << value;
}

void JsonArray::add(const double value) {
	if (mStarted) mStream << ",";
	mStarted = true;
	mStream << value;
}

ostream &operator<<(ostream &os, const JsonArray &j) {
	return os << "[" << (j.mStream.str()) << "]";
}
