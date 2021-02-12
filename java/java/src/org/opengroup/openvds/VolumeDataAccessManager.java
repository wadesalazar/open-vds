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

import java.nio.*;

import static org.opengroup.openvds.VolumeDataChannelDescriptor.Format.*;

/**
 * JNI wrapper for OpenVDS::VolumeDataAccessManager class.
 */
public class VolumeDataAccessManager extends JniPointerWithoutDeletion {

    private static native long cpGetVolumeDataLayout(long handle);

    private static native int cpGetVDSProduceStatus(long managerHandle,
                                                    int dimensionsND, int lod, int channel);

    private static native long cpCreateVolumeDataPageAccessor(long managerHandle,
                                                              int dimensionsND, int lod, int channel,
                                                              int maxPages, int accessMode);

    private static native long cpCreateVolumeDataPageAccessor(long managerHandle,
                                                              int dimensionsND, int lod, int channel,
                                                              int maxPages, int accessMode, int chunkMetadataPageSize);

    private static native void cpDestroyVolumeDataPageAccessor(long managerHandle, long pageAccessorHandle);

    private static native void cpDestroyVolumeDataAccessor(long managerHandle, long dataAccessorHandle);

    private static native long cpCloneVolumeDataAccessor(long managerHandle, long dataAccessorHandle);

    private static native long cpGetVolumeSubsetBufferSize(long managerHandle,
            int[] minVoxelCoordinates, int[] maxVoxelCoordinates, int format, int lod, int channel);

    private static native long cpRequestVolumeSubset(long managerHandle, Buffer outBuf, long bufferSize,
                                                     int dimensionsND, int lod, int channel,
                                                     int[] minVoxelCoordinates, int[] maxVoxelCoordinates, int formatCode);

    private static native long cpRequestVolumeSubsetR(long managerHandle, Buffer outBuf, long bufferSize,
                                                      int dimensionsND, int lod, int channel,
                                                      int[] minVoxelCoordinates, int[] maxVoxelCoordinates, int formatCode,
                                                      float replacementValue);

    private static native long cpGetProjectedVolumeSubsetBufferSize(long managerHandle,
                                                                    int[] minVoxelCoordinates, int[] maxVoxelCoordinates, int projectedDimensions, int format, int lod, int channel);

    private static native long cpRequestProjectedVolumeSubset(long handle, FloatBuffer outBuf, long bufferSize,
                                                              int dimensionsND, int lod, int channel,
                                                              int[] minVoxelCoordinates, int[] maxVoxelCoordinates, float voxelPlane0, float voxelPlane1, float voxelPlane2, float voxelPlane3,
                                                              int projectedDimensions, int format, int interpolation);

    private static native long cpRequestProjectedVolumeSubsetR(long handle, FloatBuffer outBuf, long bufferSize,
                                                               int dimensionsND, int lod, int channel,
                                                               int[] minVoxelCoordinates, int[] maxVoxelCoordinates, float voxelPlane0, float voxelPlane1, float voxelPlane2, float voxelPlane3,
                                                               int projectedDimensions, int format, int interpolation, float replacementNoValue);

    private static native long cpGetVolumeSamplesBufferSize(long managerHandle,
                                                            int sampleCount, int channel);

    private static native long cpRequestVolumeSamples(long managerHandle, FloatBuffer bufHandle, long bufferSize,
                                                      int dimensionsND, int lod, int channel,
                                                      FloatBuffer samplePositions, int sampleCount, int interpolationMethod);

    private static native long cpRequestVolumeSamplesR(long managerHandle, FloatBuffer outBufHandle, long bufferSize,
                                                       int dimensionsND, int lod, int channel,
                                                       FloatBuffer samplePositions, int sampleCount, int interpolationMethod, float replacementNoValue);

    private static native long cpGetVolumeTracesBufferSize(long managerHandle,
                                                           int traceCount, int traceDimension, int lod, int channel);

    private static native long cpRequestVolumeTraces(long managerHandle, FloatBuffer outBuf, long bufferSize,
                                                     int dimensionsND, int lod, int channel,
                                                     FloatBuffer tracePositions, int traceCount, int interpolationMethod, int traceDimension);

