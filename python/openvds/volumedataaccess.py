import openvds.core
import numpy as np
from openvds.core import VolumeDataLayout

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
    """Convert a tuple of integers to a 6D integer vector as a numpy array"""
    arr = np.ones((6), dtype=np.int32)
    arr[:len(vec)] = vec[:]
    return arr

def _ndarrayarray(tuples: List[Tuple[float]]):
    """Convert a list of float tuples to a numpy array of 6D vectors"""
    l = len(tuples)
    arr = np.ones((l, 6), dtype=np.float32)
    for i in range(l):
        arr[i][:len(tuples[i])] = tuples[i]
    return arr
    
class VolumeDataRequest(object):
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
        self._data_out          = data_out
        self._accessManager     = accessManager
        self._layout            = layout
        self._iscanceled        = False
        self._iscompleted       = False

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
        return self._data_out.view(dtype=self.buffer_format)
        
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
            self._iscompleted = True if self._accessManager.waitForCompletion(self.requestID, nMillisecondsBeforeTimeout) else False
        return self.isCompleted  

class VolumeDataSubsetRequest(VolumeDataRequest):
    """Encapsulates volume data subset request.
    
    Returned by VolumeDataAccess.requestVolumeSubset()
    """
    def __init__(
        self, 
        accessManager: openvds.core.VolumeDataAccessManager, 
        layout: openvds.core.VolumeDataLayout, 
        data_out: np.array, 
        dimensionsND: DimensionsND, 
        lod: int,
        channel: int,
        min: Tuple[int], 
        max: Tuple[int], 
        format: VoxelFormat, 
        replacementNoValue
    ):
        super().__init__(accessManager, layout, data_out, dimensionsND, lod, channel, min, max, format, replacementNoValue)
        if self._data_out is None:
            self._data_out = np.zeros(self._accessManager.getVolumeSubsetBufferSize(self._layout, self.min, self.max, format, lod), dtype=np.int8)
        if self.replacementNoValue is None:
            self.requestID = self._accessManager.requestVolumeSubset(self._data_out, self._layout, self.dimensionsND, self.lod, self.channel, self.min, self.max, self.format)
        else:
            self.requestID = self._accessManager.requestVolumeSubset(self._data_out, self._layout, self.dimensionsND, self.lod, self.channel, self.min, self.max, self.format, self.replacementNoValue)

class ProjectedVolumeDataSubsetRequest(VolumeDataRequest):
    """Encapsulates a projected volume data subset request.
    
    Returned by VolumeDataAccess.requestProjectedVolumeSubset()
    """
    def __init__(
        self, 
        accessManager: openvds.core.VolumeDataAccessManager, 
        layout: openvds.core.VolumeDataLayout, 
        data_out: np.array, 
        dimensionsND: DimensionsND, 
        lod: int, 
        channel: int, 
        min: Tuple[int], 
        max: Tuple[int], 
        voxelPlane: Tuple[int], 
        projectedDimensions: DimensionsND, 
        interpolationMethod: InterpolationMethod, 
        format: VoxelFormat, 
        replacementNoValue
    ):
        super().__init__(accessManager, layout, data_out, dimensionsND, lod, channel, min, max, format, replacementNoValue)
        self.voxelPlane = voxelPlane
        self.projectedDimensions = projectedDimensions
        self.interpolationMethod = interpolationMethod
        if self._data_out is None:
            self._data_out = np.zeros(self._accessManager.getProjectedVolumeSubsetBufferSize(self._layout, self.min, self.max, projectedDimensions, format, lod), dtype=np.int8)
        if self.replacementNoValue is None:
            self.requestID = self._accessManager.requestProjectedVolumeSubset(self._data_out, self._layout, self.dimensionsND, self.lod, self.channel, self.min, self.max, voxelPlane, projectedDimensions, self.format, interpolationMethod)
        else:
            self.requestID = self._accessManager.requestProjectedVolumeSubset(self._data_out, self._layout, self.dimensionsND, self.lod, self.channel, self.min, self.max, voxelPlane, projectedDimensions, self.format, interpolationMethod, self.replacementNoValue)

