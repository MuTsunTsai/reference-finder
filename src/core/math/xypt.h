
#pragma once

#include <cmath>
#include <iostream>

#include "database/binaryOutputStream.hpp"
#include "global/global.h"

/**********
struct XYPt - a 2-vector that represents a point or a direction.
**********/
struct XYPt {
	double x; // x coordinate
	double y; // y coordinate

	// Constructor
	XYPt(double xx = 0, double yy = 0): x(xx), y(yy) {}

	// Arithmetic with XYPts and scalars
	XYPt operator+(const XYPt &p) const { return {x + p.x, y + p.y}; }
	XYPt operator-(const XYPt &p) const { return {x - p.x, y - p.y}; }
	XYPt operator*(const XYPt &p) const { return {x * p.x, y * p.y}; }
	XYPt operator/(const XYPt &p) const { return {x / p.x, y / p.y}; }

	XYPt operator+(double z) const { return {x + z, y + z}; }
	XYPt operator-(double z) const { return {x - z, y - z}; }
	XYPt operator*(double z) const { return {x * z, y * z}; }
	XYPt operator/(double z) const { return {x / z, y / z}; }

	friend XYPt operator+(double d, const XYPt &pp) {
		return {d + pp.x, d + pp.y};
	}
	friend XYPt operator-(double d, const XYPt &pp) {
		return {d - pp.x, d - pp.y};
	}
	friend XYPt operator*(double d, const XYPt &pp) {
		return {d * pp.x, d * pp.y};
	}
	friend XYPt operator/(double d, const XYPt &pp) {
		return {d / pp.x, d / pp.y};
	}

	XYPt &operator+=(const XYPt &p) {
		x += p.x;
		y += p.y;
		return (*this);
	}
	XYPt &operator-=(const XYPt &p) {
		x -= p.x;
		y -= p.y;
		return (*this);
	}
	XYPt &operator*=(const XYPt &p) {
		x *= p.x;
		y *= p.y;
		return (*this);
	}
	XYPt &operator/=(const XYPt &p) {
		x /= p.x;
		y /= p.y;
		return (*this);
	}

	XYPt &operator+=(double z) {
		x += z;
		y += z;
		return (*this);
	}
	XYPt &operator-=(double z) {
		x -= z;
		y -= z;
		return (*this);
	}
	XYPt &operator*=(double z) {
		x *= z;
		y *= z;
		return (*this);
	}
	XYPt &operator/=(double z) {
		x /= z;
		y /= z;
		return (*this);
	}

	// Counterclockwise rotation
	XYPt Rotate90() const { return {-y, x}; }
	XYPt RotateCCW(double a) const { // a is in radians
		double sa = std::sin(a);
		double ca = std::cos(a);
		return {ca * x - sa * y, sa * x + ca * y};
	}

	// Scalar products and norms
	double Dot(const XYPt &p) const {
		return x * p.x + y * p.y;
	}
	double Mag2() const {
		return x * x + y * y;
	}
	double Mag() const {
		return std::sqrt(x * x + y * y);
	}
	XYPt Normalize() const {
		double m = Mag();
		return {x / m, y / m};
	}
	XYPt &NormalizeSelf() {
		double m = Mag();
		x /= m;
		y /= m;
		return *this;
	}

	// Other utilities
	friend XYPt MidPoint(const XYPt &p1, const XYPt &p2) {
		return {0.5 * (p1.x + p2.x), 0.5 * (p1.y + p2.y)};
	}

	// Chop() makes numbers close to zero equal to zero.
	XYPt Chop() const {
		return {std::abs(x) < EPS ? 0 : x, std::abs(y) < EPS ? 0 : y};
	}
	XYPt &ChopSelf() {
		if(std::abs(x) < EPS) x = 0;
		if(std::abs(y) < EPS) y = 0;
		return *this;
	}

	// Comparison
	bool equals(const XYPt &p) const {
		return (*this - p).Mag() < EPS;
	}

	// Stream I/O
	friend std::ostream &operator<<(std::ostream &os, const XYPt &p);

	friend BinaryOutputStream &operator<<(BinaryOutputStream &os, const XYPt &p);
};