    private static native long cpRequestVolumeTraces(long managerHandle, FloatBuffer outBuf, long bufferSize,
                                                     int dimensionsND, int lod, int channel,
                                                     FloatBuffer tracePositions, int traceCount, int interpolationMethod, int traceDimension,
                                                     float replacementNoValue);

    private static native long cpPrefetchVolumeChunk(long managerHandle,
                                                     int dimensionsND, int lod, int channel,
                                                     long chunk);

    private static native boolean cpIsCompleted(long handle, long requestID);

    private static native boolean cpIsCanceled(long handle, long requestID);

    private static native boolean cpWaitForCompletion(long handle, long requestID, int millisecondsBeforeTimeout);

    private static native void cpCancel(long handle, long requestID);

    private static native float cpGetCompletionFactor(long handle, long requestID);

    private static native void cpFlushUploadQueue(long handle);

    private static native void cpFlushUploadQueue(long handle, boolean writeUpdatedLayerStatus);

    private static native void cpClearUploadErrors(long handle);

    private static native void cpForceClearAllUploadErrors(long handle);

    private static native int cpUploadErrorCount(long handle);

    public VolumeDataAccessManager(long handle) {
        super(handle);
    }

    /**
     * the maximum number of dimensions a VDS can have
     */
    public static final int Dimensionality_Max = VolumeDataLayout.Dimensionality_Max;

    /**
     * Get the VolumeDataLayout object for a VDS.
     *
     * @return the VolumeDataLayout object associated with the VDS or null if
     * there is no valid VolumeDataLayout.
     */
    VolumeDataLayout getVolumeDataLayout() {
        return new VolumeDataLayout(cpGetVolumeDataLayout(_handle));
    }

    /**
     * Get the produce status for the specific DimensionsND/LOD/Channel
     * combination.
     *
     * @param dimensionsND the dimensions group we're getting the produce status
     * for.
     * @param lod the LOD level we're getting the produce status for.
     * @param channel the channel index we're getting the produce status for.
     * @return The produce status for the specific DimensionsND/LOD/Channel
     * combination.
     */
    VDSProduceStatus getVDSProduceStatus(DimensionsND dimensionsND, int lod, int channel) {
        return VDSProduceStatus.values()[cpGetVDSProduceStatus(_handle, dimensionsND.ordinal(), lod, channel)];
    }

    /**
     * Create a volume data page accessor object for the VDS associated with the
     * given VolumeDataLayout object.
     *
     * @param dimensionsND the dimensions group that the volume data page
     * accessor will access.
     * @param lod the LOD level that the volume data page accessor will access.
     * @param channel the channel index that the volume data page accessor will
     * access.
     * @param maxPages the maximum number of pages that the volume data page
     * accessor will cache.
     * @param accessMode this specifies the access mode
     * (ReadOnly/ReadWrite/Create) of the volume data page accessor.
     * @return a VolumeDataPageAccessor object for the VDS associated with the
     * given VolumeDataLayout object.
     */
    VolumeDataPageAccessor createVolumeDataPageAccessor(int dimensionsND, int lod, int channel, int maxPages, int accessMode) {
        return new VolumeDataPageAccessor(cpCreateVolumeDataPageAccessor(_handle, dimensionsND, lod, channel, maxPages, accessMode));
    }

    /**
     * Create a volume data page accessor object for the VDS associated with the
     * given VolumeDataLayout object.
     *
     * @param dimensionsND the dimensions group that the volume data page
     * accessor will access.
     * @param lod the LOD level that the volume data page accessor will access.
     * @param channel the channel index that the volume data page accessor will
     * access.
     * @param maxPages the maximum number of pages that the volume data page
     * accessor will cache.
     * @param accessMode this specifies the access mode
     * (ReadOnly/ReadWrite/Create) of the volume data page accessor.
     * @param chunkMetadataPageSize the chunk metadata page size of the layer. This controls how many chunk metadata entries are written per page, and is only used when the access mode is Create.
     * If this number is too low it will degrade performance, but in certain situations it can be advantageous to make this number a multiple
     * of the number of chunks in some of the dimensions. Do not change this from the default (1024) unless you know exactly what you are doing.
     * @return a VolumeDataPageAccessor object for the VDS associated with the
     * given VolumeDataLayout object.
     */
    VolumeDataPageAccessor createVolumeDataPageAccessor(int dimensionsND, int lod, int channel, int maxPages, int accessMode, int chunkMetadataPageSize) {
        return new VolumeDataPageAccessor(cpCreateVolumeDataPageAccessor(_handle, dimensionsND, lod, channel, maxPages, accessMode, chunkMetadataPageSize));
    }

