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
    LOG_DEBUG("fset id:" << fsid << " length:"<< length);

    // start a new reading frame within the packet of length 'length'
    // but start it at the point where we have read the id and length fields
    packet->moveCursor(-2*(sizeof(unsigned short)));
    packet->startFrame(length);
    packet->moveCursor( 2*(sizeof(unsigned short)));
    
    if (0 == fsid) {
      recordsRemaining--;
      LOG_DEBUG("got a template fs id:"<< fsid << " remaining:" << recordsRemaining);
      const size_t entryLength = sizeof(unsigned short);

      // read all template definitions as long as there are enough bytes left
      // to read the template id and the fieldcount, the rest is then padding
      while (packet->dataLeftInFrame() > (2*entryLength)) {
	unsigned short templateId = packet->getNextShort();
	unsigned short fieldCount = packet->getNextShort();
	LOG_DEBUG("tid:" << templateId << " fcount:" << fieldCount);

	Template::TemplatePtr templ(new Template(templateId));

	packet->startFrame(fieldCount * 2*sizeof(unsigned short));
	while (packet->dataLeftInFrame()) {
	  unsigned short fieldType = packet->getNextShort();
	  unsigned short fieldLength = packet->getNextShort();
	  templ->addField(fieldType,fieldLength);
	  LOG_DEBUG("field:"<<fieldType<<" length:"<<fieldLength);
	}
	packet->endFrame();

	templ->update();
	getTemplateManager(engine)->put(templ);
      }
      LOG_DEBUG("pad:" << packet->dataLeftInFrame());
    } else if (1 == fsid) {
      LOG_DEBUG("got an option template, skipping");
      // got an options template
      packet->skipFrame();
    } else {
      // got a data flow set
      if (getTemplateManager(engine)->isKnown(fsid) && getTemplateManager(engine)->get(fsid)->getTotalLength() > 0) {
	LOG_DEBUG("got a known data flow set:" << fsid);
	const size_t entryLength = sizeof(unsigned short);

	Template::TemplatePtr templ = getTemplateManager(engine)->get(fsid);

	// read all records as long as there are enough bytes left
	// to read them.
	while (packet->dataLeftInFrame() >= templ->getTotalLength()) {
	  recordsRemaining--;
	  //	  LOG_DEBUG("a new record of size:" << templ->getTotalLength() << " begins, remaining records:" << recordsRemaining);

	  packet->startFrame(templ->getTotalLength());
	  Record::RecordPtr r(new Record(fsid, ntohl(header.tstamp), engine));
	  for (unsigned int fieldIdx = 0; fieldIdx < templ->getNumFields(); ++fieldIdx) {
	    unsigned int fId = templ->getFieldId(fieldIdx);
	    unsigned int fLen= templ->getFieldLength(fieldIdx);
	    Field::FieldPtr field;
	    
	    // TODO: make this configurable:
	    switch (fId) {
	    case 8:
	    case 12:
	    case 15:
	    case 18:
	    case 27:
	    case 28:
	    case 62:
	    case 63:
	      field = Field::FieldPtr(new IPField(fId,packet->getCurrentBytes(),fLen));
	      break;
	    case 56:
	    case 57:
	    case 80:
	    case 81:
	      field = Field::FieldPtr(new MACField(fId,packet->getCurrentBytes(),fLen));
	      break;
	    case 82:
	    case 83:
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
	LOG_DEBUG("got an unknown data flow set:" << fsid);
      }
      packet->skipFrame();
    }

    packet->endFrame();
  }

  LOG_DEBUG(packet->getCurrentPosition() << "/" << packet->getLength() << " bytes read");
  
  return records->size() - recordsStartSize;
}
