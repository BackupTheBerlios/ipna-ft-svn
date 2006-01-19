#ifndef PACKET_PARSER_HPP
#define PACKET_PARSER_HPP

#include <boost/shared_ptr.hpp>
#include <vector>

#include "Record.hpp"
#include "TemplateManager.hpp"
#include "PacketHandler.hpp"

namespace ipna {
  class PacketParser {
  public:
    PacketParser(boost::shared_ptr<TemplateManager> templateManager)
      : _templateManager(templateManager) {}
    virtual ~PacketParser() { }

    virtual boost::shared_ptr<std::vector<Record> > parse(Packet packet, int len) const = 0;
  protected:
    inline boost::shared_ptr<TemplateManager> getTemplateManager() { return _templateManager; }
  private:
    boost::shared_ptr<TemplateManager> _templateManager;
  };
}

#endif // PACKET_PARSER_HPP
