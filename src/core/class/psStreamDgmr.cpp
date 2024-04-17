/******************************************************************************
This module is not used in WASM build, but we keep it for reference.
******************************************************************************/

#include "../ReferenceFinder.h"

#include "psStreamDgmr.h"
#include "paper.h"

using namespace std;

/**********
class PSStreamDgmr - a specialization of RefDgmr that writes a PostScript
stream of diagrams.
**********/

/* Notes on class PSStreamDgmr.
This class is used in the command-line version of ReferenceFinder to create
a Postscript graphics file of folding diagrams. It's also a good model for how
to implement a true graphic outputter.
*/

/*****
PSStreamDgmr static member initialization
*****/
double PSStreamDgmr::sPSUnit = 64;    // 72 pts = 1 inch, 1 unit = 64 pts, fits 7 dgms
const XYRect PSStreamDgmr::sPSPageSize(40, 40, 572, 752);  // printable area on the page


/*****
Constructor
*****/
PSStreamDgmr::PSStreamDgmr(ostream& os) :
   mStream(&os),
   mPSPageCount(0)
{
}


/*****
Stream output for a PostScript point
*****/
ostream& operator<<(ostream& os, const PSStreamDgmr::PSPt& pp)
{
  return os << pp.px << " " << pp.py;
}


/*****
Set the current graphics state to the given PointStyle.
*****/
void PSStreamDgmr::SetPointStyle(PointStyle pstyle)
{
  switch (pstyle) {
    case POINTSTYLE_NORMAL:
      (*mStream) << "1 setlinewidth 0 setgray " << endl;
      break;
    case POINTSTYLE_HILITE:
      (*mStream) << "3 setlinewidth .5 .25 .25 setrgbcolor " << endl;
      break;
    case POINTSTYLE_ACTION:
      (*mStream) << "3 setlinewidth .5 0 0 setrgbcolor " << endl;
      break;
  }
}


/*****
Set the current graphics state to the given LineStyle
*****/
void PSStreamDgmr::SetLineStyle(LineStyle lstyle)
{
  switch (lstyle) {
    case LINESTYLE_CREASE:
      (*mStream) << "[] 0 setdash .20 setlinewidth 0 setgray " << endl;
      break;
    case LINESTYLE_EDGE:
      (*mStream) << "[] 0 setdash .5 setlinewidth 0 setgray " << endl;
      break;
    case LINESTYLE_HILITE:
      (*mStream) << "[] 0 setdash 1 setlinewidth 1 .5 .5 setrgbcolor " << endl;
      break;
    case LINESTYLE_VALLEY:
      (*mStream) << "[4 3] 0 setdash .5 setlinewidth .5 .5 0 setrgbcolor " << endl;
      break;
    case LINESTYLE_MOUNTAIN:
      (*mStream) << "[3 3 0 3 0 3] 0 setdash .5 setlinewidth 0 0 0 setrgbcolor " << 
        endl;
      break;
    case LINESTYLE_ARROW:
      (*mStream) << "[] 0 setdash .4 setlinewidth 0 .5 0 setrgbcolor " << endl;
      break;
  }
}


/*****
Set the current graphics state to the given PolyStyle
*****/
void PSStreamDgmr::SetPolyStyle(PolyStyle pstyle)
{
  switch (pstyle) {
    case POLYSTYLE_WHITE:
      (*mStream) << ".95 .95 1 setrgbcolor " << endl;
      break;
    case POLYSTYLE_COLORED:
      (*mStream) << "0 0 .5 setrgbcolor " << endl;
      break;
    case POLYSTYLE_ARROW:
      (*mStream) << ".95 1 .95 setrgbcolor " << endl;
      break;
  }
}


