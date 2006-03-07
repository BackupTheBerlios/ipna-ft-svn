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
