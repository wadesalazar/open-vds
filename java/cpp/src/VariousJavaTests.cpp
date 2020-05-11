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

#include "org_opengroup_openvds_experimental_VariousJavaTests.h"
#include <iostream>

JNIEXPORT void JNICALL Java_org_opengroup_openvds_experimental_VariousJavaTests_segFault
        (JNIEnv *, jclass){
    int *p = 0;
    *p = 1;
}

/*
 * Class:     org_opengroup_openvds_experimental_VariousJavaTests
 * Method:    requestUsingJavaBuffer
 * Signature: ([F[F)V
 */
JNIEXPORT void JNICALL Java_org_opengroup_openvds_experimental_VariousJavaTests_requestUsingJavaBuffer
        (JNIEnv *env, jclass type, jfloatArray jSamplePositions, jfloatArray jOutputBuffer){
    float *samplePositions = env->GetFloatArrayElements( jSamplePositions, NULL);
    float *outputBuffer = env->GetFloatArrayElements( jOutputBuffer, NULL );
    int len = env->GetArrayLength(jOutputBuffer);
    for(int i = 0; i < len; i++)
    {
        outputBuffer[i] = samplePositions[i * 6];
    }
    env->ReleaseFloatArrayElements(jSamplePositions, samplePositions, 0);
    env->ReleaseFloatArrayElements(jOutputBuffer, outputBuffer, 0);
}

/*
 * Class:     org_opengroup_openvds_experimental_VariousJavaTests
 * Method:    requestUsingBuffer
 * Signature: (Ljava/nio/Buffer;Ljava/nio/Buffer;)V
 */
JNIEXPORT void JNICALL Java_org_opengroup_openvds_experimental_VariousJavaTests_requestUsingBuffer
        (JNIEnv *env, jclass type, jobject jpositionBuffer, jobject joutputBuffer){
        float *positions = (float *)env->GetDirectBufferAddress(jpositionBuffer);
        float *output = (float *)env->GetDirectBufferAddress(joutputBuffer);
        int len = env->GetDirectBufferCapacity(joutputBuffer) / sizeof(float);

        for(int i = 0; i < len; i++)
        {
            output[i] = positions[i * 6];
        }
}

/*
 * Class:     org_opengroup_openvds_experimental_VariousJavaTests
 * Method:    requestUsingFloatBuffer
 * Signature: (Ljava/nio/FloatBuffer;Ljava/nio/FloatBuffer;)V
 */
JNIEXPORT void JNICALL Java_org_opengroup_openvds_experimental_VariousJavaTests_requestUsingFloatBuffer
        (JNIEnv *env, jclass type, jobject jpositionBuffer, jobject joutputBuffer){
    float *positions = (float *)env->GetDirectBufferAddress(jpositionBuffer);
    float *output = (float *)env->GetDirectBufferAddress(joutputBuffer);
    int len = env->GetDirectBufferCapacity(joutputBuffer);

    for(int i = 0; i < len; i++)
    {
        output[i] = positions[ i * 6 ];
    }
}

/*
 * Class:     org_opengroup_openvds_experimental_VariousJavaTests
 * Method:    requestUsingNativeBuffer
 * Signature: (JJI)V
 */
JNIEXPORT void JNICALL Java_org_opengroup_openvds_experimental_VariousJavaTests_requestUsingNativeBuffer
        (JNIEnv *env, jclass clazz, jlong queryArray, jlong outputArray, jint nbSample){
    float *positions = (float *)queryArray;
    float *output = (float *)outputArray;
    int len = nbSample;

    for(int i = 0; i < len; i++)
    {
        output[i] = positions[ i * 6 ];
    }
}
