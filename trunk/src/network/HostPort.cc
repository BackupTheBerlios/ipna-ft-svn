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
#include <ipna/network/HostPort.hpp>

#include <vector>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>

using namespace ipna;
using namespace ipna::network;

Logger::LoggerPtr HostPort::logger = Logger::getLogger("ipna.network.HostPort");

template<class T>
T parseString(const std::string& s) {
  std::stringstream strm(s);
  T val;
  strm >> val;
  return val;
}

HostPort::HostPort(const std::string& hpString, bool useIPv6) {
  std::vector<std::string> splitted;
  boost::split(splitted, hpString, boost::algorithm::is_any_of("/"));

  if (splitted.size() < 2) {
    LOG_ERROR("invalid ip/port! " << hpString);
    throw std::string("invalid ip/port!");
  }

  if (splitted[0] == "*") {
    if (useIPv6)
      host = QHostAddress::AnyIPv6;
    else
      host = QHostAddress::Any;
  } else {
    host = QHostAddress(QString(splitted[0].c_str()));
  }
  port = parseString<unsigned short>(splitted[1]);
}
