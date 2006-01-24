#ifndef TEMPLATE_HPP
#define TEMPLATE_HPP

#include <vector>
#include <utility>
#include <time.h>

#include <boost/shared_ptr.hpp>

#include <ipna/parser/Field.hpp>

namespace ipna {
  namespace parser {
    class Template {
    public:
      typedef unsigned int TemplateId;
      typedef std::pair<Field::FieldId, unsigned int> FieldLengthTuple;
      typedef std::vector<FieldLengthTuple> FieldDescriptions;
      typedef FieldDescriptions::iterator FieldIterator;
      typedef boost::shared_ptr<Template> TemplatePtr;
      
      Template(TemplateId id, unsigned int refreshTime = 60*15)
	: _totalLength(0), _id(id), _lastUpdated(0), _refreshTime(refreshTime) {
      }
      virtual ~Template() {}

      inline TemplateId getId() const { return _id; }
      inline FieldDescriptions& getFieldDescriptions() { return _fields; }
      inline unsigned int getTotalLength() { return _totalLength; }
      inline unsigned int getLength(Field::FieldId field) {
	for (FieldIterator it = _fields.begin(); it != _fields.end(); it++) {
	  if (it->first == field)
	    return it->second;
	}
	return 0;
      }
      inline void addField(Field::FieldId id, unsigned int length) {
	_fields.push_back(std::make_pair<Field::FieldId,unsigned int>(id,length));
	_totalLength += length;
      }
      inline void clear() { _fields.clear(); _totalLength = 0; }
      inline time_t lastUpdateTime() { return _lastUpdated; }
      inline void update() { _lastUpdated = time(NULL); }
      inline bool needsUpdate() { return (_lastUpdated+_refreshTime) < time(NULL); }
    private:
      FieldDescriptions _fields;
      unsigned int _totalLength;
      TemplateId _id;
      time_t _lastUpdated;
      unsigned int _refreshTime;
    };
  }
}

#endif // TEMPLATE_HPP
