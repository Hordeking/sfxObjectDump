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

#include <iostream>
#include <iomanip>

#include "SFXObject.h"

SFXObject::SFXObject(std::ifstream & inData, size_t vertexAddress, size_t facedataAddress): buf(inData), vertexAddress(vertexAddress), facedataAddress(facedataAddress)
{

	//Start by sucking in those vertices

	//std::vector<std::vector<Vertex>> vertexAll;
	std::vector<Vertex> vertexFrame;

	buf.seekg(vertexAddress);

	size_t iFrame = 0;

	while ( (iFrame < nFrames) && buf.good() )
	{
		// add vertex bloc
		append(vertexFrame, buildVertexBloc() );

		uint8_t listType = buf.nextUByte();

		switch (listType)
		{
			case 0x0C:	// end vertex list --> We will go to the next frame
				vertexList.push_back(vertexFrame);
				iFrame++;
				//This is intended to reset the buffer to the beginning of the object. We'll do a seekg with the vertex address to get this.
				buf.seekg(vertexAddress, buf.beg);	//buf = new BufferReader(buf, 0);
				vertexFrame.clear();	//Clear vertexFrame so we can use it again.
				break;

			case 0x20:	// jump marker
				//Jump to end of animated vertex list
				buf.seekg(buf.nextShort() -1 , buf.cur); //buf.offset += buf.nextUShort() + 1;
				break;

			case 0x1C:	// animated vertex list --> update frame number
				nFrames = buf.nextUByte();

				// go to frame offset
				buf.seekg(iFrame*2, buf.cur);		//buf.offset += iFrame * 2;

				// jump to offset for vertex
				buf.seekg(buf.nextShort() - 1, buf.cur);		//buf.offset += buf.nextUShort() + 1;
				//std::cerr << "Animated Frame " << iFrame << std::endl;
				break;

			default:
				// unknown
				std::cerr << std::hex << std::setfill('0');
				std::cerr << "In Frame " << (int) iFrame << ", Unknown vertex list type 0x" << std::setw(2) << (int)listType << " at address 0x" << std::setw(8) << (unsigned int)buf.tellg() -1 << std::endl;
				std::cerr << std::setw(0) << std::setfill(' ') << std::dec;
				valid = false;
				return;	//Return empty vector if failure.

		}

	}

	if (vertexList.size() <= 0) return;		//If for some reason we wound up with no frames, then it isn't valid and don't even bother continuing.

	// We need to make sure each frame has the same number of elements.
    for(auto it = vertexList.begin(); it!=vertexList.end(); ++it){

		if ( vertexList.end()==(it+1) ) break;

		// If we're here, then we should be able to compare. Essentially we're doing a O(n) comparison of a[n] =? a[n+1]
		// Assuming that if any differ at all, we exit immediately. if they're all the same, then each one should be
		// equal to the previous, and they should all be equal to each other. Essentially, if A = B, and B = C, then A = C.
		// Conversely, if A != B, and B = C, then A != C, and we kick out immediately.

		//This looks a little wierd, because we have to dereference the iterators once we have the,
		if ((*it).size() != (*(it+1)).size() ) return;	//If they aren't equal in size, then something is wrong.
    }

	// Probably not strictly required, but since the header can specify an address for the face-data,
	// we'll use that. Do any objects have face-triangle data that doesn't immediately follow the
	// vertex list?
	// YES. Strictly required. More objects need this than not. Some objects such as billboards do reuse vertex data,
	// while using their own face data. Since the face colors and textures are defined by the polygons in the face
	// groups, each facedata section is unique to that object, while the vertex data might be shared among many objects
	// that have the same shape such as asteroids, flat objects intended to display a texture, and ship reskins/repaints.
	buf.seekg(facedataAddress, buf.beg);

	uint8_t listType = buf.nextUByte();

	// is there a triangle list? Probably should not be optional.
	if (listType == 0x30)
	{
		const size_t nTriangles = buf.nextUByte();

		triangles.reserve(nTriangles);

		for (size_t i = 0; i < nTriangles; i++){
			const uint8_t tri_1 = buf.nextUByte(), tri_2 = buf.nextUByte(), tri_3 = buf.nextUByte();
			triangles.push_back(Triangle(tri_1, tri_2, tri_3));
		}

		// get next list type
		listType = buf.nextUByte();
	}

	/*

	// This is the model data for the Andross Square [ID: AC4D] (the comm sprite, or the one seen in the boss fight?)
	// It's interesting because it has no bsp tree, but it does have a face group.
	// This is probably more common when the object is flat (all vertices in the same plane) or isn't really splittable.

	// To be determined: If the object has one face group, the original object doesn't have a bsp tree. We can unify our
	// interface by simply creating a tree with node type 0x44 and creating the pointers to the face group. This might be
	// a little wierd if we ever try to push it back into code, but we can always try to do the reverse operation if our
	// tree only has the one node.

	// Is it possible for an object to have two face groups? A single-level bsp tree can do one group, and a two-level
	// can accomodate three group. Two groups might be done without using a tree at all?

	// This is the vertex list at 0x8f26a. All vertices are coplanar.
	// This particular vertex list is shared by several different objects.
	38 02 64 64 00 9C 9C 00 0C

	// This is the face data at 0x8f282
	30 02		// Two triangles
	02 03 00	// Neither of which use vertex 01.
	00 03 02	// It does get used in the face group polys (the real polygon list)

	// One face group with two faces, but no bsp tree. Probably common for much simpler objects such as billboard objects.
	14
	04 00 00 00 00 7F 02 03 00 01		// This is the front side, note the normal here is positive 0,0,127 and the vertex order
	04 01 01 00 00 81 01 00 03 02		// This one is the back side. Normal is 0,0-127 with a vertex order exactly reversed
	FE 00

	*/

	// is there a BSP Tree ?
	if (0x3C == listType)
	{
		bsptreeAddress = size_t(buf.tellg())-1;
		bsptreeOffset = bsptreeAddress - facedataAddress;

		bspTree = new BSPTree(buf);

		if( nullptr!=bspTree && !bspTree->isValid() ) return;

		// We've already read them in, but once we finish the bsp tree, the
		// buffer should be ppointed at the first byte of the first facegroup.
		facegroupsAddress = size_t(buf.tellg());
		facegroupsOffset = facegroupsAddress - facedataAddress;

		//If we got to this point, and the BSP Tree is valid, then the object is valid.
		valid = true;

		// Why do we return here instead of reading more? Easy, because if we have a
		// bsp tree, we've already read all of the face groups. If something went
		// wrong, we can handle it with the BSP tree.

		return;
	}


	// face group ?
	// Do we need to check this if we didn't have a bsp tree?	YES!
	// Can we have facegroups without a bsp tree?	YES! ABSOLUTELY!

	// I suspect that if we don't have a bsp tree, then we only wind up with one
	// face group in all cases. There are three face data lumps that are a little
	// strange. These are at 0x8f22d, 0x8f2c0, & 0x8f2ca

	//For the moment, if we got here, assume that our object is perfectly valid
	if (0x14==listType){
		buf.seekg(-1, buf.cur);	//Reset the position back one, so that we also bring in the 0x14
		facegroupsAddress = size_t(buf.tellg());
		facegroupsOffset = facegroupsAddress - facedataAddress;
		bsptreeOffset = -1;
		if (nullptr==bspTree){	// Only do this if we don't already have a bsp tree.
			bspTree = new BSPTree(buf);
			if (nullptr!=bspTree) valid = bspTree->isValid();
		}
	}


	return;

}

