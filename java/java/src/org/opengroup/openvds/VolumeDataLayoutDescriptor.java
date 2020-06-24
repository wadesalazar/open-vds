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
 * Analog of OpenVDS::VolumeDataLayoutDescriptor class.
 */
public class VolumeDataLayoutDescriptor {

    public enum BrickSize {
        BRICK_SIZE_32(5),
        BRICK_SIZE_64(6),
        BRICK_SIZE_128(7),
        BRICK_SIZE_256(8),
        BRICK_SIZE_512(9),
        BRICK_SIZE_1024(10),
        BRICK_SIZE_2048(11),
        BRICK_SIZE_4096(12);

        private final int code;

        BrickSize(int code) {
            this.code = code;
        }

        public static BrickSize fromCode(int code) {
            Optional<BrickSize> first = Arrays
                    .stream(values())
                    .filter(b -> b.getCode() == code)
                    .findFirst();
            if (!first.isPresent())
                throw new IllegalArgumentException("invalid code");
            return first.get();
        }

        public int getCode() {
            return code;
        }
    }

    public enum LODLevels {
        LOD_LEVELS_NONE,
        LOD_LEVELS_1,
        LOD_LEVELS_2,
        LOD_LEVELS_3,
        LOD_LEVELS_4,
        LOD_LEVELS_5,
        LOD_LEVELS_6,
        LOD_LEVELS_7,
        LOD_LEVELS_8,
        LOD_LEVELS_9,
        LOD_LEVELS_10,
        LOD_LEVELS_11,
        LOD_LEVELS_12;
    }

    private static final int BRICK_SIZE = 0,
            NEGATIVE_MARGIN = 1,
            POSITIVE_MARGIN = 2,
            BRICK_SIZE_2D_MULTIPLIER = 3,
            LOD_LEVELS = 4,
            IS_CREATE_2D_LODS = 5,
            IS_FORCE_FULL_RESOLUTION_DIMENSION = 6,
            FULL_RESOLUTION_DIMENSION = 7;

    private int[] _values;

    public VolumeDataLayoutDescriptor(BrickSize brickSize, int negativeMargin,
            int positiveMargin, int brickSize2DMultiplier, LODLevels lodLevels,
            boolean isCreate2DLODs, boolean isForceFullResolutionDimension,
            int fullResolutionDimension) {
        _values[BRICK_SIZE] =  brickSize.ordinal();
        _values[NEGATIVE_MARGIN] = negativeMargin;
        _values[POSITIVE_MARGIN] = positiveMargin;
        _values[BRICK_SIZE_2D_MULTIPLIER] = brickSize2DMultiplier;
        _values[LOD_LEVELS ] = lodLevels.ordinal();
        _values[IS_CREATE_2D_LODS] =  isCreate2DLODs ? 1 : 0;
        _values[IS_FORCE_FULL_RESOLUTION_DIMENSION] = isForceFullResolutionDimension ? 1 : 0;
        _values[FULL_RESOLUTION_DIMENSION] = fullResolutionDimension;
    }

    public VolumeDataLayoutDescriptor(int[] values) {
        _values = values;
    }

    public boolean isValid() {
        return _values[BRICK_SIZE] != 0;
    }

    public BrickSize getBrickSize() {
        return BrickSize.fromCode(_values[BRICK_SIZE]);
    }

    public int getNegativeMargin() {
        return _values[NEGATIVE_MARGIN];
    }

    public int getPositiveMargin() {
        return _values[POSITIVE_MARGIN];
    }

    public int getBrickSizeMultiplier2D() {
        return _values[BRICK_SIZE_2D_MULTIPLIER];
    }

    public LODLevels getLODLevels() {
        return LODLevels.values()[_values[LOD_LEVELS]];
    }

    public boolean isCreate2DLODs() {
        return _values[IS_CREATE_2D_LODS] != 0;
    }

    public boolean isForceFullResolutionDimension() {
        return _values[IS_FORCE_FULL_RESOLUTION_DIMENSION] != 0;
    }

    public int getFullResolutionDimension() {
        return _values[FULL_RESOLUTION_DIMENSION];
    }
}
