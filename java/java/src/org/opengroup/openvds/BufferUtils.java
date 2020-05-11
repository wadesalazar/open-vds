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

import java.nio.Buffer;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.DoubleBuffer;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import java.nio.ShortBuffer;
import java.util.Objects;

public abstract class BufferUtils {
    /**
     * @param array Th array to be copied into the new buffer
     * @return A direct buffer copy of the given array
     */
    public static FloatBuffer toBuffer(float[] array) {
        return (FloatBuffer) createFloatBuffer(array.length).put(array).clear();
    }

    /**
     * @param array Th array to be copied into the new buffer
     * @return A direct buffer copy of the given array
     */
    public static IntBuffer toBuffer(int[] array) {
        return (IntBuffer) createIntBuffer(array.length).put(array).clear();
    }

    /**
     * @param capacity The number of byte
     * @return A ready to use direct byte buffer
     */
    public static ByteBuffer createBuffer(int capacity) {
        return ByteBuffer.allocateDirect(capacity).order(ByteOrder.nativeOrder());
    }

    /**
     * @param capacity The number of float
     * @return A ready to use direct float buffer
     */
    public static ByteBuffer createByteBuffer(int capacity) {
        return BufferUtils.createBuffer(capacity * Byte.BYTES);
    }

    /**
     * @param capacity The number of float
     * @return A ready to use direct float buffer
     */
    public static FloatBuffer createFloatBuffer(int capacity) {
        return BufferUtils.createBuffer(capacity * Float.BYTES).asFloatBuffer();
    }

    /**
     * @param capacity The number of int
     * @return A ready to use direct int buffer
     */
    public static IntBuffer createIntBuffer(int capacity) {
        return BufferUtils.createBuffer(capacity * Integer.BYTES).asIntBuffer();
    }

    /**
     * @param capacity The number of double
     * @return A ready to use direct double buffer
     */
    public static DoubleBuffer createDoubleBuffer(int capacity) {
        return BufferUtils.createBuffer(capacity * Double.BYTES).asDoubleBuffer();
    }

    /**
     * @param capacity The number of short
     * @return A ready to use direct short buffer
     */
    public static ShortBuffer createShortBuffer(int capacity) {
        return BufferUtils.createBuffer(capacity * Short.BYTES).asShortBuffer();
    }

    /**
     * @param samplesBuffer The buffer to be copied into a new array
     * @return An array copy of the given buffer
     */
    public static float[] toArray(FloatBuffer samplesBuffer, float[] dst, boolean releaseBuffer) {
        samplesBuffer.clear();
        if (dst == null) dst = new float[samplesBuffer.capacity()];
        samplesBuffer.get(dst);
        if (releaseBuffer) release(samplesBuffer);
        return dst;
    }

    /**
     * @param samplesBuffer The buffer to be copied into a new array
     * @return An array copy of the given buffer
     */
    public static int[] toArray(IntBuffer samplesBuffer, int[] dst, boolean releaseBuffer) {
        samplesBuffer.clear();
        if (dst == null) dst = new int[samplesBuffer.capacity()];
        samplesBuffer.get(dst);
        if (releaseBuffer) release(samplesBuffer);
        return dst;
    }

    /**
     * @param samplesBuffer The buffer to be copied into a new array
     * @param dst           The destination array
     * @return An array copy of the given buffer
     */
    public static byte[] toArray(ByteBuffer samplesBuffer, byte[] dst, boolean releaseBuffer) {
        samplesBuffer.clear();
        if (dst == null) dst = new byte[samplesBuffer.capacity()];
        samplesBuffer.get(dst);
        if (releaseBuffer) release(samplesBuffer);
        return dst;
    }

    /**
     * @param samplesBuffer The buffer to be copied into a new array
     * @return A new array filled with content of the given buffer
     */
    public static float[] toArray(FloatBuffer samplesBuffer) {
        return toArray(samplesBuffer, null, false);
    }

    /**
     * @param samplesBuffer The buffer to be copied into a new array
     * @return A new array filled with content of the given buffer
     */
    public static int[] toArray(IntBuffer samplesBuffer) {
        return toArray(samplesBuffer, null, false);
    }

    /**
     * @param samplesBuffer The buffer to be copied into a new array
     * @return A new array filled with content of the given buffer
     */
    public static byte[] toArray(ByteBuffer samplesBuffer) {
        return toArray(samplesBuffer, null, false);
    }

    /**
     * @param samplesBuffer The buffer to be copied into a new array
     * @return A new array filled with content of the given buffer. Buffer is released afterward.
     * @see #release(Buffer)
     */
    public static float[] toArrayAndRelease(FloatBuffer samplesBuffer) {
        return toArray(samplesBuffer, new float[samplesBuffer.capacity()], true);
    }

    /**
     * @param samplesBuffer The buffer to be copied into a new array
     * @return A new array filled with content of the given buffer. Buffer is released afterward.
     * @see #release(Buffer)
     */
    public static int[] toArrayAndRelease(IntBuffer samplesBuffer) {
        return toArray(samplesBuffer, new int[samplesBuffer.capacity()], true);
    }

    /**
     * @param samplesBuffer The buffer to be copied into a new array
     * @return A new array filled with content of the given buffer. Buffer is released afterward.
     * @see #release(Buffer)
     */
    public static byte[] toArrayAndRelease(ByteBuffer samplesBuffer) {
        return toArray(samplesBuffer, new byte[samplesBuffer.capacity()], true);
    }

    /**
     * This methods tries to release resources backing the given buffer. This is a best effort methods meant to
     * manage all commonly allocated direct byte buffers depending on the JRE versions and platforms.
     * If the proper release methods fails this methods return false and buffer is to be considered still allocated.
     *
     * @param buffer The buffer to release resources
     * @return true if the appropriate release methods have been successfully executed. False otherwise.
     */
    public static boolean release(Buffer buffer) {
        return Cleaner.tryRelease(buffer);
    }

    /**
     * Cleans several buffer resources at once. Calls {@link #release(Buffer)} for each provide buffers.
     *
     * @param buffer the list of buffer
     * @return true if all the appropriate release methods have been successfully executed. False otherwise.
     */
    public static boolean release(Buffer... buffer) {
        boolean clean = true;
        for (Buffer b : buffer) {
            clean &= release(b);
        }
        return clean;
    }

    /**
     * Check if the provided buffer is direct
     *
     * @param buf to be checked
     * @throws IllegalArgumentException when buffer is not a usable direct buffer
     */
    public static void checkDirectBuffer(Buffer buf) {
        if (Objects.isNull(buf)) throw new IllegalArgumentException("buffer is null");
        if (!buf.isDirect())
            throw new IllegalArgumentException("buffer must be direct. Please Use BufferUtils methods.");
        if (buf.capacity() <= 0)
            throw new IllegalArgumentException("buffer has no valid capacity. It probably have bean release.");
    }

    /**
     * @param outBuf
     * @return The capacity in bytes of the provided buffer
     */
    public static int getCapacityInBytes(Buffer outBuf) {
        if (outBuf instanceof FloatBuffer)
            return outBuf.capacity() * Float.BYTES;
        else if (outBuf instanceof IntBuffer)
            return outBuf.capacity() * Integer.BYTES;
        else if (outBuf instanceof ByteBuffer)
            return outBuf.capacity() * Byte.BYTES;
        throw new IllegalArgumentException();
    }
}
