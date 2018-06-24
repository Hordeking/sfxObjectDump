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

#include "Vertex.h"

#include <iostream>
    using namespace std;

Vertex::Vertex(): x(0), y(0), z(0), sh(false)
{
    //ctor
}

Vertex::Vertex(signed int x, signed int y, signed int z, bool sh): x(x), y(y), z(z), sh(sh)
{
    //Fancy ctor
}

Vertex::Vertex(const Vertex & v, bool xflip, bool yflip, bool zflip){

    //Copy and Mirror ctor

    sh = v.sh;

    x = xflip?(-v.x):(v.x);
    y = yflip?(-v.y):(v.y);
    z = zflip?(-v.z):(v.z);

//  if (xflip)
//      x = -v.x;
//  else
//      x = v.x;

//  if (yflip)
//      y = -v.y;
//  else
//      y = v.y;


//  if (zflip)
//      z = -v.z;
//  else
//      z = v.z;

}

Vertex::Vertex(BufferReader & buf, bool sh): sh(sh) {

    x = sh?buf.nextShort():buf.nextByte();
    y = sh?buf.nextShort():buf.nextByte();
    z = sh?buf.nextShort():buf.nextByte();

}

//Vertex::Vertex(const Vertex& other): x(other.x), y(other.y), z(other.y), sh(other.sh)
//{
    //copy ctor
//}

Vertex& Vertex::operator=(const Vertex& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator

    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    sh = rhs.sh;

    return *this;
}

std::vector<int> Vertex::getCoords(void){
    std::vector<int> outval;
    outval.push_back(x);
    outval.push_back(y);
    outval.push_back(z);
    return outval;
}
std::string Vertex::getFormattedString(){
    //Starfox Coords: +x-left, +z-forward
    return  std::to_string(x) +
            std::string(" ") +
            std::to_string(y) +
            std::string(" ") +
            std::to_string(z);

    //Standard Coords: +x-left, +z-forward
/*    return  std::to_string(z) +
            std::string(" ") +
            std::to_string(-x) +
            std::string(" ") +
            std::to_string(-y);
*/
}
