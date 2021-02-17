############################################################################
# Copyright 2019 The Open Group
# Copyright 2019 Bluware, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
###########################################################################/

import openvds.core
import numpy as np
import sys

from openvds.core import VolumeDataLayout, VolumeDataPageAccessor

from .volumedataaccessors import VolmeDataAccessorManager

from typing import Dict, Tuple, Sequence, List

VoxelFormat = openvds.core.VolumeDataChannelDescriptor.Format
DimensionsND = openvds.core.DimensionsND
InterpolationMethod = openvds.core.InterpolationMethod

DataBlockNumpyTypes = {
#  VoxelFormat.Format_Any:
  VoxelFormat.Format_1Bit:    np.uint8,
  VoxelFormat.Format_U8:      np.uint8,
  VoxelFormat.Format_U16:     np.uint16,
  VoxelFormat.Format_R32:     np.float32,
  VoxelFormat.Format_U32:     np.uint32,
  VoxelFormat.Format_R64:     np.float64,
  VoxelFormat.Format_U64:     np.uint64
}
NumpyDataBlockTypes = i = { DataBlockNumpyTypes[k]: k for k in DataBlockNumpyTypes }

def _ndarraymin(vec: Tuple[int]):
  """Convert a tuple of integers to a 6D integer vector as a numpy array, padding with zeroes"""
  arr = np.zeros((6), dtype=np.int32)
  arr[:len(vec)] = vec[:]
  return arr

def _ndarraymax(vec: Tuple[int]):
  """Convert a tuple of integers to a 6D integer vector as a numpy array, padding with ones"""
  arr = np.ones((6), dtype=np.int32)
  arr[:len(vec)] = vec[:]
  return arr

def _ndarraypositions(tuples: List[Tuple[float]]):
  """Convert a list of float tuples to a numpy array of 6D vectors, padding with zeros"""
  l = len(tuples)
  arr = np.zeros((l, 6), dtype=np.float32)
  for i in range(l):
      arr[i][:len(tuples[i])] = tuples[i]
  return arr

class VolumeDataRequest(object):
  def __init__(self, min: Tuple[int] = None, max: Tuple[int] = None, data_out=None, dimensionsND=DimensionsND.Dimensions_012, lod=0, channel=0, format=VoxelFormat.Format_R32, replacementNoValue=None, projectedDimensions=DimensionsND.Dimensions_012, interpolationMethod=InterpolationMethod.Cubic, voxelPlane=None):
    array_interface = None
    if hasattr(data_out, "__array_interface__"):
      array_interface = data_out.__array_interface__
    elif not data_out is None:
      raise TypeError("data_out: Invalid type: Only numpy.ndarray is supported")
    if array_interface:
      ptr, readonly = array_interface["data"]
      if readonly:
        raise ValueError("array is read-only")
    self._request             = None
    self.lod                  = lod
    self.channel              = channel
    self.data_out             = data_out
    self.replacementNoValue   = replacementNoValue
    self.min                  = _ndarraymin(min)
    self.max                  = _ndarraymax(max)
    self.dimensionsND         = dimensionsND
    self.format               = format
    self.interpolationMethod  = interpolationMethod

  def __iter__(self):
      return self

  __await__ = __iter__

  def __next__(self):
      if self.isCompleted or self.isCanceled:
          raise StopIteration()

  if sys.version_info.major >= 3 and sys.version_info.minor > 5:
      from . asyncmethods import awaitCompletion

  @property
  def isCompleted(self):
    """Has request completed successfully"""
    return self._request.completed
    
  @property
  def isCanceled(self):
    """Has request been canceled"""
    return self._request.canceled
    
  @property
  def data(self):
    """The requested data. Will call `waitForCompletion()` if request has not completed"""
    if self.waitForCompletion():
        return self.data_out
    
  def cancel(self):
    """Cancel request"""
    self._request.Cancel()
    
  def waitForCompletion(self, timeout=0.0):
    """Wait for request to complete, or time out after a specified amount of time.
    
    Parameters
    ----------
    timeout : float
        Number of seconds to wait before timing out.
        
    Returns
    -------
    success : bool
        True if complete, False otherwise.
    """
    nMillisecondsBeforeTimeout = int(timeout * 1000)
    return self._request.WaitForCompletion(nMillisecondsBeforeTimeout)

