/****************************************************************************
** Copyright 2020 The Open Group
** Copyright 2020 Bluware, Inc.
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
****************************************************************************/

#ifndef OPENVDS_GLOBAL_STATE_H
#define OPENVDS_GLOBAL_STATE_H

#include <OpenVDS/OpenVDS.h>
#include <stdint.h>
namespace OpenVDS
{

/// <summary>
/// Object that contains global runtime data
/// </summary>
class GlobalState
{
protected:
  ~GlobalState(){}
public:
  /// <summary>
  /// Get the global amount of downloaded bytes from a cloud vendor.
  /// </summary>
  /// <param name="connectionType"> The counter to be retireved. </param>
  /// <returns> Global number of bytes downloaded from the connection. This does not include any http header data. </returns>
  virtual uint64_t GetBytesDownloaded(OpenOptions::ConnectionType connectionType) = 0;

  /// <summary>
  /// Get the global count of downloaded chunks.
  /// </summary>
  /// <param name="connectionType"> The counter to be retireved. </param>
  /// <returns>Number of chunks downloaded.</returns>
  virtual uint64_t GetChunksDownloaded(OpenOptions::ConnectionType connectionType) = 0;

  /// <summary>
  /// Get the global amount of decompressed bytes. This amount might
  /// be smaller than the amount of downloaded bytes because of a small header
  /// pr chunk. It can also be larger for non compressed data sets since chunks
  /// can be cached.
  /// </summary>
  /// <param name="connectionType"></param>
  /// <returns>Amount of decompressed bytes served the process.</returns>
  virtual uint64_t GetBytesDecompressed(OpenOptions::ConnectionType connectionType) = 0;

  /// <summary>
  /// Get the global count of decompressed chunks.
  /// </summary>
  /// <param name="connectionType"> The counter to be retireved. </param>
  /// <returns>Number of chunks decompressed.</returns>
  virtual uint64_t GetChunksDecompressed(OpenOptions::ConnectionType connectionType) = 0;
};
}

#endif