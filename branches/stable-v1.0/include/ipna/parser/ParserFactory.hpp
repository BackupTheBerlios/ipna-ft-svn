#ifndef PARSER_FACTORY_HPP
#define PARSER_FACTORY_HPP

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

#include <ipna/parser/cnfp.hpp>
#include <map>

namespace ipna {
  namespace parser {
    class ParserFactory;
    class PacketParser;

    typedef boost::shared_ptr<ParserFactory> ParserFactoryPtr;
    typedef boost::shared_ptr<PacketParser> ParserPtr;
    
    class ParserFactory {
    public:
      static ParserFactoryPtr getInstance();
      virtual ~ParserFactory() {}

      ParserPtr getParser(NetflowVersion version);
      ParserPtr getParser(unsigned short version);
      ParserPtr getParser(const char * packet);
      ParserPtr getParser(boost::shared_array<char> packet);
      ParserPtr getParser(const cnfp_common_hdr& hdr);
    protected:
      ParserFactory() {}
      ParserPtr newParser(NetflowVersion version) const;
      typedef std::map<NetflowVersion, ParserPtr> ParserMap;
      ParserMap _parserMap;
    private:
      static ParserFactoryPtr _factory;
    };
  }
}

#endif // PARSER_FACTORY_HPP
