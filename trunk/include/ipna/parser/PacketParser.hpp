#ifndef PACKET_PARSER_HPP
#define PACKET_PARSER_HPP

#include <boost/shared_ptr.hpp>
#include <vector>

#include <ipna/network/PacketHandler.hpp>

namespace ipna {
  namespace parser {
    class TemplateManager;
    class Record;
    class Packet;
    
    class PacketParser {
    public:
      typedef boost::shared_ptr<PacketParser> ParserPtr;
      
      PacketParser() {}
      virtual ~PacketParser() { }
      
      virtual boost::shared_ptr<std::vector<boost::shared_ptr<Record> > > parse(Packet packet, int len) const = 0;
    protected:
      inline void setTemplateManager(boost::shared_ptr<TemplateManager> manager) { _templateManager = manager; }
      inline boost::shared_ptr<TemplateManager> getTemplateManager() { return _templateManager; }
    private:
      boost::shared_ptr<TemplateManager> _templateManager;
    };
  }
}
#endif // PACKET_PARSER_HPP
