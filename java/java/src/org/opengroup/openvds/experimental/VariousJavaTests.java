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

import java.nio.Buffer;
import java.nio.FloatBuffer;

public class VariousJavaTests {

    public static native void segFault();

    public static native void requestUsingJavaBuffer(float[] queryArray, float[] floatArray);

    public static native void requestUsingBuffer(Buffer queryArray, Buffer floatArray);

    public static native void requestUsingFloatBuffer(FloatBuffer queryArray, FloatBuffer floatArray);

    public static native void requestUsingNativeBuffer(long queryArray, long floatArray, int sampleCount);
}
