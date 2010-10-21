/// @file
/// Definitions of a vector class.

#ifndef __VB_VECTOR_H
#define __VB_VECTOR_H

#include <vb/common.h>
#include <boost/numeric/ublas/vector.hpp>

namespace vb {

  /// A vector class.

  class Vector : public boost::numeric::ublas::vector<double> {
    public:
      /** Constructor.
       *
       * @param size The size of the constructed vector.
       * @param t    The value of the entries of the vector.
       */

      Vector (unsigned int size = 0, double t = 0);
  };

  /** Multiply a vector by a scalar.
   *
   * @param X The vector.
   * @param l The scalar.
   */

  Vector operator* (double l, const Vector &X);

  /** Multiply a vector by a scalar.
   *
   * @param X The vector.
   * @param l The scalar.
   */

  Vector operator* (const Vector &X, double l);

  /** Divide a vector by a scalar.
   *
   * @param X The vector.
   * @param l The scalar.
   */

  Vector operator/ (const Vector &X, double l);

  /** Compute the sum of two vectors.
   *
   * @param X The first vector.
   * @param Y The second vector.
   */

  Vector operator+ (const Vector &X, const Vector &Y);

  /** Compute the difference between two vectors.
   *
   * @param X The first vector.
   * @param Y The second vector.
   */

  Vector operator- (const Vector &X, const Vector &Y);

  /** Output a vector to an output stream.
   *
   * @param os The output stream.
   * @param V  The vector to output.
   */

  std::ostream &operator<< (std::ostream &os, const Vector &V);
}

#endif

