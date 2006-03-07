/*
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
#include <ipna/parser/Field.hpp>
#include <iostream>
#include <sstream>

using namespace std;
using namespace ipna::parser;

Field::Field(Field::FieldId id, const char* bytes, size_t numBytes)
  : _id(id), _numBytes(numBytes) {
  memcpy(_bytes,bytes,numBytes);
}

std::string
Field::toString() const {
  stringstream s;
  s << asUInt();
  return s.str();
}

unsigned int
Field::asUInt() const {
  assert(0 < _numBytes && _numBytes <= 4);

  switch (_numBytes) {
  case 1:
    return (unsigned int)(*_bytes);
  case 2:
    return ntohs(*(unsigned short*)_bytes);
  case 4:
    return ntohl(*(unsigned int*)_bytes);
  default:
    assert(false);
  }
}

std::string
Field::asIp() const {
  if (4 == _numBytes) {
    // ipv4
    stringstream s;
    for (unsigned int byte = 0; byte < _numBytes; byte++) {
      unsigned int b = (unsigned char)(_bytes[byte]);
      s << b;
      if (byte < _numBytes-1) s << ".";
    }
    return s.str();
  } else if (16 == _numBytes) {
    // ipv6
    return "not yet implemented";
  } else {
    return "not an ip";
  }
}

std::ostream&
operator<<(std::ostream& os, const Field& field) {
  return os << field.toString();
}
