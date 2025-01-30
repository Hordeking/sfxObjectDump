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

#include "Face.h"

Face::Face()
{
	//ctor
}

Face::Face(BufferReader & buf)
{
	if (0xFE >= buf.getUByte() || !buf.good() ) return;

	nVerts = buf.nextUByte(); //nVerts;
	faceID = buf.nextUByte();
	material = buf.nextUByte();
	nx = buf.nextByte();
	ny = buf.nextByte();
	nz = buf.nextByte();

	for(size_t i = 0; i<nVerts; ++i)
		vertex.push_back(buf.nextByte());

}

Face::~Face()
{
	//dtor
}
