/// @file Map.cpp
/// Implementation of the vb::Map class.

#include <vb/Map.h>

namespace vb {
  Map::Map (int nn) : AutoWindow (400,400,"A planar map"), n(nn), zero(-1), one(-1), infinity(-1) {
    for (int i=0; i<n; ++i) {
      v.push_back(new Vertex(0.0));
      bd.push_back(false);
    }
#ifdef LIBVB_FLTK
    //resizable (*this);
#endif
  }

  Map::~Map () {
  }

  real Map::left () { 
    real l = 0.0;
    for (int i=0; i<n; ++i) l = min (l, v[i]->z.real());
    return l;
  }

  real Map::right () { 
    real l = 0.0;
    for (int i=0; i<n; ++i) l = max (l, v[i]->z.real());
    return l;
  }

  real Map::top () { 
    real l = 0.0;
    for (int i=0; i<n; ++i) l = max (l, v[i]->z.imag());
    return l;
  }

  real Map::bottom () { 
    real l = 0.0;
    for (int i=0; i<n; ++i) l = min (l, v[i]->z.imag());
    return l;
  }

#ifdef LIBVB_FLTK
  void Map::draw () {
    fl_color (FL_WHITE);
    fl_rectf (0,0,w(),h());

    fl_push_matrix();
    fl_scale(w()/(float)(right()-left()), h()/(float)(top()-bottom()));
    fl_translate((float)(-left()),(float)(-bottom()));

    fl_color (FL_BLACK);

    for (int i=0; i<n; ++i) {
      for (adj_list::iterator j = v[i]->adj.begin(); j != v[i]->adj.end(); ++j) {
        fl_begin_line();
        fl_vertex ((float) v[i]->z.real(), (float) v[i]->z.imag());
        fl_vertex ((float) v[*j]->z.real(), (float) v[*j]->z.imag());
        fl_end_line();
      }
    }

    fl_pop_matrix();
  }
#endif

  adj_list::iterator Map::find_edge (Edge e) const {
    if (e.first >= n) return (adj_list::iterator) NULL;
    for (adj_list::iterator i = v[e.first]->adj.begin(); i != v[e.first]->adj.end(); ++i)
      if (*i == e.second) return i;
    return (adj_list::iterator) NULL;
  }

  Edge Map::turn_left (Edge e) {
    adj_list::iterator ee = find_edge (Edge(e.second,e.first));
    if (ee == v[e.second]->adj.begin()) ee = v[e.second]->adj.end();
    --ee;
    return Edge(e.second,*ee);
  }

  Edge Map::turn_right (Edge e) {
    adj_list::iterator ee = find_edge (Edge(e.second,e.first));
    ++ee;
    if (ee == v[e.second]->adj.end()) ee = v[e.second]->adj.begin();
    return Edge(e.second,*ee);
  }

  void Map::add_before (Edge e, int vv) {
    adj_list::iterator ee = find_edge(e);
    v[e.first]->adj.insert (ee,vv);
  }

  void Map::add_after (Edge e, int vv) {
    adj_list::iterator ee = find_edge(e);
    ++ee;
    v[e.first]->adj.insert (ee,vv);
  }

  std::list<int> Map::face (Edge e) {
    std::list<int> l;
    int first = e.first; l.push_back(first);
    e = turn_left(e);
    while (e.first != first) {
      l.push_back(e.first);
      e = turn_left(e);
    }
    return l;
  }

  void Map::inscribe (std::list<int> face_ext, real radius, bool reverse) {
    int n_ext = face_ext.size();

    for (int i=0; i<n; ++i) { bd[i] = false; }
    scale = radius;

    int k=0;
    for (std::list<int>::iterator i = face_ext.begin(); i != face_ext.end(); ++i, --k) {
      bd[*i] = true;
      v[*i]->z = radius * exp(cpx(0,(reverse?-2.0:2.0)*3.1415927*k/n_ext));
    }
  }

  void Map::balance () {
    bool dirty=true;

    while (dirty) {
      dirty = 0;
      for (int i=0; i<n; ++i) {
        if (bd[i]) continue;
        if (v[i]->adj.size() == 0) continue;

        cpx old = v[i]->z; v[i]->z = 0.0;
        for (adj_list::iterator j = v[i]->adj.begin(); j != v[i]->adj.end(); ++j)
          v[i]->z += v[*j]->z;
        v[i]->z /= v[i]->adj.size();
        if (v[i]->z != old) dirty = true;
      }
      step();
    }
    update();
  }

