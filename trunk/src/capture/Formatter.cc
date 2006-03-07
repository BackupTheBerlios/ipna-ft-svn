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

using namespace ipna;
using namespace ipna::capture;

Formatter::Formatter() {
  _columns.push_back(std::make_pair<unsigned int, EntryType>(10, NUMBER));
  _columns.push_back(std::make_pair<unsigned int, EntryType>(8,  IP));
  _columns.push_back(std::make_pair<unsigned int, EntryType>(9,  NUMBER));
  _columns.push_back(std::make_pair<unsigned int, EntryType>(14, NUMBER));
  _columns.push_back(std::make_pair<unsigned int, EntryType>(12, IP));
  _columns.push_back(std::make_pair<unsigned int, EntryType>(13, NUMBER));
  _columns.push_back(std::make_pair<unsigned int, EntryType>(4,  NUMBER));
  _columns.push_back(std::make_pair<unsigned int, EntryType>(7,  NUMBER));
  _columns.push_back(std::make_pair<unsigned int, EntryType>(11, NUMBER));
  _columns.push_back(std::make_pair<unsigned int, EntryType>(2,  NUMBER));
  _columns.push_back(std::make_pair<unsigned int, EntryType>(1,  NUMBER));
}

std::ostream&
Formatter::format(ipna::parser::Record::RecordPtr record, std::ostream& os) {
  os
    << record->tstamp() << '\t'
    << record->engineId() << '\t';
  
  for (std::vector<std::pair<unsigned int, EntryType> >::iterator it = _columns.begin();
       it != _columns.end();
       it++) {
    unsigned int fieldId = it->first;
    EntryType entryType = it->second;

    if (record->has(fieldId)) {
      switch (entryType) {
      case NUMBER:
	os << record->get(fieldId)->asUInt();
	break;
      case IP:
	os << record->get(fieldId)->asIp();
	break;
      default:
	assert(false);
      }
    } else {
      os << (unsigned int)0;
    }

    os << '\t';
  }

  os << std::endl;
  return os;
}
