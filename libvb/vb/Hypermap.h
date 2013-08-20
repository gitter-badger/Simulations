#pragma once /// @file
#include <vector>
#include <cassert>
#include <iostream>

// Implementation of hypermaps as triples of permutations, following A. Zvonkin, "Belyi Functions: Examples, Properties, and Applications".

namespace vb {
	class Permutation : public std::vector<int> {
	public:
		Permutation (std::vector<int> s = {})         	: std::vector<int> (s)	{ s_to_c(); }
		Permutation (std::vector<std::vector<int>> cc)	: c(cc)               	{ c_to_s(); }

		Permutation & operator= (const std::vector<int> s)              	{ vector<int>::operator=(s);	s_to_c(); return *this; }
		Permutation & operator= (const std::vector<std::vector<int>> cc)	{ c = cc;                   	c_to_s(); return *this; }

		void s_to_c ();
		void c_to_s ();

		std::vector<std::vector<int>> c;
	};

	class Hypermap {
	public:
		void validate ();

		int n_black () { return sigma.c.size(); }
		int n_white () { return alpha.c.size(); }
		int n_faces () { return phi.c.size(); }
		int n_edges () { return sigma.size(); }

		bool is_graph () { for (std::vector<int> v : alpha.c) if (v.size() != 2) return false; return true; }

		int euler () { return n_black() + n_white() - n_edges() + n_faces(); }
		int genus () { return 1-euler()/2; }

		Permutation sigma, alpha, phi; // black, white, faces
	};

	std::ostream & operator<< (std::ostream &os, Permutation &P);
	std::ostream & operator<< (std::ostream &os, Hypermap &H);
}
