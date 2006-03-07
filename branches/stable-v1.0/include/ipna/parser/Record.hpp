#ifndef RECORD_HPP
#define RECORD_HPP

#include <map>
#include <time.h>

#include <boost/shared_ptr.hpp>
#include <ipna/parser/Field.hpp>

namespace ipna {
  namespace parser {
    class Record {
    public:
      typedef boost::shared_ptr<Record> RecordPtr;
      
      Record(unsigned int templateId, time_t tstamp, unsigned int engine = 0) : _templateId(templateId), _tstamp(tstamp), _engineId(engine) {}
      virtual ~Record() {
	_values.clear();
      }
      
      inline Record* add(Field::FieldPtr field) {
	_values[field->getId()] = field;
	return this;
      }
      inline unsigned int getTemplateId() {
	return _templateId;
      }
      
      inline bool has(Field::FieldId id) const {
	return _values.find(id) != _values.end();
      }
      inline Field::FieldPtr get(Field::FieldId id) {
	return _values[id];
      }
      inline time_t tstamp() const {
	return _tstamp;
      }
      inline unsigned int engineId() const {
	return _engineId;
      }

      static inline bool compare_lt(RecordPtr r1, RecordPtr r2) {
	return r1->tstamp() < r2->tstamp();
      }
    private:
      std::map<Field::FieldId, Field::FieldPtr> _values;
      unsigned int _templateId;
      time_t _tstamp;
      unsigned int _engineId;
    };
  }
}

#endif // RECORD_HPP
