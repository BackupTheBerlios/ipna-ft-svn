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
#include <stdexcept>

#include <QHostAddress>

using namespace std;
using namespace ipna::parser;

Field::Field(Field::FieldId id, const char* bytes, size_t numBytes)
  : _id(id), _numBytes(numBytes == 0 ? 1 : numBytes) {
  _bytes = new char[_numBytes];
  memcpy(_bytes,bytes,_numBytes);
}

Field::~Field() {
  if (NULL != _bytes) {
    delete [] _bytes;
    _bytes = NULL;
  }
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
    return (unsigned int)(_bytes[0] & 0xff);
  case 2:
    return ntohs(*(unsigned short*)_bytes & 0xffff);
  case 4:
    return ntohl(*(unsigned int*)_bytes);
  default:
    throw std::runtime_error("cannot convert to an int longer than 4 bytes");
  }
}

std::ostream&
operator<<(std::ostream& os, const Field& field) {
  return os << field.toString();
}

std::string
IPField::toString() const {
  if (4 == _numBytes) {
    return QHostAddress(asUInt()).toString().toStdString();
  } else if (16 == _numBytes) {
    return QHostAddress(_bytes).toString().toStdString();
  } else {
    throw std::runtime_error("no ip address");
  }
}

std::string
MACField::toString() const {
  if (6 == _numBytes) {
    std::stringstream sstr;
    sstr << std::hex;
    for (std::size_t i = 0; i < _numBytes - 1; i++) {
      sstr << _bytes[i] << ":";
    }
    sstr << _bytes[_numBytes-1];
    return sstr.str();
  } else {
    throw std::runtime_error("no mac address");
  }
}

std::string
StringField::toString() const {
  return std::string(_bytes);
}
