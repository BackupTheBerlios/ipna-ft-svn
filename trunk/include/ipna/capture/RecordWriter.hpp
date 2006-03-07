/*
  RecordWriter.hpp - writes records to a stdout.

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
#ifndef RECORD_WRITER_HPP
#define RECORD_WRITER_HPP

#include <iostream>

#include <ipna/parser/Record.hpp>
#include <ipna/parser/PacketParser.hpp>
#include <ipna/capture/Formatter.hpp>
#include <boost/shared_ptr.hpp>

namespace ipna {
  namespace capture {
    class RecordWriter {
    public:
      typedef boost::shared_ptr<RecordWriter> RecordWriterPtr;
      
      RecordWriter(Formatter::FormatterPtr formatter);
      virtual ~RecordWriter() {}

      virtual void write(ipna::parser::Record::RecordPtr record);
      virtual void write(ipna::parser::PacketParser::RecordVectorPtr records);

      void setStream(std::ostream& os) {
	_stream = &os;
      }
    protected:
      inline virtual std::ostream& getStream() {
	return *_stream;
      }
      inline Formatter::FormatterPtr getFormatter() {
	return _formatter;
      }
    private:
      std::ostream *_stream;
      Formatter::FormatterPtr _formatter;
    }; // class RecordWriter

    RecordWriter::RecordWriterPtr operator<<(RecordWriter::RecordWriterPtr w, ipna::parser::Record::RecordPtr record);
    RecordWriter::RecordWriterPtr operator<<(RecordWriter::RecordWriterPtr w, ipna::parser::PacketParser::RecordVectorPtr records);
  } // ns capture
} // ns ipna

#endif
