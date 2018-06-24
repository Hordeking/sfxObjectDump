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

#ifndef FACE_H
#define FACE_H

#include <string>
#include <vector>

#include "BufferReader.h"
#include "Vertex.h"

class Face
{
	public:
		/** Default constructor */
		Face();
		Face(BufferReader & buf);
		/** Default destructor */
		virtual ~Face();

	public: unsigned int nVerts;
	public: uint8_t faceID;
	public: uint8_t color;
	public: int8_t nx, ny, nz;
	public: std::vector<uint8_t> vertex;
	public: std::string getFormattedString()
			{
				return	std::string("nVerts: ")+std::to_string(int(nVerts))+
						std::string("	faceID: ")+std::to_string(faceID)+
						std::string("	Color: ")+std::to_string(int(color))+
						std::string("	Normal: [")+std::to_string(nx)+", "+std::to_string(ny)+", "+std::to_string(nz)+"]";

			}
};

#endif // FACE_H