  std::list<int> Map::split_edges () {
    std::vector<int> tmp;
    for (int i=0; i<n*n; ++i) tmp.push_back(-1);

    std::list<int> output;
    std::vector<adj_list> new_vertices;

    for (int v1=0; v1<n; ++v1) {
      for (adj_list::iterator v2 = v[v1]->adj.begin(); v2 != v[v1]->adj.end(); ++v2) {
        int v3 = tmp[v1 + n*(*v2)];
        if (v3 >= 0) {                    // If the reverse is already filled
          (*v2) = v3;                     // Then point to it
        } else {                          // If it is not
          adj_list l;                     // Create it,
          l.push_back(v1);                // Populate it,
          l.push_back(*v2);               // With its two neighbors,
          new_vertices.push_back(l);      // Add it to the new vertices,
          v3 = n+new_vertices.size()-1;   // Get its number,
          tmp[(*v2) + n*v1] = v3;         // And mark it for the second way,
          output.push_back(v3);           // Add it to the return list,
          (*v2) = v3;                     // Point to it.
        }
      }
    }

    for (std::vector<adj_list>::iterator i = new_vertices.begin(); i != new_vertices.end(); ++i) {
      v.push_back(new Vertex(0.0));
      v.back()->adj = (*i);
      v.back()->z = ( v[(*i).front()]->z + v[(*i).back()]->z )/2.0;
      bd.push_back(false);
      ++n;
    }

    return output;
  }

  void Map::hex_to_triangle (std::list<int> f) {
    std::list<int>::iterator i = f.begin();
    int x = *i; ++i;
    int b = *i; ++i;
    int y = *i; ++i;
    int c = *i; ++i;
    int z = *i; ++i;
    int a = *i; ++i;

    add_after (Edge(x,b),y);
    add_after (Edge(y,c),z);
    add_after (Edge(z,a),x);

    add_before (Edge(x,a),z);
    add_before (Edge(y,b),x);
    add_before (Edge(z,c),y);
  }

  void Map::barycentric () {
    std::list<int> l = split_edges();
    for (std::list<int>::iterator i = l.begin(); i != l.end(); ++i) {
      std::list<int> f = face(Edge(*i,v[*i]->adj.front()));
      if (f.size() == 6) hex_to_triangle(f);

      f = face(Edge(*i,v[*i]->adj.back()));
      if (f.size() == 6) hex_to_triangle(f);
    }
  }

  void Map::print_as_dot (std::ostream &os) {
    os << "digraph G {" << std::endl;
    for (int i=0; i<n; ++i)
      for (adj_list::iterator j = v[i]->adj.begin(); j != v[i]->adj.end(); ++j) {
        if (find_edge(Edge(*j,i)) == (adj_list::iterator) NULL)
          os << "  " << i << " -> " << *j << ";" << std::endl;
        else if (i<=*j)
          os << "  " << i << " -> " << *j << " [arrowhead = none]" << std::endl;
      }
    os << "}" << std::endl;
  }

  void Map::plot_vertices (Figure &F) {
    for (int i=0; i<n; ++i) {
      F.dot(v[i]->z);
    }
  }

  void Map::plot_edges (Figure &F) {
    for (int i=0; i<n; ++i) {
      for (adj_list::iterator j = v[i]->adj.begin(); j != v[i]->adj.end(); ++j)
        if ((i<*j) || (find_edge(Edge(*j,i)) == (adj_list::iterator) NULL)) F.segment(v[i]->z,v[*j]->z);
    }
  }

  void Map::plot_circles (Figure &F) {
    for (int i=0; i<n; ++i) {
      if (v[i]->r>0) F.circle(v[i]->z,v[i]->r);
    }
  }

  int Map::nb_aretes () {
    int tmp = 0;
    for (int i=0; i<n; ++i) tmp += v[i]->adj.size();
    return (tmp/2);
  }

  int Map::nb_faces () {
    real tmp = 0.0;
    for (int i=0; i<n; ++i)
      for (adj_list::iterator j = v[i]->adj.begin(); j != v[i]->adj.end(); ++j)
        tmp += 1.0/((real) face(Edge(i,*j)).size());
    return (int) floor (tmp + .1);
  }

  int Map::euler () {
    return nb_sommets() - nb_aretes() + nb_faces();
  }

  int Map::genre () {
    return 1 - (euler()/2);
  }

  void Map::mobius (cpx w, real theta) {
    for (int i=0; i<n; ++i) {
      cpx z = v[i]->z / scale;
      w /= scale;
      v[i]->z = scale * (z-w)*exp(cpx(0,theta))/(cpx(1,0)-z*conj(w));
    }
  }

