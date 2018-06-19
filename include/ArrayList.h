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

#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include <vector>

template <class T>
class List: public std::vector<T> {

//If this causes memory leaks, implement it as a function in SFXObject, rather than here. We like this form better because it
// is more object oriented, though.
	public: bool addAll(List<T> inList){

		if (this == &inList)
			return false;

		this->insert( this->end(), std::begin(inList), std::end(inList));
		return true;
	}

};

#endif // ARRAYLIST_H
