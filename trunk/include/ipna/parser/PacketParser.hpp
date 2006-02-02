#ifndef PACKET_PARSER_HPP
#define PACKET_PARSER_HPP

#include <boost/shared_ptr.hpp>
#include <vector>

#include <ipna/network/PacketHandler.hpp>
#include <ipna/network/Packet.hpp>
#include <ipna/parser/Record.hpp>
#include <ipna/parser/TemplateManager.hpp>

namespace ipna {
  namespace parser {
    class PacketParser {
    public:
      typedef boost::shared_ptr<PacketParser> ParserPtr;
      typedef std::vector<Record::RecordPtr> RecordVector;
      typedef boost::shared_ptr<std::vector<Record::RecordPtr> > RecordVectorPtr;
      
      PacketParser() : _templateManager(new TemplateManager) {}
      virtual ~PacketParser() { }
      
      virtual size_t parse(network::Packet::PacketPtr packet, RecordVectorPtr records) = 0;
    protected:
      inline void setTemplateManager(boost::shared_ptr<TemplateManager> manager) { _templateManager = manager; }
      inline boost::shared_ptr<TemplateManager> getTemplateManager() { return _templateManager; }
    private:
      boost::shared_ptr<TemplateManager> _templateManager;
    };
  }
}
#endif // PACKET_PARSER_HPP
