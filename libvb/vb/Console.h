/// @file
/// Definitions for the vb::Console class

#pragma once
#include <vb/AutoWindow.h>

namespace vb {
	class Slot : public Fl_Group {
	public:
		Slot (const std::string &nn, int w) : Fl_Group (0,0,w,30), n(nn), l(0,0,150,30,n.c_str()), o(150,0,w-150,30) {}
		const std::string n;
		Fl_Button l;
		Fl_Output o;
	};

	template <typename T> class Watcher : public Slot {
	public:
		Watcher (T &tt, const std::string &nn, int w) : Slot (nn,w), t(tt) {}
		void draw () {
			std::ostringstream os; os << t; o.value(os.str().c_str());
			Slot::draw();
		}
		T &t;
	};

	template <typename T> class Tracer : public Slot {
	public:
		Tracer (T ff (void*), void *dd, const std::string &nn, int w) : Slot (nn,w), f(ff), d(dd) {}
		void draw () {
			std::ostringstream os; os << f(d); o.value(os.str().c_str());
			Slot::draw();
		}
		T (*f) (void*);
		void *d;
	};

	template <typename T> void apply (Fl_Widget *W);

	template <typename T> class Manager : public Fl_Hor_Nice_Slider {
	public:
		Manager (T &tt, T t1, T t2, int w) : Fl_Hor_Nice_Slider (0,0,w,30), t(tt) {
			bounds (t1,t2); set_value (tt); callback (apply<T>);
		}
		T &t;
	};

	template <typename T> void apply (Fl_Widget *W) { Manager<T> *M = (Manager<T> *) W; M->t = M->value(); }

	class Console : public AutoWindow {
	public:
		Console () : AutoWindow (400,0,"Console") {}

		void add (Fl_Widget *S) { S->position (0,h()); size (w(),h()+S->h()); AutoWindow::add (S); }

		template <typename T> void watch (T &t,                	const std::string &n)	{ add (new Watcher<T> (t,n,w())); }
		template <typename T> void trace (T f (void*), void *d,	const std::string &n)	{ add (new Tracer<T> (f,d,n,w())); }
		template <typename T> void manage (T &t, T t1, T t2)   	                     	{ add (new Manager<T> (t,t1,t2,w())); }
	};
}
