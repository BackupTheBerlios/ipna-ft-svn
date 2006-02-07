#include <ipna/capture/RecordWriter.hpp>

using namespace ipna;
using namespace ipna::capture;
using namespace ipna::parser;

RecordWriter::RecordWriter(/*formatter missing*/ std::ostream& stream)
  : _stream(stream) {
}

void
RecordWriter::write(PacketParser::RecordVectorPtr records) {
  for (PacketParser::RecordVector::iterator it = records->begin(); it != records->end(); it++) {
    getStream()
      << (*it)->tstamp() << '\t'
      << (*it)->get(10)->asUInt() << '\t'
      << (*it)->get(8)->asIp()    << '\t'
      << (*it)->get(9)->asUInt() << '\t'
      << (*it)->get(14)->asUInt() << '\t'
      << (*it)->get(12)->asIp()   << '\t'
      << (*it)->get(13)->asUInt() << '\t'
      << (*it)->get(4)->asUInt()  << '\t'
      << (*it)->get(7)->asUInt()  << '\t'
      << (*it)->get(11)->asUInt() << '\t'
      << (*it)->get(2)->asUInt()  << '\t'
      << (*it)->get(1)->asUInt()
      << std::endl;
  }
}
