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

/**
 * A class that contains axis and channel information for a VDS A layout is
 * associated with a VDS object and contains information about axis and channels
 * in the VDS. A VolumeDataLayout can be invalidated when the VDS is
 * invalidated.
 * <p>
 * JNI wrapper for C++ OpenVDS::VolumeDataLayout class.
 */
public class VolumeDataLayout extends MetadataReadAccess {

    private static native long cpGetContentsHash(long handle);

    private static native int cpGetDimensionality(long handle);

    private static native int cpGetChannelCount(long handle);

    private static native boolean cpIsChannelAvailable(long handle, String channelName);

    private static native int cpGetChannelIndex(long handle, String channelName);

    private static native int[] cpGetLayoutDescriptor(long handle);

    private static native VolumeDataChannelDescriptor cpGetChannelDescriptor(long handle, int channel);

    private static native VolumeDataAxisDescriptor cpGetAxisDescriptor(long handle, int dimension);

    private static native int cpGetChannelFormat(long handle, int channel);

    private static native int cpGetChannelComponents(long handle, int channel);

    private static native String cpGetChannelName(long handle, int channel);

    private static native String cpGetChannelUnit(long handle, int channel);

    private static native float cpGetChannelValueRangeMin(long handle, int channel);

    private static native float cpGetChannelValueRangeMax(long handle, int channel);

    private static native boolean cpIsChannelDiscrete(long handle, int channel);

    private static native boolean cpIsChannelRenderable(long handle, int channel);

    private static native boolean cpIsChannelAllowingLossyCompression(long handle, int channel);

    private static native boolean cpIsChannelUseZipForLosslessCompression(long handle, int channel);

    private static native long cpGetChannelMapping(long handle, int channel);

    private static native int cpGetDimensionNumSamples(long handle, int dimension);

    private static native String cpGetDimensionName(long handle, int dimension);

    private static native String cpGetDimensionUnit(long handle, int dimension);

    private static native float cpGetDimensionMin(long handle, int dimension);

    private static native float cpGetDimensionMax(long handle, int dimension);

    private static native boolean cpIsChannelUseNoValue(long handle, int channel);

    private static native float cpGetChannelNoValue(long handle, int channel);

    private static native float cpGetChannelIntegerScale(long handle, int channel);

    private static native float cpGetChannelIntegerOffset(long handle, int channel);

    public VolumeDataLayout(long handle) {
        super(handle);
    }

    /**
     * @return the descriptor for the layout
     */
    public VolumeDataLayoutDescriptor getLayoutDescriptor() {
        return new VolumeDataLayoutDescriptor(cpGetLayoutDescriptor(_handle));
    }

    /**
     * @return the contents hash of this VDS
     */
    public long getContentsHash() {
        return cpGetContentsHash(_handle);
    }

    /**
     * @return the number of dimensions in this VDS
     * @see Dimensionality
     */
    public Dimensionality getDimensionality() {
        final int nbDimension = cpGetDimensionality(_handle);
        return Dimensionality.fromNbDimension(nbDimension);
    }

    /**
     * @return the number of channels in this VDS
     */
    public int getChannelCount() {
        return cpGetChannelCount(_handle);
    }

    /**
     * @param channelName channel name
     * @return true if the VDS contains channel with the given name
     */
    public boolean isChannelAvailable(String channelName) {
        return cpIsChannelAvailable(_handle, channelName);
    }

    /**
     * @param channelName channel name
     * @return the index of the channel with the given name
     */
    public int getChannelIndex(String channelName) {
        return cpGetChannelIndex(_handle, channelName);
    }

    /**
     * @param channel channel index
     * @return the descriptor for the given channel
     */
    public VolumeDataChannelDescriptor getChannelDescriptor(int channel) {
        return cpGetChannelDescriptor(_handle, channel);
    }

    /**
     * @param channel channel index
     * @return the format for the given channel
     * @see org.opengroup.openvds.VolumeDataChannelDescriptor.Format
     */
    public VolumeDataChannelDescriptor.Format getChannelFormat(int channel) {
        final int format = cpGetChannelFormat(_handle, channel);
        return VolumeDataChannelDescriptor.Format.fromCode(format);
    }

    /**
     * @param channel channel index
     * @return the vector count for the given channel
     * @see org.opengroup.openvds.VolumeDataChannelDescriptor.Components
     */
    public int getChannelComponents(int channel) {
        return cpGetChannelComponents(_handle, channel);
    }