    /**
     * Destroy a volume data page accessor object.
     *
     * @param volumeDataPageAccessor the VolumeDataPageAccessor object to
     * destroy.
     */
    void destroyVolumeDataPageAccessor(VolumeDataPageAccessor volumeDataPageAccessor) {
        cpDestroyVolumeDataPageAccessor(_handle, volumeDataPageAccessor.handle());
    }

    /**
     * Destroy a volume data accessor object.
     *
     * @param accessor the VolumeDataAccessor object to destroy.
     */
    void destroyVolumeDataAccessor(VolumeDataAccessor accessor) {
        cpDestroyVolumeDataAccessor(_handle, accessor.handle());
    }

    /**
     * Clone a volume data accessor object.
     *
     * @param accessor the VolumeDataAccessor object to clone.
     * @return a clone of supplied VolumeDataAccessor object
     */
    VolumeDataAccessor cloneVolumeDataAccessor(VolumeDataAccessor accessor) {
        return new VolumeDataAccessor(cpCloneVolumeDataAccessor(_handle, accessor.handle()), true);
    }

    /**
     * Compute the buffer size (in bytes) for a volume subset request.
     *
     * @param box the box describing the volume subset coordinates.
     * @param format voxel format of the destination buffer.
     * @param lod the LOD level the requested data is read from.
     * @param channel the channel index the requested data is read from.
     * @return the buffer size needed
     */
    public long getVolumeSubsetBufferSize(NDBox box, VolumeDataChannelDescriptor.Format format, int lod, int channel) {
        return cpGetVolumeSubsetBufferSize(_handle, box.getMin(), box.getMax(), format.getCode(), lod, channel);
    }

    /**
     * Request a subset of the input VDS.
     *
     * @param outBuf preallocated buffer holding at least as many elements of
     * format as indicated by minVoxelCoordinates and maxVoxelCoordinates.
     * @param dimensionsND the dimensiongroup the requested data is read from.
     * @param lod the LOD level the requested data is read from.
     * @param channel the channel index the requested data is read from.
     * @param box the box describing the volume subset coordinates.
     * @return the requestID which can be used to query the status of the
     * request, cancel the request or wait for the request to complete.
     */
    public long requestVolumeSubset(DoubleBuffer outBuf,
                                    DimensionsND dimensionsND, int lod, int channel,
                                    NDBox box) {
        B.checkDirectBuffer(outBuf);
        if (B.getCapacityInBytes(outBuf) < getVolumeSubsetBufferSize(box, FORMAT_R64, lod, channel))
            throwBufferTooSmallException();
        return cpRequestVolumeSubset(_handle, outBuf, B.getCapacityInBytes(outBuf),
                dimensionsND.ordinal(), lod, channel,
                box.getMin(), box.getMax(), FORMAT_R64.getCode());
    }
    public long requestVolumeSubset(FloatBuffer outBuf,
                                    DimensionsND dimensionsND, int lod, int channel,
                                    NDBox box) {
        B.checkDirectBuffer(outBuf);
        if (B.getCapacityInBytes(outBuf) < getVolumeSubsetBufferSize(box, FORMAT_R32, lod, channel))
            throwBufferTooSmallException();
        return cpRequestVolumeSubset(_handle, outBuf, B.getCapacityInBytes(outBuf),
                dimensionsND.ordinal(), lod, channel,
                box.getMin(), box.getMax(), FORMAT_R32.getCode());
    }
    public long requestVolumeSubset(ByteBuffer outBuf,
                                    DimensionsND dimensionsND, int lod, int channel,
                                    NDBox box) {
        B.checkDirectBuffer(outBuf);
        if (B.getCapacityInBytes(outBuf) < getVolumeSubsetBufferSize(box, FORMAT_U8, lod, channel))
            throwBufferTooSmallException();
        return cpRequestVolumeSubset(_handle, outBuf, B.getCapacityInBytes(outBuf),
                dimensionsND.ordinal(), lod, channel,
                box.getMin(), box.getMax(), FORMAT_U8.getCode());
    }
    public long requestVolumeSubset(IntBuffer outBuf,
                                    DimensionsND dimensionsND, int lod, int channel,
                                    NDBox box) {
        B.checkDirectBuffer(outBuf);
        if (B.getCapacityInBytes(outBuf) < getVolumeSubsetBufferSize(box, FORMAT_U32, lod, channel))
            throwBufferTooSmallException();
        return cpRequestVolumeSubset(_handle, outBuf, B.getCapacityInBytes(outBuf),
                dimensionsND.ordinal(), lod, channel,
                box.getMin(), box.getMax(), FORMAT_U32.getCode());
    }

