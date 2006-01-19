#ifndef TEMPLATE_HPP
#define TEMPLATE_HPP

#include <vector>
#include <utility>
#include <time.h>
#include <ipna/parser/Field.hpp>

namespace ipna {
  namespace parser {
    class Template {
    public:
      typedef unsigned int TemplateId;
      typedef std::pair<Field::FieldId, unsigned int> FieldLengthTuple;
      typedef std::vector<FieldLengthTuple> FieldDescription;
      typedef FieldDescription::iterator FieldIterator;
      
      Template(TemplateId id, unsigned int refreshTime = 60*15) : _totalLength(0), _id(id), _lastUpdated(0), _refreshTime(refreshTime) {}
      virtual ~Template() {}
      
      inline FieldDescription& getFieldDescriptions() { return _fields; }
      inline unsigned int getTotalLength() { return _totalLength; }
      inline void addField(Field::FieldId id, unsigned int length) {
	_fields.push_back(std::make_pair<Field::FieldId,unsigned int>(id,length));
	_totalLength += length;
      }
      inline void clear() { _fields.clear(); }
      inline time_t lastUpdateTime() { return _lastUpdated; }
      inline void updated() { _lastUpdated = time(NULL); }
      inline bool needsUpdate() { return (_lastUpdated+_refreshTime) < time(NULL); }
    private:
      std::vector<FieldLengthTuple> _fields;
      unsigned int _totalLength;
      TemplateId _id;
      time_t _lastUpdated;
      unsigned int _refreshTime;
    };
  }
}

#endif // TEMPLATE_HPP
