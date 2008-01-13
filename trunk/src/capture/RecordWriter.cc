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
#include <ipna/capture/RecordWriter.hpp>

using namespace ipna;
using namespace ipna::capture;
using namespace ipna::parser;

Logger::LoggerPtr RecordWriter::logger = Logger::getLogger("ipna.capture.RecordWriter");

RecordWriter::RecordWriter(Formatter::FormatterPtr formatter)
  : _formatter(formatter) {
  setStream(std::cout);
}

void
RecordWriter::write(PacketParser::RecordVectorPtr records) {
  getFormatter()->startRecordSet();
  for (PacketParser::RecordVector::iterator it = records->begin(); it != records->end(); it++) {
    write(*it);
  }
  getFormatter()->endRecordSet();
}

void
RecordWriter::write(Record::RecordPtr r) {
  try {
    getFormatter()->format(r, getStream());
  } catch (std::exception& e) {
    LOG_ERROR("could not format record: " << e.what());
  }
}

RecordWriter::RecordWriterPtr
operator<<(RecordWriter::RecordWriterPtr w, Record::RecordPtr r) {
  w->write(r);
  return w;
}

RecordWriter::RecordWriterPtr
operator<<(RecordWriter::RecordWriterPtr w, PacketParser::RecordVectorPtr records) {
  w->write(records);
  return w;
}
