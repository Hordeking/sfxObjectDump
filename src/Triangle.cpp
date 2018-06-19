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

#include "Triangle.h"

//TODO: Add method to return all three vertices as an ordered array. Would be nice if we could just return the vertices themselves.

Triangle::Triangle(unsigned int v1, unsigned int v2, unsigned int v3): v1(v1), v2(v2), v3(v3)
{

}

Triangle::Triangle(const Triangle& other): v1(other.v1), v2(other.v2), v3(other.v3)
{

}

Triangle& Triangle::operator=(const Triangle& rhs)
{
	if (this == &rhs) return *this; // handle self assignment
	//assignment operator
	return *this;
}

std::string Triangle::getFormattedString()
{
	return	std::to_string(v1) +
			std::string(" ") +
			std::to_string(v2) +
			std::string(" ") +
			std::to_string(v3);
}
