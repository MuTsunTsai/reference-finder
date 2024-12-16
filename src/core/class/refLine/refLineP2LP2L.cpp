
#include "../refDgmr.h"
#include "ReferenceFinder.h"
#include "math/paper.h"

#include "refLineP2LP2L.h"

using namespace std;

/**********
class RefLine_P2L_P2L - Huzita-Hatori Axiom O6.
Bring point p1 to line l1 and point p2 to line l2.
**********/

/*****
 * RefLine_P2L_P2L static member initialization
 *****/
short RefLine_P2L_P2L::order = 0;
short RefLine_P2L_P2L::irootMax = 0;

double RefLine_P2L_P2L::q1 = 0;
double RefLine_P2L_P2L::q2 = 0;

double RefLine_P2L_P2L::S = 0;
double RefLine_P2L_P2L::Sr = 0;
double RefLine_P2L_P2L::Si = 0;
double RefLine_P2L_P2L::U = 0;

/*****
Take the cube root; works for both positive and negative numbers
*****/
double CubeRoot(double x) {
	if(x >= 0)
		return pow(x, 1. / 3);
	else
		return -pow(-x, 1. / 3);
}

/*****
Constructor. Variable iroot can be 0, 1, or 2.
*****/
RefLine_P2L_P2L::RefLine_P2L_P2L(RefMark *arm1, RefLine *arl1, RefMark *arm2, RefLine *arl2, unsigned char iroot)
	: RefLine(), rm1(arm1), rl1(arl1), rm2(arm2), rl2(arl2), mRoot(iroot) {

	mScore = rm1->mScore + rl1->mScore + rm2->mScore + rl2->mScore + Shared::sAxiomWeights[5];

	// Get references to the points and lines involved in the construction
	XYPt &p1 = rm1->p;
	XYLine &l1 = rl1->l;
	XYPt &u1 = l1.u;
	double &d1 = l1.d;
	XYPt &p2 = rm2->p;
	XYLine &l2 = rl2->l;
	XYPt &u2 = l2.u;
	double &d2 = l2.d;

	// This is by far the most complex alignment, and it involves the solution of
	// a cubic equation.
	XYPt u1p = u1.Rotate90(); // we'll need this later.

	// First, some trivial checks; we can't have p1 already on l1, or p2 already
	// on l2.
	if(l1.Intersects(p1)) return;
	if(l2.Intersects(p2)) return;

	// Also make sure we're using distinct points and lines.
	if((p1 == p2) || (l1 == l2)) return;

	// Now construct the terms of the cubic equation. These are stored in static
	// member variables during the iroot==0 construction; if iroot==1 or 2, we
	// used the stored values.
	double rc = 0; // this will hold the root of the cubic equation after this switch(iroot).
	switch(iroot) {
	case 0: {
		// case iroot==0 computes a bunch of quantities that are used for all roots.
		// Some of these get stored in private static member variables.
		XYPt v1 = p1 + d1 * u1 - 2 * p2;
		XYPt v2 = d1 * u1 - p1;

		double c1 = p2.Dot(u2) - d2;
		double c2 = 2 * v2.Dot(u1p);
		double c3 = v2.Dot(v2);
		double c4 = (v1 + v2).Dot(u1p);
		double c5 = v1.Dot(v2);
		double c6 = u1p.Dot(u2);
		double c7 = v2.Dot(u2);

		// the equation is a * r^3 + b * r^2 + c * r + d == 0
		double a = c6;
		double b = c1 + c4 * c6 + c7;
		double c = c1 * c2 + c5 * c6 + c4 * c7;
		double d = c1 * c3 + c5 * c7;

		// compute the order of the equation
		if(abs(a) > EPS)
			order = 3; // cubic equation
		else if(abs(b) > EPS)
			order = 2; // quadratic equation
		else if(abs(c) > EPS)
			order = 1; // linear equation
		else
			order = 0; // ill-formed equation (no variables!)

		// what we do next depends on the order of the equation.
		switch(order) {
		case 0: // ill-formed equation has 0 roots
			return;

		case 1: // linear equation has 1 root
		{
			rc = -d / c;
		} break;

		case 2: // quadratic equation has 0, 1 or 2 roots
		{
			double disc = pow(c, 2) - 4 * b * d;
			q1 = -c / (2 * b);
			if(disc < 0) {
				irootMax = -1; // no roots
				return;
			} else if(abs(disc) < EPS) {
				irootMax = 0; // 1 degenerate root
				rc = q1;	  // and here it is
			} else {
				irootMax = 1; // 2 roots
				q2 = sqrt(disc) / (2 * b);
				rc = q1 + q2; // and here's the first
			}
		} break;

		case 3: // cubic equation, has 1, 2, or 3 roots
		{
			// Construct coefficients that give the roots from Cardano's formula.
			double a2 = b / a;
			double a1 = c / a;
			double a0 = d / a;

			double Q = (3 * a1 - pow(a2, 2)) / 9;
			double R = (9 * a2 * a1 - 27 * a0 - 2 * pow(a2, 3)) / 54;
			double D = pow(Q, 3) + pow(R, 2);
			U = -a2 / 3;

			// The number of roots depends on the value of D.

			if(D > 0) {
				irootMax = 0; // one root
				double rD = sqrt(D);
				S = CubeRoot(R + rD);
				double T = CubeRoot(R - rD);
				rc = U + S + T; // and here it is.
			} else if(abs(D) < EPS) {
				irootMax = 1; // two roots
				S = pow(R, 1. / 3);
				rc = U + 2 * S; // here's the first
			} else {			// D < 0
				irootMax = 2;	// three roots
				double rD = sqrt(-D);
				double phi = atan2(rD, R) / 3;
				double rS = pow(pow(R, 2) - D, 1. / 6);
				Sr = rS * cos(phi);
				Si = rS * sin(phi);
				rc = U + 2 * Sr; // here's the first
			}
		} break; // end of case 3 of order
		}
	} break; // end of case 0 of iroot
	// for the other two roots, we'll rely on the fact that the coefficients
	// of the equation and first root have already been constructed.
	case 1: // of iroot, meaning we're looking for the second root

		if(irootMax < 1) return;
		switch(order) {
		case 2:
			rc = q1 - q2; // second root of a quadratic
			break;

		case 3: // second root of a cubic
			if(irootMax == 1)
				rc = U - S;
			else // irootMax == 2
				rc = U - Sr - sqrt(3.) * Si;
			break;
		};
		break; // end of case 1 of iroot

	case 2: // of iroot, meaning we're looking for the third root

		if(irootMax < 2) return;
		switch(order) {
		case 3: // third root of a cubic
			rc = U - Sr + sqrt(3.) * Si;
			break;
		};
		break; // end of case 2 of iroot

	}; // end of switch(iroot).

	// If we're here, rc contains a root of the equation, which must still be validated.
	XYPt p1p = d1 * u1 + rc * u1p; // image of p1 in fold line

	if(p1p == p1) return; // we only consider p1 off of the fold line

	l.u = (p1p - p1).Normalize();				   // normal to fold line
	l.d = l.u.Dot(MidPoint(p1p, p1));			   // d-parameter of fold line
	XYPt p2p = p2 + 2 * (l.d - p2.Dot(l.u)) * l.u; // image of p2 in fold line

	// Validate; the images of p1 and p2 must lie within the square.
	if(!Shared::sPaper.Encloses(p1p) ||
	   !Shared::sPaper.Encloses(p2p)) return;

	// Validate visibility; we require that the alignment be visible even with
	// opaque paper. Meaning that the moving parts must be edge points or edge
	// lines.

	// Note whether p1 and p2 are on the same side of the fold line. If they are,
	// then either both points move or both lines move. If they're not, then one
	// of each moves.

	bool sameSide = ((p1.Dot(l.u) - l.d) * (p2.Dot(l.u) - l.d) >= 0);

	// Note which points and lines are on the edge of the paper
	bool p1edge = rm1->IsOnEdge();
	bool p2edge = rm2->IsOnEdge();
	bool l1edge = rl1->IsOnEdge();
	bool l2edge = rl2->IsOnEdge();

	// Now, check the visibility of this alignment and use it to specify which
	// parts move
	if(Shared::sVisibilityMatters) {
		if(sameSide)
			if(p1edge && p2edge)
				mWhoMoves = WHOMOVES_P1P2;
			else if(l1edge && l2edge)
				mWhoMoves = WHOMOVES_L1L2;
			else
				return;
		else if(p1edge && l2edge)
			mWhoMoves = WHOMOVES_P1L2;
		else if(p2edge && l1edge)
			mWhoMoves = WHOMOVES_P2L1;
		else
			return;
	} else {
		if(sameSide)
			mWhoMoves = WHOMOVES_P1P2;
		else
			mWhoMoves = WHOMOVES_P1L2;
	};

	// If this line creates a skinny flap, we won't use it.
	if(Shared::sPaper.MakesSkinnyFlap(l)) return;

	// Set the key.
	FinishConstructor();
}

