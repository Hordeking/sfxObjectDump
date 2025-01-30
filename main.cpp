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

#include <cassert>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <cmath>

#include "Graphics.h"
#include "SFXObject.h"

using namespace std;

bool isTexture(uint16_t m) {
  return m >= 0x4000 && m <= 0x4FFF;
}

void print_usage(void) {
    cerr << "Usage: sfxobjdump <romfile> <vertex list address> <face data address> [material table address]" << endl;
    cerr << endl;
    cerr << "	-f <n>	Frame n only" << endl;
    cerr << "	-b <basename>	Set base filename.\n\t\t\tOutput filenames will be be in format \"Basename nn\"" << endl;
    cerr << "	-v	Verbose output" << endl;
    cerr << "	-t	Validate only." << endl;
    cerr << "	-g	Triangulate faces" << endl;
    cerr << "	-m	Generate materials file" << endl;
    cerr << "\n\nsfxobjdump Copyright (C) 2018  JD Fenech\nThis is free software, and you are welcome to redistribute it under certain conditions;\nThis program comes with ABSOLUTELY NO WARRANTY; for details see COPYING file." << endl;
    cerr << endl;
}

void emitTexture(uint16_t material, ostream& outfile) {
  // Set size of BMP in total bytes = 122 + 32 * 32 * 4 = 0x107A;
  bmp_header[2] = 0x7A;
  bmp_header[3] = 0x10;
  bmp_header[4] = 0x00;
  bmp_header[5] = 0x00;

  // Set dimensions
  bmp_header[0x12] = 0x20; // 32
  bmp_header[0x16] = 0x20; // 32

  outfile.write(reinterpret_cast<char*>(bmp_header), bmp_header_len);

  uint32_t address;
  int param = material & 0xFF;
  bool hiPage = false;
  if (param >= 0x80) {
    hiPage = true;
    param -= 0x80;
    address = texture_offsets[param];
  } else {
    address = texture_offsets[param];
  }

  if (address >= 0x130000) {
    address -= 0x130000;
  } else {
    address -= 0x128000;
  }

  int row = address / 256;
  int col = address % 256;

  for (int r = row + 31; r >= row; --r) {
    for (int c = col; c < col + 32; ++c) {
      uint8_t byte = texture_data[r * 256 + c];

      uint8_t nib = byte & 0x0F;
      if (hiPage) nib = byte >> 4;

      uint8_t red   = palette[nib*3];
      uint8_t green = palette[nib*3 + 1];
      uint8_t blue   = palette[nib*3 + 2];

      outfile.put(blue);
      outfile.put(green);
      outfile.put(red);
      outfile.put(nib == 0 ? 0x00 : 0xFF);
    }
  }
}

void emitMaterialDefinition(uint16_t material, ostream& outfile) {
  int type = (material >> 8) & 0xFF;
  int param = material & 0xFF;

  if (type < 0x0A || type == 0x3E || type == 0x3F) {
    // Constant
    uint8_t byte = 0;
    if (type < 0x0A)
      byte = flat_shaded_ramps[param];
    else
      byte = constant_colors[param];

    uint8_t hi = byte >> 4;
    uint8_t lo = byte & 0xF;

    // Just average the dithered colors I guess?
    float red = (palette[hi*3] / 255.0 + palette[lo*3] / 255.0) / 2.0;
    float green = (palette[hi*3+1] / 255.0 + palette[lo*3+1] / 255.0) / 2.0;
    float blue = (palette[hi*3+2] / 255.0 + palette[lo*3+2] / 255.0) / 2.0;

    outfile << setfill('0') << "Kd " << setw(4) << red << " " << green << " " << blue << setw(0);
    outfile << endl;
  } else if (type >= 0x40 && type <= 0x4F) {
    // Textured
    outfile << "map_Kd mtl_" << hex << setfill('0') << setw(4) << right << (int)material << dec << ".bmp" << endl;
  } else {
    // Animated & Other?
    outfile << "Kd 1.0000 0.0000 0.0000" << endl;
  }
}

void emitFace(const Face& face, ostream& outfile, bool has_texture = false, bool triangulate_faces = false, bool texture_flipped = false) {
  if (has_texture) {
    assert(face.nVerts == 4);
    if (triangulate_faces) {
      outfile << "f " << (unsigned int) face.vertex[0] + 1 << (texture_flipped ? "/2" : "/1");
      outfile << " " << (unsigned int) face.vertex[1] + 1 << (texture_flipped ? "/1" : "/2");
      outfile << " " << (unsigned int) face.vertex[2] + 1 << (texture_flipped ? "/4" : "/3");
      outfile << endl;
      outfile << "f " << (unsigned int) face.vertex[0] + 1 << (texture_flipped ? "/2" : "/1");
      outfile << " " << (unsigned int) face.vertex[2] + 1 << (texture_flipped ? "/4" : "/3");
      outfile << " " << (unsigned int) face.vertex[3] + 1 << (texture_flipped ? "/3" : "/4");
      outfile << endl;
    } else {
      outfile << "f " << (unsigned int) face.vertex[0] + 1 << (texture_flipped ? "/2" : "/1");
      outfile << " " << (unsigned int) face.vertex[1] + 1 << (texture_flipped ? "/1" : "/2");
      outfile << " " << (unsigned int) face.vertex[2] + 1 << (texture_flipped ? "/4" : "/3");
      outfile << " " << (unsigned int) face.vertex[3] + 1 << (texture_flipped ? "/3" : "/4");
      outfile << endl;
    }
    return;
  }

  // Triangulate the n-gon
  if (triangulate_faces) {
    // f 0 1 2, f 0 2 3, ... etc.
    for (auto i = 1u; i < face.nVerts - 1; ++i) {
      outfile << "f " << (unsigned int) face.vertex[0] + 1;
      outfile << " " << (unsigned int) face.vertex[i] + 1;
      outfile << " " << (unsigned int) face.vertex[i+1] + 1;
      outfile << endl;
    }
  } else {
    outfile << "f";
    for (auto i = 0u; i < face.nVerts; ++i) {
      outfile << " " << (unsigned int) face.vertex[i] + 1;
    }
    outfile << endl;
  }
}