    /**
     * Request a subset of the input VDS.
     *
     * @param outBuf preallocated buffer holding at least as many elements of
     * format as indicated by minVoxelCoordinates and maxVoxelCoordinates.
     * @param dimensionsND the dimensiongroup the requested data is read from.
     * @param lod the LOD level the requested data is read from.
     * @param channel the channel index the requested data is read from.
     * @param box the box describing the volume subset coordinates.
     * @param replacementNoValue value used to replace region of the input VDS
     * that has no data.
     * @return the requestID which can be used to query the status of the
     * request, cancel the request or wait for the request to complete.
     */
    public long requestVolumeSubset(DoubleBuffer outBuf,
                                    DimensionsND dimensionsND, int lod, int channel,
                                    NDBox box,
                                    float replacementNoValue) {
        B.checkDirectBuffer(outBuf);
        if (B.getCapacityInBytes(outBuf) < getVolumeSubsetBufferSize(box, FORMAT_R64, lod, channel))
            throwBufferTooSmallException();
        return cpRequestVolumeSubsetR(_handle, outBuf, B.getCapacityInBytes(outBuf),
                dimensionsND.ordinal(), lod, channel,
                box.getMin(), box.getMax(), FORMAT_R64.getCode(), replacementNoValue);
    }
    public long requestVolumeSubset(FloatBuffer outBuf,
                                    DimensionsND dimensionsND, int lod, int channel,
                                    NDBox box,
                                    float replacementNoValue) {
        B.checkDirectBuffer(outBuf);
        if (B.getCapacityInBytes(outBuf) < getVolumeSubsetBufferSize(box, FORMAT_R32, lod, channel))
            throwBufferTooSmallException();
        return cpRequestVolumeSubsetR(_handle, outBuf, B.getCapacityInBytes(outBuf),
                dimensionsND.ordinal(), lod, channel,
                box.getMin(), box.getMax(), FORMAT_R32.getCode(), replacementNoValue);
    }
    public long requestVolumeSubset(ByteBuffer outBuf,
                                    DimensionsND dimensionsND, int lod, int channel,
                                    NDBox box,
                                    float replacementNoValue) {
        B.checkDirectBuffer(outBuf);
        if (B.getCapacityInBytes(outBuf)< getVolumeSubsetBufferSize(box, FORMAT_U8, lod, channel))
            throwBufferTooSmallException();
        return cpRequestVolumeSubsetR(_handle, outBuf, B.getCapacityInBytes(outBuf),
                dimensionsND.ordinal(), lod, channel,
                box.getMin(), box.getMax(), FORMAT_U8.getCode(), replacementNoValue);
    }
    public long requestVolumeSubset(IntBuffer outBuf,
                                    DimensionsND dimensionsND, int lod, int channel,
                                    NDBox box,
                                    float replacementNoValue) {
        B.checkDirectBuffer(outBuf);
        if (B.getCapacityInBytes(outBuf) < getVolumeSubsetBufferSize(box, FORMAT_U32, lod, channel))
            throwBufferTooSmallException();
        return cpRequestVolumeSubsetR(_handle, outBuf, B.getCapacityInBytes(outBuf),
                dimensionsND.ordinal(), lod, channel,
                box.getMin(), box.getMax(), FORMAT_U32.getCode(), replacementNoValue);
    }

