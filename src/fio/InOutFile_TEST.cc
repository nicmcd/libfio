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
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "fio/InFile.h"
#include "fio/OutFile.h"

const std::string& kContents =
    "hello\n"
    "  this is not a test of \n"
    "the emergency broadcast\n"
    "system\n";

TEST(InOutFile, bidir) {
  std::string contents = "";
  for (int copy = 0; copy < 1000; copy++) {
    contents += kContents;
  }

  std::vector<std::string> filenames({
      "/tmp/myfile.txt", "/tmp/myfile.txt.gz"});
  for (std::string& filename : filenames) {
    // write to file
    fio::OutFile::Status osts = fio::OutFile::writeFile(filename, contents);
    ASSERT_EQ(osts, fio::OutFile::Status::OK);

    // read from from
    std::string readText;
    fio::InFile::Status ists = fio::InFile::readFile(filename, &readText);
    ASSERT_EQ(ists, fio::InFile::Status::OK);

    // compare
    ASSERT_EQ(readText, contents);
  }
}
