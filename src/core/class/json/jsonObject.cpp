
#include "jsonObject.h"
#include "jsonArray.h"

using namespace std;

JsonObject::JsonObject() {}

void JsonObject::add(char const *key, const std::string &value) {
	addKey(key);
	mStream << "\"" << value << "\"";
}

void JsonObject::add(char const *key, const char value) {
	addKey(key);
	mStream << "\"" << value << "\"";
}

void JsonObject::add(char const *key, const int value) {
	addKey(key);
	mStream << value;
}

void JsonObject::add(char const *key, const double value) {
	addKey(key);
	mStream << value;
}

void JsonObject::add(char const *key, const XYPt &pt) {
	addKey(key);
	mStream << pt;
}

void JsonObject::add(char const *key, const XYLine &pt) {
	addKey(key);
	mStream << pt;
}

void JsonObject::add(char const *key, const JsonArray &array) {
	addKey(key);
	mStream << array;
}

void JsonObject::addKey(char const *key) {
	if (mStarted) mStream << ",";
	mStarted = true;
	mStream << "\"" << key << "\":";
}

ostream &operator<<(ostream &os, const JsonObject &j) {
	return os << "{" << (j.mStream.str()) << "}";
}