    /**
     * Compute the buffer size (in bytes) for a projected volume subset request.
     *
     * @param projectedDimensions the 2D dimension group that the plane in the
     * source dimensiongroup is projected into. It must be a 2D subset of the
     * source dimensions.
     * @param format voxel format of the destination buffer.
     * @param lod the LOD level the requested data is read from.
     * @param channel the channel index the requested data is read from.
     * @return the buffer size needed
     */
    long getProjectedVolumeSubsetBufferSize(NDBox box, DimensionsND projectedDimensions,
                                            VolumeDataChannelDescriptor.Format format, int lod, int channel) {
        return cpGetProjectedVolumeSubsetBufferSize(_handle,
                box.getMin(), box.getMax(), projectedDimensions.ordinal(), format.getCode(), lod, channel);
    }

    /**
     * Request a subset projected from an arbitrary 3D plane through the subset
     * onto one of the sides of the subset.
     *
     * @param outBuf preallocated buffer holding at least as many elements of
     * format as indicated by minVoxelCoordinates and maxVoxelCoordinates for
     * the projected dimensions.
     * @param dimensionsND the dimensiongroup the requested data is read from.
     * @param lod the LOD level the requested data is read from.
     * @param channel the channel index the requested data is read from.
     * @param voxelPlane0 1st component of the plane equation for the projection from the
     * dimension source to the projected dimensions (which must be a 2D subset
     * of the source dimensions).
     * @param voxelPlane1 2nd component of the plane equation for the projection from the
     * dimension source to the projected dimensions (which must be a 2D subset
     * of the source dimensions).
     * @param voxelPlane2 3rd component of the plane equation for the projection from the
     * dimension source to the projected dimensions (which must be a 2D subset
     * of the source dimensions).
     * @param voxelPlane3 4th component of the plane equation for the projection from the
     * dimension source to the projected dimensions (which must be a 2D subset
     * of the source dimensions).
     * @param projectedDimensions the 2D dimension group that the plane in the
     * source dimensiongroup is projected into. It must be a 2D subset of the
     * source dimensions.
     * @param interpolationMethod Interpolation method to use when sampling the
     * buffer.
     * @return the requestID which can be used to query the status of the
     * request, cancel the request or wait for the request to complete.
     */
    long requestProjectedVolumeSubset(FloatBuffer outBuf,
                                      DimensionsND dimensionsND, int lod, int channel,
                                      NDBox box, float voxelPlane0, float voxelPlane1, float voxelPlane2, float voxelPlane3,
                                      DimensionsND projectedDimensions, InterpolationMethod interpolationMethod) {
        return cpRequestProjectedVolumeSubset(_handle, outBuf, B.getCapacityInBytes(outBuf),
                dimensionsND.ordinal(), lod, channel,
                box.getMin(), box.getMax(), voxelPlane0, voxelPlane1, voxelPlane2, voxelPlane3,
                projectedDimensions.ordinal(), FORMAT_R32.getCode(), interpolationMethod.ordinal());
    }

    /**
     * Request a subset projected from an arbitrary 3D plane through the subset
     * onto one of the sides of the subset.
     *
     * @param outBuf preallocated buffer holding at least as many elements of
     * format as indicated by minVoxelCoordinates and maxVoxelCoordinates for
     * the projected dimensions.
     * @param dimensionsND the dimensiongroup the requested data is read from.
     * @param lod the LOD level the requested data is read from.
     * @param channel the channel index the requested data is read from.
     * @param voxelPlane0 1st component of the plane equation for the projection from the
     * dimension source to the projected dimensions (which must be a 2D subset
     * of the source dimensions).
     * @param voxelPlane1 2nd component of the plane equation for the projection from the
     * dimension source to the projected dimensions (which must be a 2D subset
     * of the source dimensions).
     * @param voxelPlane2 3rd component of the plane equation for the projection from the
     * dimension source to the projected dimensions (which must be a 2D subset
     * of the source dimensions).
     * @param voxelPlane3 4th component of the plane equation for the projection from the
     * dimension source to the projected dimensions (which must be a 2D subset
     * of the source dimensions).
     * @param projectedDimensions the 2D dimension group that the plane in the
     * source dimensiongroup is projected into. It must be a 2D subset of the
     * source dimensions.
     * @param interpolationMethod Interpolation method to use when sampling the
     * buffer.
     * @param replacementNoValue value used to replace region of the input VDS
     * that has no data.
     * @return the requestID which can be used to query the status of the
     * request, cancel the request or wait for the request to complete.
     */
    long requestProjectedVolumeSubset(FloatBuffer outBuf,
                                      DimensionsND dimensionsND, int lod, int channel,
                                      NDBox box, float voxelPlane0, float voxelPlane1, float voxelPlane2, float voxelPlane3,
                                      DimensionsND projectedDimensions, InterpolationMethod interpolationMethod, float replacementNoValue) {
        return cpRequestProjectedVolumeSubsetR(_handle, outBuf, B.getCapacityInBytes(outBuf),
                dimensionsND.ordinal(), lod, channel,
                box.getMin(), box.getMax(), voxelPlane0, voxelPlane1, voxelPlane2, voxelPlane3,
                projectedDimensions.ordinal(), FORMAT_R32.getCode(), interpolationMethod.ordinal(), replacementNoValue);
    }

