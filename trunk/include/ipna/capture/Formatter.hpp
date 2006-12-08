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
#include <vector>
#include <utility>
#include <boost/shared_ptr.hpp>
#include <ipna/parser/Record.hpp>
#include <algorithm>

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


      inline bool hasColumnField(size_t column, int field) const {
      	std::list<int>::const_iterator s = _columns[column].begin();
      	std::list<int>::const_iterator e = _columns[column].end();
	return std::find(s,e,field) != e;
      }
      inline void addFieldToColumn(size_t column, int field) {
	_columns[column].push_back(field);
      }
      inline size_t addColumn() {
        _columns.push_back(std::list<int>());
	return _columns.size()-1;
      }
      inline void clear() {
	_columns.clear();
      }
      
      std::ostream& format(ipna::parser::Record::RecordPtr record, std::ostream& os);
    protected:
      std::vector<std::list<int> > _columns;
    };
  } // capture
} // ipna

#endif // FORMATTER_HPP
