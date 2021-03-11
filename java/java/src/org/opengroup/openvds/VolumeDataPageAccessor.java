/*
 * Copyright 2019 The Open Group
 * Copyright 2019 INT, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.opengroup.openvds;

public class VolumeDataPageAccessor extends JniPointerWithoutDeletion {
    
    private static native long cpGetLayout( long handle );

    VolumeDataPageAccessor(long handle) {
        super(handle);
    }

  VolumeDataLayout getLayout() {
      return new VolumeDataLayout( cpGetLayout(_handle) );
  }

  /*
  virtual int   GetLOD() const = 0;
  virtual int   GetChannelIndex() const = 0;
  virtual VolumeDataChannelDescriptor const &GetChannelDescriptor() const = 0;
  virtual void  GetNumSamples(int (&numSamples)[Dimensionality_Max]) const = 0;

  virtual int64_t GetChunkCount() const = 0;
  virtual void  GetChunkMinMax(int64_t chunk, int (&min)[Dimensionality_Max], int (&max)[Dimensionality_Max]) const = 0;
  virtual void  GetChunkMinMaxExcludingMargin(int64_t chunk, int (&minExcludingMargin)[Dimensionality_Max], int (&maxExcludingMargin)[Dimensionality_Max]) const = 0;
  virtual int64_t GetChunkIndex(const int (&position)[Dimensionality_Max]) const = 0;
  virtual int64_t GetMappedChunkIndex(int64_t primaryChannelChunkIndex) const = 0;
  virtual int64_t GetPrimaryChannelChunkIndex(int64_t chunkIndex) const = 0;

  virtual int   AddReference() = 0;
  virtual int   RemoveReference() = 0;

  virtual int   GetMaxPages() = 0;
  virtual void  SetMaxPages(int maxPages) = 0;

  virtual VolumeDataPage *CreatePage(int64_t chunkIndex) = 0;
  virtual VolumeDataPage *ReadPage(int64_t chunkIndex) = 0;

  VolumeDataPage *ReadPageAtPosition(const int (&position)[Dimensionality_Max]) { return ReadPage(GetChunkIndex(position)); }

  virtual void  Commit() = 0;*/
}
