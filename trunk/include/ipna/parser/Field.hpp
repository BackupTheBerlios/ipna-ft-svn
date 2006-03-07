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
      virtual ~Field();
      
      virtual std::string toString() const;
      inline size_t getId() const { return _id; }

      unsigned int asUInt() const;
      std::string asIp() const;
    protected:
      char *_bytes;
      size_t _numBytes;
      FieldId _id;
    };

    class IPField : public Field {
    public:
      IPField(Field::FieldId id, const char* b, size_t num) : Field(id,b,num) {}
      ~IPField() {}
      
      virtual std::string toString() const;
    };

    class MACField : public Field {
    public:
      MACField(Field::FieldId id, const char* b, size_t num) : Field(id,b,num) {}
      ~MACField() {}

      virtual std::string toString() const;
    };

    class StringField : public Field {
    public:
      StringField(Field::FieldId id, const char* b, size_t num) : Field(id,b,num) {}
      ~StringField() {}

      virtual std::string toString() const;
    };
    
    std::ostream& operator<<(std::ostream&, const Field&);
  }
}

#endif // FIELD_HPP
