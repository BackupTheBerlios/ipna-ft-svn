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
#include <ipna/capture/CaptureProgram.hpp>

#include <QUdpSocket>
#include <iostream>

#include <ipna/capture/FileRecordWriter.hpp>
#include <ipna/network/HostPort.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>

using namespace ipna;
using namespace ipna::capture;

CaptureProgram::CaptureProgram(const std::string& name)
  : IPNAProgram(name) {
  try {
    getOptions().add_options()
      ("only-from,o", po::value<int>(), "capture packets only from this source engine")
      ("listen,l", po::value<std::string>(), "listen on ip/port")
      ("workdir,w", po::value<std::string>()->default_value("/tmp/capture"), "The directory into which the record-files shall be written")
      ("nesting,N", po::value<int>()->default_value(0),
       "The nesting level of the directory hierarchy.\n\nSupported levels are:\n"
       " -3\t YYYY/YYYY-MM/YYYY-MM-DD/flow-file\n"
       " -2\t YYYY-MM/YYYY-MM-DD/flow-file\n"
       " -1\t YYYY-MM-DD/flow-file\n"
       "  0\t flow-file\n"
       "  1\t YYYY/flow-file\n"
       "  2\t YYYY/YYYY-MM/flow-file\n"
       "  3\t YYYY/YYYY-MM/YYYY-MM-DD/flow-file"
       )
      ("rotations,n", po::value<unsigned int>()->default_value(0),
       "The number of rotations per day.\n\nHere are some examples:\n"
       "  0: \twrite to stdout\n"
       " 23: \tdo a hourly file change\n"
       " 95: \tdo a file change every 15 minutes"
       )
      ("format,f", po::value<std::string>()->default_value("-1,-2,10,27|8,29|9,14,28|12,30|13,4,7,11,2,1"),
       "The export format to be used as a comma separated list of a set of field ids (pipe | separated):\n"
       "  some examples:\n"
       "     -2: \tengine id\n"
       "     -1: \ttime stamp\n"
       "      1: \toctets in flow\n"
       "      2: \tinput packets\n"
       "      4: \tprotocol\n"
       "      7: \tsource port\n"
       "      8: \tipv4 source\n"
       "      9: \tsource netmask\n"
       "     10: \tinput snmp\n"
       "     11: \tdestination port\n"
       "     12: \tipv4 destination\n"
       "     13: \tdestination netmask\n"
       "     14: \toutput snmp\n"
       "     27: \tipv6 src address\n"
       "     28: \tipv6 dst address\n"
       "     29: \tipv6 src mask\n"
       "     30: \tipv6 dst mask\n"
       "example --format -1,-2,10,8|27,9|29,14,12|28,13|30\n"
       )
      ("queue-size,Q", po::value<unsigned int>()->default_value(1024), "The number of records hold in memory before writing them")
      ;
  } catch (po::error & ex) {
    logger->error() << ex.what() << std::endl;
    exit(1);
  }
}

void
CaptureProgram::initialize(int argc, char **argv) {
  IPNAProgram::initialize(argc,argv);

  if (!isSet(("listen"))) {
    logger->error() << "no ip/port pair specified to listen on!" << std::endl;
    exit(1);
  }

  network::HostPort listenOn(getArgumentMap()["listen"].as<std::string>(), isSet("ipv6"));
  logger->debug() << "listening on: [" << listenOn.host.toString().toStdString() << "]:" << listenOn.port << std::endl;

  typedef boost::shared_ptr<QUdpSocket> SocketPtr;
  SocketPtr listenSocket;
  listenSocket = SocketPtr(new QUdpSocket());
  if(! listenSocket->bind(listenOn.host, listenOn.port)) {
    logger->error() << "could not bind to listen-socket: " << listenSocket->errorString().toStdString() << std::endl;
    exit(3);
  }

  _listener  = boost::shared_ptr<network::Listener>(new network::Listener(listenSocket));
  _formatter = boost::shared_ptr<capture::Formatter>(new capture::Formatter());

  // refactor this to a Format class or so
  std::vector<std::string> columns;
  boost::split(columns, getArgumentMap()["format"].as<std::string>(), boost::algorithm::is_any_of(","));
  for (std::vector<std::string>::const_iterator it = columns.begin(); it != columns.end(); it++) {
    std::vector<std::string> fields;
    boost::split(fields, *it, boost::algorithm::is_any_of("|"));

    if (fields.empty()) {
      logger->error() << "illegal column definiton, at least one field required" << *it;
      exit(1);
    }
    size_t col = _formatter->addColumn();
    
    for (std::vector<std::string>::const_iterator f = fields.begin(); f != fields.end(); f++) {
      int fieldId;
      std::stringstream sstr(*f);
      sstr >> fieldId;
      if (sstr.bad()) {
	logger->error() << "illegal format given: not a field id: " << *f << std::endl;
	exit(1);
      } else {
	logger->debug() << "added field " << fieldId << " to column " << col << std::endl;
      }

      _formatter->addFieldToColumn(col, fieldId);
    }
  }
  
  unsigned int rotations = getArgumentMap()["rotations"].as<unsigned int>();
  if (rotations > 0) {
    std::string workDir = getArgumentMap()["workdir"].as<std::string>();
    int nesting = getArgumentMap()["nesting"].as<int>();
    if (nesting < -3 || nesting > 3) {
      logger->error() << "unknown nesting level: " << nesting << "!" << std::endl;
      exit(1);
    }
    _writer  = boost::shared_ptr<capture::FileRecordWriter>(new capture::FileRecordWriter(_formatter,workDir,nesting,rotations));
  } else {
    _writer  = boost::shared_ptr<capture::RecordWriter>(new capture::RecordWriter(_formatter));
    _writer->setStream(std::cout);
  }
  _handler   = boost::shared_ptr<capture::CapturePacketHandler>
    (new capture::CapturePacketHandler(_writer,getArgumentMap()["queue-size"].as<unsigned int>()));
  _listener->addHandler(_handler);
}

void
CaptureProgram::start() {
  _listener->start();
}
