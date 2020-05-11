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
 * Interpolation method used for sampling.
 * <p>
 * Analog of C++ OpenVDS::InterpolationMethod class.
 */
public enum InterpolationMethod {

    /**
     * Snap to the closest sample
     */
     NEAREST,
    /**
     * Linear interpolation
     */
     LINEAR,
    /**
     * Cubic interpolation
     */
     CUBIC,
    /**
     * Angular interpolation (same as linear but wraps around the value range)
     */
     ANGULAR,
    /**
     * Triangular interpolation used to interpolate heightmap data
     */
     TRIANGULAR;
}
