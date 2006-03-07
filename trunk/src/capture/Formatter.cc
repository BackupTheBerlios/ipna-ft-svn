/*
  Formatter.cc - a class to format records.

  Copyright (C) 2006 Alexander Petry

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <ipna/capture/Formatter.hpp>
#include <stdexcept>

using namespace ipna;
using namespace ipna::capture;

std::ostream&
Formatter::format(ipna::parser::Record::RecordPtr record, std::ostream& os) {
  for (std::list<int>::iterator it = _columns.begin();
       it != _columns.end();
       it++) {
    int fieldId = *it;

    if (fieldId < 0) {
      switch (fieldId) {
      case -1:
	os << record->tstamp();
	break;
      case -2:
	os << record->engineId();
	break;
      default:
	throw std::out_of_range("");
      }
    } else {
      if (record->has(fieldId)) {
	os << record->get(fieldId)->toString();
      } else {
	os << (unsigned int)0;
      }
    }

    os << '\t';
  }

  os << std::endl;
  return os;
}

ipna::capture::Formatter&
operator<<(ipna::capture::Formatter& f, int field) {
  f.addField(field);
  return f;
}