std::vector<Vertex> SFXObject::buildVertexBloc()
{
	std::vector<Vertex> result;

	while (true)
	{
		uint8_t listType = buf.getUByte();	// Peek at next byte

		switch (listType)
		{
			case 0x04:	// pointsb
			case 0x08:	// pointsw
			case 0x34:	// pointsxw
			case 0x38:	// pointsxb
				// add vertex bloc to result
				append(result, buildVertexSubBloc() );
				break;

			case 0x0C:
				// end vertex list
			case 0x20:
				// 16 bits offset jump
			case 0x1C:
				// animated vertex list (handled in parent)
				return result;

			default:
				// unknown
				std::cerr << std::hex << std::setfill('0');
				std::cerr << "Unknown vertex list type 0x" << std::setw(2) << (int) listType << " at address 0x" << std::setw(8) << (unsigned int)buf.tellg() - 1 << std::endl;
				std::cerr << std::setw(0) << std::setfill(' ') << std::dec;
				valid = false;
				exit(-1);
				return result;
		}
	}

}


std::vector<Vertex> SFXObject::buildVertexSubBloc()
{
	bool xflip = false;
	bool yflip = false;
	bool sh = false;

	int listType = buf.nextUByte();
	//std::cerr << "List Type: " << listType << std::endl;

	switch (listType)
	{
		case 0x04:
			// normal vertex list
			break;

		case 0x08:
			// 16 bits vertex list
			sh = true;
			break;

		case 0x34:
			// 16 bits + X flip vertex list
			xflip = true;
			sh = true;
			break;

		case 0x38:
			// X flip vertex list
			xflip = true;
			break;
	}

	std::vector<Vertex> result;
	size_t num = buf.nextUByte();	// How many vertices to read in

	//std::cerr << num << " vertices in this subblock." << std::endl;

	for (size_t i = 0; i < num; i++)
	{
		Vertex v(buf, sh);

		result.push_back(v);

			if (xflip)
				result.push_back(Vertex(v, true, false, false));
			if (yflip)
				result.push_back(Vertex(v, false, true, false));
	}

	return result;
}

