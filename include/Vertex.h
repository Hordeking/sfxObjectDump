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

#ifndef VERTEX_H
#define VERTEX_H

#include <string>
#include <vector>
#include "BufferReader.h"

// Vertices are stored in the native Star Fox coordinate system.
// +z forward
// -y up
// +x right
// TODO: Build in a few different conversions, in particular +z forward to +x forward, -y up to +z up, +x right to -y right

class Vertex
{
    public:
        /** Default constructor */
        Vertex();

        /** Copy constructor
         *  \param other Object to copy from
         */
//      Vertex(const Vertex& other);

        /** Assignment operator
         *  \param other Object to assign from
         *eturn A reference to this
         */
        Vertex& operator=(const Vertex& other);

        Vertex(signed int x, signed int y, signed int z, bool sh = false);
        Vertex(BufferReader & buf, bool sh = false);

        /** Flip Copy Constructor
         *  \param v Object to copy from
		 *	\param xflip Mirror X coord
		 *	\param yflip Mirror Y coord
		 *	\param zflip Mirror Z coord
		 */
        Vertex(const Vertex & v, bool xflip = false, bool yflip = false, bool zflip = false);

        std::vector<int> getCoords(void);

        std::string getFormattedString();

    protected:
    private:
        signed int x; //!< Member variable "x"
        signed int y; //!< Member variable "y"
        signed int z; //!< Member variable "z"
        bool sh; //!< Member variable "sh"
};

#endif // VERTEX_H
