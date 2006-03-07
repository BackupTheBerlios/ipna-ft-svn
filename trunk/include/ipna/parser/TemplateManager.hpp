/*
  Copyright (C) 2006 Alexander Petry

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
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
      
      void put(Template::TemplatePtr t) {
	_templates[t->getId()] = t;
      }
      
      Template::TemplatePtr get(Template::TemplateId id) {
	return _templates[id];
      }

      bool isKnown(Template::TemplateId id) {
	return _templates.find(id) != _templates.end();
      }
    private:
      std::map<Template::TemplateId, Template::TemplatePtr> _templates;
    };
  }
}

#endif // TEMPLATEMANAGER_HPP
