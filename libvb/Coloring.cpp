#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <vb/Coloring.h>

namespace vb {
	Coloring::Coloring (cpx z1_, cpx z2_, int n, std::function <Color(cpx)> f_) :
			Picture(n,n*imag(z2_-z1_)/real(z2_-z1_)), z1(z1_), z2(z2_), f(f_) {}

	void Coloring::show () {
		Picture::show();
		stage = (Color *) (cairo_image_surface_get_data (surface));
		eps = real(z2-z1)/pixel_w();
		for (int i=0; i<pixel_w(); ++i) for (int j=0; j<pixel_h(); ++j) at(coo(i,j)) = BLACK;
		run ([&](){ tessel ({0,0}, {pixel_w()-1,pixel_h()-1}); });
		if (aa) run ([&](){do_aa();});
		update();
	}

	void Coloring::do_aa () {
		int pw = pixel_w(), ph = pixel_h(); std::vector<coo> cs;
		for (int y=0; y<ph; ++y) {
			for (int x=0; x<pw; ++x) {
				coo c {x,y}; Color cc = at(c); bool u = true;
				for (int d=0; d<4; ++d) {
					coo c2 = c + dz[d];
					if ((c2.x>=0) && (c2.x<pw) && (c2.y>=0) && (c2.y<ph) && (at(c2)!=cc)) u = false;
				}
				if (!u) cs.push_back(c);
			}
		}
		cilk_for (unsigned i=0; i<cs.size(); ++i) at(cs[i]) = aa_color(cs[i],true);
	}

	void Coloring::scale (double s) { cpx mid = (z1+z2)/2.0; z1 = mid + s * (z1-mid); z2 = mid + s * (z2-mid); }
	void Coloring::shift (cpx z) { z1 += z; z2 += z; }

	cpx Coloring::c_to_z (coo c) const { return z1 + cpx(c) * eps; }

	Color & Coloring::at (coo z) const { return stage[z.x + stride * z.y]; }

	Color Coloring::aa_color (coo c, bool pre) const {
		cpx z = c_to_z(c); int r(0), g(0), b(0);
		if (pre) { Color C = at(c); r=C.r; g=C.g; b=C.b; }
		for (int i=-1; i<=1; ++i) for (int j=-1; j<=1; ++j) if ((!pre) || (i!=0) || (j!=0)) {
			Color c = f (z + eps*cpx(i,j)/3.0);
			r+=c.r; g+=c.g; b+=c.b;
		}
		return Color(r/9,g/9,b/9);
	}

    void Coloring::line (coo s, coo d, int l) {
		cilk_for (int i=0; i<l; ++i) { coo c = s+d*i; at(c) = f(c_to_z(c)); }
	}

    void Coloring::tessel (coo ul, coo lr) {
        cilk_spawn line (ul, coo(1,0), lr.x-ul.x);
        cilk_spawn line (coo(lr.x,ul.y), coo(0,1), lr.y-ul.y);
        cilk_spawn line (lr, coo(-1,0), lr.x-ul.x);
        cilk_spawn line (coo(ul.x,lr.y), coo(0,-1), lr.y-ul.y);
        cilk_sync; tessel_go (ul, lr);
    }

    void Coloring::tessel_go (coo ul, coo lr) {
        int size = std::min(lr.x-ul.x,lr.y-ul.y); if (size <= 1) return;

        Color tmp = at(ul); bool mono = true; coo z = ul;
        for (; mono && (z != coo {lr.x,ul.y}); z += {1,0}) mono = mono && (at(z) == tmp);
        for (; mono && (z != coo {lr.x,lr.y}); z += {0,1}) mono = mono && (at(z) == tmp);
        for (; mono && (z != coo {ul.x,lr.y}); z += {-1,0}) mono = mono && (at(z) == tmp);
        for (; mono && (z != coo {ul.x,ul.y}); z += {0,-1}) mono = mono && (at(z) == tmp);

        if (mono) { for (int i=ul.x+1; i<lr.x; ++i) for (int j=ul.y+1; j<lr.y; ++j) at(coo(i,j)) = tmp; return; }

        coo ul_ = (lr.x-ul.x > lr.y-ul.y) ? coo {(ul.x+lr.x)/2,ul.y} : coo {ul.x,(ul.y+lr.y)/2};
        coo lr_ = (lr.x-ul.x > lr.y-ul.y) ? coo {(ul.x+lr.x)/2,lr.y} : coo {lr.x,(ul.y+lr.y)/2};
        coo dd_ = (lr.x-ul.x > lr.y-ul.y) ? coo {0,1} : coo {1,0};

        line (ul_,dd_,size); cilk_spawn tessel_go (ul,lr_); cilk_spawn tessel_go (ul_,lr); cilk_sync;
    }

	int Coloring::handle (int event) {
		if (event == FL_KEYDOWN) switch (Fl::event_key()) {
			case '-':     	scale (1.25);                    	show();	return 1; break;
			case '+':     	scale (0.80);                    	show();	return 1; break;
			case '=':     	scale (0.80);                    	show();	return 1; break;
			case 'a':     	aa = !aa;                        	show();	return 1; break;
			case FL_Left: 	shift (cpx(+.1,0) * real(z2-z1));	show(); return 1; break;
			case FL_Right:	shift (cpx(-.1,0) * real(z2-z1));	show(); return 1; break;
			case FL_Up:   	shift (cpx(0,-.1) * real(z2-z1));	show(); return 1; break;
			case FL_Down: 	shift (cpx(0,+.1) * real(z2-z1));	show(); return 1; break;
		}
		return Picture::handle(event);
	}
}
