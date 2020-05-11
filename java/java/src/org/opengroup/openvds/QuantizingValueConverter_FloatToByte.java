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
 * JNI wrapper for QuantizingValueConverterWithNoValue<uint8_t, float, false>.
 */
public class QuantizingValueConverter_FloatToByte extends JniPointer {

    private static native long cpCreateHandle(float valueRangeMin, float valueRangeMax,
            float integerScale, float integerOffset);

    private static native void cpDeleteHandle(long handle);

    private static native byte cpConvertValue(long handle, float val);

    public QuantizingValueConverter_FloatToByte(float valueRangeMin, float valueRangeMax,
            float integerScale, float integerOffset) {
        super(cpCreateHandle(valueRangeMin, valueRangeMax,
                integerScale, integerOffset), true);
    }

    // Called by JniPointer.release()
    @Override
    protected synchronized void deleteHandle() {
        cpDeleteHandle(_handle);
    }

    public byte convertValue(float val) {
        return cpConvertValue(_handle, val);
    }
}
