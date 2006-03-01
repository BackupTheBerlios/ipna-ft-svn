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
