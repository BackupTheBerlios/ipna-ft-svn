#ifndef RECORD_WRITER_HPP
#define RECORD_WRITER_HPP

#include <iostream>

#include <ipna/parser/PacketParser.hpp>
#include <boost/shared_ptr.hpp>

namespace ipna {
  namespace capture {
    class RecordWriter {
    public:
      typedef boost::shared_ptr<RecordWriter> RecordWriterPtr;
      
      RecordWriter(/* formatter missing */ std::ostream& os = std::cout);
      virtual ~RecordWriter() {}

      virtual void write(ipna::parser::PacketParser::RecordVectorPtr records);
      
    protected:
      virtual std::ostream& getStream() {
	return _stream;
      }

    private:
      std::ostream& _stream;

      
    }; // class RecordWriter
  } // ns capture
} // ns ipna

#endif
