#ifndef FIELD_HPP
#define FIELD_HPP

#include <boost/shared_ptr.hpp>
#include <string>

namespace ipna {
  namespace parser {
    class Field {
    public:
      typedef unsigned int FieldId;
      typedef boost::shared_ptr<Field> FieldPtr;
      
      Field(FieldId id, const char* bytes, size_t numBytes)
	: _id(id), _bytes(bytes), _numBytes(numBytes) {}
      virtual ~Field() {}
      
      virtual std::string toString() const { return "not implemented,yet"; }
      inline size_t getId() const { return _id; }
    private:
      const char* _bytes;
      size_t _numBytes;
      FieldId _id;
    };
  }
}
#endif // FIELD_HPP
