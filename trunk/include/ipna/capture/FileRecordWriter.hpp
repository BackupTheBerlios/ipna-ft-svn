/*
  FileRecordWriter.hpp - writes records to a file-hierarchy.

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
#ifndef FILE_RECORD_WRITER_HPP
#define FILE_RECORD_WRITER_HPP

#include <time.h>

#include <ipna/capture/RecordWriter.hpp>
#include <string>
#include <vector>
#include <boost/filesystem/fstream.hpp>

#include <ipna/Logger.hpp>

namespace ipna {
  namespace capture {
    class FileRecordWriter : public RecordWriter {
    public:
      FileRecordWriter(Formatter::FormatterPtr formatter, const std::string& workingDir, int nesting, unsigned int rotations);
      virtual ~FileRecordWriter() { closeOldFile(); }
      virtual void write(ipna::parser::Record::RecordPtr r);
    private:
      time_t getBlockEnd(time_t start);
      std::string getFormattedName(const std::string& fmt, time_t* t = NULL);
      std::string getNestingFormat(int level);
      void openNewFile(time_t start);
      void closeOldFile();
      void writeHeader();
      
      static ipna::Logger::LoggerPtr logger;
      
      int _nesting;
      unsigned int _rotations;
      std::string _workingDir;
      std::string _nestingFormat;
      std::string _fileFormat; // default is: ipna-ft-v09.%Y-%m-%d.%H%M%S
      std::string _fileName;
      std::string _curDir;
      time_t _curBlockStart;
      time_t _curBlockEnd;
      unsigned int _timeDelta;
      boost::filesystem::ofstream _file;
    };
  }
}

#endif // FILE_RECORD_WRITER_HPP
