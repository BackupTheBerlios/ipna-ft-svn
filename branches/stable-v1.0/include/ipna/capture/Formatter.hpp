#ifndef FORMATTER_HPP
#define FORMATTER_HPP


#include <ostream>
#include <vector>
#include <utility>
#include <boost/shared_ptr.hpp>
#include <ipna/parser/Record.hpp>

namespace ipna {
  namespace capture {
    class Formatter {
    public:
      typedef boost::shared_ptr<Formatter> FormatterPtr;

      enum EntryType { NUMBER, IP, MAC };
      
      Formatter();
      virtual ~Formatter() {}

      // these functions may be used to group records in some way
      virtual void startRecordSet() {}
      virtual void endRecordSet() {}
      
      std::ostream& format(ipna::parser::Record::RecordPtr record, std::ostream& os);
    private:
      std::vector<std::pair<unsigned int, EntryType> > _columns;
    };
  } // capture
} // ipna

#endif // FORMATTER_HPP
