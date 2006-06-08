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
#include <ipna/parser/CNFPv5PacketParser.hpp>
#include <stdio.h>
#include <vector>

using namespace ipna;
using namespace ipna::network;
using namespace ipna::parser;

static unsigned int v5TemplateId = 0x555;

ipna::Logger::LoggerPtr CNFPv5PacketParser::logger = ipna::Logger::getLogger("ipna.parser.CNFPv5PacketParser");

CNFPv5PacketParser::CNFPv5PacketParser()
  : PacketParser() {
  Template::TemplatePtr tmpl(new Template(v5TemplateId));
  logger->setPriority(Logger::DEBUG);
  // add v5 format
  tmpl->addField(IPV4_SRC_ADDR, 4);
  tmpl->addField(IPV4_DST_ADDR, 4);
  tmpl->addField(IPV4_NEXT_HOP, 4);
  tmpl->addField(INPUT_SNMP, 1);
  tmpl->addField(OUTPUT_SNMP, 1);
  tmpl->addField(IN_PKTS, 4);
  tmpl->addField(IN_BYTES, 4);
  tmpl->addField(FIRST_SWITCHED, 4);
  tmpl->addField(LAST_SWITCHED, 4);
  tmpl->addField(L4_SRC_PORT, 2);
  tmpl->addField(L4_DST_PORT, 2);
  tmpl->addField(43, 1); // pad1
  tmpl->addField(TCP_FLAGS, 1);
  tmpl->addField(PROTOCOL, 1);
  tmpl->addField(SRC_TOS, 1);
  tmpl->addField(SRC_AS, 2);
  tmpl->addField(DST_AS, 2);
  tmpl->addField(SRC_MASK, 1);
  tmpl->addField(DST_MASK, 1);
  tmpl->addField(43, 2);
  tmpl->update();
  
  getTemplateManager(0)->put(tmpl);
}

bool
CNFPv5PacketParser::analyze(Packet::PacketPtr packet) {
  header = (cnfp_v5_hdr*)packet->getCurrentBytes();

  if (logger->isDebugEnabled()) {
    fprintf(stderr, "version:%d count:%u uptime:%u tstamp:%u seq:%u source:%d\n",
	    ntohs(header->common.version),
	    ntohs(header->common.count),
	    ntohl(header->uptime),
	    ntohl(header->tstamp),
	    ntohl(header->seq),
	    ntohl(header->engine_id)
	    );
    fflush(stderr);
  }

  return ntohs(header->common.version) == 5;
}

size_t
CNFPv5PacketParser::getSequenceNumber() {
  return ntohl(header->seq);
}

size_t
CNFPv5PacketParser::parse(Packet::PacketPtr packet, PacketParser::RecordVectorPtr records) {
  const size_t recordsStartSize = records->size();
  char* bytes = (char*)packet->getBytes();
  cnfp_v5_hdr* _hdr = (cnfp_v5_hdr*)(bytes); bytes += sizeof(cnfp_v5_hdr);
  uint32_t _count = ntohs(_hdr->common.count);

  for (uint32_t i = 0; i < _count; ++i) {
      cnfp_v5_pkt* _pkt = (cnfp_v5_pkt*)(bytes); bytes += sizeof(cnfp_v5_pkt);
      Record::RecordPtr r(new Record(0, time(NULL), ntohl(_hdr->engine_id)));
      Field::FieldPtr field;
      field = Field::FieldPtr(new IPField((uint32_t)IPV4_SRC_ADDR, (char*)&_pkt->src, 4));
      r->add(field);
      field = Field::FieldPtr(new IPField((uint32_t)IPV4_DST_ADDR, (char*)&_pkt->dst, 4));
      r->add(field);
      field = Field::FieldPtr(new IPField((uint32_t)IPV4_NEXT_HOP, (char*)&_pkt->nexthop, 4));
      r->add(field);
      field = Field::FieldPtr(new IPField((uint32_t)IPV4_NEXT_HOP, (char*)&_pkt->nexthop, 4));
      r->add(field);
      field = Field::FieldPtr(new Field((uint32_t)INPUT_SNMP, (char*)&_pkt->input_if, 1));
      r->add(field);
      field = Field::FieldPtr(new Field((uint32_t)OUTPUT_SNMP, (char*)&_pkt->output_if, 1));
      r->add(field);
      field = Field::FieldPtr(new Field((uint32_t)IN_PKTS, (char*)&_pkt->packets, 4));
      r->add(field);
      field = Field::FieldPtr(new Field((uint32_t)IN_BYTES, (char*)&_pkt->octets, 4));
      r->add(field);
      field = Field::FieldPtr(new Field((uint32_t)FIRST_SWITCHED, (char*)&_pkt->first, 4));
      r->add(field);
      field = Field::FieldPtr(new Field((uint32_t)LAST_SWITCHED, (char*)&_pkt->last, 4));
      r->add(field);
      field = Field::FieldPtr(new Field((uint32_t)L4_SRC_PORT, (char*)&_pkt->srcport, 2));
      r->add(field);
      field = Field::FieldPtr(new Field((uint32_t)L4_DST_PORT, (char*)&_pkt->dstport, 2));
      r->add(field);
      field = Field::FieldPtr(new Field((uint32_t)TCP_FLAGS, (char*)&_pkt->flags, 1));
      r->add(field);
      field = Field::FieldPtr(new Field((uint32_t)PROTOCOL, (char*)&_pkt->prot, 1));
      r->add(field);
      field = Field::FieldPtr(new Field((uint32_t)SRC_TOS, (char*)&_pkt->tos, 1));
      r->add(field);
      field = Field::FieldPtr(new Field((uint32_t)SRC_AS, (char*)&_pkt->srcas, 2));
      r->add(field);
      field = Field::FieldPtr(new Field((uint32_t)DST_AS, (char*)&_pkt->dstas, 2));
      r->add(field);
      field = Field::FieldPtr(new Field((uint32_t)SRC_MASK, (char*)&_pkt->src_mask, 1));
      r->add(field);
      field = Field::FieldPtr(new Field((uint32_t)DST_MASK, (char*)&_pkt->src_mask, 1));
      r->add(field);
      records->push_back(r);
   }

#if 0
  header = (cnfp_v5_hdr*)packet->getCurrentBytes();
  packet->moveCursor(sizeof(cnfp_v5_hdr));

  size_t numRecordsInPacket = ntohs(header->common.count);
  logger->debug() << "reading " << numRecordsInPacket << " records" << std::endl;
  uint32_t engine = ntohl(header->engine_id);

  Template::TemplatePtr tmpl = getTemplateManager(0)->get(v5TemplateId);

  // read records and skip padding if necessary
  while (packet->dataLeft() >= tmpl->getTotalLength() && numRecordsInPacket) {
    numRecordsInPacket--;

    Record::RecordPtr r(new Record(0, ntohl(header->tstamp), engine));
    for (uint32_t fieldIdx = 0; fieldIdx < tmpl->getNumFields(); ++fieldIdx) {
      uint32_t fId = tmpl->getFieldId(fieldIdx);
      uint32_t fLen = tmpl->getFieldLength(fieldIdx);
      Field::FieldPtr field;
      // TODO: make this configurable:
      logger->debug() << "\treading field " << fId << " with len:" << fLen << std::endl;
      switch (fId) {
      case IPV4_SRC_ADDR:
      case IPV4_DST_ADDR:
      case IPV4_NEXT_HOP:
      case BGP_IPV4_NEXT_HOP:
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

    records->push_back(r);
  }
#endif

  return records->size() - recordsStartSize;
}
