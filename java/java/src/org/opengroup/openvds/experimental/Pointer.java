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

package org.opengroup.openvds.experimental;

import java.nio.*;
import java.util.Arrays;


/**
 * A Java representation of a void pointer.
 */
public class Pointer extends NativePointerObject
{
    /**
     * The offset from the nativePointer, in bytes
     */
    private long byteOffset;

    /**
     * The buffer the pointer points to
     */
    private Buffer buffer;

    /**
     * The array of NativePointerObjects this pointer points to
     */
    private NativePointerObject pointers[];


    /**
     * Creates a new Pointer to the given values.
     *
     * @param values The values the pointer should point to
     * @return The pointer
     */
    public static Pointer to(byte values[])
    {
        return new Pointer(ByteBuffer.wrap(values));
    }

    /**
     * Creates a new Pointer to the given values.
     * The values may not be null.
     *
     * @param values The values the pointer should point to
     * @return The pointer
     */
    public static Pointer to(char values[])
    {
        return new Pointer(CharBuffer.wrap(values));
    }

    /**
     * Creates a new Pointer to the given values.
     * The values may not be null.
     *
     * @param values The values the pointer should point to
     * @return The pointer
     */
    public static Pointer to(short values[])
    {
        return new Pointer(ShortBuffer.wrap(values));
    }

    /**
     * Creates a new Pointer to the given values.
     * The values may not be null.
     *
     * @param values The values the pointer should point to
     * @return The pointer
     */
    public static Pointer to(int values[])
    {
        return new Pointer(IntBuffer.wrap(values));
    }

    /**
     * Creates a new Pointer to the given values.
     * The values may not be null.
     *
     * @param values The values the pointer should point to
     * @return The pointer
     */
    public static Pointer to(float values[])
    {
        return new Pointer(FloatBuffer.wrap(values));
    }

    /**
     * Creates a new Pointer to the given values.
     * The values may not be null.
     *
     * @param values The values the pointer should point to
     * @return The pointer
     */
    public static Pointer to(long values[])
    {
        return new Pointer(LongBuffer.wrap(values));
    }

    /**
     * Creates a new Pointer to the given values.
     * The values may not be null.
     *
     * @param values The values the pointer should point to
     * @return The pointer
     */
    public static Pointer to(double values[])
    {
        return new Pointer(DoubleBuffer.wrap(values));
    }

    /**
     * <b>NOTE:</b> This method does not take into account the position
     * and array offset of the given buffer. In order to create a
     * pointer that takes the position and array offset into account,
     * use the {@link #toBuffer(Buffer)} method. <br />
     * <br />
     *
     * If the given buffer has a backing array, then the returned
     * pointer will in any case point to the start of the array,
     * even if the buffer has been created using the <code>slice</code>
     * method (like {@link ByteBuffer#slice()}). If the buffer is
     * direct, then this method will return a Pointer to the address
     * of the direct buffer. If the buffer has been created using the
     * <code>slice</code> method, then this will be the actual start
     * of the slice. Although this implies a different treatment of
     * direct- and non direct buffers, the method is kept for
     * backward compatibility. <br />
     * <br />
     * In both cases, for direct and array-based buffers, this method
     * does not take into account the position of the given buffer. <br />
     * <br />
     * The buffer must not be null, and either be a direct buffer, or
     * have a backing array
     *
     * @param buffer The buffer the pointer should point to
     * @return The pointer
     * @throws IllegalArgumentException If the given buffer
     * is null or is neither direct nor has a backing array
     */
    public static Pointer to(Buffer buffer)
    {
        if (buffer == null || (!buffer.isDirect() && !buffer.hasArray()))
        {
            throw new IllegalArgumentException(
                "Buffer may not be null and must have an array or be direct");
        }
        return new Pointer(buffer);
    }

    /**
     * Creates a new Pointer to the given buffer.<br />
     * <br />
     * Note that this method takes into account the array offset and position
     * of the given buffer, in contrast to the {@link #to(Buffer)} method.
     *
     * @param buffer The buffer
     * @return The new pointer
     * @throws IllegalArgumentException If the given buffer
     * is null or is neither direct nor has a backing array
     */
    public static Pointer toBuffer(Buffer buffer)
    {
        if (buffer == null || (!buffer.isDirect() && !buffer.hasArray()))
        {
            throw new IllegalArgumentException(
                "Buffer may not be null and must have an array or be direct");
        }
        if (buffer instanceof ByteBuffer)
        {
            return computePointer((ByteBuffer)buffer);
        }
        if (buffer instanceof ShortBuffer)
        {
            return computePointer((ShortBuffer)buffer);
        }
        if (buffer instanceof IntBuffer)
        {
            return computePointer((IntBuffer)buffer);
        }
        if (buffer instanceof LongBuffer)
        {
            return computePointer((LongBuffer)buffer);
        }
        if (buffer instanceof FloatBuffer)
        {
            return computePointer((FloatBuffer)buffer);
        }
        if (buffer instanceof DoubleBuffer)
        {
            return computePointer((DoubleBuffer)buffer);
        }
        throw new IllegalArgumentException(
            "Unknown buffer type: "+buffer);

    }

