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
#include <sstream>
#include <stdexcept>
#include <iomanip>

using namespace ipna;
using namespace ipna::capture;

Logger::LoggerPtr Formatter::logger = Logger::getLogger("ipna.capture.Formatter");

std::ostream&
Formatter::format(ipna::parser::Record::RecordPtr record, std::ostream& os) {
  std::stringstream out;
  
  for (std::vector<std::list<int> >::iterator col = _columns.begin();
       col != _columns.end();
       col++) {

    bool found = false;
    // check if any of the fields in the current column are in the record
    for (std::list<int>::iterator field = col->begin(); field != col->end() && !found; field++) {
      int id = *field;
	
      if (id < 0) {
	switch (id) {
	case -1:
	  out << std::setiosflags(std::ios::left);
	  out << record->tstamp();
	  out << std::resetiosflags(std::ios::left);
	  found = true;
	  break;
	case -2:
	  out << std::setiosflags(std::ios::left);
	  out << record->engineId();
	  out << std::resetiosflags(std::ios::left);
	  found = true;
	  break;
	default:
	  throw std::out_of_range("");
	}
      } else {
	if (record->has(id)) {
	  found = true;
	  try {
	        out << std::setiosflags(std::ios::left);
		size_t old_width =
			out.width(record->get(id)->getMaximumWidth());
	  	out << record->get(id)->toString();
		out.width(old_width);
	        out << std::resetiosflags(std::ios::left);
	  } catch(...) {
		LOG_WARN("field-id " << id << " of record could not be formatted!");
	  	out << "X";
	        out << std::resetiosflags(std::ios::left);
	  }
	} else {
	  found = false;
	}
      }
    }

    if (!found) {
      std::stringstream sstr;
      sstr << "the processed record did not have any of the given fields: ";
      for (std::list<int>::iterator field = col->begin(); field != col->end(); field++) {
	sstr << " " << *field;
      }
      throw std::runtime_error(sstr.str());
    }
    
    out << "  ";
  }

  out << std::endl;
  return os << out.str();
}

