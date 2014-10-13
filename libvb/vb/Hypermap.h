#pragma once /// @file
#include <vb/Permutation.h>
#include <vb/cpx.h>
#include <iostream>

namespace vb {
	class Hypermap {
	public:
		class Vertex	{ public: cpx z;  	double r=1.0;	unsigned bone;	std::vector<int> adj;	std::vector<unsigned> edges;	};
		class Edge  	{ public: int src;	double a=NAN;	              	                     	                            	};

		Hypermap (Permutation s, Permutation a, Permutation p)	: sigma(s), alpha(a), phi(p), initial(s.size(),3)          	{}
		Hypermap (Cycles s, Cycles a, Cycles p)               	: Hypermap (Permutation(s), Permutation(a), Permutation(p))	{}

		void from_hypermap	();	//< If the Hypermap part is correct, fill in V and E.

		bool operator== (const Hypermap & o) const { return (sigma==o.sigma) && (alpha==o.alpha); }

		unsigned n_black () const { return sigma.cycles().size(); }
		unsigned n_white () const { return alpha.cycles().size(); }
		unsigned n_faces () const { return phi.cycles().size(); }
		unsigned n_edges () const { return sigma.size(); }

		bool	validate        	() const;
		bool	is_graph        	() const;
		bool	is_triangulation	() const;
		bool	is_simple       	() const; // ! parallel but non-consecutive edges (like an eye) are not detected

		int euler () const { return n_black() + n_white() - n_edges() + n_faces(); }
		int genus () const { return 1-euler()/2; }

		std::string name () const; // For triangulations, assumed to be normalized

		void flip (unsigned e);

		void	relabel	(const Permutation & p);

		void	normalize  	();
		void	mirror     	();
		void	dual       	();
		void	simplify1  	();
		void	simplify2  	();
		void	simplify   	();
		void	split_edges	();
		void	dessin     	();

		Permutation sigma, alpha, phi;

		std::vector<unsigned> initial;
		std::vector<Vertex>	V;
		std::vector<Edge>  	E;

		std::string prog;

	protected:
		double alpha_xyz	(double x, double y, double z)	const;
		double ccn      	(int n)                       	const;

	private:
		Permutation	rebasing	(unsigned i)	const;
		Permutation	rebasing	()          	const;
	};

	std::ostream & operator<< (std::ostream &os, Hypermap &H);
}
