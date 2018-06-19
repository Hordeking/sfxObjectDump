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

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <string>

class Triangle
{
	public:
		/** Set constructor */
		Triangle(unsigned int v1, unsigned int v2, unsigned int v3);

		/** Copy constructor
		 *  \param other Object to copy from
		 */
		Triangle(const Triangle& other);

		/** Assignment operator
		 *  \param other Object to assign from
		 *  eturn A reference to this
		 */
		Triangle& operator=(const Triangle& other);

		std::string getFormattedString();

		unsigned int Getv1() { return v1;}
		unsigned int Getv2() { return v2;}
		unsigned int Getv3() { return v3;}

	protected:

		unsigned int v1;
		unsigned int v2;
		unsigned int v3;

	private:
};


#endif // TRIANGLE_H
