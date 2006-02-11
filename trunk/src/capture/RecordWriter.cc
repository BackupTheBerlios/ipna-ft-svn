#include <ipna/capture/RecordWriter.hpp>

using namespace ipna;
using namespace ipna::capture;
using namespace ipna::parser;

RecordWriter::RecordWriter(Formatter::FormatterPtr formatter, std::ostream& stream)
  : _stream(stream), _formatter(formatter) {
}

void
RecordWriter::write(PacketParser::RecordVectorPtr records) {
  _formatter->startRecordSet();
  for (PacketParser::RecordVector::iterator it = records->begin(); it != records->end(); it++) {
    write(*it);
  }
  _formatter->endRecordSet();
}

void
RecordWriter::write(Record::RecordPtr r) {
  _formatter->format(r, getStream());
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