class VolumeDataTracesRequest(VolumeDataRequest):
    """Encapsulates volume data traces request.
    
    Returned by VolumeDataAccess.requestVolumeTraces()
    """
    def __init__(
        self, 
        accessManager: openvds.core.VolumeDataAccessManager, 
        layout: openvds.core.VolumeDataLayout, 
        data_out: np.array, 
        dimensionsND: DimensionsND, 
        lod: int,
        channel: int,
        tracePositions: List[Tuple[float]],
        interpolationMethod: InterpolationMethod, 
        traceDimension: int,
        replacementNoValue
    ):
        self.tracePositions         = tracePositions
        self.interpolationMethod    = interpolationMethod
        self.traceCount             = len(tracePositions)
        self.traceDimension         = traceDimension
        min_, max_ = layout.shape
        super().__init__(accessManager, layout, data_out, dimensionsND, lod, channel, min_, max_, VoxelFormat.Format_R32, replacementNoValue)
        if self._data_out is None:
            self._data_out = np.zeros(self._accessManager.getVolumeTracesBufferSize(self._layout, traceCount, traceDimension, lod), dtype=np.int8)
        if self.replacementNoValue is None:
            self.requestID = self._accessManager.requestVolumeTraces(self._data_out, self._layout, self.dimensionsND, self.lod, self.channel, self.tracePositions, self.interpolationMethod, self.traceDimension)
        else:
            self.requestID = self._accessManager.requestVolumeTraces(self._data_out, self._layout, self.dimensionsND, self.lod, self.channel, self.tracePositions, self.interpolationMethod, self.traceDimension, self.replacementNoValue)

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
        self._accessManager  = openvds.core.getAccessManager(handle)
        self._layout        = openvds.core.getLayout(handle)

    @staticmethod
    def getMinMaxFromOffsetAndShape(offset: Tuple[int], shape: Tuple[int]):
        """Translate offset and shape vectors into voxel min, max coordinates""" 
        if len(offset) != len(offset):
            raise ValueError("offset and shape must be the same size")
        min = offset
        max = tuple([min[x]+shape[x] for x in range(len(min))])
        return min, max
    
    @property
    def manager(self):
        return self._accessManager
        
    @property
    def layout(self):
        return self._layout
        
    def requestVolumeSubset(
        self, 
        min: Tuple[int], 
        max: Tuple[int], 
        data_out: np.ndarray = None, 
        dimensionsND: DimensionsND = DimensionsND.Dimensions_012, 
        lod: int = 0, 
        channel: int = 0, 
        format: VoxelFormat = VoxelFormat.Format_R32, 
        replacementNoValue: float = None
        ):
        """Request a subset of the VDS data.
            
        Parameters
        ----------
        min : tuple
            Specifies the minimum voxel coordinates of the request (inclusive)
        max : tuple
            Specifies the maximum voxel coordinates of the request (exclusive)
        data_out : numpy.ndarray or numba.cuda.devicearray, optional
            If specified, the data requested is copied to this array. Otherwise, a suitable numpy array is allocated.
        dimensionsND : DimensionsND, optional
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
        request : VolumeDataSubsetRequest
            An object encapsulating the request, the request state, and the requested data.
        """
        return VolumeDataSubsetRequest(
            self._accessManager,
            self._layout,
            min                   = min,
            max                   = max,
            data_out              = data_out,
            dimensionsND          = dimensionsND,
            lod                   = lod,
            channel               = channel,
            format                = format,
            replacementNoValue    = replacementNoValue)

    def requestProjectedVolumeSubset(
        self, 
        min: Tuple[int], 
        max: Tuple[int], 
        data_out: np.ndarray = None, 
        dimensionsND: DimensionsND = DimensionsND.Dimensions_012, 
        lod: int = 0, 
        channel: int = 0, 
        voxelPlane: Tuple[int] = (1,1,0,0), 
        projectedDimensions: DimensionsND = DimensionsND.Dimensions_12, 
        interpolationMethod: InterpolationMethod = InterpolationMethod.Cubic, 
        format: VoxelFormat = VoxelFormat.Format_R32, 
        replacementNoValue: float = None
        ):
        """Request a subset projected from an arbitrary 3D plane through the
        subset onto one of the sides of the subset.

        Parameters
        ----------
        min : tuple
            Specifies the minimum voxel coordinates of the request (inclusive)
        max : tuple
            Specifies the maximum voxel coordinates of the request (exclusive)
        data_out : numpy.ndarray or numba.cuda.devicearray, optional
            If specified, the data requested is copied to this array. Otherwise, a suitable numpy array is allocated.
        dimensionsND : DimensionsND, optional
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
        voxelPlane : tuple
            A 4-tuple of floats representing the plane equation for the projection from the dimension source to
            the projected dimensions (which must be a 2D subset of the source dimensions).
        projectedDimensions : DimensionsND
            The 2D dimension group that the plane in the source dimensiongroup
            is projected into. It must be a 2D subset of the source
            dimensions.
        interpolationMethod : InterpolationMethod
            Interpolation method to use when sampling the buffer.
        
        Returns
        -------
        request : ProjectedVolumeDataSubsetRequest
            An object encapsulating the request, the request state, and the requested data.
        """
        return ProjectedVolumeDataSubsetRequest(
            self._accessManager,
            self._layout,
            min                   = min,
            max                   = max,
            data_out              = data_out,
            dimensionsND          = dimensionsND,
            lod                   = lod,
            channel               = channel,
            voxelPlane            = voxelPlane,
            projectedDimensions   = projectedDimensions,
            interpolationMethod   = interpolationMethod,
            format                = format,
            replacementNoValue    = replacementNoValue)
        
    def requestVolumeTraces(
        self, 
        tracePositions: List[Tuple[float]],
        traceDimension: int,
        data_out: np.ndarray = None, 
        dimensionsND: DimensionsND = DimensionsND.Dimensions_012, 
        lod: int = 0, 
        channel: int = 0, 
        interpolationMethod: InterpolationMethod = InterpolationMethod.Cubic, 
        replacementNoValue: float = None
        ):
        """Request traces from the input VDS.
        
        Parameters:
        -----------
        
        tracePositions :
            A list of trace positions.
        
        traceDimension :
            The dimension to trace
        
        data_out : optional
            A preallocated numpy array of float32 holding at least len(tracePositions) *
            number of samples in the traceDimension.
        
        dimensionsND : optional, defaults to Dimensions_012
            The dimensiongroup the requested data is read from.
        
        lod : optional, defaults to 0
            The LOD level the requested data is read from.
           
        channel : optional, defaults to 0
            The channel index the requested data is read from.
        
        interpolationMethod : optional, defaults to Cubic
            Interpolation method to use when sampling the buffer.
        
        replacementNoValue : optional
            Value used to replace region of the input VDS that has no data.
        
        Returns:
        --------
            The requestID which can be used to query the status of the
            request, cancel the request or wait for the request to complete
        """
        return VolumeDataTracesRequest(
            self,
            self._accessManager,
            self._layout,
            tracePositions      = tracePositions,
            traceDimension      = traceDimension,
            data_out            = data_out,
            dimensionsND        = dimensionsND,
            lod                 = lod,
            channel             = channel,
            interpolationMethod = interpolationMethod,
            replacementNoValue  = replacementNoValue)

    