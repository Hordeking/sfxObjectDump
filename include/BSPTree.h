// sfxObj Object. Parser and container for Starfox (1991) and possibly
// other Argonaut Software models. Copyright (C) 2018 JD Fenech (hordeking@users.noreply.github.com)
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

#ifndef BSPTREE_H
#define BSPTREE_H

#include <string>
#include <vector>
#include <iostream>

#include "BufferReader.h"
#include "Face.h"

class BSPTree
{
	public:
		/** Default constructor */
		BSPTree();

		/** Copy Ctor */
		BSPTree(const BSPTree & rhs);

		BSPTree(BufferReader & buf);

		/** Default destructor */
		~BSPTree();

		//Returns the status of the BSP Tree.
		bool isValid(void){
							// 0x28 is a little more complicated, but essentially how it works is this:
							//  We can't call isValid() on a null pointer, and if it's null we pretty
							//	much just return true (a termination condition), otherwise we call the
							//	child's isValid(). Since non-leaf nodes also can be valid or not, we need
							//	to also pass back up their validity. Thus the complicated construction.
							if (0x40==nodeType) return true;	//Null nodes are always valid, basically deprecated since we're using null properly now.
							if (0x44==nodeType) return valid;
							if (0x28==nodeType) return (valid && (nullptr!=back?back->isValid():true) && (nullptr!=front?front->isValid():true));
							return false;
						}

		bool isNull(void){	return (0x40==nodeType);	}

		std::vector<Face> getFacegroup(void) { return facegroup; }
		std::vector<Face> getAllFaces(void);


		BSPTree& operator=(const BSPTree& rhs);

		friend std::ostream & operator<<(std::ostream & out, const BSPTree & rhs){
			out << std::hex << std::setw(2) << std::setfill('0') << (unsigned int) rhs.nodeType << " ";
			if (0x28==rhs.nodeType) out << std::hex << std::setw(2) << std::setfill('0') << (unsigned int) rhs.splittingTriangle << " ";
			out << std::hex << std::setw(4) << std::setfill('0') << (unsigned int) rhs.facegroupOffset << " ";
			if (0x28==rhs.nodeType) out << std::hex << std::setw(2) << std::setfill('0') << (unsigned int) rhs.frontBranchOffset;

			if (nullptr!=rhs.back) !rhs.back->isNull()?(out << " " << *rhs.back):(out<<40);
			if (nullptr!=rhs.front && !rhs.front->isNull() ) (out << " " << *rhs.front);

			return out;}

	protected:

		std::vector<Face> facegroup;

		bool readFacegroup(BufferReader & buf);

		uint8_t nodeType = 0;
		uint8_t splittingTriangle = 0;
		size_t facegroupOffset = 0;
		size_t frontBranchOffset = 0;
		BSPTree * back = nullptr;
		BSPTree * front = nullptr;

		bool valid = false;

	private:

			template <class T> bool append(std::vector<T> & fixedList, std::vector<T> inList){
			if (&fixedList == &inList)
				return false;

			fixedList.insert( fixedList.end(), std::begin(inList), std::end(inList));
			return true;
		}

};

#endif // BSPTREE_H
