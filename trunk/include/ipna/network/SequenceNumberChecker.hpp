#ifndef SEQUENCE_NUMBER_CHECKER_HPP
#define SEQUENCE_NUMBER_CHECKER_HPP

#include <vector>
#include <string>

namespace ipna {
  typedef unsigned int seq_t;

  class SequenceNumberChecker {
  public:
    typedef std::vector<seq_t> SequenceNumberVector;
    
    enum SequenceError {
      SEQ_OK,
      SEQ_MISSED,
      SEQ_DUPLICATE,
      SEQ_REORDER
    };
    
    SequenceNumberChecker(size_t maxSize = 128);
    virtual ~SequenceNumberChecker() {}

    SequenceError check(seq_t sequenceNumber);
    SequenceError state();
    bool ok();
    int missed();
    bool duplicate();

    std::string toString();
    
  private:
    SequenceNumberVector _sequence;
    seq_t _current;
    size_t _maxSize;
    SequenceError _lastState;
    unsigned int _lastSequenceIdx;
  };
}

#endif // SEQUENCE_NUMBER_CHECKER_HPP