void SFXObject::buildFace()
{
/*
	if (buf.offset >= buf.buffer.length)
	{
		System.err.println("Face data address incorrect : " + Integer.toHexString(buf.offset));
		valid = false;
		return;
	}

	List<FaceGroup> faceGroupList = new ArrayList<FaceGroup>();

	int listType = buf.nextUByte();

	// is there a triangle list ?
	if (listType == 0x30)
	{
		triangles = new Triangle[buf.nextUByte()];
		for (int i = 0; i < triangles.length; i++)
			triangles[i] = new Triangle(buf);

		// get next list type
		listType = buf.nextUByte();
	}

	// is there a BSP Tree ?
	if (listType == 0x3C)
	{
		bspTree = new BSPTree(null, buf, triangles, faceGroups);
		buf.offset += bspTree.getTotalSize() - bspTree.size;

		// get next list type
		listType = buf.nextUByte();
	}

	// face group ?
	while (listType == 0x14)
	{
		faceGroupList.add(new FaceGroup(buf));

		// get next list type
		listType = buf.nextUByte();
	}

	if (faceGroupList.size() == 0x00)
	{
		System.err.println("No facegroup found !");
		valid = false;
	}

	faceGroups = faceGroupList.toArray(new FaceGroup[faceGroupList.size()]);

	// link facegroup address to facegroup object in BSP tree
	if (bspTree != null)
		bspTree.setFaceGroup(faceGroups);

	if (listType != 0x00)
	{
		System.err.println("00 end mark expected but " + Integer.toHexString(listType) + " found !");
		valid = false;
	}
*/
}

bool SFXObject::isValid(void) {

	/*if (nullptr!=bspTree) {
		return valid&&bspTree->isValid();
	}
	return false;
	*/
	return valid;

}

void SFXObject::buildTriangle()
{
/*
		triangles = new Triangle[buf.nextUByte()];
		for (int i = 0; i < triangles.length; i++)
			triangles[i] = new Triangle(buf);
*/
}
