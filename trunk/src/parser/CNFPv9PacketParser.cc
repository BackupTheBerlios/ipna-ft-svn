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
  packet->moveCursor(sizeof(cnfp_v9_hdr_s));

  const size_t recordsStartSize = records->size();
  size_t recordsRemaining = ntohs(header.common.count);
  unsigned int engine = ntohl(header.engine_id);
  
  while (packet->dataLeft()) {
    // get the flow-set id
    const unsigned int fsid = packet->getNextShort();
    const unsigned int length = packet->getNextShort();
    logger->debug() << "fset id:" << fsid << " length:"<< length << std::endl;

    // start a new reading frame within the packet of length 'length'
    // but start it at the point where we have read the id and length fields
    packet->moveCursor((int)(-2*(sizeof(unsigned short))));
    packet->startFrame(length);
    packet->moveCursor( 2*(sizeof(unsigned short)));
    
    if (0 == fsid) {
      recordsRemaining--;
      logger->debug() << "got a template fs id:"<< fsid << " remaining:" << recordsRemaining << std::endl;
      const size_t entryLength = sizeof(unsigned short);

      // read all template definitions as long as there are enough bytes left
      // to read the template id and the fieldcount, the rest is then padding
      while (packet->dataLeftInFrame() > (2*entryLength)) {
	unsigned short templateId = packet->getNextShort();
	unsigned short fieldCount = packet->getNextShort();
	logger->debug() << "tid:" << templateId << " fcount:" << fieldCount << std::endl;

	Template::TemplatePtr templ(new Template(templateId));

	packet->startFrame(fieldCount * 2*sizeof(unsigned short));
	while (packet->dataLeftInFrame()) {
	  unsigned short fieldType = packet->getNextShort();
	  unsigned short fieldLength = packet->getNextShort();
	  templ->addField(fieldType,fieldLength);
	  logger->debug() << "field:"<<fieldType<<" length:"<<fieldLength << std::endl;
	}
	packet->endFrame();

	templ->update();
	getTemplateManager(engine)->put(templ);
      }
      logger->debug() << "pad:" << packet->dataLeftInFrame() << std::endl;
    } else if (1 == fsid) {
      logger->debug() << "got an option template, skipping" << std::endl;
      // got an options template
      packet->skipFrame();
    } else {
      // got a data flow set
      if (getTemplateManager(engine)->isKnown(fsid) && getTemplateManager(engine)->get(fsid)->getTotalLength() > 0) {
	logger->debug() << "got a known data flow set:" << fsid << std::endl;
	const size_t entryLength = sizeof(unsigned short);

	Template::TemplatePtr templ = getTemplateManager(engine)->get(fsid);

	// read all records as long as there are enough bytes left
	// to read them.
	while (packet->dataLeftInFrame() >= templ->getTotalLength()) {
	  recordsRemaining--;

	  packet->startFrame(templ->getTotalLength());
	  Record::RecordPtr r(new Record(fsid, ntohl(header.tstamp), engine));
	  for (unsigned int fieldIdx = 0; fieldIdx < templ->getNumFields(); ++fieldIdx) {
	    unsigned int fId = templ->getFieldId(fieldIdx);
	    unsigned int fLen= templ->getFieldLength(fieldIdx);
	    Field::FieldPtr field;
	    
	    // TODO: make this configurable:
	    switch (fId) {
	    case IPV4_SRC_ADDR:
	    case IPV4_DST_ADDR:
	    case IPV4_NEXT_HOP:
	    case BGP_IPV4_NEXT_HOP:
	    case IPV6_SRC_ADDR:
	    case IPV6_DST_ADDR:
	    case IPV6_NEXT_HOP:
	    case BGP_IPV6_NEXT_HOP:
	      field = Field::FieldPtr(new IPField(fId,packet->getCurrentBytes(),fLen));
	      break;
	    case IN_SRC_MAC:
	    case OUT_DST_MAC:
	    case IN_DST_MAC:
	    case OUT_SRC_MAC:
	      field = Field::FieldPtr(new MACField(fId,packet->getCurrentBytes(),fLen));
	      break;
	    case IF_NAME:
	    case IF_DESC:
	      field = Field::FieldPtr(new StringField(fId,packet->getCurrentBytes(),fLen));
	      break;
	    default:
	      field = Field::FieldPtr(new Field(fId,packet->getCurrentBytes(),fLen));
	    }
	    r->add(field);
	    packet->moveCursor(fLen);
	  }
	  packet->endFrame();
	  
	  records->push_back(r);
	}
      } else {
	logger->debug() << "got an unknown data flow set:" << fsid << std::endl;
      }
      packet->skipFrame();
    }

    packet->endFrame();
  }

  logger->debug() << packet->getCurrentPosition() << "/" << packet->getLength() << " bytes read" << std::endl;
  
  return records->size() - recordsStartSize;
}
