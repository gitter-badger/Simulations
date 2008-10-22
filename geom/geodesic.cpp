#include <iostream>
#include <vector>
#include <vb/PRNG.h>
#include <vb/CL_Parser.h>
#include <vb/Image.h>

using namespace std;
using namespace vb;

PRNG prng;

void fill_dyadic (vector<double> &f, int n) {
  int nn = 1<<n;
  for (int l=0; l<n; ++l) {
    int ll = 1<<l;
    for (int i=0; i<nn/ll; ++i)
      for (int j=0; j<nn/ll; ++j) {
        double g = prng.gaussian();
        for (int x=i*ll; x<(i+1)*ll; ++x)
          for (int y=j*ll; y<(j+1)*ll; ++y)
            f[x+nn*y] += g;
      }
  }
}

void trace (Image &img, vector<int> &direction, int x, int y) {
  int nn = img.width;

  while (img(x,y) != 255) {
    img.putpoint (x,y,255);
    int xy = direction[x+nn*y];
    x = xy%nn;
    y = xy/nn;
  }
}

void find_geodesics (const vector<double> &field, vector<double> &distance, vector<int> &direction, int nn) {
  unsigned int changed = 1; while (changed>0) {
    changed=0;

    for (int x=0; x<nn; ++x) {
      for (int y=0; y<nn; ++y) {
        int xy = x+nn*y;
        bool flag = false;

        if ((x>0)    && (distance[xy-1]  + field[xy] < distance[xy])) { direction[xy] = xy-1;  distance[xy] = distance[xy-1]  + field[xy]; flag=true; }
        if ((x<nn-1) && (distance[xy+1]  + field[xy] < distance[xy])) { direction[xy] = xy+1;  distance[xy] = distance[xy+1]  + field[xy]; flag=true; }
        if ((y>0)    && (distance[xy-nn] + field[xy] < distance[xy])) { direction[xy] = xy-nn; distance[xy] = distance[xy-nn] + field[xy]; flag=true; }
        if ((y<nn-1) && (distance[xy+nn] + field[xy] < distance[xy])) { direction[xy] = xy+nn; distance[xy] = distance[xy+nn] + field[xy]; flag=true; }

        if (flag) ++changed;
      }
    }

    for (int x=nn-1; x>=0; --x) {
      for (int y=nn-1; y>=0; --y) {
        int xy = x+nn*y;
        bool flag = false;

        if ((x>0)    && (distance[xy-1]  + field[xy] < distance[xy])) { direction[xy] = xy-1;  distance[xy] = distance[xy-1]  + field[xy]; flag=true; }
        if ((x<nn-1) && (distance[xy+1]  + field[xy] < distance[xy])) { direction[xy] = xy+1;  distance[xy] = distance[xy+1]  + field[xy]; flag=true; }
        if ((y>0)    && (distance[xy-nn] + field[xy] < distance[xy])) { direction[xy] = xy-nn; distance[xy] = distance[xy-nn] + field[xy]; flag=true; }
        if ((y<nn-1) && (distance[xy+nn] + field[xy] < distance[xy])) { direction[xy] = xy+nn; distance[xy] = distance[xy+nn] + field[xy]; flag=true; }

        if (flag) ++changed;
      }
    }

    for (int x=0; x<nn; ++x) {
      for (int y=nn-1; y>=0; --y) {
        int xy = x+nn*y;
        bool flag = false;

        if ((x>0)    && (distance[xy-1]  + field[xy] < distance[xy])) { direction[xy] = xy-1;  distance[xy] = distance[xy-1]  + field[xy]; flag=true; }
        if ((x<nn-1) && (distance[xy+1]  + field[xy] < distance[xy])) { direction[xy] = xy+1;  distance[xy] = distance[xy+1]  + field[xy]; flag=true; }
        if ((y>0)    && (distance[xy-nn] + field[xy] < distance[xy])) { direction[xy] = xy-nn; distance[xy] = distance[xy-nn] + field[xy]; flag=true; }
        if ((y<nn-1) && (distance[xy+nn] + field[xy] < distance[xy])) { direction[xy] = xy+nn; distance[xy] = distance[xy+nn] + field[xy]; flag=true; }

        if (flag) ++changed;
      }
    }

    for (int x=nn-1; x>=0; --x) {
      for (int y=0; y<nn; ++y) {
        int xy = x+nn*y;
        bool flag = false;

        if ((x>0)    && (distance[xy-1]  + field[xy] < distance[xy])) { direction[xy] = xy-1;  distance[xy] = distance[xy-1]  + field[xy]; flag=true; }
        if ((x<nn-1) && (distance[xy+1]  + field[xy] < distance[xy])) { direction[xy] = xy+1;  distance[xy] = distance[xy+1]  + field[xy]; flag=true; }
        if ((y>0)    && (distance[xy-nn] + field[xy] < distance[xy])) { direction[xy] = xy-nn; distance[xy] = distance[xy-nn] + field[xy]; flag=true; }
        if ((y<nn-1) && (distance[xy+nn] + field[xy] < distance[xy])) { direction[xy] = xy+nn; distance[xy] = distance[xy+nn] + field[xy]; flag=true; }

        if (flag) ++changed;
      }
    }

    cerr << "\r" << changed << "       ";
  }
}

int main (int argc, char **argv) {
  CL_Parser CLP (argc, argv, "n=8,g=1,s=0");
  int n = CLP('n');
  int nn = 1<<n;
  double g = CLP('g');
  int s = CLP('s');
  if (s) prng.srand(s);

  vector<double> field;
  for (int i=0; i<nn*nn; ++i) field.push_back(0.0);
  fill_dyadic (field,n);

  double big = 0.0;
  double max = field[0];
  double min = field[0];

  for (int i=0; i<nn*nn; ++i) {
    if (field[i]<min) min=field[i];
    if (field[i]>max) max=field[i];

    field[i] = exp(g * field[i]);
    big += field[i];
  }

  vector<double> distance;
  for (int i=0; i<nn*nn; ++i) distance.push_back(big);
  distance[nn*(nn+1)/2] = 0.0;

  vector<int> direction;
  for (int i=0; i<nn*nn; ++i) direction.push_back(nn*(nn+1)/2);

  find_geodesics (field, distance, direction, nn);

  Image img (nn,nn,8,"A dyadic GFF");
  for (int i=0; i<nn; ++i)
    for (int j=0; j<nn; ++j) {
      double renorm = log(field[i+nn*j])/(g*sqrt((double)n));
      int color = 32 * (2.0 + renorm);

      if (color>128) color=128;
      if (color<0) color=0;
      img.putpoint (i,j,color);
    }

  for (int i=0; i<=nn-1; i+=15) {
    trace (img, direction, 0, i);
    trace (img, direction, nn-1, i);
    trace (img, direction, i, 0);
    trace (img, direction, i, nn-1);
  }

  cout<<img;
}