#pragma once /// \file
#include <vb/Image.h>
#include <vb/CoarseImage.h>

namespace vb {
	class PointQueue;

	class Point {
		public:
			coo z;   	///< The location of the point.
			double t;	///< The time at which the clock rings.
			char c;  	///< The color of the point.

			Point (coo zz=0, double tt=0, char cc=1) : z(zz), t(tt), c(cc) {};
			Point (PointQueue &pq);

			bool operator< (const Point &pt) const { return t > pt.t; };
	};

	class PointQueue : public std::priority_queue<Point> {
	public:
		Point     	&operator>> (Point &p)      	{ p = top(); pop(); return p; }
		PointQueue	&operator<< (const Point &p)	{ push(p); return *this; }
	};

	Point::Point (PointQueue &pq) { pq >> *this; };

	Image      	&operator<< (Image &img,      	const Point &p) { img.put (p.z,p.c); return img; }
	CoarseImage	&operator<< (CoarseImage &img,	const Point &p) { img.put (p.z,p.c); return img; }
}
