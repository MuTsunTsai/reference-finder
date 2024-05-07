
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
		JsonObject *solution = new JsonObject();
		vr[i]->PutDistanceAndRank(*solution, ar);
		vr[i]->BuildAndNumberSequence();
		vr[i]->PutHowtoSequence(*solution);

		JsonArray *diagrams = new JsonArray();
		vr[i]->BuildDiagrams();
		for (size_t j = 0; j < RefBase::sDgms.size(); j++) {
			mArray = new JsonArray();
			RefBase::DrawDiagram(*this, RefBase::sDgms[j]);
			diagrams->add(*mArray);
			delete mArray;
		};
		solution->add("diagrams", *diagrams);

		(*mStream) << *solution << endl;

		delete solution;
		delete diagrams;
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
	JsonObject *j = new JsonObject();
	j->add("type", 0);
	j->add("pt", aPt);
	j->add("style", pstyle);
	mArray->add(*j);
	delete j;
}

/*****
Draw a line in the indicated style.
*****/
void JsonStreamDgmr::DrawLine(const XYPt &fromPt, const XYPt &toPt, LineStyle lstyle) {
	JsonObject *j = new JsonObject();
	j->add("type", 1);
	j->add("from", fromPt);
	j->add("to", toPt);
	j->add("style", lstyle);
	mArray->add(*j);
	delete j;
}

/*****
Draw an arc in the indicated style.
*****/
void JsonStreamDgmr::DrawArc(const XYPt &ctr, double rad, double fromAngle, double toAngle, bool ccw, LineStyle lstyle) {
	JsonObject *j = new JsonObject();
	j->add("type", 2);
	j->add("center", ctr);
	j->add("radius", rad);
	j->add("from", fromAngle);
	j->add("to", toAngle);
	j->add("ccw", ccw);
	j->add("style", lstyle);
	mArray->add(*j);
	delete j;
}

/*****
Fill and stroke the given poly in the indicated style.
*****/
void JsonStreamDgmr::DrawPoly(const vector<XYPt> &poly, PolyStyle pstyle) {
	// In practice, the method is called only for drawing sheet boundary.
	JsonObject *j = new JsonObject();
	j->add("type", 3);
	j->add("width", ReferenceFinder::sPaper.mWidth);
	j->add("height", ReferenceFinder::sPaper.mHeight);
	mArray->add(*j);
	delete j;
}

/*****
Draw a text label at the point aPt in the indicated style
*****/
void JsonStreamDgmr::DrawLabel(const XYPt &aPt, const string &aString, LabelStyle lstyle) {
	JsonObject *j = new JsonObject();
	j->add("type", 4);
	j->add("pt", aPt);
	j->add("text", aString);
	j->add("style", lstyle);
	mArray->add(*j);
	delete j;
}
