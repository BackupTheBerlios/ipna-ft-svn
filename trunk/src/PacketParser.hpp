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
    PacketParser() {}
    virtual ~PacketParser() { }

    boost::shared_ptr<std::vector<Record> > parse(Packet packet, int len);
  private:
    boost::shared_ptr<TemplateManager> _templateManager;
  };
}

#endif // PACKET_PARSER_HPP
