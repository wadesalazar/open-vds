import openvds.core
import numpy as np

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

NumpyDataBlockTypes = i = { DataBlockNumpyTypes[k]: k for k in DataBlockNumpyTypes }

def minmax(shape: Tuple[int], pos: Tuple[int] = None):
    min = np.zeros((6), dtype=np.int32)
    max = np.zeros((6), dtype=np.int32)
    if pos is None:
        pos = (0,) * len(shape)
    min[:len(pos)]   = pos[:]
    max[:len(shape)] = shape[:]
    max += min
    return min, max

class VolumeDataRequest(object):
  """Encapsulates volume data request.
  
  Returned by VolumeDataAccess.requestVolumeSubset()
  """
  def __init__(self, accessManager: openvds.core.VolumeDataAccessManager, layout: openvds.core.VolumeDataLayout, data_out: np.array = None, dimensionsND: DimensionsND = DimensionsND.Dimensions_012, lod: int = 0, channel: int = 0, pos: Tuple[int] = None, shape: Tuple[int] = None, format: VoxelFormat = VoxelFormat.Format_R32, replacementNoValue = None):
    self.data_out_ptr = 0
    array_interface = None
    if hasattr(data_out, "__cuda_array_interface__"):
      array_interface = data_out.__cuda_array_interface__
    elif hasattr(data_out, "__array_interface__"):
      array_interface = data_out.__array_interface__
    elif not data_out is None:
      raise TypeError("data_out: Invalid type: Only numpy.ndarray and numba.cuda.devicearray are supported")
    if array_interface:
      shape = array_interface["shape"]
      dtype = np.dtype(array_interface["typestr"])
      ptr, readonly = array_interface["data"]
      if readonly:
        raise ValueError("array is read-only")
      format = NumpyDataBlockTypes[dtype.type]
      self.data_out_ptr     = ptr
    assert shape
    self.dimensionsND       = dimensionsND
    self.lod                = lod
    self.channel            = channel
    self.min, self.max      = minmax(shape, pos)
    self.data_out           = data_out
    self.format             = format
    self.replacementNoValue = replacementNoValue
    self.buffer_format      = DataBlockNumpyTypes[format]
    self.accessManager      = accessManager
    self.layout             = layout
    self._iscanceled        = False
    self._iscompleted       = False
    if self.data_out is None:
      self.data_out = np.zeros(self.accessManager.getVolumeSubsetBufferSize(self.layout, self.min, self.max, format, lod), dtype=self.buffer_format)
      self.data_out_ptr = self.data_out.ctypes.data
    if self.replacementNoValue is None:
        self.requestID = self.accessManager.requestVolumeSubset(self.data_out, self.layout, self.dimensionsND, self.lod, self.channel, self.min, self.max, self.format)
    else:
        self.requestID = self.accessManager.requestVolumeSubset(self.data_out, self.layout, self.dimensionsND, self.lod, self.channel, self.min, self.max, self.format, self.replacementNoValue)
        
  @property                      
  def isCompleted(self):
    """Has request completed successfully"""
    if self._iscompleted:
      return True
    if self._iscanceled:
      return False
    self._iscompleted = True if accessManager.isCompleted(self.requestID) else False
    return self._iscompleted
    
  @property
  def isCanceled(self):
    """Has request been canceled"""
    if self._iscanceled:
      return True
    if self._iscompleted:
      return False
    self._iscanceled = True if accessManager.isCanceled(self.requestID) else False
    return self._iscanceled
    
  @property
  def data(self):
    """The requested data. Will call `waitForCompletion()` if request has not completed"""
    if self.isCanceled:
      raise RuntimeError("Request was canceled. Data not available")
    if not self.isCompleted:
      self.waitForCompletion()
    return self.data_out
    
  def cancel(self):
    """Cancel request"""
    if self.isCompleted:
      raise RuntimeError("Operation already completed")
    if not self.isCanceled:
      accessManager.Cancel(self.requestID)
    
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
    self.layout         = openvds.core.getLayout(handle)

  def requestVolumeSubset(self, data_out = None, dimensionsND: DimensionsND = DimensionsND.Dimensions_012, lod: int = 0, channel: int = 0, shape: tuple = None, pos: tuple = None, format: VoxelFormat = VoxelFormat.Format_R32, replacementNoValue: float = None):
    """Request a subset of the VDS data.

     The subset can be specified in the following ways:
          1. Specify a preallocated numpy array, a cuda devicearray (numba.cuda.devicearray), or a cuda pinnedarray in the `data_out` parameter. 
          The extent/shape of the data request is then derived from the array shape. The offset into the data volume is then taken from the
          `pos` parameter, or if if this is not specified, assumed to be at the origin, e.g.(0,0,0).
          2. Specify `shape` and optionally `pos`
          3. If no shape can be determined because neither `data_out`, nor `shape` is specified, the whole volume is requested.
          
    Parameters
    ----------
    data_out : numpy.ndarray or numba.cuda.devicearray, optional
        If specified, the data requested is copied to this array. Otherwise, a suitable numpy array is allocated.
    dimensionsND : hue.DimensionsND, optional
        If specified, determine the dimensiongroup requested. Defaults to DimensionGroup012
    lod : int, optional
        Which LOD level to request. Defaults to 0
    channel : int, optional
        Channel index. Defaults to 0.
    shape : tuple, optional
        Specifies the shape/extent of the request. Defaults to None.
    pos : tuple, optional
        Specifies the offset into the volume. Defaults to None, which is interpreted as
        at the origin, e.g (0,0,0).
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
    return VolumeDataRequest(
      self.accessManager,
      self.layout,
      data_out              = data_out,
      dimensionsND          = dimensionsND,
      lod                   = lod,
      channel               = channel,
      shape                 = shape,
      pos                   = pos,
      format                = format,
      replacementNoValue    = replacementNoValue)