    /**
     * Compute the buffer size (in bytes) for a volume samples request.
     *
     * @param sampleCount number of samples to request.
     * @param channel the channel index the requested data is read from.
     * @return
     */
    public long getVolumeSamplesBufferSize(int sampleCount, int channel) {
        return cpGetVolumeSamplesBufferSize(_handle, sampleCount, channel);
    }

    /**
     * Request sampling of the input VDS at the specified coordinates.
     *
     * @param outBuf preallocated buffer holding at least sampleCount elements.
     * @param dimensiongroup the dimensiongroup the requested data is read from.
     * @param lod the LOD level the requested data is read from.
     * @param channel the channel index the requested data is read from.
     * @param samplePositions array containing groups of
     * Dimensionality_Max elements indicating the positions to
     * sample. May be deleted once requestVolumeSamples return, as OpenVDS makes
     * a deep copy of the data.
     * @param sampleCount number of samples to request.
     * @param interpolationMethod interpolation method to use when sampling the
     * buffer.
     * @return the requestID which can be used to query the status of the
     * request, cancel the request or wait for the request to complete.
     */
    public long requestVolumeSamples(FloatBuffer outBuf, DimensionsND dimensiongroup,
            int lod, int channel, FloatBuffer samplePositions, int sampleCount, InterpolationMethod interpolationMethod) {
        B.checkDirectBuffer(outBuf);
        B.checkDirectBuffer(samplePositions);
        return cpRequestVolumeSamples(_handle, outBuf, B.getCapacityInBytes(outBuf), dimensiongroup.ordinal(), lod, channel,
                samplePositions, sampleCount, interpolationMethod.ordinal());
    }

    /**
     * Request sampling of the input VDS at the specified coordinates.
     *
     * @param outBuf preallocated buffer holding at least sampleCount elements.
     * @param dimensiongroup the dimensiongroup the requested data is read from.
     * @param lod the LOD level the requested data is read from.
     * @param channel the channel index the requested data is read from.
     * @param samplePositions array containing groups of
     * Dimensionality_Max elements indicating the positions to
     * sample. May be deleted once requestVolumeSamples return, as OpenVDS makes
     * a deep copy of the data.
     * @param sampleCount number of samples to request.
     * @param interpolationMethod interpolation method to use when sampling the
     * buffer.
     * @param replacementNoValue value used to replace region of the input VDS
     * that has no data.
     * @return the requestID which can be used to query the status of the
     * request, cancel the request or wait for the request to complete.
     */
    public long requestVolumeSamples(FloatBuffer outBuf, DimensionsND dimensiongroup,
                                     int lod, int channel, FloatBuffer samplePositions, int sampleCount,
                                     InterpolationMethod interpolationMethod, float replacementNoValue) {
        B.checkDirectBuffer(outBuf);
        B.checkDirectBuffer(samplePositions);
        if (B.getCapacityInBytes(outBuf) < getVolumeSamplesBufferSize(sampleCount, channel)) {
            return throwBufferTooSmallException();
        }
        return cpRequestVolumeSamplesR(_handle, outBuf, B.getCapacityInBytes(outBuf), dimensiongroup.ordinal(), lod, channel,
                samplePositions, sampleCount, interpolationMethod.ordinal(), replacementNoValue);
    }

