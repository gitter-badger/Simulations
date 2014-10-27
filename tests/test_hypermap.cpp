#include <vb/Constellation.h>
#include <vb/Hub.h>
#include <vb/Hypermap_lib.h>

using namespace vb; using namespace std;

int main (int argc, char ** argv) {
	Hub H ("Testing",argc,argv,"n=3,m=228,d,v");
	// Hypermap M { {{1},{3},{5},{0,4,2}}, {{0,1},{2,3},{4,5}}, {{0,1,2,3,4,5}} };                                	// Graph tripod
	// Hypermap M { {{0,1,2}}, {{0},{1},{2}}, {{2,1,0}} };                                                        	// Dessin tripod
	// Hypermap M { {{0,1,2,3}}, {{0},{1},{2},{3}}, {{3,2,1,0}} };                                                	// Dessin 4-star
	// Hypermap M { {{0,1,2,3,4}}, {{0},{1},{2},{3},{4}}, {{4,3,2,1,0}} };                                        	// Dessin 5-star
	// Hypermap M { {{1},{5},{7},{3,4},{2,6,0}}, {{2,1},{0,3},{4,5},{6,7}}, {{2,1,0,4,5,3,6,7}} };                	// Long graph tripod - use n=4
	// Hypermap M { {{2,1,0},{3}}, {{2},{1},{0,3}}, {{2,0,3,1}} };                                                	// Long dessin tripod - Lando page 87
	// Hypermap M { {{0,1,2},{3,4},{5,6}}, {{0},{1,3},{2,5},{4},{6}}, {{0,2,6,5,1,4,3}} };                        	// Long-legged tripod - Lando page 88
	// Hypermap M { {{4,1,2},{3,0},{5,6}}, {{4,3},{1},{2},{0,5},{6}}, {{4,0,6,5,3,2,1}} };                        	// Long-tailed tripod - Lando page 89
	// Hypermap M { {{1},{5},{7},{9},{2,8,0},{4,3,6}}, {{2,1},{0,3},{4,5},{6,7},{8,9}}, {{2,1,0,4,5,6,7,3,8,9}} };	// Graph H - use n=4
	// Hypermap M { {{0,1,2},{3},{4}}, {{0,3,4},{1},{2}}, {{0,4,3,2,1}} };                                        	// Dessin H
	// Hypermap M { {{0,2,3},{1}}, {{0,1},{2,3}}, {{0,1,3},{2}} };                                                	// Simple map - Lando page 107
	// Hypermap M = H_H1();
	// Hypermap M = H_H3();

	Hypermap M = {	{{0,2,4},{1,11,6},{3,7,8},{5,9,10}},
	              	{{0,1},{2,3},{4,5},{6,7},{8,9},{10,11}},
	              	{{0,6,3},{1,4,10},{2,8,5},{7,11,9}} }; // Tetrahedron

	// Hypermap M = {	{{0,7,8},{1,10,2},{3,12,4},{5,14,6},{9,23,16},{11,17,18},{13,19,20},{15,21,22}},
	//               	{{0,1},{2,3},{4,5},{6,7},{8,9},{10,11},{12,13},{14,15},{16,17},{18,19},{20,21},{22,23}},
	//               	{{0,2,4,6},{1,8,16,11},{3,10,18,13},{5,12,20,15},{7,14,22,9},{17,23,21,19}} }; // Cube

	// Hypermap M = {	{{0,9,10},{1,12,2},{3,4,14},{5,16,6},{8,7,18},{11,39,20},{13,23,24},{15,27,28},{17,31,32},{19,35,36},
	//               		{21,40,22},{25,42,26},{29,44,30},{33,46,34},{38,37,48},{41,59,50},{43,51,52},{45,53,54},{47,55,56},{49,57,58}},
	//               	{{0,1},{2,3},{4,5},{6,7},{8,9},{10,11},{12,13},{14,15},{16,17},{18,19},
	//               		{20,21},{22,23},{24,25},{26,27},{28,29},{30,31},{32,33},{34,35},{36,37},{38,39},
	//               		{40,41},{42,43},{44,45},{46,47},{48,49},{50,51},{52,53},{54,55},{56,57},{58,59}},
	//               	{{0,2,4,6,8},{1,10,20,22,13},{3,12,24,26,15},{5,14,28,30,17},{7,16,32,34,19},{9,18,36,38,11},
	//               		{21,39,48,58,41},{23,40,50,43,25},{27,42,52,45,29},{31,44,54,47,33},{35,46,56,49,37},{51,59,57,55,53}} }; // Icosahedron

	cout << M << endl;

	Constellation<float> C1 (M,H,H['n']); if (H['v']) { C1.show(); }   	C1.find();	C1.belyi();
	Constellation<double> C2 = cconvert <float,double> (C1);           	C2.find();	C2.belyi();
	Constellation<long double> C3 = cconvert <double,long double> (C2);	C3.find();	C3.belyi();
	Constellation<float128> C4 = cconvert <long double,float128> (C3); 	C4.find();	C4.belyi();

	cout << endl << C4;
}
