#include <ipna/capture/Formatter.hpp>

using namespace ipna;
using namespace ipna::capture;

Formatter::Formatter() {
  _columns.push_back(std::make_pair<unsigned int, EntryType>(10, NUMBER));
  _columns.push_back(std::make_pair<unsigned int, EntryType>(8,  IP));
  _columns.push_back(std::make_pair<unsigned int, EntryType>(9,  NUMBER));
  _columns.push_back(std::make_pair<unsigned int, EntryType>(14, NUMBER));
  _columns.push_back(std::make_pair<unsigned int, EntryType>(12, IP));
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
    << record->engineId() << '\t';
  
  for (std::vector<std::pair<unsigned int, EntryType> >::iterator it = _columns.begin();
       it != _columns.end();
       it++) {
    unsigned int fieldId = it->first;
    EntryType entryType = it->second;

    if (record->has(fieldId)) {
      switch (entryType) {
      case NUMBER:
	os << record->get(fieldId)->asUInt();
	break;
      case IP:
	os << record->get(fieldId)->asIp();
	break;
      default:
	assert(false);
      }
      os << '\t';
    } else {

      os << (unsigned int)0;
    }
  }

  os << std::endl;
  return os;
  
  /*  
  os
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
  */
}
