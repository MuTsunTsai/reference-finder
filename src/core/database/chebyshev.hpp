
#ifndef _CHEBYSHEV_H_
#define _CHEBYSHEV_H_

/**
 * Traverse all points of Chebyshev distance r.
 */
class Chebyshev {
  public:
	struct Pt {
		int x;
		int y;
	};

	class Iterator {
	  public:
		Iterator(int r, Pt cursor) : r(r), cursor(cursor) {}

		Pt operator*() const {
			const int i = cursor.x, j = cursor.y;
			const int f = i % 2 ? 1 : -1;
			int dx = i < 2 ? f * (j - r) : f * r;
			int dy = i < 2 ? f * r : f * (r - j);
			return {dx, dy};
		}

		Iterator &operator++() {
			cursor.y++;
			if (cursor.y == 2 * r) {
				cursor.y = 0;
				cursor.x++;
			}
			return *this;
		}

		bool operator!=(const Iterator &that) const {
			return cursor.x < that.cursor.x;
		}

	  private:
		int r;
		Pt cursor;
	};

	Chebyshev(int r) : r(r) {}

	Iterator begin() const { return Iterator(r, {0, 0}); }
	Iterator end() const { return Iterator(r, {4, 0}); }

  private:
	int r;
};

#endif
