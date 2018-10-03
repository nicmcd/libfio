/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * - Neither the name of prim nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific prior
 * written permission.
 *
 * See the NOTICE file distributed with this work for additional information
 * regarding copyright ownership.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "fio/InFile.h"

#include <prim/prim.h>

#include <cassert>
#include <cerrno>
#include <cstring>

namespace fio {

InFile::InFile(const char* _filepath, char _delim, u64 _blockSize)
    : InFile(std::string(_filepath), _delim, _blockSize) {}

InFile::InFile(const std::string& _filepath, char _delim, u64 _blockSize)
    : delim_(_delim), blockSize_(_blockSize) {
  u64 namelen = _filepath.size();
  assert(namelen > 0);
  compress_ = _filepath.size() >= 3 && _filepath.substr(namelen-3) == ".gz";

  if (compress_) {
    gzFile_ = gzopen(_filepath.c_str(), "rb");
    if (!gzFile_) {
      fprintf(stderr, "gzopen of %s failed: %s.\n", _filepath.c_str(),
              strerror(errno));
      assert(false);
    }
  } else {
    stdin_ = _filepath == "-";
    if (stdin_) {
      regFile_ = stdin;
    } else {
      regFile_ = fopen(_filepath.c_str(), "rb");
      if (!regFile_) {
        fprintf(stderr, "fopen of %s failed: %s.\n", _filepath.c_str(),
                strerror(errno));
        assert(false);
      }
    }
  }

  buf_ = new char[blockSize_ + 1];
  buf_[blockSize_] = '\0';
  eof_ = false;
}

InFile::~InFile() {
  if (compress_) {
    gzclose(gzFile_);
  } else if (!stdin_) {
    fclose(regFile_);
  }
  delete[] buf_;
}

bool InFile::compressed() const {
  return compress_;
}

InFile::Status InFile::getLine(std::string* _line) {
  if (eof_ && queue_.size() == 0) {
    return Status::END;
  }

  // read until end of line of end of file
  bool done = false;
  while (!done) {
    // inspect all queue chars
    while (!done && queue_.size() > 0) {
      char c = queue_.front();
      queue_.pop();
      if (c == delim_) {
        done = true;
      } else {
        stream_.put(c);
      }
    }
    if (eof_ && !done) {
      done = true;
    }
    if (done) {
      break;
    }
    assert(!eof_);

    // read data from file
    u64 read;
    if (compress_) {
      s64 bytesRead = gzread(gzFile_, buf_, blockSize_);
      assert(bytesRead > 0 || (bytesRead == 0 && gzeof(gzFile_)));
      read = (u64)bytesRead;
    } else {
      u64 bytesRead = fread(buf_, 1, blockSize_, regFile_);
      assert(bytesRead > 0 || feof(regFile_));
      read = bytesRead;
    }

    // transfer all chars into streams
    for (u64 c = 0; c < read; c++) {
      if (!done) {
        if (buf_[c] != delim_) {
          stream_.put(buf_[c]);
        } else {
          done = true;
        }
      } else {
        queue_.push(buf_[c]);
      }
    }

    // detect eof
    if (read == 0) {
      eof_ = true;
      done = true;
    }
  }

  // return the string
  _line->clear();
  *_line = stream_.str();
  stream_.str(std::string());
  return Status::OK;
}

}  // namespace fio
