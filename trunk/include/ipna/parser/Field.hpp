#ifndef FIELD_HPP
#define FIELD_HPP

#include <boost/shared_ptr.hpp>
#include <ipna/network/Packet.hpp>

#include <arpa/inet.h>
#include <string>

namespace ipna {
  namespace parser {
    class Field {
    public:
      typedef unsigned int FieldId;
      typedef boost::shared_ptr<Field> FieldPtr;
      
      Field(FieldId id, const char* bytes, size_t numBytes);
      virtual ~Field() {}
      
      virtual std::string toString() const;
      inline size_t getId() const { return _id; }

      unsigned int asUInt() const;
      std::string asIp() const;
    private:
      char _bytes[16];
      size_t _numBytes;
      FieldId _id;
    };
  }
}

#endif // FIELD_HPP
