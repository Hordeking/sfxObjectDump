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

#include "Normal.h"

//Should really add a method to this one to allow it to calculate from any three vertices and/or one triangle.

Normal::Normal(signed int nx, signed int ny, signed int nz): nx(nx), ny(ny), nz(nz)
{
	//ctor
}

Normal::Normal(const Normal& other): nx(other.nx), ny(other.ny), nz(other.nz)
{
	//copy ctor
}

Normal& Normal::operator=(const Normal& rhs)
{
	if (this == &rhs) return *this; // handle self assignment
	//assignment operator
	nx = rhs.nx;
	ny = rhs.ny;
	nz = rhs.nz;
	return *this;
}

float Normal::length(void) { return sqrt(nx*nx+ny*ny+nz*nz); }
