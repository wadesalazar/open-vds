/****************************************************************************
** Copyright 2019 The Open Group
** Copyright 2019 Bluware, Inc.
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

#ifndef VOLUMEDATALAYOUT_H
#define VOLUMEDATALAYOUT_H

#include <OpenVDS/VolumeDataChannelDescriptor.h>
#include <OpenVDS/VolumeDataAxisDescriptor.h>
#include <OpenVDS/VolumeDataLayoutDescriptor.h>
#include <OpenVDS/Metadata.h>

namespace OpenVDS
{
/// \class VolumeDataLayout
/// \brief a class that contains axis and channel information for a VDS
/// A layout is associated with a VDS object and contains information about axis and channels in the VDS.
/// A VolumeDataLayout can be invalidated when the VDS is invalidated.
class VolumeDataLayout : public MetadataReadAccess
{
protected:
                VolumeDataLayout() {};
  virtual      ~VolumeDataLayout() {};
public:
  virtual uint64_t
                GetContentsHash() const = 0; ///< gets the contents hash of this VDS

  virtual int   GetDimensionality() const = 0; ///< gets the number of dimensions in this VDS

  virtual int   GetChannelCount() const = 0; ///< gets the number of channels in this VDS

  virtual bool  IsChannelAvailable(const char *channelName) const = 0; ///< Returns true of the VDS contains

  virtual int   GetChannelIndex(const char *channelName) const = 0; ///< Returns the index of a the channel with the given name

  virtual VolumeDataLayoutDescriptor
                GetLayoutDescriptor() const = 0; ///< Returns the descriptor for the layout

  virtual VolumeDataChannelDescriptor
                GetChannelDescriptor(int channel) const = 0; ///< Returns the descriptor for the given channel index

  virtual VolumeDataAxisDescriptor
                GetAxisDescriptor(int dimension) const = 0; ///< Returns the axis descriptor for the given dimension

  // These convenience functions provide access to the individual elements of the value descriptor
  virtual VolumeDataChannelDescriptor::Format
                GetChannelFormat(int channel) const = 0; ///< get the format for the given channel index

  virtual VolumeDataChannelDescriptor::Components
                GetChannelComponents(int channel) const = 0; ///< get the vector count for the given channel index

  virtual const char *
                GetChannelName(int channel) const = 0; ///< get the name for the given channel index

  virtual const char *
                GetChannelUnit(int channel) const = 0; ///< get the unit for the given channel index

  virtual float GetChannelValueRangeMin(int channel) const = 0; ///< get the value range minimum for the given channel index

  virtual float GetChannelValueRangeMax(int channel) const = 0; ///< get the value range maximum for the given channel index

  virtual bool  IsChannelDiscrete(int channel) const = 0; ///< get the discrete flag for the the given channel index

  virtual bool  IsChannelRenderable(int channel) const = 0; ///< get the renderable flag for the given channel index

  virtual bool  IsChannelAllowingLossyCompression(int channel) const = 0; ///< get the allow lossy compression flag for the given channel index

  virtual bool  IsChannelUseZipForLosslessCompression(int channel) const = 0; ///< get the use Zip when compressing flag for the given channel index

  virtual VolumeDataMapping
                GetChannelMapping(int channel) const = 0; ///< get the mapping for the given channel index

  // These convenience functions provide access to the individual elements of the axis descriptors
  virtual int   GetDimensionNumSamples(int dimension) const = 0; ///< get the number of samples for the given dimension
  virtual const char *
                GetDimensionName(int dimension) const = 0; ///< get the name for the given dimension
  virtual const char *
                GetDimensionUnit(int dimension) const = 0; ///< get the unit for the given dimension
  virtual float GetDimensionMin(int dimension) const = 0; ///< get the coordinate minimum for the given dimension
  virtual float GetDimensionMax(int dimension) const = 0; ///< get the coordinate maximum for the given dimension

//  virtual VDSIJKGridDefinition 
//                getVDSIJKGridDefinitionFromMetadata() const = 0; ///< get the VDSIJKGridDefinition from the metadata in this VDS

  virtual bool  IsChannelUseNoValue(int channel) const = 0; ///< Returns true if the given channel index uses No Value
  virtual float GetChannelNoValue(int channel) const = 0;   ///< gets the No Value for the given channel index

  virtual float GetChannelIntegerScale(int channel) const = 0;  ///< Returns the integer scale for the given channel index
  virtual float GetChannelIntegerOffset(int channel) const = 0; ///< Returns the integer offset for the given channel index
};

}

#endif //VOLUMEDATALAYOUT_H