int main(int argc, char * argv[])
{

	int option = 0;
	size_t n = 0;
	bool verbose = false;
	bool testonly = false;
	bool singleframe = false;
	bool triangulate_faces = false;
	bool generate_materials = false;

	string base_filename("Model");

	if (argc<4) { print_usage(); return -1;}

	while ((option = getopt(argc, argv,"vtgmf:b:s:")) != -1) {
		switch (option) {

			case 'g' : //Triangulate faces
				triangulate_faces = true;
				break;

			case 'm' : //Generate materials
				generate_materials = true;
				break;

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
	if (!test.isValid()) return -1;
	if (testonly && test.isValid()) return 0;
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

	std::vector<uint16_t> materials;

	for(;n<test.frameCount();++n){

		string out_modelname = base_filename;
		if (test.frameCount()>1) out_modelname = out_modelname + "_" + ((test.frameCount()>=100&&n<100)?to_string(0):string()) + ((test.frameCount()>=10&&n<10)?to_string(0):string()) + to_string(n);

		ofstream outfile(out_modelname + string(".obj"));

		cerr << "Outputting frame " << n << " of this object in Wavefront OBJ format." << endl;

		outfile << "# Frame " << n << endl << endl;

		outfile << "o " << out_modelname << endl << endl;

		outfile << "# Vertex List. To convert float x' to signed 8bit x, x = floor(x'*127+.5)." << endl;

		for(auto vertex: test.getVertexList(n)) {
			outfile << "v " << vertex.toString() << endl;
		}

		outfile << endl << "# Texture Coordinates" << endl;
		outfile << "vt 0.0 1.0" << endl;
		outfile << "vt 1.0 1.0" << endl;
		outfile << "vt 1.0 0.0" << endl;
		outfile << "vt 0.0 0.0" << endl;

		outfile << endl << "# Normals components range from -1.0 to 1.0. To convert back to signed 8bit, x = floor(x'*127+.5)" << endl;

    if (generate_materials)
		  outfile << endl << "mtllib ./" << (base_filename + ".mtl") << endl; 

    // Group faces by materials
		auto list = test.getFaceList();
		std::sort(list.begin(), list.end(), [&test] (const auto& lhs, const auto& rhs) { 
				uint16_t left = test.getMaterial(lhs.material);
				uint16_t right = test.getMaterial(rhs.material);
				return left < right;
		});

		uint16_t prev_material = 0xFF;
		for(auto face: list) {
      // Material
      uint16_t material = test.getMaterial(face.material);
      bool has_texture = isTexture(material); // material >= 0x4000 && material <= 0x4F00;
      bool texture_flipped = (material >> 8) == 0x48;
      if (generate_materials) {
        if (material != prev_material) {
          outfile << endl << "usemtl mtl_" << hex << setfill('0') << setw(4) << right << (int)material 
                  << endl << dec << setw(0);
          prev_material = material;
          // TODO: May have duplicates for multi-frame models
          materials.push_back(material);
        }
      }

      // Primitives
			if (face.nVerts >= 3) {
        emitFace(face, outfile, has_texture, triangulate_faces, texture_flipped);
			} else if (face.nVerts == 2) {
				outfile << "l";
				for(auto iVert: face.vertex) {
					outfile << " " << (unsigned int) iVert+1;
				}
				outfile << endl;
      } else if (face.nVerts == 1) {
				outfile << "p";
				for(auto iVert: face.vertex) {
					outfile << " " << (unsigned int) iVert+1;
				}
				outfile << endl;
			}
		}

		outfile.close();

		if (singleframe) break;
	}

  // TODO: Gather and emit all materials
  if (generate_materials) {
    ofstream matfile(base_filename + string(".mtl"));
    for (auto material : materials) {
      matfile << "newmtl mtl_" << hex << setfill('0') << setw(4) << right << (int)material 
              << endl << dec << setw(0);
      emitMaterialDefinition(material, matfile);
      matfile << endl;
    }
    matfile.close();

    for (auto material : materials) {
      stringstream name;
      name << "mtl_" << hex << setfill('0') << setw(4) << right << (int)material << ".bmp";
			if (isTexture(material)) {
        ofstream texfile(name.str(), ofstream::binary);
	      emitTexture(material, texfile);
        texfile.close();
      }
    }
  }

    return 0;
}
