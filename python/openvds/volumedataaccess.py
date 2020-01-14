import openvds.core
import numpy as np
from openvds.volumedatalayout import VolumeDataLayout

from typing import Dict, Tuple, Sequence

VoxelFormat = openvds.core.VolumeDataChannelDescriptor.Format
DimensionsND = openvds.core.DimensionsND

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

NumpyDataBlockTypes = { 
np.int8:     VoxelFormat.Format_U8,
np.uint8:    VoxelFormat.Format_U8,
np.int16:    VoxelFormat.Format_U16,
np.uint16:   VoxelFormat.Format_U16,
np.int32:    VoxelFormat.Format_U32,
np.uint32:   VoxelFormat.Format_U32,
np.int64:    VoxelFormat.Format_U64,
np.uint64:   VoxelFormat.Format_U64,
np.float32:  VoxelFormat.Format_R32,
np.float64:  VoxelFormat.Format_R64,
}

def _ndarray(vec: Tuple[int]):
    arr = np.zeros((6), dtype=np.int32)
    arr[:len(vec)] = vec[:]
    return arr

class VolumeDataRequest(object):
  """Encapsulates volume data request.
  
  Returned by VolumeDataAccess.requestVolumeSubset()
  """
  def __init__(self, accessManager: openvds.core.VolumeDataAccessManager, layout: openvds.core.VolumeDataLayout, data_out: np.array = None, dimensionsND: DimensionsND = DimensionsND.Dimensions_012, lod: int = 0, channel: int = 0, min: Tuple[int] = None, max: Tuple[int] = None, format: VoxelFormat = VoxelFormat.Format_R32, replacementNoValue = None):
    array_interface = None
    if hasattr(data_out, "__cuda_array_interface__"):
      array_interface = data_out.__cuda_array_interface__
    elif hasattr(data_out, "__array_interface__"):
      array_interface = data_out.__array_interface__
    elif not data_out is None:
      raise TypeError("data_out: Invalid type: Only numpy.ndarray and numba.cuda.devicearray are supported")
    if array_interface:
      dtype = np.dtype(array_interface["typestr"])
      ptr, readonly = array_interface["data"]
      if readonly:
        raise ValueError("array is read-only")
      format = NumpyDataBlockTypes[dtype.type]
    if min is None or max is None:
        raise TypeError("The shape of the VolumeDataRequest must be specified.")
    self.dimensionsND       = dimensionsND
    self.lod                = lod
    self.channel            = channel
    self.min                = _ndarray(min)
    self.max                = _ndarray(max)
    self.format             = format
    self.replacementNoValue = replacementNoValue
    self.buffer_format      = DataBlockNumpyTypes[format]
    self._data_out           = data_out
    self._accessManager      = accessManager
    self._layout            = layout
    self._iscanceled        = False
    self._iscompleted       = False
    if self._data_out is None:
      self._data_out = np.zeros(self._accessManager.getVolumeSubsetBufferSize(self._layout, self.min, self.max, format, lod), dtype=self.buffer_format)
    if self.replacementNoValue is None:
        self.requestID = self._accessManager.requestVolumeSubset(self._data_out, self._layout, self.dimensionsND, self.lod, self.channel, self.min, self.max, self.format)
    else:
        self.requestID = self._accessManager.requestVolumeSubset(self._data_out, self._layout, self.dimensionsND, self.lod, self.channel, self.min, self.max, self.format, self.replacementNoValue)
        
  @property                      
  def isCompleted(self):
    """Has request completed successfully"""
    if self._iscompleted:
      return True
    if self._iscanceled:
      return False
    self._iscompleted = True if self._accessManager.isCompleted(self.requestID) else False
    return self._iscompleted
    
  @property
  def isCanceled(self):
    """Has request been canceled"""
    if self._iscanceled:
      return True
    if self._iscompleted:
      return False
    self._iscanceled = True if self._accessManager.isCanceled(self.requestID) else False
    return self._iscanceled
    
  @property
  def data(self):
    """The requested data. Will call `waitForCompletion()` if request has not completed"""
    if self.isCanceled:
      raise RuntimeError("Request was canceled. Data not available")
    if not self.isCompleted:
      self.waitForCompletion()
    return self._data_out
    
  def cancel(self):
    """Cancel request"""
    if self.isCompleted:
      raise RuntimeError("Operation already completed")
    if not self.isCanceled:
      self._accessManager.Cancel(self.requestID)
    
  def waitForCompletion(self, timeout = 0.0):
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
    if self.isCanceled:
      raise RuntimeError("Operation already canceled")
    if not self.isCompleted:
      nMillisecondsBeforeTimeout = int(timeout * 1000)
      self._iscompleted = True if accessManager.waitForCompletion(self.requestID, nMillisecondsBeforeTimeout) else False
    return self.isCompleted  

class VolumeDataAccess(object):
  """Interface class for VDS data access.
  
  This class has functions for making asynchronous data requests on a VDS object using numpy arrays or cuda devicearrays.

  Parameters:
  -----------
  handle : vds handle
      The VDS to interface with.
  """
  def __init__(self, handle: int):
    self.handle         = handle
    self.accessManager  = openvds.core.getAccessManager(handle)
    self.layout         = VolumeDataLayout(handle)

  def requestVolumeSubset(self, min: Tuple[int]=None, max: Tuple[int] = None, shape: Tuple[int]=None, data_out: np.ndarray = None, dimensionsND: DimensionsND = DimensionsND.Dimensions_012, lod: int = 0, channel: int = 0, format: VoxelFormat = VoxelFormat.Format_R32, replacementNoValue: float = None):
    """Request a subset of the VDS data.
          
    Parameters
    ----------
    min : tuple, optional
        Specifies the minimum voxel coordinates of the request (inclusive)
    max : tuple, optional
        Specifies the maximum voxel coordinates of the request (exclusive)
    shape : tuple, optional
        Specifies the shape/extent of the request. 
    min: tuple, optional
        Specifies the minimu
    data_out : numpy.ndarray or numba.cuda.devicearray, optional
        If specified, the data requested is copied to this array. Otherwise, a suitable numpy array is allocated.
    dimensionsND : hue.DimensionsND, optional
        If specified, determine the dimensiongroup requested. Defaults to DimensionGroup012
    lod : int, optional
        Which LOD level to request. Defaults to 0
    channel : int, optional
        Channel index. Defaults to 0.
    format : VoxelFormat, optional
        Specifies the format of the delivered data: Bytes, floats, doubles. etc.
        If the `data_out` array is specified, the format is determined from the array.
    replacementNoValue: float, optional
        If specified, NoValue data in the dataset is replaced with this value.

    Returns
    -------
    request : VolumeDataRequest
        An object encapsulating the request, the request state, and the requested data.
    """
    min = min or (0,) * self.layout.dimensions
    if shape is None and data_out is not None:
        shape = data_out.shape
    if max is None:
        if shape is None:
            raise TypeError("You must supply either 'max', 'shape' or 'data_out' parameters")
        max = tuple([min[x]+shape[x] for x in range(len(min))])
    return VolumeDataRequest(
      self.accessManager,
      self.layout._layout,
      data_out              = data_out,
      dimensionsND          = dimensionsND,
      lod                   = lod,
      channel               = channel,
      min                   = min,
      max                   = max,
      format                = format,
      replacementNoValue    = replacementNoValue)
