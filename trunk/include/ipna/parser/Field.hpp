/*
  Field.hpp - a class that holds information about record fields.

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


#ifndef FIELD_HPP
#define FIELD_HPP

#include <boost/shared_ptr.hpp>
#include <ipna/network/Packet.hpp>

#include <arpa/inet.h>
#include <ostream>
#include <string>

namespace ipna {
  namespace parser {
    class Field {
    public:
      typedef unsigned int FieldId;
      typedef boost::shared_ptr<Field> FieldPtr;
      
      Field(FieldId id, const char* bytes, size_t numBytes);
      virtual ~Field() {}
      
      virtual std::string toString() const;
      inline size_t getId() const { return _id; }

      unsigned int asUInt() const;
      std::string asIp() const;
    private:
      char _bytes[16];
      size_t _numBytes;
      FieldId _id;
    };

    std::ostream& operator<<(std::ostream&, const Field&);
  }
}

#endif // FIELD_HPP