/*****
Set the current graphics state to the given LabelStyle
*****/
void PSStreamDgmr::SetLabelStyle(LabelStyle lstyle)
{
  switch (lstyle) {
    case LABELSTYLE_NORMAL:
      (*mStream) << "0 setgray " << endl;
      break;
    case LABELSTYLE_HILITE:
      (*mStream) << ".5 .25 .25 setrgbcolor " << endl;
      break;
    case LABELSTYLE_ACTION:
      (*mStream) << ".5 0 0 setrgbcolor " << endl;
      break;
  }
}


/*****
Coordinate conversion
*****/
PSStreamDgmr::PSPt PSStreamDgmr::ToPS(const XYPt& aPt)
{
  return PSPt(mPSOrigin.x + sPSUnit * aPt.x, mPSOrigin.y + sPSUnit * aPt.y);
}


/*****
Draw a PostScript point in the indicated style.
*****/
void PSStreamDgmr::DrawPt(const XYPt& aPt, PointStyle pstyle)
{
  SetPointStyle(pstyle);
  (*mStream) << "newpath " << ToPS(aPt) << " moveto 0 0 rlineto stroke" << endl;
}


/*****
Draw a PostScript line in the indicated style.
*****/
void PSStreamDgmr::DrawLine(const XYPt& fromPt, const XYPt& toPt, 
  LineStyle lstyle)
{
  SetLineStyle(lstyle);
  (*mStream) << "newpath " << ToPS(fromPt) << " moveto " << ToPS(toPt) << 
    " lineto stroke" << endl;
}


/*****
Draw a PostScript arc in the indicated style.
*****/
void PSStreamDgmr::DrawArc(const XYPt& ctr, double rad, double fromAngle,
  double toAngle, bool ccw, LineStyle lstyle)
{
  SetLineStyle(lstyle);
  const double RADIANS = 57.29577951;
  if (ccw)
    (*mStream) << "newpath " << ToPS(ctr) << " " << rad * sPSUnit << " " << 
    fromAngle * RADIANS << " " << toAngle * RADIANS  << " arc stroke" << endl;
  else
    (*mStream) << "newpath " << ToPS(ctr) << " " << rad * sPSUnit << " " << 
    fromAngle * RADIANS  << " " << toAngle * RADIANS  << " arcn stroke" << endl;
}


/*****
Fill and stroke the given poly in the indicated style.
*****/
void PSStreamDgmr::DrawPoly(const vector<XYPt>& poly, PolyStyle pstyle)
{
  (*mStream) << "newpath " << ToPS(poly[poly.size()-1]) << " moveto " << endl;
  for (size_t i = 0; i < poly.size(); i++)
    (*mStream) << ToPS(poly[i]) << " lineto" << endl;
  (*mStream) << "gsave " << endl;

  // Fill the poly
  SetPolyStyle(pstyle);
  (*mStream) << "fill grestore " << endl;
  
  // Stroke the poly
  switch (pstyle) {
    case POLYSTYLE_WHITE:
    case POLYSTYLE_COLORED:
      SetLineStyle(LINESTYLE_EDGE);
      break;
    case POLYSTYLE_ARROW:
      SetLineStyle(LINESTYLE_ARROW);
      break;
  };
  (*mStream) << "stroke " << endl;
}


/*****
Draw a text label at the point aPt in the indicated style
*****/
void PSStreamDgmr::DrawLabel(const XYPt& aPt, const string& aString, 
  LabelStyle lstyle)
{
  SetLabelStyle(lstyle);
  (*mStream) << ToPS(aPt) << " moveto (" << aString << ") show " << endl;
}


/*****
Decrement the mPSOrigin by a distance d. If we drop below the bottom margin,
start a new page.
*****/

