#ifndef CNFP_H
#define CNFP_H

namespace ipna {
  namespace parser {
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
  }
}

#endif // CNFP_H
