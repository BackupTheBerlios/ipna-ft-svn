#ifndef PACKET_PARSER_HPP
#define PACKET_PARSER_HPP

#include <boost/shared_ptr.hpp>
#include <vector>
#include <map>

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
      
      PacketParser() {}
      virtual ~PacketParser() { }

      virtual bool analyze(network::Packet::PacketPtr packet) = 0;
      virtual size_t getSequenceNumber() = 0;
      virtual size_t parse(network::Packet::PacketPtr packet, RecordVectorPtr records) = 0;
    protected:
      inline void setTemplateManager(unsigned int engine, boost::shared_ptr<TemplateManager> manager) { _templateManager[engine] = manager; }
      inline boost::shared_ptr<TemplateManager> getTemplateManager(unsigned int engine) {
	if (_templateManager.find(engine) == _templateManager.end()) {
	  setTemplateManager(engine, boost::shared_ptr<TemplateManager>(new TemplateManager));
	}
	return _templateManager[engine];
      }
    private:
      std::map<unsigned int, boost::shared_ptr<TemplateManager> > _templateManager;
    };
  }
}
#endif // PACKET_PARSER_HPP