RefBase::type_t RefLine_P2L_P2L::GetType() const {
	return RefType::LINE_P2L_P2L;
}

rank_t RefLine_P2L_P2L::GetRank() const {
	return 1 + rm1->GetRank() + rl1->GetRank() + rm2->GetRank() + rl2->GetRank();
}

/*****
Return true if this line uses rb for immediate reference.
*****/
bool RefLine_P2L_P2L::UsesImmediate(RefBase *rb) const {
	return (rb == rl1 || rb == rm1 || rb == rl2 || rb == rm2);
}

/*****
Build the folding sequence that constructs this object.
*****/
void RefLine_P2L_P2L::SequencePushSelf() {
	switch(mWhoMoves) {
	case WHOMOVES_P1P2:
		rl2->SequencePushSelf();
		rl1->SequencePushSelf();
		rm2->SequencePushSelf();
		rm1->SequencePushSelf();
		break;

	case WHOMOVES_L1L2:
		rm2->SequencePushSelf();
		rm1->SequencePushSelf();
		rl2->SequencePushSelf();
		rl1->SequencePushSelf();
		break;

	case WHOMOVES_P1L2:
		rm2->SequencePushSelf();
		rl1->SequencePushSelf();
		rl2->SequencePushSelf();
		rm1->SequencePushSelf();
		break;

	case WHOMOVES_P2L1:
		rl2->SequencePushSelf();
		rm1->SequencePushSelf();
		rl1->SequencePushSelf();
		rm2->SequencePushSelf();
		break;
	};
	RefBase::SequencePushSelf();
}

