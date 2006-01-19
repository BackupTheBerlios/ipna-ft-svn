#ifndef FIELD_HPP
#define FIELD_HPP

#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <string>
#include <vector>

namespace ipna {
  class Field {
  public:
    typedef unsigned int FieldId;
    typedef boost::shared_ptr<Field> FieldPtr;

    Field(FieldId id, char* bytes, unsigned int numBytes);
    Field(FieldId id, std::vector<char> bytes);
    virtual ~Field() {}

    virtual std::string str();

  private:
    boost::any value;
  };
}

#endif // FIELD_HPP