    /**
     * Creates a new Pointer to the given buffer, taking into
     * account the position and array offset of the given buffer.
     * The buffer is assumed to be non-<code>null</code>, and
     * be either direct or have a backing array.
     *
     * @param buffer The buffer
     * @return The pointer
     */
    private static Pointer computePointer(ByteBuffer buffer)
    {
        Pointer result = null;
        if (buffer.isDirect())
        {
            int oldPosition = buffer.position();
            buffer.position(0);
            result = Pointer.to(buffer.slice()).withByteOffset(
                oldPosition * Sizeof.BYTE);
            buffer.position(oldPosition);
        }
        else if (buffer.hasArray())
        {
            ByteBuffer t = ByteBuffer.wrap(buffer.array());
            int elementOffset = buffer.position() + buffer.arrayOffset();
            result = Pointer.to(t).withByteOffset(
                elementOffset * Sizeof.BYTE);
        }
        return result;
    }


    /**
     * Creates a new Pointer to the given buffer, taking into
     * account the position and array offset of the given buffer.
     * The buffer is assumed to be non-<code>null</code>, and
     * be either direct or have a backing array.
     *
     * @param buffer The buffer
     * @return The pointer
     */
    private static Pointer computePointer(ShortBuffer buffer)
    {
        Pointer result = null;
        if (buffer.isDirect())
        {
            int oldPosition = buffer.position();
            buffer.position(0);
            result = Pointer.to(buffer.slice()).withByteOffset(
                oldPosition * Sizeof.SHORT);
            buffer.position(oldPosition);
        }
        else if (buffer.hasArray())
        {
            ShortBuffer t = ShortBuffer.wrap(buffer.array());
            int elementOffset = buffer.position() + buffer.arrayOffset();
            result = Pointer.to(t).withByteOffset(
                elementOffset * Sizeof.SHORT);
        }
        return result;
    }


    /**
     * Creates a new Pointer to the given buffer, taking into
     * account the position and array offset of the given buffer.
     * The buffer is assumed to be non-<code>null</code>, and
     * be either direct or have a backing array.
     *
     * @param buffer The buffer
     * @return The pointer
     */
    private static Pointer computePointer(IntBuffer buffer)
    {
        Pointer result = null;
        if (buffer.isDirect())
        {
            int oldPosition = buffer.position();
            buffer.position(0);
            result = Pointer.to(buffer.slice()).withByteOffset(
                oldPosition * Sizeof.INT);
            buffer.position(oldPosition);
        }
        else if (buffer.hasArray())
        {
            IntBuffer t = IntBuffer.wrap(buffer.array());
            int elementOffset = buffer.position() + buffer.arrayOffset();
            result = Pointer.to(t).withByteOffset(
                elementOffset * Sizeof.INT);
        }
        return result;
    }


    /**
     * Creates a new Pointer to the given buffer, taking into
     * account the position and array offset of the given buffer.
     * The buffer is assumed to be non-<code>null</code>, and
     * be either direct or have a backing array.
     *
     * @param buffer The buffer
     * @return The pointer
     */
    private static Pointer computePointer(LongBuffer buffer)
    {
        Pointer result = null;
        if (buffer.isDirect())
        {
            int oldPosition = buffer.position();
            buffer.position(0);
            result = Pointer.to(buffer.slice()).withByteOffset(
                oldPosition * Sizeof.LONG);
            buffer.position(oldPosition);
        }
        else if (buffer.hasArray())
        {
            LongBuffer t = LongBuffer.wrap(buffer.array());
            int elementOffset = buffer.position() + buffer.arrayOffset();
            result = Pointer.to(t).withByteOffset(
                elementOffset * Sizeof.LONG);
        }
        return result;
    }