    /**
     * Compute the buffer size (in bytes) for a volume traces request.
     *
     * @param traceCount number of traces to request.
     * @param traceDimension the dimension to trace
     * @param lod the LOD level the requested data is read from.
     * @param channel the channel index the requested data is read from.
     * @return
     */
    public long getVolumeTracesBufferSize(int traceCount, int traceDimension, int lod, int channel) {
        return cpGetVolumeTracesBufferSize(_handle, traceCount, traceDimension, lod, channel);
    }

    /**
     * Request traces from the input VDS.
     *
     * @param outBuf preallocated buffer holding at least traceCount * number of
     * samples in the traceDimension.
     * @param dimensionsND the dimensiongroup the requested data is read from.
     * @param lod the LOD level the requested data is read from.
     * @param channel the channel index the requested data is read from.
     * @param tracePositions array containing groups of
     * Dimensionality_Max elements indicating the trace
     * positions.
     * @param traceCount number of traces to request.
     * @param interpolationMethod interpolation method to use when sampling the
     * buffer.
     * @param traceDimension the dimension to trace
     * @return the requestID which can be used to query the status of the
     * request, cancel the request or wait for the request to complete.
     */
    public long requestVolumeTraces(FloatBuffer outBuf,
                                    DimensionsND dimensionsND, int lod, int channel,
                                    FloatBuffer tracePositions,
                                    int traceCount, InterpolationMethod interpolationMethod, int traceDimension) {
        B.checkDirectBuffer(outBuf);
        B.checkDirectBuffer(tracePositions);
        if (B.getCapacityInBytes(outBuf) < getVolumeTracesBufferSize(traceCount, traceDimension, lod, channel)) {
            return throwBufferTooSmallException();
        }
        return cpRequestVolumeTraces(_handle, outBuf, B.getCapacityInBytes(outBuf),
                dimensionsND.ordinal(), lod, channel,
                tracePositions,
                traceCount, interpolationMethod.ordinal(), traceDimension);
    }

    private long throwBufferTooSmallException() {
        throw new IllegalArgumentException("Buffer is too small");
    }

    /**
     * Request traces from the input VDS.
     *
     * @param outBuf preallocated buffer holding at least traceCount * number of
     * samples in the traceDimension.
     * @param dimensionsND the dimensiongroup the requested data is read from.
     * @param lod the LOD level the requested data is read from.
     * @param channel the channel index the requested data is read from.
     * @param tracePositions array containing groups of
     * Dimensionality_Max elements indicating the trace
     * positions.
     * @param traceCount number of traces to request.
     * @param interpolationMethod interpolation method to use when sampling the
     * buffer.
     * @param traceDimension the dimension to trace
     * @param replacementNoValue value used to replace region of the input VDS
     * that has no data.
     * @return the requestID which can be used to query the status of the
     * request, cancel the request or wait for the request to complete.
     */
    public long requestVolumeTraces(FloatBuffer outBuf,
                                    DimensionsND dimensionsND, int lod, int channel,
                                    FloatBuffer tracePositions,
                                    int traceCount, InterpolationMethod interpolationMethod, int traceDimension, float replacementNoValue) {
        B.checkDirectBuffer(outBuf);
        B.checkDirectBuffer(tracePositions);
        if (B.getCapacityInBytes(outBuf) < getVolumeTracesBufferSize(traceCount, traceDimension, lod, channel))
            throwBufferTooSmallException();
        return cpRequestVolumeTraces(_handle, outBuf, B.getCapacityInBytes(outBuf),
                dimensionsND.ordinal(), lod, channel,
                tracePositions,
                traceCount, interpolationMethod.ordinal(), traceDimension, replacementNoValue);
    }

    /**
     * Force production of a specific volume data chunk.
     *
     * @param dimensionsND the dimensiongroup the requested chunk belongs to.
     * @param lod the LOD level the requested chunk belongs to.
     * @param channel the channel index the requested chunk belongs to.
     * @param chunk the index of the chunk to prefetch.
     * @return the requestID which can be used to query the status of the
     * request, cancel the request or wait for the request to complete.
     */
    public long prefetchVolumeChunk(int dimensionsND,
            int lod, int channel, long chunk) {
        return cpPrefetchVolumeChunk(_handle,
                dimensionsND, lod, channel, chunk);
    }

