
#pragma once

#include "xyline.h"
#include "xyrect.h"

/**********
class Paper - specialization of XYRect for representing the paper
**********/
class Paper: public XYRect {
  public:
	double mWidth;			  // width of the paper
	double mHeight;			  // height of the paper
	XYPt mBotLeft;			  // only used by BuildAndExportDatabase
	XYPt mBotRight;			  // ditto
	XYPt mTopLeft;			  // ditto
	XYPt mTopRight;			  // ditto
	XYLine mTopEdge;		  // used by InteriorOverlaps() and BuildAndExportDatabase
	XYLine mLeftEdge;		  // ditto
	XYLine mRightEdge;		  // ditto
	XYLine mBottomEdge;		  // ditto
	XYLine mUpwardDiagonal;	  // only used by BuildAndExportDatabase
	XYLine mDownwardDiagonal; // ditto

	std::string mWidthAsText;  // paper width, as an expression
	std::string mHeightAsText; // paper height, as an expression

	Paper(double aWidth, double aHeight);
	void SetSize(double aWidth, double aHeight);

	bool ClipLine(const XYLine &al, XYPt &ap1, XYPt &ap2) const;
	bool InteriorOverlaps(const XYLine &al) const;
	bool MakesSkinnyFlap(const XYLine &al) const;
};
