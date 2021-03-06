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
#include "fio/OutFile.h"

#include <cassert>

#include "prim/prim.h"

namespace fio {

OutFile::OutFile(const char* _filepath) : OutFile(std::string(_filepath)) {}

OutFile::OutFile(const std::string& _filepath) {
  u64 namelen = _filepath.size();
  assert(namelen > 0);
  compress_ = _filepath.size() >= 3 && _filepath.substr(namelen - 3) == ".gz";

  if (compress_) {
    gz_file_ = gzopen(_filepath.c_str(), "wb");
    assert(gz_file_ != nullptr);
  } else {
    reg_file_ = fopen(_filepath.c_str(), "wb");
    assert(reg_file_ != nullptr);
  }
}

OutFile::~OutFile() {
  if (compress_) {
    gzclose(gz_file_);
  } else {
    fclose(reg_file_);
  }
}

bool OutFile::compressed() const {
  return compress_;
}

OutFile::Status OutFile::write(const std::string& _text) {
  const void* cstr = reinterpret_cast<const void*>(_text.c_str());
  size_t len = _text.size();

  if (compress_) {
    s64 wlen = gzwrite(gz_file_, cstr, len);
    assert(wlen == (s64)len);
  } else {
    u64 wlen = fwrite(cstr, sizeof(char), len, reg_file_);
    assert(wlen == len);
  }

  return OutFile::Status::OK;
}

OutFile::Status OutFile::writeFile(const char* _filepath,
                                   const std::string& _text) {
  return OutFile::writeFile(std::string(_filepath), _text);
}

OutFile::Status OutFile::writeFile(const std::string& _filepath,
                                   const std::string& _text) {
  OutFile outfile(_filepath);
  return outfile.write(_text);
}

}  // namespace fio
