#pragma once
#include <vb/Image.h>

namespace vb {
	struct QuadIndex { coo z; int d; };

	class QuadTree {
	public:
		QuadTree (coo UL, coo BR, int M);
		~QuadTree ();

		void insert (coo z);

		void nn (coo z, QuadIndex & qi) const;

		void paint (Image & img, coo ul, int w);

		int n;

	private:
		static std::vector <QuadTree *> store;

		int index (coo z) const;
		int idist (coo z) const;
		int odist (coo z) const;

		void split ();

		coo ul,br,center,iul,ibr;
		int m, ch;
		std::vector <coo> pts;
	};
}
