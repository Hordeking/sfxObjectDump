// sfxObjdump. Driver program to test sfxObj object and dump
// the contained model to wavefront obj. Copyright (C) 2018 JD Fenech
// (hordeking@users.noreply.github.com) based on sfxObjReader by
// Stéphane Dallongeville.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <getopt.h>

#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <cmath>

#include "SFXObject.h"

using namespace std;

void print_usage(void) {
    cerr << "Usage: sfxobjdump <romfile> <vertex list address> <face data address>" << endl;
    cerr << endl;
    cerr << "	-f <n>	Frame n only" << endl;
    cerr << "	-b <basename>	Set base filename.\n\t\t\tOutput filenames will be be in format \"Basename nn\"" << endl;
    cerr << "	-v	Verbose output" << endl;
    cerr << "	-t	Validate only." << endl;
    cerr << "\n\nsfxobjdump Copyright (C) 2018  JD Fenech\nThis is free software, and you are welcome to redistribute it under certain conditions;\nThis program comes with ABSOLUTELY NO WARRANTY; for details see COPYING file." << endl;
    cerr << endl;
}


int main(int argc, char * argv[])
{

	int option = 0;
	size_t n = 0;
	bool verbose = false;
	bool testonly = false;
	bool singleframe = false;
	string base_filename("Model");

	if (argc<4) { print_usage(); return -1;}

	while ((option = getopt(argc, argv,"vtf:b:")) != -1) {
		switch (option) {

			case 'v' : //Verbose mode
				verbose = true;
				break;

			case 't' : //Validate object mode
				testonly = true;
				break;

			case 'b' : //List only
				base_filename = optarg;
				break;

			case 'f' :
				n = atoi(optarg);
				singleframe = true;
				break;

			default: print_usage();
				exit(EXIT_FAILURE);
		}
	}

	// TODO: Check that the user actually provided a filename for this.
	ifstream romfile(string(argv[optind]), ifstream::binary);
	if (!romfile.good()) { cerr << (string(argv[1]) + ": File not found, or unable to be opened.") << endl << endl; print_usage(); return -1;}

	//	cout << stoull(string(argv[1]), nullptr, 0) << endl;

    SFXObject test(romfile, stoull(string(argv[optind+1]), nullptr, 0), stoull(string(argv[optind+2]), nullptr, 0));

	if (verbose) cerr << "This SFXObject is " << (test.isValid()?"valid.":"not valid.") << endl;
	if(!test.isValid()) return -1;
	if(testonly && test.isValid()) return 0;

	if (verbose) cerr << "Number of Frames: " << test.frameCount() << endl;
	if (verbose) cerr << "Base filename: " << base_filename << endl;

/*
	cerr << endl << "Outputting frame " << n << " of this object in Stanford PLY format." << endl;

	cout << "ply" << endl << "format ascii 1.0" << endl << "element vertex " << test.getVertexList().size() << endl;
	cout << "property float x" << endl << "property float y" << endl << "property float z" << endl;
	cout << "element face " << test.getFaceList().size() << endl << "property list uchar int vertex_index" << endl;
	cout << "end_header" << endl;

	for(auto vertex: test.getVertexList(n))
		cout << vertex.getFormattedString() << endl;

	for(auto face: test.getFaceList()){
		cout << face.vertex.size();

		for(auto iVert: face.vertex) cout << "	" << (unsigned int) iVert;

		cout << endl;
	}
*/

	for(;n<test.frameCount();++n){

		string out_modelname = base_filename;
		if (test.frameCount()>1) out_modelname = out_modelname + "_" + ((test.frameCount()>=100&&n<100)?to_string(0):string()) + ((test.frameCount()>=10&&n<10)?to_string(0):string()) + to_string(n);

		ofstream outfile(out_modelname + string(".obj"));

		cerr << "Outputting frame " << n << " of this object in Wavefront OBJ format." << endl;

		outfile << "# Frame " << n << endl << endl;

		outfile << "o " << out_modelname << endl << endl;

		outfile << "# Vertex List. To convert float x' to signed 8bit x, x = floor(x'*127+.5)." << endl;

		for(auto vertex: test.getVertexList(n)){
			outfile << "v";

			for(auto coord: vertex.getCoords()){
				outfile << " " << coord/127.0f;
			}

			outfile << endl;
		}

		outfile << endl << "# Normals components range from -1.0 to 1.0. To convert back to signed 8bit, x = floor(x'*127+.5)" << endl;

		outfile << endl << "usemtl Default" << endl;

		for(auto face: test.getFaceList()){
			//cout << face.vertex.size();

			float w = sqrt( face.nx*face.nx + face.ny*face.ny + face.nz*face.nz);

			outfile << "vn " << (int) face.nx/(-w) << "\t" << (int) face.ny/(-w) << "\t" << (int) face.nz/(-w) << endl;

			if (1==face.nVerts) outfile << "p";
			if (2==face.nVerts) outfile << "l";
			if (3<=face.nVerts) outfile << "f";

			for(auto iVert: face.vertex){
				outfile << "	" << (unsigned int) iVert+1;
				//outfile << "/" /* << (int) vt */;
				//outfile << "/-1" /* << (int) vn+1 */;
			}

			outfile << endl;
		}

		outfile.close();

		if (singleframe) break;
	}


    return 0;
}
