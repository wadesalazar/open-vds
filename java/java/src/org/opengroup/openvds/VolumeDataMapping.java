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
 * Mapping volume data channels. Analog of C++ OpenVDS::VolumeDataMapping class.
 */
public enum VolumeDataMapping {

    /**
     * Each voxel in the volume has a value
     */
    DIRECT(0xFFFFFFFFFFFFFFFFL),
    /**
     * Each trace in the volume has a specified number of values. A trace is the
     * entire length of dimension 0
     */
    PER_TRACE(0x1B6F015EB8864888L);

    public final long code;

    VolumeDataMapping(long code) {
        this.code = code;
    }

    public static VolumeDataMapping fromCode(long code) {
        Optional<VolumeDataMapping> first = Arrays
                .stream(values())
                .filter(v -> v.getCode() == code)
                .findFirst();
        if (!first.isPresent())
            throw new IllegalArgumentException("invalid code");
        return first.get();
    }

    public long getCode() {
        return code;
    }


}