    /**
     * Creates a new Pointer to the given buffer, taking into
     * account the position and array offset of the given buffer.
     * The buffer is assumed to be non-<code>null</code>, and
     * be either direct or have a backing array.
     *
     * @param buffer The buffer
     * @return The pointer
     */
    private static Pointer computePointer(FloatBuffer buffer)
    {
        Pointer result = null;
        if (buffer.isDirect())
        {
            int oldPosition = buffer.position();
            buffer.position(0);
            result = Pointer.to(buffer.slice()).withByteOffset(
                oldPosition * Sizeof.FLOAT);
            buffer.position(oldPosition);
        }
        else if (buffer.hasArray())
        {
            FloatBuffer t = FloatBuffer.wrap(buffer.array());
            int elementOffset = buffer.position() + buffer.arrayOffset();
            result = Pointer.to(t).withByteOffset(
                elementOffset * Sizeof.FLOAT);
        }
        return result;
    }


    /**
     * Creates a new Pointer to the given buffer, taking into
     * account the position and array offset of the given buffer.
     * The buffer is assumed to be non-<code>null</code>, and
     * be either direct or have a backing array.
     *
     * @param buffer The buffer
     * @return The pointer
     */
    private static Pointer computePointer(DoubleBuffer buffer)
    {
        Pointer result = null;
        if (buffer.isDirect())
        {
            int oldPosition = buffer.position();
            buffer.position(0);
            result = Pointer.to(buffer.slice()).withByteOffset(
                oldPosition * Sizeof.DOUBLE);
            buffer.position(oldPosition);
        }
        else if (buffer.hasArray())
        {
            DoubleBuffer t = DoubleBuffer.wrap(buffer.array());
            int elementOffset = buffer.position() + buffer.arrayOffset();
            result = Pointer.to(t).withByteOffset(
                elementOffset * Sizeof.DOUBLE);
        }
        return result;
    }

    /**
     * Creates a new Pointer to the given Pointers. The array
     * of pointers may not be <code>null</code>, and may not
     * contain <code>null</code> elements.
     *
     * @param pointers The pointers the pointer should point to
     * @return The new pointer
     * @throws IllegalArgumentException If the given array
     * is null
     */
    public static Pointer to(NativePointerObject ... pointers)
    {
        if (pointers == null)
        {
            throw new IllegalArgumentException(
                "Pointer may not point to null objects");
        }
        return new Pointer(pointers);
    }



    /**
     * Creates a new (<code>null</code>) Pointer
     */
    public Pointer()
    {
        buffer = null;
        pointers = null;
        byteOffset = 0;
    }

    /**
     * Constructor which accepts a constant value as the pointer value.
     * Solely for the constant pointer instances in the JCudaDriver
     * class, to create the CU_LAUNCH_PARAM_END,
     * CU_LAUNCH_PARAM_BUFFER_POINTER, and CU_LAUNCH_PARAM_BUFFER_SIZE
     * pointer constants.
     *
     * @param nativePointerValue The native pointer value
     */
    protected Pointer(long nativePointerValue)
    {
        super(nativePointerValue);
        buffer = null;
        pointers = null;
        byteOffset = 0;
    }

    /**
     * Creates a Pointer to the given Buffer
     *
     * @param buffer The buffer to point to
     */
    private Pointer(Buffer buffer)
    {
        this.buffer = buffer;
        pointers = null;
        byteOffset = 0;
    }

    /**
     * Creates a Pointer to the given array of pointers
     *
     * @param pointers The array the pointer points to
     */
    private Pointer(NativePointerObject pointers[])
    {
        buffer = null;
        this.pointers = pointers;
        byteOffset = 0;
    }

    /**
     * Copy constructor
     *
     * @param other The other Pointer
     */
    protected Pointer(Pointer other)
    {
        super(other.getNativePointer());
        this.buffer = other.buffer;
        this.pointers = other.pointers;
        this.byteOffset = other.byteOffset;
    }

    /**
     * Creates a copy of the given pointer, with an
     * additional byte offset
     *
     * @param other The other pointer
     * @param byteOffset The additional byte offset
     */
    protected Pointer(Pointer other, long byteOffset)
    {
        this(other);
        this.byteOffset += byteOffset;
    }

    /**
     * Returns a new pointer with an offset of the given number
     * of bytes
     *
     * @param byteOffset The byte offset for the pointer
     * @return The new pointer with the given byte offset
     */
    public Pointer withByteOffset(long byteOffset)
    {
        return new Pointer(this, byteOffset);
    }