  real Map::optimize (real func(const Map&), real delta_func(const Map&,int,cpx,real)) {
    real cost = func(*this);
    real delta_cost = 1;
    real delta = 1.0;
    real dv;

    while (delta_cost > 0) {
      std::cerr << cost << "         \r";
      delta_cost = 0.0;
      delta = sqrt(cost/n/10000);

      for (int i=0; i<n; ++i) {
        if (i != zero) {
          dv = delta_func(*this,i,v[i]->z-cpx(delta,0),v[i]->r); if (dv<0) { v[i]->z -= cpx(delta,0); delta_cost -= dv; }
          else {
            dv = delta_func(*this,i,v[i]->z+cpx(delta,0),v[i]->r); if (dv<0) { v[i]->z += cpx(delta,0); delta_cost -= dv; }
          }
        }

        if ((i != zero) && (i != one)) {
          dv = delta_func(*this,i,v[i]->z-cpx(0,delta),v[i]->r); if(dv<0) { v[i]->z -= cpx(0,delta); delta_cost -= dv; }
          else {
            dv = delta_func(*this,i,v[i]->z+cpx(0,delta),v[i]->r); if(dv<0) { v[i]->z += cpx(0,delta); delta_cost -= dv; }
          }
        }

        dv = delta_func(*this,i,v[i]->z,v[i]->r-delta); if(dv<0) { v[i]->r -= delta; delta_cost -= dv; }
        else {
          dv = delta_func(*this,i,v[i]->z,v[i]->r+delta); if(dv<0) { v[i]->r += delta; delta_cost -= dv; }
        }
      }

      //delta_cost = cost - func(*this);
      cost -= delta_cost;
    }

    return cost;
  }

  real Map::ACPA () {
    for (int i=0; i<n; ++i) if (v[i]->r == 0.0) v[i]->r = 1.0;

    std::vector<real> pre_delta;
    pre_delta.push_back(0.0);

    int max_degree = 0;
    for (int i=0; i<n; ++i) {
      int degree = v[i]->adj.size();
      if (degree > max_degree) max_degree = degree;
    }
    for (int k=1; k<=max_degree; ++k) {
      real delta = sin (4*atan(1)/k);
      pre_delta.push_back((1-delta)/delta);
    }

    std::vector<real> old_rad;
    for (int i=0; i<n; ++i) old_rad.push_back(v[i]->r);

    real old_lambda = 0.0;
    real lambda = 0.0;

    real old_E = 8.0*n + 1;
    real E = 8.0*n;

    int steps = 0;

    while (E < old_E) {
      if (!(steps%100)) std::cerr << E << "           \r";
      ++steps;

      old_E = E; E = 0.0;

      for (int i=0; i<n; ++i) {
        if (bd[i]) continue;
        if (v[i]->adj.size() == 0) continue;

        int k = v[i]->adj.size();

        real x = v[i]->r;
        real y = v[v[i]->adj.back()]->r, z;
        real theta = 0.0;

        for (adj_list::iterator j = v[i]->adj.begin(); j != v[i]->adj.end(); ++j) {
          z = y; y = v[*j]->r;
          theta += acos (((x+y)*(x+y) + (x+z)*(x+z) - (y+z)*(y+z)) / (2*(x+y)*(x+z)));
        }
        E += (theta - TWO_PI)*(theta - TWO_PI);

        real beta = sin (theta/(2*k));

        old_rad[i] = v[i]->r;
        v[i]->r *= pre_delta[k] * (beta/(1-beta));
      }

      old_lambda = lambda; lambda = (old_E-E)/E;

      real dlambda = (old_lambda-lambda)/lambda;
      if (fabs(dlambda) < .01) {
        real sqrtlambda = sqrt(lambda);
        for (int i=0; i<n; ++i) {
          real nr = v[i]->r + (v[i]->r-old_rad[i])/sqrtlambda;
          if (nr>0) v[i]->r = nr;
        }
        old_lambda = 0.0;
      }

    }

    return E;
  }