    /**
     * @param channel channel index
     * @return the name for the given channel
     */
    public String getChannelName(int channel) {
        return cpGetChannelName(_handle, channel);
    }

    /**
     * @param channel channel index
     * @return the unit for the given channel
     */
    public String getChannelUnit(int channel) {
        return cpGetChannelUnit(_handle, channel);
    }

    /**
     * @param channel channel index
     * @return the value range minimum for the given channel
     */
    public float getChannelValueRangeMin(int channel) {
        return cpGetChannelValueRangeMin(_handle, channel);
    }

    /**
     * @param channel channel index
     * @return the value range maximum for the given channel
     */
    public float getChannelValueRangeMax(int channel) {
        return cpGetChannelValueRangeMax(_handle, channel);
    }

    /**
     * @param channel channel index
     * @return the discrete flag for the the given channel
     */
    public boolean isChannelDiscrete(int channel) {
        return cpIsChannelDiscrete(_handle, channel);
    }

    /**
     * @param channel channel index
     * @return the renderable flag for the given channel index
     */
    public boolean isChannelRenderable(int channel) {
        return cpIsChannelRenderable(_handle, channel);
    }

    /**
     * @param channel channel index
     * @return the allow lossy compression flag for the given channel
     */
    public boolean isChannelAllowingLossyCompression(int channel) {
        return cpIsChannelAllowingLossyCompression(_handle, channel);
    }

    /**
     * @param channel channel index
     * @return the use Zip when compressing flag for the given channel
     */
    public boolean isChannelUseZipForLosslessCompression(int channel) {
        return cpIsChannelUseZipForLosslessCompression(_handle, channel);
    }

    /**
     * @param channel channel index
     * @return the mapping for the given channel
     */
    public VolumeDataMapping getChannelMapping(int channel) {
        final long channelMapp = cpGetChannelMapping(_handle, channel);
        return VolumeDataMapping.fromCode(channelMapp);
    }

    /**
     * @param channel channel index
     * @return true if the given channel uses No Value
     */
    public boolean isChannelUseNoValue(int channel) {
        return cpIsChannelUseNoValue(_handle, channel);
    }

    /**
     * @param channel channel index
     * @return the No Value for the given channel
     */
    public float getChannelNoValue(int channel) {
        return cpGetChannelNoValue(_handle, channel);
    }

    /**
     * @param channel channel index
     * @return the integer scale for the given channel
     */
    public float getChannelIntegerScale(int channel) {
        return cpGetChannelIntegerScale(_handle, channel);
    }

    /**
     * @param channel channel index
     * @return the integer offset for the given channel
     */
    public float getChannelIntegerOffset(int channel) {
        return cpGetChannelIntegerOffset(_handle, channel);
    }

    /**
     * @param dimension dimension, @see org.opengroup.openvds.Dimensionality
     * @return the axis descriptor for the given dimension
     */
    public VolumeDataAxisDescriptor getAxisDescriptor(int dimension) {
        return cpGetAxisDescriptor(_handle, dimension);
    }

    /**
     * @param dimension dimension, @see org.opengroup.openvds.Dimensionality
     * @return the number of samples for the given dimension
     */
    public int getDimensionNumSamples(int dimension) {
        return cpGetDimensionNumSamples(_handle, dimension);
    }

    /**
     * @param dimension dimension, @see org.opengroup.openvds.Dimensionality
     * @return the name for the given dimension
     */
    public String getDimensionName(int dimension) {
        return cpGetDimensionName(_handle, dimension);
    }

    /**
     * @param dimension dimension, @see org.opengroup.openvds.Dimensionality
     * @return the unit for the given dimension
     */
    public String getDimensionUnit(int dimension) {
        return cpGetDimensionUnit(_handle, dimension);
    }

    /**
     * @param dimension dimension, @see org.opengroup.openvds.Dimensionality
     * @return the coordinate minimum for the given dimension
     */
    public float getDimensionMin(int dimension) {
        return cpGetDimensionMin(_handle, dimension);
    }

    /**
     * @param dimension dimension, @see org.opengroup.openvds.Dimensionality
     * @return the coordinate maximum for the given dimension
     */
    public float getDimensionMax(int dimension) {
        return cpGetDimensionMax(_handle, dimension);
    }
}
