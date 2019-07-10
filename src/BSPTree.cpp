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

#include "BSPTree.h"

BSPTree::BSPTree(){nodeType=0x40;}	//A null node has type 0x40

BSPTree::BSPTree(BufferReader & buf)
{
	//ctor
	nodeType = buf.getUByte();

	if (0x3c == nodeType) {

		// This just handles a potential situation where someone handed us the bsp with the start marker in it.
		// That should not happen in real life.

		// If it ever does get passed in, we should only see it once, at the very beginning.
		// The design of this object isn't really conducive to a keeping track of a single initial
		// marker.

		// If it does become needed for some reason to handle it, all we need to do is advance to the next byte.
		// For now, let's just fail.
		throw std::runtime_error("BSP Tree Initial Marker Found, but not handled by this object");
		buf.nextUByte();	// Just jump to the next byte, we'll be using that one.
		nodeType = buf.getUByte();

	}

	if (0x14 == nodeType) {

		// Not a real BSP Tree in the object. Just a facegroup.
		// This is meant to handle the situation where all we have is a facegroup.
		// If we just have a bare facegroup, then we create a single 0x44 node
		// and populate it with the facegroup.

		nodeType = 0x44;			// A single node with no children
		facegroupOffset = 0x0000;	// No jump to get to the facegroup.

		// If we want to serialize this thing, we can see if there's any offset at all.
		// If not, then we know there was really no tree to begin with.
		// If it turns out there really are 0x44 nodes with offset 0x0000, we'll have
		// to bite the bullet and add a flag to indicate this. That would get set here,

		// No need to seek, we should already be sitting at the beginning
		// of the face group.
		valid = readFacegroup(buf);

		return;

	}

	//Monarch dodora head seems to have a strange node that starts with 0x40. Is this a null node?
	//Moreover, do we need to handle this? Yes. See below.

	// Anything after this should be valid in a bsp tree, even a null node indicator.
	if(nodeType != 0x28 && nodeType != 0x44 && nodeType != 0x40) {
		throw std::runtime_error("Invalid BSP Tree Node");
	}

	buf.nextUByte();	// Advance offset by a byte, we haven't actually done that yet

	if (0x40 == nodeType){
		//This turns out to be a null child. Set it to valid and return.
		return;
	}

	if (nodeType == 0x28) splittingTriangle = buf.nextUByte();	// Only happens in a 0x28

	facegroupOffset = buf.nextShort();			// All nodes have a face group.

	size_t currentPosition = buf.tellg();
	buf.seekg(facegroupOffset-1, buf.cur);		//Jump to the facegroup.
	valid = readFacegroup(buf);					//Read in polygons
	buf.seekg(currentPosition, buf.beg);		//Restore position as if we never left

	if (nodeType == 0x28) {						// Only 0x28 nodes have children.

		frontBranchOffset = buf.nextUByte();	//Once we have this, we're actually at the left node now, and we can suck that node right in.

		//Allocate children.

		// Back child. This is the left branch. Always follows immediately in flat data.
		// Peek at the next node just to see if se should create it. If the node begins
		// with 0x40, it's actually a null node. Don't even fool with it.
		if (0x40!=buf.getUByte()){
			back = new BSPTree(buf);	// Create branch
		}
		else {
			buf.nextUByte();	// Waste byte
		}

		//Front child. This is the right branch. Our node will have an offset that points at it.
		//	We don't even need to seekg to it with the buffer, as once we process each back branch, we can move on to the front branch.
		//	The front branch offset is more for direct traversal in live code, an efficiency which we don't actually need here.
		if (frontBranchOffset!=0x0000){
			front = new BSPTree(buf);
		}
		else {
			// Explicitly handle the potential situation where we have a null front/right child
			// Turns out this happens.
			//front = new BSPTree();	//Default ctor creates a null child.
			// Do nothing.
		}
	}

	return;
}

bool BSPTree::readFacegroup(BufferReader & buf){

	if(0x14==buf.nextUByte()){	//Face group start marker 0x14. Better have one.
		//std::cerr << "Valid face group marker at " << std::hex << size_t(buf.tellg())-1 << std::endl;
	}
	else {
		//std::cerr << "Invalid face group marker at " << std::hex << (int) (buf.tellg()-1) << std::endl;
		//throw std::runtime_error("Invalid face group marker.");
		return false;
	}

	while (0xFE > buf.getUByte() && buf.good() ){

		Face myPolygon;

		myPolygon.nVerts = buf.nextUByte(); //nVerts;
		myPolygon.faceID = buf.nextUByte();
		myPolygon.color = buf.nextUByte();
		myPolygon.nx = buf.nextByte();
		myPolygon.ny = buf.nextByte();
		myPolygon.nz = buf.nextByte();

		for(size_t i = 0; i<myPolygon.nVerts; ++i)
			myPolygon.vertex.push_back(buf.nextByte());

		facegroup.push_back(myPolygon);
	}

	//Need to issue warning if we encounter an 0xFE group end marker. Not sure how to do this in a reasonable manner.
	return true;
}

BSPTree::BSPTree(const BSPTree & rhs){

	// Now we proceed with the copy
	nodeType = rhs.nodeType;
	splittingTriangle = rhs.splittingTriangle;
	facegroupOffset = rhs.facegroupOffset;
	frontBranchOffset = rhs.frontBranchOffset;
	valid = rhs.valid;

	//Copy the actual polygon data.
	for(auto polygon: rhs.facegroup){
		facegroup.push_back(polygon);
	}

	if (nullptr!=rhs.back) {
		back = new BSPTree();
		*back = *(rhs.back);
	}

	if (nullptr!=rhs.front) {
		front = new BSPTree();
		*front = *(rhs.front);
	}

	return;
}

BSPTree& BSPTree::operator=(const BSPTree& rhs){

	// Handle self assignment
	if (&rhs==this) return *this;

	// This is a deep copy. If this item was already set up, just blow away everything
	if (nullptr!=back) delete back; back = nullptr;
	if (nullptr!=front) delete front; front = nullptr;

	// Now we proceed with the copy
	nodeType = rhs.nodeType;
	splittingTriangle = rhs.splittingTriangle;
	facegroupOffset = rhs.facegroupOffset;
	frontBranchOffset = rhs.frontBranchOffset;
	valid = rhs.valid;

	//Copy the actual polygon data.
	for(auto polygon: rhs.facegroup){
		facegroup.push_back(polygon);
	}

	if (nullptr!=rhs.back) {
		back = new BSPTree();
		*back = *(rhs.back);
	}

	if (nullptr!=rhs.front) {
		front = new BSPTree();
		*front = *(rhs.front);
	}

	return *this;

}


BSPTree::~BSPTree()
{
	//dtor
	//Destruct children first.
	if(nullptr!=back) { delete back; back=nullptr;}
	if(nullptr!=front) { delete front; front=nullptr;}
}

std::vector<Face> BSPTree::getAllFaces(void) {

	std::vector<Face> tempFacegroup = facegroup;

	if (nullptr!=back) append(tempFacegroup, back->getAllFaces());
	if (nullptr!=front) append(tempFacegroup, front->getAllFaces());
	return tempFacegroup;

}
