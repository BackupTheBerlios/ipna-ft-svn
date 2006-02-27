#include <ipna/capture/Formatter.hpp>

using namespace ipna;
using namespace ipna::capture;

Formatter::Formatter() {
  _columns.push_back(std::make_pair<unsigned int, EntryType>(10, NUMBER));
  _columns.push_back(std::make_pair<unsigned int, EntryType>(8,  NUMBER));
  _columns.push_back(std::make_pair<unsigned int, EntryType>(9,  NUMBER));
  _columns.push_back(std::make_pair<unsigned int, EntryType>(14, NUMBER));
  _columns.push_back(std::make_pair<unsigned int, EntryType>(12, NUMBER));
  _columns.push_back(std::make_pair<unsigned int, EntryType>(13, NUMBER));
  _columns.push_back(std::make_pair<unsigned int, EntryType>(4,  NUMBER));
  _columns.push_back(std::make_pair<unsigned int, EntryType>(7,  NUMBER));
  _columns.push_back(std::make_pair<unsigned int, EntryType>(11, NUMBER));
  _columns.push_back(std::make_pair<unsigned int, EntryType>(2,  NUMBER));
  _columns.push_back(std::make_pair<unsigned int, EntryType>(1,  NUMBER));
}

std::ostream&
Formatter::format(ipna::parser::Record::RecordPtr record, std::ostream& os) {
  os
    << record->tstamp() << '\t'
    << record->engineId() << '\t'
    << record->get(10)->asUInt() << '\t'
    << record->get(8)->asIp()    << '\t'
    << record->get(9)->asUInt() << '\t'
    << record->get(14)->asUInt() << '\t'
    << record->get(12)->asIp()   << '\t'
    << record->get(13)->asUInt() << '\t'
    << record->get(4)->asUInt()  << '\t'
    << record->get(7)->asUInt()  << '\t'
    << record->get(11)->asUInt() << '\t'
    << record->get(2)->asUInt()  << '\t'
    << record->get(1)->asUInt()
    << std::endl;
}
