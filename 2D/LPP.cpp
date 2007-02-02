
#include <vb.h>

using namespace std;
using namespace vb;

int main (int argc, char **argv) {
  CL_Parser CLP (argc,argv, "n=500,r=.1,l=.01,d=0");
  int n = CLP.as_int('n');
  double r = CLP.as_double('r');
  double l = CLP.as_double('l');
  double d = CLP.as_double('d');

  srand48(time(0));

  Image img (n,n,1,"Broken line process");
  img.onscreen();

  int field[n];

  for (int x=0; x<n; ++x)
    if (drand48() < r) {
      img.putpoint(x,0,1,0);
      field[x]=1;
    } else {
      field[x]=0;
    }

  img.update();

  for (int y=1; y<n; ++y) {
    for (int x=0; x<n; ++x)
      if  ( (drand48() < l) ||
            ((x==y)&&(drand48() < d)) ) {
        img.putpoint(x,y,1,0);
        field[x]++;
        int xx=(x+1)%n;
        while (field[xx]==0) {
          img.putpoint(xx,y,1,0);
          xx = (xx+1)%n;
        }
        field[xx]--;
        img.putpoint(xx,y,1,0);
      }
    for (int x=0; x<n; ++x)
      if (field[x])
        img.putpoint(x,y,1,0);
  
    if (!(y%100)) img.update();
  }

  cout << img;
  return 0;
}
