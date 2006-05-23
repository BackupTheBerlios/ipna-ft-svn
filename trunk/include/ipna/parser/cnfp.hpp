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
#ifndef CNFP_H
#define CNFP_H

namespace ipna {
  namespace parser {

    enum NetflowVersion {
      CNFPv1 = 1,
      CNFPv5 = 5,
      CNFPv7 = 7,
      CNFPv8 = 8,
      CNFPv9 = 9
    };

    enum FieldType {
      IN_BYTES         	    = 1,
      IN_PKTS          	    = 2,
      FLOWS            	    = 3,
      PROTOCOL         	    = 4,
      SRC_TOS          	    = 5,
      TCP_FLAGS        	    = 6,
      L4_SRC_PORT      	    = 7,
      IPV4_SRC_ADDR    	    = 8,
      SRC_MASK         	    = 9,
      INPUT_SNMP       	    = 10,
      L4_DST_PORT      	    = 11,
      IPV4_DST_ADDR    	    = 12,
      DST_MASK         	    = 13,
      OUTPUT_SNMP      	    = 14,
      IPV4_NEXT_HOP    	    = 15,
      SRC_AS           	    = 16,
      DST_AS           	    = 17,
      BGP_IPV4_NEXT_HOP	    = 18,
      MUL_DST_PKTS     	    = 19,
      MUL_DST_BYTES    	    = 20,
      LAST_SWITCHED    	    = 21,
      FIRST_SWITCHED   	    = 22,
      OUT_BYTES        	    = 23,
      OUT_PKTS         	    = 24,
      MIN_PKT_LNGTH    	    = 25,
      MAX_PKT_LNGTH    	    = 26,
      IPV6_SRC_ADDR    	    = 27,
      IPV6_DST_ADDR    	    = 28,
      IPV6_SRC_MASK    	    = 29,
      IPV6_DST_MASK    	    = 30,
      IPV6_FLOW_LABEL  	    = 31,
      ICMP_TYPE             = 32,
      MUL_IGMP_TYPE         = 33,
      SAMPLING_INTERVAL     = 34,
      SAMPLING_ALGORITHM    = 35,
      FLOW_ACTIVE_TIMEOUT   = 36,
      FLOW_INACTIVE_TIMEOUT = 37,
      ENGINE_TYPE           = 38,
      ENGINE_ID             = 39,
      TOTAL_BYTES_EXP       = 40,
      TOTAL_PKTS_EXP        = 41,
      TOTAL_FLOWS_EXP       = 42,
      // vendor proprietary = 43,
      IPV4_SRC_PREFIX       = 44,
      IPV4_DST_PREFIX       = 45,
      MPLS_TOP_LABEL_TYPE   = 46,
      MPLS_TOP_LABEL_IP_ADDR= 47,
      FLOW_SAMPLER_ID       = 48,
      FLOW_SAMPLER_MODE     = 49,
      FLOW_SAMPLER_RANDOM_INTERVAL = 50,
      // vendor proprietary = 51,
      MIN_TTL = 52,
      MAX_TTL = 53,
      IPV4_IDENT = 54,
      DST_TOS = 55,
      IN_SRC_MAC = 56,
      OUT_DST_MAC = 57,
      SRC_VLAN = 58,
      DST_VLAN = 59,
      IP_PROTOCOL_VERSION = 60,
      DIRECTION = 61,
      IPV6_NEXT_HOP = 62,
      BGP_IPV6_NEXT_HOP = 63,
      IPV6_OPTION_HEADERS = 64,
      // vendor proprietary and mpls 65 -> 79
      IN_DST_MAC = 80,
      OUT_SRC_MAC = 81,
      IF_NAME = 82,
      IF_DESC = 83,
      SAMPLER_NAME = 84,
      IN_PERMANENT_BYTES = 85,
      IN_PERMANENT_PKTS = 86,
      // vendor proprietary = 87,
      FRAGMENT_OFFSET = 88,
      FORWARDING_STATUS = 89
    };
    
    struct cnfp_common_hdr {
      unsigned short version;
      unsigned short count;
    };

    struct cnfp_v1_hdr {
      struct cnfp_common_hdr common;
      unsigned int uptime;
      unsigned int tstamp;
      unsigned int nanosecs;
    };

    struct cnfp_v5_hdr {
      struct cnfp_common_hdr common;
      unsigned int uptime;   // uptime of exporting device
      unsigned int tstamp;   // current time of exporting device
      unsigned int nanosecs; // nano seconds residual
      unsigned int seq; // number of total flows seen
      unsigned char engine_type;
      unsigned char engine_id;
      unsigned short padding;
    };

    struct cnfp_v7_hdr {
      struct cnfp_common_hdr common;
      unsigned int uptime;
      unsigned int tstamp;
      unsigned int nanosecs;
      unsigned int seq;
      unsigned int reserved;
    };

    struct cnfp_v8_hdr {
      struct cnfp_common_hdr common;
      unsigned int uptime;
      unsigned int tstamp;
      unsigned int nanosecs;
      unsigned int seq;
      unsigned char engine_type;
      unsigned char engine_id;
      unsigned char agg;
      unsigned char aggversion;
    };

    struct cnfp_v9_hdr {
      struct cnfp_common_hdr common;
      unsigned int uptime;
      unsigned int tstamp;
      unsigned int seq;
      int engine_id;
    };

    typedef struct cnfp_v9_hdr cnfp_v9_hdr_s;
    typedef cnfp_v9_hdr_s* cnfp_v9_hdr_t;

    struct cnfp_v9_flowset_hdr {
      unsigned short flowset_id;
      unsigned short length;
    };
    typedef struct cnfp_v9_flowset_hdr cnfp_v9_flowset_hdr_s;
    typedef cnfp_v9_flowset_hdr_s* cnfp_v9_flowset_hdr_t;
  }
}

#endif // CNFP_H
