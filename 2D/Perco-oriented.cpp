#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>

using namespace vb;

int main (int argc, char **argv) {
  vb::CL_Parser CLP(argc,argv,"n=500,p=.75");
  const int n = CLP('n');
  const double p = CLP('p');

  vb::Image img (n,n);
  img.show();

  img.put(coo(0,0),255);

  for (int i=0;i<n;++i)
    for (int j=0;j<n;++j)
      if (vb::prng.bernoulli(p))
	if ( ((i>0)&&img.at(vb::coo(i-1,j))) || ((j>0)&&img.at(vb::coo(i,j-1))) )
	  img.put(coo(i,j),255);

  return 0;
}
