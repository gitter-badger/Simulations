#include <vb/Bitmap.h>
#include <vb/Console.h>
#include <vb/Hub.h>

using namespace vb;

class Site { public:
	Site (int d_ = 0) : d(d_) {}

	operator Color () { static std::vector<Color> key = { BLACK, RED, GREEN }; return key[d]; }

	int d;
};

class Nematic : public vb::Bitmap<Site> { public:
	Nematic (int n_, int k_, double b_) : Bitmap<Site> (n_,n_), k(k_), b(b_), P(n_,0) {};

	void prec () {
		ok = std::min(k,w()); ob = b; z = exp(2*ob); std::vector<double> Z(w(),1);
		for (int i=ok; i<Z.size(); ++i) Z[i] = Z[i-1] + z * Z[i-ok];
		for (int i=ok; i<P.size(); ++i) P[i] = z * Z[i-ok] / Z[i];
		dd = ok * z * Z[w()-ok] / (Z[w()-1] + ok * z * Z[w()-ok]);
	}

	void add (coo z, int d) { (d==1 ? nh : nv) += ok; for (int i=0; i<ok; ++i) { atp(z) = d; z += dz[d-1]; } }

	void fill (coo z, int d, int l) {
		while (l >= ok) {
			if (prng.bernoulli(P[l])) { add (z,d); z += dz[d-1] * ok; l -= ok; }
			else { z += dz[d-1]; l--; }
		}
	}

	void redo (coo z, int d) {
		bool empty = true;
		for (int x=0; x<w(); z+=dz[d-1], ++x) {
			if (at(z).d == 3-d) empty = false;
			if (at(z).d == d) { at(z) = 0; (d==1 ? nh : nv) --; }
		}
		if (empty) {
			if (prng.bernoulli(dd)) {
				z += dz[d-1] * (-prng.uniform_int(ok));
				add (z,d);
				fill (z + dz[d-1] * ok, d, w()-ok);
			} else fill (z + dz[d-1], d, w()-1);
		} else {
			while (atp(z).d != 3-d) { z += dz[d-1]; }
			for (coo zz=z; zz.x + zz.y < z.x + z.y + w(); zz += dz[d-1]) {
				if (atp(zz).d == 0) { int l=0; coo zzz = zz; while (atp(zzz).d == 0) { ++l; zzz += dz[d-1]; } fill (zz, d, l); zz = zzz; }
			}
		}
		step();
	}

	void go () {
		Console C;          	C.watch (order, "order");
		C.watch (b, "beta");	C.manage (b,-5.0,5.0);
		C.watch (k, "k");   	C.manage (k,1,100);
		show(); C.show(); if (H['v']) snapshot_setup("movie",10);
		for (int t=int(H['t'])-1 ; t!=0 ; --t) {
			if ((k != ok) || (b != ob)) prec();
			for (int i=0; i<h(); ++i) redo (coo(0,i), 1);
			for (int i=0; i<w(); ++i) redo (coo(i,0), 2);
			order = nh+nv>0 ? double (nh-nv) / double (nh+nv) : 0;
			if ((!(t%100)) && H['l']) std::cout << order << std::endl;
		}
	};

	int k, ok=0, nh=0, nv=0;
	double b, ob=0, z, order=0, dd;
	std::vector<double> P;
};

int main (int argc, char ** argv) {
	H.init ("Nematic system on the square lattice", argc,argv, "n=500,k=20,b=-2,v,l,t=0");
	Nematic(H['n'],H['k'],H['b']).go();
}