/*****
Export the construction of this line.
*****/
JsonObject RefLine_P2L_P2L::Serialize() const {
	JsonObject step;
	step.add("axiom", 6);
	switch(mWhoMoves) {
	case WHOMOVES_P1P2:
		rm1->PutName("p0", step);
		rl1->PutName("l0", step);
		rm2->PutName("p1", step);
		rl2->PutName("l1", step);
		break;

	case WHOMOVES_L1L2:
		rl1->PutName("l0", step);
		rm1->PutName("p0", step);
		rl2->PutName("l1", step);
		rm2->PutName("p1", step);
		break;

	case WHOMOVES_P1L2:
		rm1->PutName("p0", step);
		rl1->PutName("l0", step);
		rl2->PutName("l1", step);
		rm2->PutName("p1", step);
		break;

	case WHOMOVES_P2L1:
		rl1->PutName("l0", step);
		rm1->PutName("p0", step);
		rm2->PutName("p1", step);
		rl2->PutName("l1", step);
		break;
	};
	PutName("x", step);

	if(mForMark != nullptr) step.add("pinch", 1);
#ifdef _DEBUG_DB_
	PutDebug(step);
#endif
	return step;
}

/*****
Draw this line, adding arrows if appropriate
*****/
void RefLine_P2L_P2L::DrawSelf(RefStyle rstyle, short ipass) const {
	// Call inherited method to draw the lines
	RefLine::DrawSelf(rstyle, ipass);

	// If we're moving, we need an arrow
	if((ipass == PASS_ARROWS) && (rstyle == REFSTYLE_ACTION)) {

		XYPt &p1a = rm1->p;
		XYPt p1b = l.Fold(p1a);
		XYPt &p2a = rm2->p;
		XYPt p2b = l.Fold(p2a);
		switch(mWhoMoves) {
		case WHOMOVES_P1P2:
			sDgmr->DrawArrow(p1a, p1b);
			sDgmr->DrawArrow(p2a, p2b);
			break;

		case WHOMOVES_L1L2:
			sDgmr->DrawArrow(p1b, p1a);
			sDgmr->DrawArrow(p2b, p2a);
			break;

		case WHOMOVES_P1L2:
			sDgmr->DrawArrow(p1a, p1b);
			sDgmr->DrawArrow(p2b, p2a);
			break;

		case WHOMOVES_P2L1:
			sDgmr->DrawArrow(p1b, p1a);
			sDgmr->DrawArrow(p2a, p2b);
			break;
		}
	}
}