    /**
     * Returns a ByteBuffer that corresponds to the specified
     * segment of the memory that this pointer points to.<br>
     * <br>
     * The returned byte buffer will have the byte order that is implied
     * by <code>ByteOrder#nativeOrder()</code>. It will be a slice of the
     * buffer that is stored internally. So it will share the same memory,
     * but its position and limit will be independent of the internal buffer.
     * <br>
     * This function may only be applied to pointers that have been set to
     * point to a region of host- or unified memory using one of these
     * methods:
     * <ul>
     *   <li>{@link jcuda.driver.JCudaDriver#cuMemAllocHost}</li>
     *   <li>{@link jcuda.driver.JCudaDriver#cuMemHostAlloc}</li>
     *   <li>{@link jcuda.driver.JCudaDriver#cuMemAllocManaged}</li>
     *   <li>{@link jcuda.runtime.JCuda#cudaMallocHost}</li>
     *   <li>{@link jcuda.runtime.JCuda#cudaHostAlloc}</li>
     *   <li>{@link jcuda.runtime.JCuda#cudaMallocManaged}</li>
     *   <li>{@link Pointer#to(byte[])}</li>
     * </ul>
     * <br>
     * For other pointer types, <code>null</code> is returned.
     *
     * @param byteOffset The offset in bytes
     * @param byteSize The size of the byte buffer, in bytes
     * @return The byte buffer
     * @throws IllegalArgumentException If the <code>byteOffset</code> and
     * <code>byteSize</code> describe an invalid memory range (for example,
     * when the <code>byteOffset</code> is negative)
     * @throws ArithmeticException If the <code>byteOffset</code> or
     * <code>byteOffset + byteSize</code> overflows an <code>int</code>.
     */
    public ByteBuffer getByteBuffer(long byteOffset, long byteSize)
    {
        if (buffer == null)
        {
            return null;
        }
        if (!(buffer instanceof ByteBuffer))
        {
            return null;
        }
        ByteBuffer internalByteBuffer = (ByteBuffer)buffer;
        ByteBuffer byteBuffer = internalByteBuffer.slice();
        byteBuffer.limit(Math.toIntExact(byteOffset + byteSize));
        byteBuffer.position(Math.toIntExact(byteOffset));
        return byteBuffer.slice().order(ByteOrder.nativeOrder());
    }

    /**
     * Returns a ByteBuffer that corresponds to the memory that this
     * pointer points to.<br>
     * <br>
     * The returned byte buffer will have the byte order that is implied
     * by <code>ByteOrder#nativeOrder()</code>. It will be a slice of the
     * buffer that is stored internally. So it will share the same memory,
     * but its position and limit will be independent of the internal buffer.
     * <br>
     * This function may only be applied to pointers that have been set to
     * point to a region of host- or unified memory using one of these
     * methods:
     * <ul>
     *   <li>{@link jcuda.driver.JCudaDriver#cuMemAllocHost}</li>
     *   <li>{@link jcuda.driver.JCudaDriver#cuMemHostAlloc}</li>
     *   <li>{@link jcuda.driver.JCudaDriver#cuMemAllocManaged}</li>
     *   <li>{@link jcuda.runtime.JCuda#cudaMallocHost}</li>
     *   <li>{@link jcuda.runtime.JCuda#cudaHostAlloc}</li>
     *   <li>{@link jcuda.runtime.JCuda#cudaMallocManaged}</li>
     *   <li>{@link Pointer#to(byte[])}</li>
     * </ul>
     * <br>
     * For other pointer types, <code>null</code> is returned.
     *
     * @return The byte buffer
     */
    public ByteBuffer getByteBuffer()
    {
        if (buffer == null)
        {
            return null;
        }
        if (!(buffer instanceof ByteBuffer))
        {
            return null;
        }
        ByteBuffer internalByteBuffer = (ByteBuffer)buffer;
        ByteBuffer byteBuffer = internalByteBuffer.slice();
        return byteBuffer.order(ByteOrder.nativeOrder());
    }


    /**
     * Returns the byte offset
     *
     * @return The byte offset
     */
    protected long getByteOffset()
    {
        return byteOffset;
    }

    /**
     * Returns a String representation of this object.
     *
     * @return A String representation of this object.
     */
    @Override
    public String toString()
    {
        if (buffer != null)
        {
            return "Pointer["+
                "buffer="+buffer+","+
                "byteOffset="+byteOffset+"]";

        }
        else if (pointers != null)
        {
            return "Pointer["+
                "pointers="+Arrays.toString(pointers)+","+
                "byteOffset="+byteOffset+"]";
        }
        else
        {
            return "Pointer["+
                "nativePointer=0x"+Long.toHexString(getNativePointer())+","+
                "byteOffset="+byteOffset+"]";
        }
    }
}