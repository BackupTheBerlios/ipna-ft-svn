#include <ipna/capture/RecordWriter.hpp>

using namespace ipna;
using namespace ipna::capture;
using namespace ipna::parser;

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
  getFormatter()->format(r, getStream());
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
