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

#ifndef BUFFERREADER_H
#define BUFFERREADER_H

#include <fstream>
#include <iostream>
#include <iomanip>

class BufferReader : public std::stringstream {

	public: BufferReader(std::ifstream & input){	operator<<(input.rdbuf());	}

	public:  uint8_t getUByte(int off);
	public: uint16_t getUShort(int off);
	public: uint32_t getUInt(int off);
	public:   int8_t getByte(int off);
	public:  int16_t getShort(int off);
	public:  int32_t getInt(int off);

	public:  uint8_t getUByte();
	public: uint16_t getUShort();
	public: uint32_t getUInt();

	public:  int8_t getByte();
	public: int16_t getShort();
	public: int32_t getInt();

	public:  uint8_t nextUByte();
	public: uint16_t nextUShort();
	public: uint32_t nextUInt();

	public:  int8_t nextByte();
	public: int16_t nextShort();
	public: int32_t nextInt();

};

#endif // BUFFERREADER_H
