
#ifndef _XYPT_H_
#define _XYPT_H_

#include <cmath>
#include <iostream>

#include "database/binaryOutputStream.hpp"
#include "global/global.h"

/**********
class XYPt - a 2-vector that represents a point or a direction.
**********/
class XYPt {
  public:
	double x; // x coordinate
	double y; // y coordinate

	// Constructor
	XYPt(double xx = 0, double yy = 0) : x(xx), y(yy) {}

	// Arithmetic with XYPts and scalars
	const XYPt operator+(const XYPt &p) const { return XYPt(x + p.x, y + p.y); }
	const XYPt operator-(const XYPt &p) const { return XYPt(x - p.x, y - p.y); }
	const XYPt operator*(const XYPt &p) const { return XYPt(x * p.x, y * p.y); }
	const XYPt operator/(const XYPt &p) const { return XYPt(x / p.x, y / p.y); }

	const XYPt operator+(double z) const { return XYPt(x + z, y + z); }
	const XYPt operator-(double z) const { return XYPt(x - z, y - z); }
	const XYPt operator*(double z) const { return XYPt(x * z, y * z); }
	const XYPt operator/(double z) const { return XYPt(x / z, y / z); }

	friend const XYPt operator+(double d, const XYPt &pp) {
		return XYPt(d + pp.x, d + pp.y);
	}
	friend const XYPt operator-(double d, const XYPt &pp) {
		return XYPt(d - pp.x, d - pp.y);
	}
	friend const XYPt operator*(double d, const XYPt &pp) {
		return XYPt(d * pp.x, d * pp.y);
	}
	friend const XYPt operator/(double d, const XYPt &pp) {
		return XYPt(d / pp.x, d / pp.y);
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
	const XYPt Rotate90() const { return XYPt(-y, x); }
	const XYPt RotateCCW(double a) const { // a is in radians
		double sa = std::sin(a);
		double ca = std::cos(a);
		return XYPt(ca * x - sa * y, sa * x + ca * y);
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
	const XYPt Normalize() const {
		double m = Mag();
		return XYPt(x / m, y / m);
	}
	XYPt &NormalizeSelf() {
		double m = Mag();
		x /= m;
		y /= m;
		return *this;
	}

	// Other utilities
	friend const XYPt MidPoint(const XYPt &p1, const XYPt &p2) {
		return XYPt(0.5 * (p1.x + p2.x), 0.5 * (p1.y + p2.y));
	}

	// Chop() makes numbers close to zero equal to zero.
	const XYPt Chop() const {
		return XYPt(std::abs(x) < EPS ? 0 : x, std::abs(y) < EPS ? 0 : y);
	}
	XYPt &ChopSelf() {
		if (std::abs(x) < EPS) x = 0;
		if (std::abs(y) < EPS) y = 0;
		return *this;
	}

	// Comparison
	bool operator==(const XYPt &p) const {
		return (*this - p).Mag() < EPS;
	}

	// Stream I/O
	friend std::ostream &operator<<(std::ostream &os, const XYPt &p);

	friend BinaryOutputStream &operator<<(BinaryOutputStream &os, const XYPt &p);
};

#endif
