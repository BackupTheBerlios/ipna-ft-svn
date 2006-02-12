#include <ipna/parser/CNFPv9PacketParser.hpp>
#include <stdio.h>
#include <vector>

using namespace ipna;
using namespace ipna::network;
using namespace ipna::parser;

ipna::Logger::LoggerPtr CNFPv9PacketParser::logger = ipna::Logger::getLogger("ipna.parser.CNFPv9PacketParser");

CNFPv9PacketParser::CNFPv9PacketParser() : PacketParser() {}

bool
CNFPv9PacketParser::analyze(Packet::PacketPtr packet) {
  _header = *(cnfp_v9_hdr_t)packet->getCurrentBytes();

  if (logger->isDebugEnabled()) {
    fprintf(stderr, "version:%d count:%u uptime:%u tstamp:%u seq:%u source:%d\n",
	    ntohs(_header.common.version),
	    ntohs(_header.common.count),
	    ntohl(_header.uptime),
	    ntohl(_header.tstamp),
	    ntohl(_header.seq),
	    ntohl(_header.engine_id)
	    );
    fflush(stderr);
  }

  return ntohs(_header.common.version) == 9;
}

size_t
CNFPv9PacketParser::getSequenceNumber() {
  return ntohl(_header.seq);
}

size_t
CNFPv9PacketParser::parse(Packet::PacketPtr packet, PacketParser::RecordVectorPtr records) {
  cnfp_v9_hdr_s header = *(cnfp_v9_hdr_t)packet->getCurrentBytes();
  packet->advanceBytes(sizeof(cnfp_v9_hdr_s));

  const size_t recordsStartSize = records->size();
  size_t recordsRemaining = ntohs(header.common.count);
  
  while (packet->dataLeft()) {
    // get the flow-set id
    const unsigned int fsid = packet->getNextShort();
    const unsigned int length = packet->getNextShort();
    LOG_DEBUG("fset id:" << fsid << " length:"<< length);

    // start a new reading frame within the packet of length 'length'
    // but start it at the point where we have read the id and length fields
    packet->startFrame(length, -2*(sizeof(unsigned short)));
    
    if (0 == fsid) {
      recordsRemaining--;
      LOG_DEBUG("got a template fs id:"<< fsid << " remaining:" << recordsRemaining);
      const size_t entryLength = sizeof(unsigned short);

      // read all template definitions as long as there are enough bytes left
      // to read the template id and the fieldcount, the rest is then padding
      while (packet->numRemainingBytesInFrame() > (2*entryLength)) {
	unsigned short templateId = packet->getNextShort();
	unsigned short fieldCount = packet->getNextShort();
	LOG_DEBUG("tid:" << templateId << " fcount:" << fieldCount);

	Template::TemplatePtr templ(new Template(templateId));
	for (unsigned int field = 0; field < fieldCount; field++) {
	  unsigned short fieldType = packet->getNextShort();
	  unsigned short fieldLength = packet->getNextShort();
	  templ->addField(fieldType,fieldLength);
	  LOG_DEBUG("field:"<<fieldType<<" length:"<<fieldLength);
	}
	templ->update();
	getTemplateManager()->put(templ);
      }
      LOG_DEBUG("pad:" << packet->numRemainingBytesInFrame());
    } else if (1 == fsid) {
      LOG_DEBUG("got an option template");
      // got an options template
      packet->skipFrame();
    } else {
      // got a data flow set
      if (getTemplateManager()->isKnown(fsid) && getTemplateManager()->get(fsid)->getTotalLength() > 0) {
	LOG_DEBUG("got a known data flow set:" << fsid);
	const size_t entryLength = sizeof(unsigned short);

	Template::TemplatePtr templ = getTemplateManager()->get(fsid);

	// read all records as long as there are enough bytes left
	// to read them.
	while (packet->numRemainingBytesInFrame() >= templ->getTotalLength()) {
	  recordsRemaining--;
	  //	  LOG_DEBUG("a new record of size:" << templ->getTotalLength() << " begins, remaining records:" << recordsRemaining);

	  Record::RecordPtr r(new Record(fsid, ntohl(header.tstamp)));
	  for (unsigned int fieldIdx = 0; fieldIdx < templ->getNumFields(); ++fieldIdx) {
	    unsigned int fId = templ->getFieldId(fieldIdx);
	    unsigned int fLen= templ->getFieldLength(fieldIdx);

	    Field::FieldPtr field(new Field(fId,packet->getCurrentBytes(),fLen));
	    r->add(field);
	    packet->advanceBytes(fLen);
	  }
	  
	  records->push_back(r);
	}
      } else {
	LOG_DEBUG("got an unknown data flow set:" << fsid);
      }
      packet->skipFrame();
    }
  }

  LOG_DEBUG(packet->getCurrentPosition() << "/" << packet->getLength() << " bytes read");
  
  return records->size() - recordsStartSize;
}