void PSStreamDgmr::DecrementOrigin(double d)
{
  mPSOrigin.y -= d;
  if (mPSOrigin.y >= sPSPageSize.bl.y) return;
  (*mStream) << "showpage" << endl;
  (*mStream) << "%%Page: " << ++mPSPageCount;
  (*mStream) << " " << mPSPageCount << endl;
  mPSOrigin.y = sPSPageSize.tr.y - d;
}

  
/*****
Draw a set of marks or lines to a PostScript stream, showing distance and rank
for each sequence.
*****/
template <class R>
void PSStreamDgmr::PutRefList(const typename R::bare_t& ar, vector<R*>& vr)
{
  ReferenceFinder::sClarifyVerbalAmbiguities = false;
  ReferenceFinder::sAxiomsInVerbalDirections = false;

  // Put some comments so our readers are happy 
  (*mStream) << "%!PS-Adobe-1.0" << endl;
  (*mStream) << "%%Pages: (atend)" << endl;
  (*mStream) << "%%EndComments" << endl;
  (*mStream) << "%%Page: 1 1" << endl;
  
  // Set the page number. DecrementOrigin will update it as needed
  mPSPageCount = 1;
  
  // Put some initial setup information 
  (*mStream) << "1 setlinecap" << endl;
  (*mStream) << "1 setlinejoin" << endl;
  
  // Setup and draw a header. 
  mPSOrigin.x = sPSPageSize.bl.x;
  mPSOrigin.y = sPSPageSize.tr.y;
  (*mStream) << "/Times-Roman findfont 12 scalefont setfont" << endl;
  (*mStream) << "0 setgray" << endl;
  DecrementOrigin(12);
  DrawLabel(XYPt(0), "ReferenceFinder 4.0 by Robert J. Lang", LABELSTYLE_NORMAL);
  
  // Note the point we're searching for.
  (*mStream) << "/Times-Roman findfont 9 scalefont setfont" << endl;
  DecrementOrigin(12);
  stringstream targstr;
  targstr << "Paper: \\(" << ReferenceFinder::sPaper.mWidthAsText.c_str()
	  << " x " << ReferenceFinder::sPaper.mHeightAsText.c_str()
	  << "\\), Target: " << ar;
  DrawLabel(XYPt(0), targstr.str(), LABELSTYLE_NORMAL);
  
  // Go through our list and draw all the diagrams in a single row. 
  for (size_t irow = 0; irow < vr.size(); irow++) {
    DecrementOrigin(1.2 * sPSUnit * ReferenceFinder::sPaper.mHeight);
    vr[irow]->BuildDiagrams();
    mPSOrigin.x = sPSPageSize.bl.x;
    for (size_t icol = 0; icol < RefBase::sDgms.size(); icol++) {
      RefBase::DrawDiagram(*this, RefBase::sDgms[icol]);
      mPSOrigin.x += 1.2 * ReferenceFinder::sPaper.mWidth * sPSUnit;
    };
    
    // Also put the text description below the diagrams   
    mPSOrigin.x = sPSPageSize.bl.x;
    DecrementOrigin(11);
    ostringstream sd;
    vr[irow]->PutDistanceAndRank(sd, ar);
    DrawLabel(XYPt(0), sd.str(), LABELSTYLE_NORMAL);
    for (size_t i = 0; i < RefBase::sSequence.size(); i++) {
      mPSOrigin.x = sPSPageSize.bl.x;
      ostringstream s;
      if (RefBase::sSequence[i]->PutHowto(s)) {
        DecrementOrigin(11);
        s << ".";
        DrawLabel(XYPt(0), s.str(), LABELSTYLE_NORMAL);
      }
    }
  }
  
  // Close the file.  
  (*mStream) << "showpage" << endl;
  (*mStream) << "%%Trailer" << endl;
  (*mStream) << "%%Pages: " << mPSPageCount << endl;
}


/*****
Write the PostScript code that draws folding sequences for a list of marks.
*****/

void PSStreamDgmr::PutMarkList(const XYPt& pp, vector<RefMark*>& vm)
{
  PutRefList(pp, vm);
}


/*****
Write the PostScript code that draws folding sequences for a list of lines.
*****/
void PSStreamDgmr::PutLineList(const XYLine& ll, vector<RefLine*>& vl)
{
  PutRefList(ll, vl);
}