/*****
Go through existing lines and marks and create RefLine_P2L_P2Ls with rank equal
arank up to a cumulative total of sMaxLines.
*****/
void RefLine_P2L_P2L::MakeAll(rank_t arank) {
	// psrank == sum of ranks of the two points
	// lsrank == sum of ranks of the two lines
	for(rank_t psrank = 0; psrank <= (arank - 1); psrank++) {
		for(rank_t lsrank = 0; lsrank <= (arank - 1) - psrank; lsrank++) {

			// point order doesn't matter, so rank(pt[i]) will always be <= rank(pt[j])
			for(rank_t irank = 0; irank <= psrank / 2; irank++) {
				rank_t jrank = psrank - irank;

				// line order does matter, so both lines vary over all ranks
				for(rank_t krank = 0; krank <= lsrank; krank++) {
					for(rank_t lrank = 0; lrank <= lsrank - krank; lrank++) {
						MakeAllCore(irank, jrank, krank, lrank);
					}
				}
			}
		}
	}
}

void RefLine_P2L_P2L::MakeAllCore(rank_t irank, rank_t jrank, rank_t krank, rank_t lrank) {
	auto &lines = ReferenceFinder::sBasisLines;
	auto &marks = ReferenceFinder::sBasisMarks;
	bool psameRank = (irank == jrank);

	// iterate over all combinations of points & lines with given rank
	auto &imap = marks.ranks[irank];
	for(auto mi = imap.begin() + (psameRank ? 1 : 0); mi != imap.end(); mi++) {
		auto &jmap = marks.ranks[jrank];
		for(auto mj = jmap.begin(); mj != (psameRank ? mi : jmap.end()); mj++) {
			for(auto lk: lines.ranks[krank]) {
				for(auto ll: lines.ranks[lrank]) {
					if((krank != lrank) || (lk != ll)) { // cmpr iterators only if same container
						if(ReferenceFinder::GetNumLines() >= Shared::sMaxLines) return;
						RefLine_P2L_P2L rlp0(*mi, lk, *mj, ll, 0);
						lines.AddCopyIfValidAndUnique(rlp0);
						if(ReferenceFinder::GetNumLines() >= Shared::sMaxLines) return;
						RefLine_P2L_P2L rlp1(*mi, lk, *mj, ll, 1);
						lines.AddCopyIfValidAndUnique(rlp1);
						if(ReferenceFinder::GetNumLines() >= Shared::sMaxLines) return;
						RefLine_P2L_P2L rlp2(*mi, lk, *mj, ll, 2);
						lines.AddCopyIfValidAndUnique(rlp2);
					}
				}
			}
		}
	}
}

void RefLine_P2L_P2L::Export(BinaryOutputStream &os) const {
	RefBase::Export(os);
	os << rm1->id << rl1->id << rm2->id << rl2->id << mRoot;
}

RefLine *RefLine_P2L_P2L::Import(BinaryInputStream &is) {
	size_t id1, id2, id3, id4;
	unsigned char root;
	is.read(id1).read(id2).read(id3).read(id4).read(root);
	RefMark *rm1 = ReferenceFinder::sBasisMarks[id1];
	RefLine *rl1 = ReferenceFinder::sBasisLines[id2];
	RefMark *rm2 = ReferenceFinder::sBasisMarks[id3];
	RefLine *rl2 = ReferenceFinder::sBasisLines[id4];

	// We must run the case root = 0 to gather the parameters
	if(root > 0) RefLine_P2L_P2L temp(rm1, rl1, rm2, rl2, 0);

	return new RefLine_P2L_P2L(rm1, rl1, rm2, rl2, root);
}
