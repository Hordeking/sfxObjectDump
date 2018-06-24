// sfxObj Object. Parser and container for Starfox (1991) and possibly
// other Argonaut Software models. Copyright (C) 2018 JD Fenech
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

#ifndef SFXOBJECT_H
#define SFXOBJECT_H

#include <fstream>
#include <vector>

#include "BufferReader.h"
#include "Vertex.h"
#include "Triangle.h"
#include "BSPTree.h"


class SFXObject
{
	public:

		// This object will expect that data begins with proper object data
		// vertexAddress and faceAddress are really just holdovers from Stef's decoder, and not really necessary here.
		// We might use them to cross-check stuff, though.
		SFXObject(std::ifstream & inData, size_t vertexAddress=0, size_t facedataAddress=0);
		~SFXObject(){ if (nullptr!=bspTree) delete bspTree;}

		// Returns the vertex list for a particular frame.
		std::vector<Vertex> getVertexList(size_t iFrame = 0) {	return vertexList.at(iFrame); }

		// Returns the triangle list. All frames share the same polygon and triangle data.
		std::vector<Triangle> getTriangleList(void) {	return triangles; }

		// Returns the triangle list. All frames share the same BSP Tree.
		BSPTree getBSPTree(void) {	if (nullptr!=bspTree) return *bspTree; else throw; }

		// Returns the full face list. All frames share the same face list.
		std::vector<Face> getFaceList(void) {	if (nullptr!=bspTree) return bspTree->getAllFaces(); else throw; }

		size_t frameCount(void) { return nFrames; }

		bool isValid(void);

		BSPTree * bspTree = nullptr;

	protected:

		BufferReader buf;
		size_t vertexAddress = 0;
		size_t facedataAddress = 0;
		size_t bsptreeAddress = 0;
		size_t facegroupsAddress = 0;
		bool valid = false;

		std::vector<std::vector<Vertex>> vertexList;
		std::vector<Triangle> triangles;

		// We also need some auxilliary data from the header, like the palette and scaling. These should get passed in on creation.

		// We can use this to output the original object data, since we build this as we go.
		// This will be the original object as in the rom, not constructed from our interpretation of it.
		// In theory, this should be a correct object in the end, without any trailing rom data.
		std::stringstream rawObject_Final;

		// Some internal variables, for our use later
		// These refer to the offset from the beginning of the data lump we're given.
		// We'll set these as we parse through the object, saving them for later.
		size_t vertexArrayOffset = 0;	//Always 0, only data in this lump anyway.
		size_t triangleArrayOffset = 0;	//Always 0, first data in this lump.
		size_t bsptreeOffset = 0;
		size_t facegroupsOffset = 0;

		size_t nFrames = 1;		// One frame by default

		std::vector<Vertex> buildVertexBloc();
		std::vector<Vertex> buildVertexSubBloc();

		void buildFace();
		void buildTriangle();

	private:

		template <class T>
		bool append(std::vector<T> & fixedList, std::vector<T> inList){
			if (&fixedList == &inList)
				return false;

			fixedList.insert( fixedList.end(), std::begin(inList), std::end(inList));
			return true;
		}
};

#endif // SFXOBJECT_H
