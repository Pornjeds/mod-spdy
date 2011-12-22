// Copyright 2011 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef MOD_SPDY_COMMON_SPDY_CONNECTION_IO_H_
#define MOD_SPDY_COMMON_SPDY_CONNECTION_IO_H_

#include "base/basictypes.h"

namespace spdy {
class SpdyFrame;
class SpdyFramer;
}  // namespace spdy

namespace mod_spdy {

class SpdyStream;

// SpdyConnectionIO is a helper interface for the SpdyConnection class.  The
// SpdyConnectionIO takes care of implementation-specific details about how to
// send and receive data, allowing the SpdyConnection to focus on the SPDY
// protocol itself.  For example, a SpdyConnectionIO for Apache would hold onto
// a conn_rec object and invoke the input and output filter chains for
// ProcessAvailableInput and SendFrameRaw, respectively.  The SpdyConnectionIO
// itself does not need to be thread-safe -- it is only ever used by the main
// connection thread.
class SpdyConnectionIO {
 public:
  // Status to describe whether reading succeeded.
  enum ReadStatus {
    READ_SUCCESS,  // we successfully pushed data into the SpdyFramer
    READ_NO_DATA,  // no data is currently available
    READ_CONNECTION_CLOSED,  // the connection has been closed
    READ_ERROR  // an unrecoverable error (e.g. client sent malformed data)
  };

  SpdyConnectionIO();
  virtual ~SpdyConnectionIO();

  // Return true if the connection has been externally aborted and should
  // stop, false otherwise.
  virtual bool IsConnectionAborted() = 0;

  // Pull any available input data from the connection and feed it into the
  // ProcessInput() method of the given SpdyFramer.  If no input data is
  // currently available and the block argument is true, this should block
  // until more data arrives; otherwise, this should not block.
  virtual ReadStatus ProcessAvailableInput(bool block,
                                           spdy::SpdyFramer* framer) = 0;

  // Send a single SPDY frame to the client as-is; block until it has been
  // sent down the wire.  Return true on success.
  //
  // TODO(mdsteele): We do need to be able to flush a single frame down the
  //   wire, but we probably don't need/want to flush every single frame
  //   individually in places where we send multiple frames at once.  We'll
  //   probably want to adjust this API a bit.
  virtual bool SendFrameRaw(const spdy::SpdyFrame& frame) = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(SpdyConnectionIO);
};

}  // namespace mod_spdy

#endif  // MOD_SPDY_COMMON_SPDY_CONNECTION_IO_H_
