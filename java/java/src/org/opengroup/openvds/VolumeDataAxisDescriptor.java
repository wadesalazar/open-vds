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
 * Analog of OpenVDS::VolumeDataAxisDescriptor class.
 */
public class VolumeDataAxisDescriptor {

    private int numSamples;
    private String name;
    private String unit;
    private float coordinateMin;
    private float coordinateMax;

    public int getNumSamples() {
        return numSamples;
    }

    public String getName() {
        return name;
    }

    public String getUnit() {
        return unit;
    }

    public float getCoordinateMin() {
        return coordinateMin;
    }

    public float getCoordinateMax() {
        return coordinateMax;
    }
}
