#include <ipna/parser/CNFPv9PacketParser.hpp>
#include <ipna/parser/cnfp.hpp>
#include <stdio.h>
#include <vector>

using namespace ipna;
using namespace ipna::network;
using namespace ipna::parser;

ipna::Logger::LoggerPtr CNFPv9PacketParser::logger = ipna::Logger::getLogger("ipna.parser.CNFPv9PacketParser");

CNFPv9PacketParser::CNFPv9PacketParser() : PacketParser() {}

PacketParser::RecordVectorPtr
CNFPv9PacketParser::parse(Packet::PacketPtr packet) {
  PacketParser::RecordVectorPtr records(new PacketParser::RecordVector);

  cnfp_v9_hdr_s header = *(cnfp_v9_hdr_t)packet->getCurrentBytes();
  packet->advanceBytes(sizeof(cnfp_v9_hdr_s));

  if (logger->isDebugEnabled()) {
    fprintf(stdout, "version:%d count:%u uptime:%u tstamp:%u seq:%u source:%d\n",
	    ntohs(header.common.version),
	    ntohs(header.common.count),
	    ntohl(header.uptime),
	    ntohl(header.tstamp),
	    ntohl(header.seq),
	    ntohl(header.engine_id)
	    );
  }

  while (packet->dataLeft()) {
    // get the flow-set id
    const unsigned int fsid = packet->getNextShort();
    const unsigned int length = packet->getNextShort();
    LOG_DEBUG("fset id:" << fsid << " length:"<< length);

    // start a new reading frame within the packet of length 'length'
    packet->startFrame(length, -2*(sizeof(unsigned short)));
    
    if (0 <= fsid && fsid < 256) {
      LOG_DEBUG("got a template fs id:"<< fsid);
      static const size_t entryLength = sizeof(unsigned short);

      while (packet->numRemainingBytesInFrame() > (2*entryLength)) {
	// got a template flow set
      
	unsigned short templateId = packet->getNextShort();
	unsigned short fieldCount = packet->getNextShort();
	LOG_DEBUG("tid:" << templateId << " fcount:" << fieldCount);

	Template::TemplatePtr templ(new Template(templateId));
	for (unsigned int field = 0; field < fieldCount; field++) {
	  unsigned short fieldType = packet->getNextShort();
	  unsigned short fieldLength = packet->getNextShort();
	  templ->addField(fieldType,fieldLength);
	}
	templ->update();
	getTemplateManager()->put(templ);
      }
      LOG_DEBUG("pad:" << packet->numRemainingBytesInFrame());
    } else {
      // got a data flow set
      if (getTemplateManager()->isKnown(fsid)) {
	LOG_DEBUG("got a known data flow set:" << fsid);
      } else {
	LOG_DEBUG("got an unknown data flow set:" << fsid);
      }
      packet->skipFrame();
    }
  }

  LOG_DEBUG(packet->getCurrentPosition() << "/" << packet->getLength() << " bytes read.");
  
  return records;
}
  
