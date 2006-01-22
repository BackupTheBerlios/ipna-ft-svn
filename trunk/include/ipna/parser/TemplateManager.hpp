#ifndef TEMPLATEMANAGER_HPP
#define TEMPLATEMANAGER_HPP

#include <map>
#include <boost/shared_ptr.hpp>
#include <ipna/parser/Template.hpp>

namespace ipna {
  namespace parser {
    using boost::shared_ptr;
    
    class TemplateManager {
    public:
      TemplateManager() {}
      virtual ~TemplateManager() {}
      
      void put(Template::TemplateId id, shared_ptr<Template> t) {
	_templates[id] = t;
      }
      
      shared_ptr<Template> get(Template::TemplateId id) {
	return _templates[id];
      }
    private:
      std::map<Template::TemplateId, shared_ptr<Template> > _templates;
    };
  }
}

#endif // TEMPLATEMANAGER_HPP
