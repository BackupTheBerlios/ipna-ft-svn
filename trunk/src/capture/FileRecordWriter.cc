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
#include <ipna/capture/FileRecordWriter.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

#include <QHostInfo>

#include <assert.h>
#include <time.h>
#include <algorithm>
#include <unistd.h>
#include <errno.h>

using namespace ipna;
using namespace ipna::capture;
using namespace ipna::parser;

namespace fs = boost::filesystem;

Logger::LoggerPtr FileRecordWriter::logger = Logger::getLogger("ipna.capture.FileRecordWriter");

FileRecordWriter::FileRecordWriter(Formatter::FormatterPtr formatter,
				   const std::string& workingDir,
				   int nesting,
				   unsigned int rotations)
  : RecordWriter(formatter),
    _rotations(rotations),
    _workingDir(workingDir),
    _fileFormat("ipna-ft-v09.%Y-%m-%d.%H%M%S%z") {

  fs::path::default_name_check(fs::native);
  _nestingFormat = getNestingFormat(nesting);
  
  if (chdir(workingDir.c_str())) {
    LOG_WARN("chdir: " << strerror(errno) << ": " << workingDir << ", creating it.");
    try {
      fs::create_directories(fs::path(workingDir));
    } catch (...) {
      LOG_ERROR("could not create directory '" << workingDir << "'");
      exit(1);
    }
    assert(chdir(workingDir.c_str()) == 0);
  }
  
  _timeDelta = std::max(24*60 / (_rotations+1), (unsigned int)1) * 60;
  openNewFile(time(NULL));
}

time_t
FileRecordWriter::getBlockEnd(time_t blockStart) {
  return blockStart + (_timeDelta - (blockStart % _timeDelta));
}

std::string
FileRecordWriter::getNestingFormat(int level) {
  assert(-3 <= level && level <= 3);
  switch (level) {
  case -3:
    return "%Y/%Y-%m/%Y-%m-%d";
    break;
  case -2:
    return "%Y-%m/%Y-%m-%d";
    break;
  case -1:
    return "%Y-%m-%d";
    break;
  case 0:
    return ".";
    break;
  case 1:
    return "%Y";
    break;
  case 2:
    return "%Y/%Y-%m";
    break;
  case 3:
    return "%Y/%Y-%m/%Y-%m-%d";
    break;
  default:
    assert(false);
  }
}


std::string
FileRecordWriter::getFormattedName(const std::string& fmt, time_t *t) {
  const size_t tmpSize = 256;
  char tmp[tmpSize];
  std::string name;
  if (NULL == t)
    t = &_curBlockStart;
  
  struct tm *tmPointer = localtime(t);
  if (strftime(tmp,tmpSize,fmt.c_str(), tmPointer) == 0) {
    LOG_ERROR("strftime returned 0: could not put date with format '" << fmt << "' in buffer of size " << tmpSize);
    exit(1);
  } else {
    name.assign(tmp);
  }
  return name;
}

void
FileRecordWriter::writeHeader() {
  if (_file.good()) {
    _file << "#" << std::endl;
    _file << "# version:\t 0.2"   << std::endl;
    _file << "# capture host: \t" << QHostInfo::localHostName().toStdString() << std::endl;
    _file << "# capture start:\t" << asctime(localtime(&_curBlockStart));
    _file << "# capture end:  \t" << asctime(localtime(&_curBlockEnd));
    _file << "#" << std::endl;
  }
}

void
FileRecordWriter::closeOldFile() {
  // close and rename the old one
  if (_file.good()) {
    _file.close();
  }    

  try {
    if (fs::exists(fs::path(_workingDir) / fs::path(_curDir) / ("tmp-" + _fileName))) {
      fs::rename(fs::path(_workingDir) / fs::path(_curDir) / ("tmp-" + _fileName), fs::path(_workingDir) / fs::path(_curDir) / _fileName);
    }
  } catch (...) {
    LOG_ERROR("could not rename old file!");
    exit (1);
  }
}

void
FileRecordWriter::openNewFile(time_t t) {
  closeOldFile();
  
  _curBlockStart = t;
  _curBlockEnd   = getBlockEnd(_curBlockStart);
  
  _fileName = getFormattedName(_fileFormat);
  _curDir   = getFormattedName(_nestingFormat);
  
  LOG_DEBUG("writing to file '" << _fileName << "'");
  LOG_DEBUG("creating directory '" << _curDir << "'");
  LOG_DEBUG("block start: " << asctime(localtime(&_curBlockStart)));
  LOG_DEBUG("block end: " << asctime(localtime(&_curBlockEnd)));
  
  try {
    fs::create_directories(fs::path(_curDir));
  } catch ( ... ) {
    LOG_ERROR("could not create directory: '" << _curDir << "'");
    exit(1);
  }
  
  fs::path pth;
  try {
     pth /= fs::path(_workingDir);
     pth /= fs::path(_curDir);
     pth /= fs::path("tmp-" + _fileName, fs::native);
    _file.open(pth);
  } catch (...) {
    LOG_ERROR("could not open '" << pth.string() << "' for writing access...");
    exit(1);
  }
  
  setStream(_file);

  writeHeader();
}

void
FileRecordWriter::write(Record::RecordPtr r) {
  // change the stream here
  if ( r->tstamp() > _curBlockEnd /* || time(NULL) > _curBlockEnd */) {
    openNewFile(_curBlockEnd);
  }

  // now call the super method
  RecordWriter::write(r);
}
