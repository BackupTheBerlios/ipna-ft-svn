/*
  Formatter.hpp - a class to format records.

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

#ifndef _FORMATTER_HPP
#define _FORMATTER_HPP 1

#include <ostream>
#include <list>
#include <utility>
#include <boost/shared_ptr.hpp>
#include <ipna/parser/Record.hpp>

namespace ipna {
  namespace capture {
    class Formatter {
    public:
      typedef boost::shared_ptr<Formatter> FormatterPtr;

      enum EntryType { NUMBER, IP, MAC };

      // add a format object as parameter...
      explicit Formatter() {}
      virtual ~Formatter() {}

      // these functions may be used to group records in some way
      virtual void startRecordSet() {}
      virtual void endRecordSet() {}

      inline void addField(int field) {
	_columns.push_back(field);
      }
      inline void clear() {
	_columns.clear();
      }

      std::ostream& format(ipna::parser::Record::RecordPtr record, std::ostream& os);
    protected:
      std::list<int> _columns;
    };
  } // capture
} // ipna

ipna::capture::Formatter& operator<<(ipna::capture::Formatter& f, int field);

#endif // FORMATTER_HPP
