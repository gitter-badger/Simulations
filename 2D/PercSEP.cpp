
#include "vb/Bitmap.h"
#include "vb/CL_Parser.h"
#include "vb/PRNG.h"

using namespace vb;

const Color C[4] = { BLACK, Color(90), GREEN, Color(128,0,0) };

class Site {
	int state; // 0 closed, 1 empty, 2 occupied, 3 blocked
public:
	Site (int i) : state(i) {};
	operator int()   const { return state; }
	operator Color() const { return C[state]; }
};

class PercSEP : public Bitmap<Site> {
public:
	PercSEP (CL_Parser & CLP) : Bitmap<Site> (2*int(CLP('n')), CLP('n'), "Exclusion in Percolation"),
			flow(0), d(CLP('d')), tasym(CLP('t')) {
		for (int i=0; i<w(); ++i)
			for (int j=0; j<h(); ++j)
				if (prng.bernoulli(CLP('p')))
					put(coo(i,j), prng.bernoulli(CLP('l')) ? 2 : 1);
	}
	void clean() {
		for (int x=0;x<w();++x) for (int y=0;y<h();++y)
			if (at(coo(x,y)) > 0) at(coo(x,y)) = at(coo(x,y)) + 100;
		bool dirty = 1; while (dirty) {
			dirty = 0;
			for (int x=0; x<w(); ++x)
				for (int y=0; y<h(); ++y) {
					if (at(coo(x,y)) == 102) {
						if (atp(coo(x+1,y)) == 1 || atp(coo(x+1,y)) == 2 || atp(coo(x,y+1)) == 1 ||
							atp(coo(x,y+1)) == 2 || atp(coo(x,y-1)) == 1 || atp(coo(x,y-1)) == 2)
								{ atp(coo(x,y))=2; dirty=1; }
					} else if (at(coo(x,y)) == 101) {
						if (atp(coo(x+1,y)) == 1 || atp(coo(x+1,y)) == 2 || atp(coo(x+1,y)) == 101 ||
							atp(coo(x,y+1)) == 1 || atp(coo(x,y+1)) == 2 ||
							atp(coo(x,y-1)) == 1 || atp(coo(x,y-1)) == 2)
								{ atp(coo(x,y))=1; dirty=1; }
					}
				}
		}
		for (int x=0;x<w();++x) for (int y=0;y<h();++y) {
			if (at(coo(x,y)) >= 102) at(coo(x,y)) = 3;
			if (at(coo(x,y)) == 101) at(coo(x,y)) = 1;
		}
	}
	void move() {
		coo z (prng.uniform_int(w()), prng.uniform_int(h()));   	if (at(z) < 2) return;
		coo s = prng.bernoulli(d) ? 1 : dz[prng.uniform_int(4)];	if (atp(z+s) != 1) return;
		if (tasym && (s==-1)) return;
		std::swap (at(z), atp(z+s));
		step(); flow += s;
	}
	coo flow;
	double d;
	bool tasym;
};

int main (int argc, char ** argv) {
	CL_Parser CLP (argc,argv,"n=400,p=.8,l=.3,d=0,t");
	PercSEP P(CLP); P.show();

	for (long t=1 ;; ++t) {
		if ((t%(P.w()*P.h()))==0) {
			if (P.tasym) P.clean();
			int na=0; for (int x=0;x<P.w();++x) for (int y=0;y<P.h();++y) if (P.at(coo(x,y))==2) ++na;
			if (na==0) exit(0);
			std::cout << t/(P.w()*P.h()) << " " << na << " " << real(P.flow) << " " << double(real(P.flow))/na << std::endl;
			P.flow = 0;
		}
		P.move();
	}
}