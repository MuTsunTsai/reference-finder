
#include "jsonObject.h"
#include "jsonArray.h"

using namespace std;

JsonObject::JsonObject() {
	// Set the precision so that double numbers are printed with more digits
	mStream.precision(15);
}

void JsonObject::add(char const *key, const string &value) {
	addKey(key);
	mStream << "\"" << value << "\"";
}

void JsonObject::add(char const *key, char value) {
	addKey(key);
	mStream << "\"" << value << "\"";
}

void JsonObject::add(char const *key, int value) {
	addKey(key);
	mStream << value;
}

void JsonObject::add(char const *key, double value) {
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

void JsonObject::add(char const *key, const JsonObject &obj) {
	addKey(key);
	mStream << obj;
}

void JsonObject::add(char const *key, const JsonArray &array) {
	addKey(key);
	mStream << array;
}

void JsonObject::addKey(char const *key) {
	if(mStarted) mStream << ",";
	mStarted = true;
	mStream << "\"" << key << "\":";
}

ostream &operator<<(ostream &os, const JsonObject &j) {
	return os << "{" << (j.mStream.str()) << "}";
}
