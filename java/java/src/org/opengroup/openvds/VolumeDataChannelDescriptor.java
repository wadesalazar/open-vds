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

import java.util.Arrays;
import java.util.Optional;

/**
 * Analog of OpenVDS::VolumeDataChannelDescriptor class.
 */
public class VolumeDataChannelDescriptor {

    public enum Format {
        /**
         * The DataBlock can be in any format
         */
        FORMAT_ANY(-1),
        /**
         * Data is in packed 1-bit format
         */
        FORMAT_1BIT(0),
        /**
         * Data is in unsigned 8 bit
         */
        FORMAT_U8(1),
        /**
         * Data is in unsigned 16 bit
         */
        FORMAT_U16(2),
        /**
         * Data is in 32 bit float
         */
        FORMAT_R32(3),
        /**
         * Data is in unsigned 32 bit
         */
        FORMAT_U32(4),
        /**
         * Data is in 64 bit double
         */
        FORMAT_R64(5),
        /**
         * Data is in unsigned 64 bit
         */
        FORMAT_U64(6);

        private final int code;

        Format(int code) {
            this.code = code;
        }

        public static Format fromCode(int format) {
            Optional<Format> first = Arrays
                    .stream(values())
                    .filter(e -> e.getCode() == format)
                    .findFirst();
            if (!first.isPresent())
                throw new IllegalArgumentException("invalid code");
            return first.get();
        }

        public int getCode() {
            return code;
        }
    }

    public enum Components {
        COMPONENTS_1(1),
        COMPONENTS_2(2),
        COMPONENTS_4(4);

        private final int code;

        Components(int code) {
            this.code = code;
        }

        public int getCode() {
            return code;
        }

        public static Components fromCode(int format) {
            Optional<Components> first = Arrays
                    .stream(values())
                    .filter(e -> e.getCode() == format)
                    .findFirst();
            if (!first.isPresent())
                throw new IllegalArgumentException("invalid code");
            return first.get();
        }
    }

    public int m_format;
    public int m_components;
    public String m_name;
    public String m_unit;
    public float m_valueRangeMin;
    public float m_valueRangeMax;
    public long m_mapping;
    public int m_mappedValueCount;

    // bits of m_flags
    public boolean m_isDiscrete;
    public boolean m_isRenderable;
    public boolean m_isAllowLossyCompression;
    public boolean m_isUseZipForLosslessCompression;

    public boolean m_useNoValue;
    public float m_noValue;
    public float m_integerScale;
    public float m_integerOffset;

    public int getFormat() {
        return m_format;
    }

    public int getComponents() {
        return m_components;
    }

    public String getName() {
        return m_name;
    }

    public String getUnit() {
        return m_unit;
    }

    public float getValueRangeMin() {
        return m_valueRangeMin;
    }

    public float getValueRangeMax() {
        return m_valueRangeMax;
    }

    public long getMapping() {
        return m_mapping;
    }

    public int getMappedValueCount() {
        return m_mappedValueCount;
    }

    public boolean isDiscrete() {
        return m_isDiscrete;
    }

    public boolean isRenderable() {
        return m_isRenderable;
    }

    public boolean isAllowLossyCompression() {
        return m_isAllowLossyCompression;
    }

    public boolean isUseZipForLosslessCompression() {
        return m_isUseZipForLosslessCompression;
    }

    public boolean isUseNoValue() {
        return m_useNoValue;
    }

    public float getNoValue() {
        return m_noValue;
    }

    public float getIntegerScale() {
        return m_integerScale;
    }

    public float getIntegerOffset() {
        return m_integerOffset;
    }
}
