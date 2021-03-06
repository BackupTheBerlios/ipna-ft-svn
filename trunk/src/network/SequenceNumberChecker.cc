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
#include <ipna/network/SequenceNumberChecker.hpp>

#include <sstream>
#include <iostream>

using namespace std;
using namespace ipna::network;

SequenceNumberChecker::SequenceNumberChecker(size_t maxSize)
  : _maxSize(maxSize), _current(0), _lastSequenceIdx(0), _lastState(SEQ_OK) {
  _sequence.insert(_sequence.end(),maxSize,0);
}

SequenceNumberChecker::SequenceError
SequenceNumberChecker::check(seq_t sn, seq_t inc) {
  _current = sn;
  SequenceError state = SEQ_OK;
  unsigned int foundIdx = _sequence.size();

  for (unsigned int idx = _lastSequenceIdx; idx != (_lastSequenceIdx-1)%_maxSize; idx = (idx+1)%_maxSize) {
    if ((_sequence[idx]+inc) == _current) {
      foundIdx = idx;
      break;
    } else if (_sequence[idx] > _current) {
      foundIdx = idx;
      break;
    }
  }

  if (foundIdx != _sequence.size() && ((_sequence[_lastSequenceIdx]+inc) != _current)) {
    state = SEQ_REORDER;
    //    cerr << "packet-reordering occured within window: [" << _sequence[foundIdx] << ":" << _sequence[_lastSequenceIdx] << "]" << endl;
  } else if (foundIdx == _sequence.size()) {
    state = SEQ_MISSED;
    //    cerr << "missed " << (int)(_current - _sequence[_lastSequenceIdx]) << " packet(s)" << endl;
  }

  //  cerr << "state: " << state << " lastIdx: " << _lastSequenceIdx << " current: " << _current << ": " << toString() << endl;
  
  // store the new sequence
  _lastSequenceIdx = (_lastSequenceIdx+inc) % _maxSize;
  _sequence[_lastSequenceIdx] = _current;
  _lastState = state;

  return state;
}

bool
SequenceNumberChecker::ok() {
  return (_lastState == SEQ_OK);
}

SequenceNumberChecker::SequenceError
SequenceNumberChecker::state() {
  return _lastState;
}

int
SequenceNumberChecker::missed() {
  return (int)(_current - _sequence[(_lastSequenceIdx-1) % _sequence.size()]);
}

bool
SequenceNumberChecker::duplicate() {
  return (_lastState == SEQ_DUPLICATE);
}

string
SequenceNumberChecker::toString() {
  stringstream strm;
  for (SequenceNumberVector::iterator it = _sequence.begin(); it != _sequence.end(); it++) {
    strm << (*it) << ", ";
  }
  return strm.str();
}