  void Map::rad_to_pos (int _zero, int _one) {
    zero = _zero;
    one = _one;

    double R_max = 0.0;
    for (int i=0; i<n; ++i) if (v[i]->r > R_max) R_max = v[i]->r;

    cpx nowhere (-2*n*R_max, 0.0);
    for (int i=0; i<n; ++i) v[i]->z = nowhere;

    v[zero]->z = cpx(0.0,0.0);
    v[one]->z = cpx (v[zero]->r + v[one]->r, 0.0);

    bool dirty = true; while (dirty) {
      dirty = false;

      for (int i=0; i<n; ++i) {
        if (v[i]->z == nowhere) continue;
        if (v[i]->adj.size() == 0) continue;

        cpx prev_z = v[v[i]->adj.back()]->z;
        real prev_r = v[v[i]->adj.back()]->r;

        for (adj_list::iterator j = v[i]->adj.begin(); j != v[i]->adj.end(); ++j) {
          cpx z = v[*j]->z;
          if ((prev_z != nowhere) && (z == nowhere) && ((!bd[i])||(!bd[*j]))) {
            real x = v[i]->r;
            real y = v[*j]->r;
            real alpha = acos (((x+y)*(x+y) + (x+prev_r)*(x+prev_r) - (y+prev_r)*(y+prev_r)) / (2*(x+y)*(x+prev_r)));
            v[*j]->z = v[i]->z + (x+y) * exp(cpx(0.0,alpha + arg(prev_z-v[i]->z)));
            z = v[*j]->z;
            dirty = true;
          }

          prev_z = z; prev_r = v[*j]->r;
        }
      }
    }
  }

  void Map::rotate (real theta) {
    for (int i=0; i<n; ++i) v[i]->z *= exp(cpx(0.0,theta));
  }

  void Map::mobius_circle (cpx w, real r) {
    w /= scale;
    r /= scale;

    cpx Delta = (1.0+norm(w)-r*r)*(1.0+norm(w)-r*r) - 4.0*norm(w);
    cpx x = ( (1.0+norm(w)-r*r) + sqrt(Delta) ) / (2.0*conj(w));
    if (norm(x)>1.0)
      x = ( (1.0+norm(w)-r*r) - sqrt(Delta) ) / (2.0*conj(w));

    for (int i=0; i<n; ++i) {
      cpx W = v[i]->z/scale;
      real R = v[i]->r/scale;

      cpx A = norm(1.0-W*conj(x)) - R*R*norm(x);
      cpx B = (1.0-W*conj(x))*(conj(x)-conj(W)) - R*R*conj(x);
      cpx C = (x-W)*(conj(x)-conj(W)) - R*R;

      v[i]->z = scale * conj(-B/A);
      v[i]->r = scale * sqrt(abs(norm(v[i]->z/scale) - C/A));
    }
  }

  real Map::circlepack (int _zero, int _one, const std::list<int> _bord) {
    // First, add the outer vertex.

    v.push_back (new Vertex(cpx(0.0),sqrt((real)n)));
    v[n]->adj = _bord;
    scale = sqrt((real)n);
    ++n;

    int prev_i = _bord.back();
    for (adj_list::const_iterator i = _bord.begin(); i != _bord.end(); ++i) {
      add_before (Edge(*i,prev_i),n-1);
      prev_i = *i;
    }

    // Circle-pack this using ACPA

    for (int i=0; i<n; ++i) bd[i]=false;

    bd[n-1] = true; v[n-1]->r = sqrt((real)(n-1));
    bd[_bord.front()] = true; v[_bord.front()]->r = sqrt((real)(n-1));
    bd[_bord.back()] = true; v[_bord.back()]->r = sqrt((real)(n-1));
    real output = ACPA();
    rad_to_pos (_bord.front(), _bord.back());

    // Remove the added vertex

    cpx center = v[n-1]->z;
    v.pop_back(); --n;
    for (int i=0; i<n; ++i) v[i]->adj.remove(n);

    // Recenter and invert

    for (int i=0; i<n; ++i) {
      v[i]->z -= center;

      real module = abs(v[i]->z);
      real argument = arg(v[i]->z);
      real rayon = v[i]->r;

      real new_module = n*module/(module*module-rayon*rayon);

      v[i]->r = n*rayon/(module*module-rayon*rayon);
      v[i]->z = new_module * exp(cpx(0,-argument));
    }

    // Map _zero to 0

    mobius_circle (v[_zero]->z,v[_zero]->r);
    rotate (-arg(v[_one]->z));

    return output;
  }

  /// Export a vb::Map as text (kind of DOT-looking).

  std::ostream &operator<< (std::ostream &os, Map m) {
    os << m.n << " vertices:" << std::endl;
    for (int i=0; i<m.n; ++i) {
      os << "  " << i << " ->";
      for (adj_list::iterator j = m.v[i]->adj.begin(); j != m.v[i]->adj.end(); ++j)
        os << " " << *j;
      os << std::endl;
    }
    return os;
  }

  /// Add an vb::Edge to a vb::Map.

  Map &operator<< (Map &m, Edge e) {
    m.v[e.first]->adj.push_back(e.second);
    return m;
  }
}