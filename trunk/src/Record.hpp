#ifndef RECORD_HPP
#define RECORD_HPP

#include <map>

#include "Field.hpp"

namespace ipna {
  class Record {
  public:
    Record(unsigned int templateId);
    virtual ~Record() {}

    Record* add(Field::FieldPtr field);
    inline unsigned int getTemplateId() {
      return _templateId;
    }

    bool has(Field::FieldId id);
    FieldPtr get(Field::FieldId id);
    
  private:
    std::map<Field::FieldId, Field::FieldPtr> _values;
    unsigned int _templateId;    
  };
}

#endif // RECORD_HPP
