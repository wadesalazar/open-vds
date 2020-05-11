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

public class NDBox {
    private int[] min;
    private int[] max;

    public NDBox() {
        min = new int[6];
        max = new int[6];
    }

    public NDBox(int min0, int min1, int min2, int min3, int min4, int min5,
                 int max0, int max1, int max2, int max3, int max4, int max5) {
        this();
        setMin(min0, min1, min2, min3, min4, min5);
        setMax(max0, max1, max2, max3, max4, max5);
    }

    public void setMax(int max0, int max1, int max2, int max3, int max4, int max5) {
        set(max, max0, max1, max2, max3, max4, max5);
    }

    private void set(int[] array, int v0, int v1, int v2, int v3, int v4, int v5) {
        array[0] = v0;
        array[1] = v1;
        array[2] = v2;
        array[3] = v3;
        array[4] = v4;
        array[5] = v5;
    }

    public void setMin(int min0, int min1, int min2, int min3, int min4, int min5) {
        set(min, min0, min1, min2, min3, min4, min5);
    }

    public int[] getMax() {
        return max;
    }

    public int[] getMin() {
        return min;
    }
}
