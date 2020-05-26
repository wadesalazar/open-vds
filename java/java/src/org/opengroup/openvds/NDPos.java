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

public class NDPos {
    private float[] values;

    public NDPos() {
        values = new float[6];
    }

    public NDPos(float v0, float v1, float v2, float v3, float v4, float v5) {
        this();
        set(v0, v1, v2, v3, v4, v5);
    }

    private void set(float[] array, float v0, float v1, float v2, float v3, float v4, float v5) {
        array[0] = v0;
        array[1] = v1;
        array[2] = v2;
        array[3] = v3;
        array[4] = v4;
        array[5] = v5;
    }

    public void set(float v0, float v1, float v2, float v3, float v4, float v5) {
        set(values, v0, v1, v2, v3, v4, v5);
    }

    public float[] getMin() {
        return values;
    }
}
