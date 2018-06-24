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

#include "BufferReader.h"

	int8_t BufferReader::getByte(int off)
	{
		return 0;
	}

	uint8_t BufferReader::getUByte(int off)
	{
		return 0;
	}

	int16_t BufferReader::getShort(int off)
	{
		return 0;
	}

	uint16_t BufferReader::getUShort(int off)
	{
		return 0;
	}

	int32_t BufferReader::getInt(int off)
	{
		return 0;
	}

	uint32_t BufferReader::getUInt(int off)
	{
		return 0;
	}



	uint8_t BufferReader::getUByte()
	{
		return getByte();
	}

	uint16_t BufferReader::getUShort()
	{
		return getShort();
	}

	uint32_t BufferReader::getUInt()
	{
		return getInt();
	}


	int8_t BufferReader::getByte()
	{
		int8_t output = 0;
		this->read((char *) &output, 1 );
		this->seekg(-1, std::stringstream::cur);
		return output;
	}

	int16_t BufferReader::getShort()
	{
		int16_t output = 0;
		this->read((char *) &output, 2 );
		this->seekg(-2, std::stringstream::cur);
		return output;
	}
	int32_t BufferReader::getInt()
	{
		int32_t output = 0;
		this->read((char *) &output, 4 );
		this->seekg(-4, std::stringstream::cur);
		return output;
	}



	uint8_t BufferReader::nextUByte()
	{
		return nextByte();
	}

	uint16_t BufferReader::nextUShort()
	{
		return nextShort();
	}

	uint32_t BufferReader::nextUInt()
	{
		return nextInt();
	}



	int8_t BufferReader::nextByte()
	{
		int8_t output = 0;
		read((char *) &output, 1 );
		return output;
	}

	int16_t BufferReader::nextShort()
	{
		int16_t output = 0;
		read((char *) &output, 2 );
		return output;
	}

	int32_t BufferReader::nextInt()
	{
		int32_t output = 0;
		read((char *) &output, 4 );
		return output;
	}
