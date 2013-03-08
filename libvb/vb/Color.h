/// @file
/// Adapter class to fit in between Cairo and FLTK.

#ifndef __VB_COLOR_H
#define __VB_COLOR_H

#include <vb/common.h>

namespace vb {
  /** A rather stupid class holding color info in Cairo format.
   *
   * It is just a 4-char structure with a few convenience methods for 
   * making it out of components, and it adapts to the endianness of the 
   * machine as determined at compilation time. The point is to be able 
   * to cast the output of Cairo::ImageSurface::get_data() to Color* to 
   * access components without caring about endianness.
   */

  class Color {
    public:
#if VB_ENDIAN == little
      unsigned char b; ///< The blue component.
      unsigned char g; ///< The green component.
      unsigned char r; ///< The red component.
      unsigned char a; ///< The alpha channel (for ARGB32).

      /// Constructor from RGBA values.
      Color (unsigned char R, unsigned char G, unsigned char B, unsigned char A=255) :
      b(B), g(G), r(R), a(A) { }

      /// Constructor from a greyscale value.
      Color (unsigned char V = 0) : b(V), g(V), r(V), a(255) { }
#else
      unsigned char a; ///< The alpha channel (for ARGB32).
      unsigned char r; ///< The red component.
      unsigned char g; ///< The green component.
      unsigned char b; ///< The blue component.

      /// Constructor from RGBA values.
      Color (unsigned char R, unsigned char G, unsigned char B, unsigned char A=255) :
      a(A), r(R), g(G), b(B) { }

      /// Constructor from a greyscale value.
      Color (unsigned char V = 0) : a(255), r(V), g(V), b(V) { }
#endif
      /// Compare to another color.
      bool operator== (const Color &o) { return (r==o.r) && (g==o.g) && (b==o.b) && (a==o.a); }
      /// Compare to another color.
      bool operator!= (const Color &o) { return ! operator==(o); }
      /// Convert to greyscale as an integer.
      operator int () { return (r+g+b)/3; }
  };

  const Color NONE(0,0,0,0), BLACK(0,0,0), WHITE(255,255,255),
              RED(255,0,0), GREEN(0,255,0), BLUE(0,0,255),
              CYAN(0,255,255), MAGENTA(255,0,255), YELLOW(255,255,0);
}
