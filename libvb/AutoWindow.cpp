/// @file
/// Implementation of the vb::AutoWindow class

#include <vb/AutoWindow.h>
#include <FL/Fl.H>

namespace vb {
	void close_window (Fl_Widget *w) { exit(1); }

	AutoWindow::AutoWindow (int wd, int ht, const std::string &t) : Auto(.05), Fl_Double_Window (wd, ht), title(t), paused (false) {
		label (title.c_str());
		callback(close_window);
	}

	void AutoWindow::show () {
		Fl_Double_Window::show();
		update();
	}

	int AutoWindow::handle (int event) {
		if (event == FL_KEYDOWN) switch (Fl::event_key()) {
			case 'q': exit (0);                  	break;
			case 'x': exit (1);                  	break;
			case ' ': paused = !paused; return 1;	break;
		}
		return Fl_Double_Window::handle(event);
	}

	void AutoWindow::update () {
		if (visible()) {
			redraw();
			Fl::check();
			while (paused) Fl::wait();
		}
	}
}
