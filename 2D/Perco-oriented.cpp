
#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>

int main (int argc, char **argv) {
  vb::CL_Parser CLP(argc,argv,"n=500,p=.75",
		"Syntax: Perco-oriented [-n <size>]");
  const int n = CLP('n');
  const double p = CLP('p');

  vb::Image img (n,n,"Oriented site-percolation");
  img.show();

  img.putpoint(0,0,255);

  for (int i=0;i<n;++i)
    for (int j=0;j<n;++j)
      if (vb::prng.bernoulli(p))
	if ( ((i>0)&&img(i-1,j)) || ((j>0)&&img(i,j-1)) )
	  img.putpoint (i,j,255);

  return 0;
}
