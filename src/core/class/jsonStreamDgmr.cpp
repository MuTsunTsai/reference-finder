
#include "../ReferenceFinder.h"

#include "jsonStreamDgmr.h"
#include "paper.h"

using namespace std;

/**********
class JsonStreamDgmr is a variation of the original VerbalStreamDgmr.
**********/

/*****
Constructor
******/
JsonStreamDgmr::JsonStreamDgmr(ostream &aStream) : RefDgmr(), mStream(&aStream) {
}

/*****
Write a list of references to the stream, along with their error and how-to
description. This template function is used in the mark- and line-specific
routines that follow.
*****/
template <class R>
void JsonStreamDgmr::PutRefList(const typename R::bare_t &ar, vector<R *> &vr) {
	for (size_t i = 0; i < vr.size(); i++) {
		vr[i]->PutDistanceAndRank(*mStream, ar);
		vr[i]->BuildAndNumberSequence();
		vr[i]->PutHowtoSequence(*mStream);
		(*mStream) << "], \"diagrams\": [";

		vr[i]->BuildDiagrams();
		for (size_t j = 0; j < RefBase::sDgms.size(); j++) {
			(*mStream) << "[";
			RefBase::DrawDiagram(*this, RefBase::sDgms[j]);
			// We don't really know how many elements are drawn,
			// so we add a `null` at the end.
			(*mStream) << "null]";
			if (j < RefBase::sDgms.size() - 1)
				(*mStream) << ", ";
		};

		(*mStream) << "]}" << endl;
	};
}

/*****
Write a list of lines to the stream, including error and how-to description.
*****/
void JsonStreamDgmr::PutLineList(const XYLine &ll, vector<RefLine *> &vl) {
	PutRefList(ll, vl);
}

/*****
Write a list of marks to the stream, including error and how-to description.
*****/
void JsonStreamDgmr::PutMarkList(const XYPt &pp, vector<RefMark *> &vm) {
	PutRefList(pp, vm);
}

/*****
Draw a point in the indicated style.
*****/
void JsonStreamDgmr::DrawPt(const XYPt &aPt, PointStyle pstyle) {
	(*mStream) << "{\"type\": 0, \"pt\": " << aPt
			   << ", \"style\": " << pstyle << "}, ";
}

/*****
Draw a line in the indicated style.
*****/
void JsonStreamDgmr::DrawLine(const XYPt &fromPt, const XYPt &toPt,
							  LineStyle lstyle) {
	(*mStream) << "{\"type\": 1, \"from\": " << fromPt
			   << ", \"to\": " << toPt
			   << ", \"style\": " << lstyle << "}, ";
}

/*****
Draw an arc in the indicated style.
*****/
void JsonStreamDgmr::DrawArc(const XYPt &ctr, double rad, double fromAngle,
							 double toAngle, bool ccw, LineStyle lstyle) {
	(*mStream) << "{\"type\": 2, \"center\": " << ctr
			   << ", \"radius\": " << rad
			   << ", \"from\": " << fromAngle
			   << ", \"to\": " << toAngle
			   << ", \"ccw\": " << ccw
			   << ", \"style\": " << lstyle << "}, ";
}

/*****
Fill and stroke the given poly in the indicated style.
*****/
void JsonStreamDgmr::DrawPoly(const vector<XYPt> &poly, PolyStyle pstyle) {
	// In practice, the method is called only for drawing sheet boundary.
	(*mStream) << "{\"type\": 3, \"width\": " << ReferenceFinder::sPaper.mWidth
			   << ", \"height\": " << ReferenceFinder::sPaper.mHeight
			   << "}, ";
}

/*****
Draw a text label at the point aPt in the indicated style
*****/
void JsonStreamDgmr::DrawLabel(const XYPt &aPt, const string &aString,
							   LabelStyle lstyle) {
	(*mStream) << "{\"type\": 4, \"pt\": " << aPt
			   << ", \"text\": \"" << aString
			   << "\", \"style\": " << lstyle
			   << "}, ";
}
