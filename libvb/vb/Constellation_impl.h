#pragma once
#include <vb/Constellation.h>
#include <iomanip>

namespace vb {
	template <typename T> Constellation<T>::Constellation () {};

	template <typename T> template <typename U> Constellation<T>::Constellation (const Constellation<U> & C) {
		for (auto zd : C.b) b.push_back({std::complex<T>(zd.z), zd.d});
		for (auto zd : C.w) w.push_back({std::complex<T>(zd.z), zd.d});
		for (auto zd : C.f) f.push_back({std::complex<T>(zd.z), zd.d});
		for (auto z : C.p)  p.push_back(std::complex<T>(z));
		dim = b.size() + w.size() + f.size() + p.size();
	};

	template <typename T> T Constellation<T>::findn () {
		Vector<cplx> x = vec();
		T c = cost(), old_c = c + T(1); auto old_x = x;
		while (c<old_c) {
			std::cerr << c << "             \r"	;
			old_c = c; old_x = x;
			x -= solve(jacvcost(),vcost());
			readvec(x); c = cost();
		}
		readvec(old_x); return old_c;
	}

	template <typename T> T Constellation<T>::cost () const {
	    T out(0); for (auto z : vcost()) out += norm(z);
		T bound = sqrt(sqrt(out));
		for (unsigned i=0; i<b.size(); ++i) for (unsigned j=0; j<b.size(); ++j) if (i!=j) if (abs(reduce(b[i].z-b[j].z)) < bound) out += T(1);
		for (unsigned i=0; i<w.size(); ++i) for (unsigned j=0; j<w.size(); ++j) if (i!=j) if (abs(reduce(w[i].z-w[j].z)) < bound) out += T(1);
		for (unsigned i=0; i<f.size(); ++i) for (unsigned j=0; j<f.size(); ++j) if (i!=j) if (abs(reduce(f[i].z-f[j].z)) < bound) out += T(1);
	    if (std::isnan(double(out))) out = T(1.234567e89);
	    return out;
	}
}