    /**
     * Check if a request completed successfully. If the request completed, the
     * buffer now contains valid data.
     *
     * @param requestID the requestID to check for completion.
     * @return either IsCompleted, IsCanceled or WaitForCompletion will return
     * true a single time, after that the request is taken out of the system.
     */
    public boolean isCompleted(long requestID) {
        return cpIsCompleted(_handle, requestID);
    }

    /**
     * Check if a request was canceled (e.g. the VDS was invalidated before the
     * request was processed). If the request was canceled, the buffer does not
     * contain valid data.
     *
     * @param requestID the requestID to check for cancellation.
     * @return either IsCompleted, IsCanceled or WaitForCompletion will return
     * true a single time, after that the request is taken out of the system.
     */
    public boolean isCanceled(long requestID) {
        return cpIsCanceled(_handle, requestID);
    }

    /**
     * Wait for a request to complete successfully. If the request completed,
     * the buffer now contains valid data.
     *
     * @param requestID the requestID to wait for completion of.
     * @return Either IsCompleted, IsCanceled or WaitForCompletion will return
     * true a single time, after that the request is taken out of the system.
     * Whenever WaitForCompletion returns false you need to call IsCanceled() to
     * know if that was because of a timeout or if the request was canceled.
     */
    public boolean waitForCompletion(long requestID) {
        return waitForCompletion(requestID, 0);
    }

    /**
     * Wait for a request to complete successfully. If the request completed,
     * the buffer now contains valid data.
     *
     * @param requestID the requestID to wait for completion of.
     * @param millisecondsBeforeTimeout the number of milliseconds to wait
     * before timing out. A value of 0 indicates there is no timeout and we will
     * wait for however long it takes. Note that the request is not
     * automatically canceled if the wait times out, you can also use this
     * mechanism to e.g. update a progress bar while waiting. If you want to
     * cancel the request you have to explicitly call CancelRequest() and then
     * wait for the request to stop writing to the buffer.
     * @return either IsCompleted, IsCanceled or WaitForCompletion will return
     * true a single time, after that the request is taken out of the system.
     * Whenever WaitForCompletion returns false you need to call IsCanceled() to
     * know if that was because of a timeout or if the request was canceled.
     */
    public boolean waitForCompletion(long requestID, int millisecondsBeforeTimeout) {
        return cpWaitForCompletion(_handle, requestID, millisecondsBeforeTimeout);
    }

    /**
     * Try to cancel the request. You still have to call
     * WaitForCompletion/IsCanceled to make sure the buffer is not being written
     * to and to take the job out of the system. It is possible that the request
     * has completed concurrently with the call to Cancel in which case
     * WaitForCompletion will return true.
     *
     * @param requestID the requestID to cancel.
     */
    public void cancel(long requestID) {
        cpCancel(_handle, requestID);
    }

    /**
     * Get the completion factor (between 0 and 1) of the request.
     *
     * @param requestID the requestID to get the completion factor of.
     * @return a factor (between 0 and 1) indicating how much of the request has
     * been completed.
     */
    public float getCompletionFactor(long requestID) {
        return cpGetCompletionFactor(_handle, requestID);
    }

    /**
     * Flush any pending writes and write updated layer status.
     */
    public void flushUploadQueue() {
        cpFlushUploadQueue(_handle);
    }

    /**
     * Flush any pending writes and write updated layer status.
     *
     * @param writeUpdatedLayerStatus Write the updated layer status (or only flush pending writes of chunks and chunk-metadata).
     */
    public void flushUploadQueue(boolean writeUpdatedLayerStatus) {
        cpFlushUploadQueue(_handle, writeUpdatedLayerStatus);
    }

    public void clearUploadErrors() {
        cpClearUploadErrors(_handle);
    }

    public void forceClearAllUploadErrors() {
        cpForceClearAllUploadErrors(_handle);
    }

    public int uploadErrorCount() {
        return cpUploadErrorCount(_handle);
    }
}