class VolumeDataAccessManager(object):
  """Interface class for VDS data access.
  
  This class has functions for making asynchronous data requests on a VDS object using numpy arrays.
  
  Parameters:
  -----------
  handle : vds handle
      The VDS to interface with.
  """
  def __init__(self, handle: int):
    self._manager = openvds.core.getAccessManager(handle)

  @property
  def manager(self):
    return self._manager

  def getVolumeSubsetBufferSize(self, min: Tuple[int], max: Tuple[int], format=VoxelFormat.Format_R32, lod=0, channel=0):
    return self._manager.getVolumeSubsetBufferSize(_ndarraymin(min), _ndarraymax(max), format, lod, channel)

  def getProjectedVolumeSubsetBufferSize(self, min: Tuple[int], max: Tuple[int], projectedDimensions, format, lod=0, channel=0):
    return self._manager.getProjectedVolumeSubsetBufferSize(_ndarraymin(min), _ndarraymax(max), projectedDimensions, format, lod, channel)

  def getVolumeSamplesBufferSize(self, sampleCount, channel=0):
    return self._manager.getVolumeSamplesBufferSize(sampleCount, channel)

  def getVolumeTracesBufferSize(self, traceCount, traceDimension, lod=0, channel=0):
    return self._manager.getVolumeTracesBufferSize(traceCount, traceDimension, lod, channel)

  def _allocateArray(self, format, data_size):
    arr = np.zeros(data_size, dtype=np.uint8)
    return arr.view(dtype=DataBlockNumpyTypes[format])

  def allocateVolumeSubsetBuffer(self, min: Tuple[int], max: Tuple[int], format=VoxelFormat.Format_R32, lod=0, channel=0):
    return self._allocateArray(format, self.getVolumeSubsetBufferSize(min=min, max=max, format=format, lod=lod, channel=channel))

  def allocateProjectedVolumeSubsetBuffer(self, min: Tuple[int], max: Tuple[int], projectedDimensions, format=VoxelFormat.Format_R32, lod=0, channel=0):
    return self._allocateArray(format, self.getProjectedVolumeSubsetBufferSize(min=min, max=max, projectedDimensions=projectedDimensions, format=format, lod=lod, channel=channel))

  def allocateVolumeSamplesBuffer(self, sampleCount, channel):
    format = VoxelFormat.Format_R32
    return self._allocateArray(format, self.getVolumeSamplesBufferSize(sampleCount, channel))

  def allocateVolumeTracesBuffer(self, traceCount, traceDimension, lod, channel):
    format = VoxelFormat.Format_R32
    return self._allocateArray(format, self.getVolumeTracesBufferSize(traceCount, traceDimension, lod, channel))

  def requestVolumeSubset(self, min: Tuple[int], max: Tuple[int], data_out = None, dimensionsND=DimensionsND.Dimensions_012, lod=0, channel=0, format=VoxelFormat.Format_R32, replacementNoValue=None):
    """Request a subset of the VDS data.
          
    Parameters
    ----------
    min : tuple
        Specifies the minimum voxel coordinates of the request (inclusive)
    max : tuple
        Specifies the maximum voxel coordinates of the request (exclusive)
    data_out : numpy.ndarray, optional
        If specified, the data requested is copied to this array. Otherwise, a suitable numpy array is allocated.
    dimensionsND : DimensionsND, optional
        If specified, determine the dimensiongroup requested. Defaults to Dimensions_012
    lod : int, optional
        Which LOD level to request. Defaults to 0
    channel : int, optional
        Channel index. Defaults to 0.
    format : VoxelFormat, optional
        Specifies the format of the delivered data: Bytes, floats, doubles. etc.
    replacementNoValue: float, optional
        If specified, NoValue data in the dataset is replaced with this value.

    Returns
    -------
    request : VolumeDataRequest
        An object encapsulating the request, the request state, and the requested data.
    """
    if data_out is None:
      data_out = self.allocateVolumeSubsetBuffer(min, max, format, lod, channel)
    else:
      if data_out.nbytes < self.getVolumeSubsetBufferSize(min=min, max=max, format=format, lod=lod, channel=channel):
        raise ValueError("output array is too small to hold the requested data with format {}".format(str(format)))
    req = VolumeDataRequest(
      min                = min,
      max                = max,
      data_out           = data_out,
      dimensionsND       = dimensionsND,
      lod                = lod,
      channel            = channel,
      format             = format,
      replacementNoValue = replacementNoValue)
    req._request = self._manager.requestVolumeSubset(
                         req.data_out,
                         req.dimensionsND,
                         req.lod,
                         req.channel,
                         req.min,
                         req.max,
                         req.format,
                         req.replacementNoValue)
    return req

  def requestProjectedVolumeSubset(self, min: Tuple[int], max: Tuple[int], data_out = None, dimensionsND = DimensionsND.Dimensions_012, lod = 0, channel = 0, voxelPlane = (1,1,0,0), projectedDimensions = DimensionsND.Dimensions_12, interpolationMethod = InterpolationMethod.Cubic, format = VoxelFormat.Format_R32, replacementNoValue = None):
    """Request a subset of the VDS data, projected onto a plane.
          
    Parameters
    ----------
    min : tuple
        Specifies the minimum voxel coordinates of the request (inclusive)
    max : tuple
        Specifies the maximum voxel coordinates of the request (exclusive)
    data_out : numpy.ndarray, optional
        If specified, the data requested is copied to this array. Otherwise, a suitable numpy array is allocated.
    dimensionsND : DimensionsND, optional
        If specified, determine the dimensiongroup requested. Defaults to Dimensions_012
    lod : int, optional
        Which LOD level to request. Defaults to 0
    channel : int, optional
        Channel index. Defaults to 0.
    voxelPlane: tuple[int,int,int,int], optional
        A 4D vector representing the projection plane.
    projectedDimensions: DimensionsND, optional
        The dimensions to be projected
    interpolationMethod: InterpolationMethod, optional
        Defaults to InterpolationMethod.Cubic
    format : VoxelFormat, optional
        Specifies the format of the delivered data: Bytes, floats, doubles. etc.
    replacementNoValue: float, optional
        If specified, NoValue data in the dataset is replaced with this value.

    Returns
    -------
    request : VolumeDataRequest
        An object encapsulating the request, the request state, and the requested data.
    """
    if data_out is None:
      data_out = self.allocateProjectedVolumeSubsetBuffer(min, max, projectedDimensions, format, lod, channel)
    else:
      if data_out.nbytes < self.getProjectedVolumeSubsetBufferSize(min, max, projectedDimensions, format, lod=lod, channel=channel):
        raise ValueError("output array is too small to hold the requested data with format {}".format(str(format)))
    req = VolumeDataRequest(
      min                 = min,
      max                 = max,
      data_out            = data_out,
      dimensionsND        = dimensionsND,
      lod                 = lod,
      channel             = channel,
      format              = format,
      replacementNoValue  = replacementNoValue,
      voxelPlane          = voxelPlane,
      projectedDimensions = projectedDimensions,
      interpolationMethod = interpolationMethod)
    req._request = self._manager.requestProjectedVolumeSubset(
                    req.data_out,
                    req.dimensionsND,
                    req.lod,
                    req.channel,
                    req.min,
                    req.max,
                    req.voxelPlane,
                    req.projectedDimensions,
                    req.interpolationMethod,
                    req.replacementNoValue)
    return req

  def requestVolumeSamples(self, samplePositions, data_out = None, dimensionsND = DimensionsND.Dimensions_012, lod = 0, channel = 0, interpolationMethod = InterpolationMethod.Cubic, replacementNoValue = None):
    """Request a set of samples from the volume. The samples are always in 32-bit floating point format.
          
    Parameters
    ----------
    samplePositions:
        A set of voxel coordinates to obtain sample values for.
    data_out : numpy.ndarray, optional
        If specified, the data requested is copied to this array. Otherwise, a suitable numpy array is allocated.
    dimensionsND : DimensionsND, optional
        If specified, determine the dimensiongroup requested. Defaults to Dimensions_012
    lod : int, optional
        Which LOD level to request. Defaults to 0
    channel : int, optional
        Channel index. Defaults to 0.
    interpolationMethod: InterpolationMethod, optional
        Defaults to InterpolationMethod.Cubic
    replacementNoValue: float, optional
        If specified, NoValue data in the dataset is replaced with this value.

    Returns
    -------
    request : VolumeDataRequest
        An object encapsulating the request, the request state, and the requested data.
    """
    if data_out is None:
      data_out = self.allocateVolumeSamplesBuffer(len(samplePositions), channel)
    else:
      if data_out.nbytes < self.getVolumeSamplesBufferSize(sampleCount, channel):
        raise ValueError("output array is too small to hold the requested data with format {}".format(str(VoxelFormat.Format_R32)))
    req = VolumeDataRequest(
      data_out            = data_out,
      dimensionsND        = dimensionsND,
      lod                 = lod,
      channel             = channel,
      format              = VoxelFormat.Format_R32,
      replacementNoValue  = replacementNoValue,
      interpolationMethod = interpolationMethod)
    req.samplePositions = _ndarraypositions(arr)
    req._request = self._manager.requestVolumeSamples(
                    req.data_out, 
                    req.dimensionsND,
                    req.lod,
                    req.channel,
                    req.samplePositions,
                    req.samplePositions.shape[0],
                    req.interpolationMethod,
                    req.replacementNoValue)
    return req

  def requestVolumeTraces(self, tracePositions, traceDimension, data_out=None, dimensionsND=DimensionsND.Dimensions_012, lod=0, channel=0, interpolationMethod=InterpolationMethod.Cubic, replacementNoValue=None):
    """Request a set of traces from the volume. The traces are always in 32-bit floating point format.
          
    Parameters
    ----------
    tracePositions:
        A set of trace coordinates to obtain sample values for.
    traceDimension:
        The dimension to obtain the traces from.
    data_out : numpy.ndarray, optional
        If specified, the data requested is copied to this array. Otherwise, a suitable numpy array is allocated.
    dimensionsND : DimensionsND, optional
        If specified, determine the dimensiongroup requested. Defaults to Dimensions_012
    lod : int, optional
        Which LOD level to request. Defaults to 0
    channel : int, optional
        Channel index. Defaults to 0.
    interpolationMethod: InterpolationMethod, optional
        Defaults to InterpolationMethod.Cubic
    replacementNoValue: float, optional
        If specified, NoValue data in the dataset is replaced with this value.

    Returns
    -------
    request : VolumeDataRequest
        An object encapsulating the request, the request state, and the requested data.
    """
    traceCount = len(tracePositions)
    arr = np.zeros(shape=(traceCount, 6), dtype=np.float32)
    for i in range(traceCount):
      pos = tracePositions[i]
      for c in range(len(pos)):
        arr[i][c] = pos[c]
    if data_out is None:
      data_out = self.allocateVolumeTracesBuffer(traceCount, traceDimension, lod, channel)
    else:
      if data_out.nbytes < self.getVolumeTracesBufferSize(traceCount, traceDimension, lod, channel):
        raise ValueError("output array is too small to hold the requested data with format {}".format(str(VoxelFormat.Format_R32)))
    req = VolumeDataRequest(
      data_out            = data_out,
      dimensionsND        = dimensionsND,
      lod                 = lod,
      channel             = channel,
      format              = VoxelFormat.Format_R32,
      replacementNoValue  = replacementNoValue,
      interpolationMethod = interpolationMethod)
    req.tracePositions = arr
    req.traceDimension = traceDimension
    req._request = self._manager.requestVolumeTraces(
                    req.data_out, 
                    req.dimensionsND,
                    req.lod,
                    req.channel,
                    req.tracePositions,
                    traceCount,
                    req.interpolationMethod,
                    req.traceDimension,
                    req.replacementNoValue)
    return req

  def prefetchVolumeChunk(self, chunkIndex, dimensionsND=DimensionsND.Dimensions_012, lod=0, channel=0):
    """Force production of a specific volume data chunk.

    Parameters
    ----------
    chunkIndex: int
        The index of the chunk to prefetch.
    dimensionsND : DimensionsND, optional
        If specified, determine the dimensiongroup requested. Defaults to Dimensions_012
    lod : int, optional
        Which LOD level to request. Defaults to 0
    channel : int, optional
        Channel index. Defaults to 0.

    Returns
    -------
    request : VolumeDataRequest
        An object encapsulating the request, the request state, and the requested data.
    """
    req = VolumeDataRequest(
      dimensionsND        = dimensionsND,
      lod                 = lod,
      channel             = channel)
    req.chunkIndex = chunkIndex
    req._request = self._manager.prefetchVolumeChunk(req.dimensionsND, req.lod, req.channel, req.chunkIndex)
    return req

    def _createVolumeDataAccessor(self, factoryName: str, pageAccessor: VolumeDataPageAccessor=None, replacementNoValue=None, interpolationMethod: InterpolationMethod=None):
        if pageAccessor is None:
            defaultDimensions = { 2: DimensionsND.Dimensions_01, 3: DimensionsND.Dimensions_012, 4: DimensionsND.Dimensions_012 }
            pageAccessor = self._manager.createVolumeDataPageAccessor(self.layout, defaultDimensions[self.layout.dimensionality], 0, 0, 8, self._manager.AccessMode.AccessMode_ReadOnly)
        return VolmeDataAccessorManager(factoryName.strip(), self._manager, pageAccessor, replacementNoValue, interpolationMethod)

    def create2DVolumeDataAccessor1Bit            (self, pageAccessor: VolumeDataPageAccessor=None, replacementNoValue=None): return self._createVolumeDataAccessor("create2DVolumeDataAccessor1Bit", pageAccessor, replacementNoValue)
    def create2DVolumeDataAccessorU8              (self, pageAccessor: VolumeDataPageAccessor=None, replacementNoValue=None): return self._createVolumeDataAccessor("create2DVolumeDataAccessorU8  ", pageAccessor, replacementNoValue)
    def create2DVolumeDataAccessorU16             (self, pageAccessor: VolumeDataPageAccessor=None, replacementNoValue=None): return self._createVolumeDataAccessor("create2DVolumeDataAccessorU16 ", pageAccessor, replacementNoValue)
    def create2DVolumeDataAccessorU32             (self, pageAccessor: VolumeDataPageAccessor=None, replacementNoValue=None): return self._createVolumeDataAccessor("create2DVolumeDataAccessorU32 ", pageAccessor, replacementNoValue)
    def create2DVolumeDataAccessorU64             (self, pageAccessor: VolumeDataPageAccessor=None, replacementNoValue=None): return self._createVolumeDataAccessor("create2DVolumeDataAccessorU64 ", pageAccessor, replacementNoValue)
    def create2DVolumeDataAccessorR32             (self, pageAccessor: VolumeDataPageAccessor=None, replacementNoValue=None): return self._createVolumeDataAccessor("create2DVolumeDataAccessorR32 ", pageAccessor, replacementNoValue)
    def create2DVolumeDataAccessorR64             (self, pageAccessor: VolumeDataPageAccessor=None, replacementNoValue=None): return self._createVolumeDataAccessor("create2DVolumeDataAccessorR64 ", pageAccessor, replacementNoValue)
    def create3DVolumeDataAccessor1Bit            (self, pageAccessor: VolumeDataPageAccessor=None, replacementNoValue=None): return self._createVolumeDataAccessor("create3DVolumeDataAccessor1Bit", pageAccessor, replacementNoValue)
    def create3DVolumeDataAccessorU8              (self, pageAccessor: VolumeDataPageAccessor=None, replacementNoValue=None): return self._createVolumeDataAccessor("create3DVolumeDataAccessorU8  ", pageAccessor, replacementNoValue)
    def create3DVolumeDataAccessorU16             (self, pageAccessor: VolumeDataPageAccessor=None, replacementNoValue=None): return self._createVolumeDataAccessor("create3DVolumeDataAccessorU16 ", pageAccessor, replacementNoValue)
    def create3DVolumeDataAccessorU32             (self, pageAccessor: VolumeDataPageAccessor=None, replacementNoValue=None): return self._createVolumeDataAccessor("create3DVolumeDataAccessorU32 ", pageAccessor, replacementNoValue)
    def create3DVolumeDataAccessorU64             (self, pageAccessor: VolumeDataPageAccessor=None, replacementNoValue=None): return self._createVolumeDataAccessor("create3DVolumeDataAccessorU64 ", pageAccessor, replacementNoValue)
    def create3DVolumeDataAccessorR32             (self, pageAccessor: VolumeDataPageAccessor=None, replacementNoValue=None): return self._createVolumeDataAccessor("create3DVolumeDataAccessorR32 ", pageAccessor, replacementNoValue)
    def create3DVolumeDataAccessorR64             (self, pageAccessor: VolumeDataPageAccessor=None, replacementNoValue=None): return self._createVolumeDataAccessor("create3DVolumeDataAccessorR64 ", pageAccessor, replacementNoValue)
    def create4DVolumeDataAccessor1Bit            (self, pageAccessor: VolumeDataPageAccessor=None, replacementNoValue=None): return self._createVolumeDataAccessor("create4DVolumeDataAccessor1Bit", pageAccessor, replacementNoValue)
    def create4DVolumeDataAccessorU8              (self, pageAccessor: VolumeDataPageAccessor=None, replacementNoValue=None): return self._createVolumeDataAccessor("create4DVolumeDataAccessorU8  ", pageAccessor, replacementNoValue)
    def create4DVolumeDataAccessorU16             (self, pageAccessor: VolumeDataPageAccessor=None, replacementNoValue=None): return self._createVolumeDataAccessor("create4DVolumeDataAccessorU16 ", pageAccessor, replacementNoValue)
    def create4DVolumeDataAccessorU32             (self, pageAccessor: VolumeDataPageAccessor=None, replacementNoValue=None): return self._createVolumeDataAccessor("create4DVolumeDataAccessorU32 ", pageAccessor, replacementNoValue)
    def create4DVolumeDataAccessorU64             (self, pageAccessor: VolumeDataPageAccessor=None, replacementNoValue=None): return self._createVolumeDataAccessor("create4DVolumeDataAccessorU64 ", pageAccessor, replacementNoValue)
    def create4DVolumeDataAccessorR32             (self, pageAccessor: VolumeDataPageAccessor=None, replacementNoValue=None): return self._createVolumeDataAccessor("create4DVolumeDataAccessorR32 ", pageAccessor, replacementNoValue)
    def create4DVolumeDataAccessorR64             (self, pageAccessor: VolumeDataPageAccessor=None, replacementNoValue=None): return self._createVolumeDataAccessor("create4DVolumeDataAccessorR64 ", pageAccessor, replacementNoValue)
    def create2DInterpolatingVolumeDataAccessorR32(self, pageAccessor: VolumeDataPageAccessor=None, replacementNoValue=None, interpolationMethod: InterpolationMethod=InterpolationMethod.Cubic): return self._createVolumeDataAccessor("create2DInterpolatingVolumeDataAccessorR32", pageAccessor, replacementNoValue, interpolationMode)
    def create2DInterpolatingVolumeDataAccessorR64(self, pageAccessor: VolumeDataPageAccessor=None, replacementNoValue=None, interpolationMethod: InterpolationMethod=InterpolationMethod.Cubic): return self._createVolumeDataAccessor("create2DInterpolatingVolumeDataAccessorR64", pageAccessor, replacementNoValue, interpolationMode)
    def create3DInterpolatingVolumeDataAccessorR32(self, pageAccessor: VolumeDataPageAccessor=None, replacementNoValue=None, interpolationMethod: InterpolationMethod=InterpolationMethod.Cubic): return self._createVolumeDataAccessor("create3DInterpolatingVolumeDataAccessorR32", pageAccessor, replacementNoValue, interpolationMode)
    def create3DInterpolatingVolumeDataAccessorR64(self, pageAccessor: VolumeDataPageAccessor=None, replacementNoValue=None, interpolationMethod: InterpolationMethod=InterpolationMethod.Cubic): return self._createVolumeDataAccessor("create3DInterpolatingVolumeDataAccessorR64", pageAccessor, replacementNoValue, interpolationMode)
    def create4DInterpolatingVolumeDataAccessorR32(self, pageAccessor: VolumeDataPageAccessor=None, replacementNoValue=None, interpolationMethod: InterpolationMethod=InterpolationMethod.Cubic): return self._createVolumeDataAccessor("create4DInterpolatingVolumeDataAccessorR32", pageAccessor, replacementNoValue, interpolationMode)
    def create4DInterpolatingVolumeDataAccessorR64(self, pageAccessor: VolumeDataPageAccessor=None, replacementNoValue=None, interpolationMethod: InterpolationMethod=InterpolationMethod.Cubic): return self._createVolumeDataAccessor("create4DInterpolatingVolumeDataAccessorR64", pageAccessor, replacementNoValue, interpolationMode)
    
