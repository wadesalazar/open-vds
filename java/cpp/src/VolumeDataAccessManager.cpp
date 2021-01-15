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

#include <org_opengroup_openvds_VolumeDataAccessManager.h>
#include <Common.h>
#include <CommonJni.h>
#include <OpenVDS/VolumeDataAccess.h>
#include <iostream>


using namespace OpenVDS;

typedef int Voxel[Dimensionality_Max];

#ifdef __cplusplus
extern "C" {
#endif

inline OpenVDS::VolumeDataAccessManager *GetManager(jlong handle) {
    return (OpenVDS::VolumeDataAccessManager *) CheckHandle(handle);
}

inline OpenVDS::VolumeDataLayout *GetLayout(jlong handle) {
    return (OpenVDS::VolumeDataLayout *) CheckHandle(handle);
}

inline OpenVDS::VolumeDataPageAccessor *GetPageAccessor(jlong handle) {
    return (OpenVDS::VolumeDataPageAccessor *) CheckHandle(handle);
}

inline OpenVDS::VolumeDataAccessor *GetDataAccessor(jlong handle) {
    return (OpenVDS::VolumeDataAccessor *) CheckHandle(handle);
}

/*
* Class:     org_opengroup_openvds_VolumeDataAccessManager
* Method:    cpGetVolumeDataLayout
* Signature: (J)J
*/
JNIEXPORT jlong JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpGetVolumeDataLayout
        (JNIEnv *env, jclass, jlong handle) {
    try {
        return (jlong) GetManager(handle)->GetVolumeDataLayout();
    }
    CATCH_EXCEPTIONS_FOR_JAVA;
    return 0;
}

/*
* Class:     org_opengroup_openvds_VolumeDataAccessManager
* Method:    cpGetVDSProduceStatus
* Signature: (JJIII)I
*/
JNIEXPORT jint JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpGetVDSProduceStatus
        (JNIEnv *env, jclass, jlong handle, jlong layoutHandle, jint dimensionsND, jint lod, jint channel) {
    try {
        return (jint) GetManager(handle)->GetVDSProduceStatus(GetLayout(layoutHandle), (DimensionsND) dimensionsND, lod,
                                                              channel);
    }
    CATCH_EXCEPTIONS_FOR_JAVA;
    return 0;
}

/*
* Class:     org_opengroup_openvds_VolumeDataAccessManager
* Method:    cpCreateVolumeDataPageAccessor
* Signature: (JJIIIII)J
*/
JNIEXPORT jlong JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpCreateVolumeDataPageAccessor__JJIIIII
        (JNIEnv *env, jclass, jlong managerHandle, jlong layoutHandle, jint dimensionsND, jint lod, jint channel,
         jint maxPages, jint accessMode) {
    try {
        return (jlong) GetManager(managerHandle)->CreateVolumeDataPageAccessor(GetLayout(layoutHandle),
                                                                               (DimensionsND) (dimensionsND), lod,
                                                                               channel, maxPages,
                                                                               (VolumeDataAccessManager::AccessMode) accessMode);
    }
    CATCH_EXCEPTIONS_FOR_JAVA;
    return 0;
}

/*
* Class:     org_opengroup_openvds_VolumeDataAccessManager
* Method:    cpCreateVolumeDataPageAccessor
* Signature: (JJIIIIII)J
*/
JNIEXPORT jlong JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpCreateVolumeDataPageAccessor__JJIIIIII
        (JNIEnv *env, jclass, jlong managerHandle, jlong layoutHandle, jint dimensionsND, jint lod, jint channel,
         jint maxPages, jint accessMode, jint chunkMetadataPageSize) {
    try {
        return (jlong) GetManager(managerHandle)->CreateVolumeDataPageAccessor(GetLayout(layoutHandle),
                                                                               (DimensionsND) (dimensionsND), lod,
                                                                               channel, maxPages,
                                                                               (VolumeDataAccessManager::AccessMode) accessMode, chunkMetadataPageSize);
    }
    CATCH_EXCEPTIONS_FOR_JAVA;
    return 0;
}

/*
* Class:     org_opengroup_openvds_VolumeDataAccessManager
* Method:    cpDestroyVolumeDataPageAccessor
* Signature: (JJ)V
*/
JNIEXPORT void JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpDestroyVolumeDataPageAccessor
        (JNIEnv *env, jclass, jlong managerHandle, jlong pageAccessorHandle) {
    try {
        GetManager(managerHandle)->DestroyVolumeDataPageAccessor(GetPageAccessor(pageAccessorHandle));
    }
    CATCH_EXCEPTIONS_FOR_JAVA;
}

/*
* Class:     org_opengroup_openvds_VolumeDataAccessManager
* Method:    cpDestroyVolumeDataAccessor
* Signature: (JJ)V
*/
JNIEXPORT void JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpDestroyVolumeDataAccessor
        (JNIEnv *env, jclass, jlong managerHandle, jlong dataAccessorHandle) {
    try {
        GetManager(managerHandle)->DestroyVolumeDataAccessor(GetDataAccessor(dataAccessorHandle));
    }
    CATCH_EXCEPTIONS_FOR_JAVA;
}

/*
* Class:     org_opengroup_openvds_VolumeDataAccessManager
* Method:    cpCloneVolumeDataAccessor
* Signature: (JJ)J
*/
JNIEXPORT jlong JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpCloneVolumeDataAccessor
        (JNIEnv *env, jclass, jlong managerHandle, jlong dataAccessorHandle) {
    try {
        return (jlong) GetManager(managerHandle)->CloneVolumeDataAccessor(*GetDataAccessor(dataAccessorHandle));
    }
    CATCH_EXCEPTIONS_FOR_JAVA;
    return 0;
}

/*
* Class:     org_opengroup_openvds_VolumeDataAccessManager
* Method:    cpGetVolumeSubsetBufferSize
* Signature: (JJ[I[IIII)J
*/
JNIEXPORT jlong JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpGetVolumeSubsetBufferSize
        (JNIEnv *env, jclass, jlong managerHandle, jlong layoutHandle, jintArray minVoxelCoordinates,
         jintArray maxVoxelCoordinates, jint format, jint lod, jint channel) {
    try {
        std::vector<int> minVoxel = JArrayToVector(env, minVoxelCoordinates);
        std::vector<int> maxVoxel = JArrayToVector(env, maxVoxelCoordinates);
        return GetManager(managerHandle)->GetVolumeSubsetBufferSize(GetLayout(layoutHandle),
                                                                    (Voxel &) *minVoxel.data(),
                                                                    (Voxel &) *maxVoxel.data(),
                                                                    (VolumeDataChannelDescriptor::Format) format, lod, channel);
    }
    CATCH_EXCEPTIONS_FOR_JAVA;
    return 0;
}

/*
 * Class:     org_opengroup_openvds_VolumeDataAccessManager
 * Method:    cpRequestVolumeSubset
 * Signature: (JLjava/nio/Buffer;JIII[I[II)J
 */
JNIEXPORT jlong JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpRequestVolumeSubset
        (JNIEnv *env, jclass, jlong managerHandle, jobject jsampleBuffer, jlong layoutHandle,
         jint dimensionsND, jint lod, jint channel, jintArray minVoxelCoordinates, jintArray maxVoxelCoordinates, jint formatCode){
    std::vector<int> minVoxel = JArrayToVector(env, minVoxelCoordinates);
    std::vector<int> maxVoxel = JArrayToVector(env, maxVoxelCoordinates);
    auto output = static_cast<void *>(env->GetDirectBufferAddress(jsampleBuffer));

    VolumeDataAccessManager *pManager = GetManager(managerHandle);
    VolumeDataLayout *layout = GetLayout(layoutHandle);
    try {
        return pManager->RequestVolumeSubset(output,
                                             layout,
                                             (DimensionsND) dimensionsND,
                                             lod, channel,
                                             (Voxel &) *minVoxel.data(),
                                             (Voxel &) *maxVoxel.data(),
                                             (OpenVDS::VolumeDataChannelDescriptor::Format) formatCode);
    }
    CATCH_EXCEPTIONS_FOR_JAVA;
    return 0;
}

/*
 * Class:     org_opengroup_openvds_VolumeDataAccessManager
 * Method:    cpRequestVolumeSubsetR
 * Signature: (JLjava/nio/Buffer;JIII[I[IIF)J
 */
JNIEXPORT jlong JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpRequestVolumeSubsetR
        (JNIEnv *env, jclass, jlong managerHandle, jobject jsampleBuffer, jlong layoutHandle,
         jint dimensionsND, jint lod, jint channel, jintArray minVoxelCoordinates, jintArray maxVoxelCoordinates, jint formatCode, jfloat replacementValue) {
    std::vector<int> minVoxel = JArrayToVector(env, minVoxelCoordinates);
    std::vector<int> maxVoxel = JArrayToVector(env, maxVoxelCoordinates);
    auto output = static_cast<void *>(env->GetDirectBufferAddress(jsampleBuffer));

    VolumeDataAccessManager *pManager = GetManager(managerHandle);
    VolumeDataLayout *layout = GetLayout(layoutHandle);
    try {
        return pManager->RequestVolumeSubset(output,
                                             layout,
                                             (DimensionsND) dimensionsND,
                                             lod, channel,
                                             (Voxel &) *minVoxel.data(),
                                             (Voxel &) *maxVoxel.data(),
                                             (OpenVDS::VolumeDataChannelDescriptor::Format) formatCode,
                                             replacementValue);
    }
    CATCH_EXCEPTIONS_FOR_JAVA;
    return 0;
}

/*
* Class:     org_opengroup_openvds_VolumeDataAccessManager
* Method:    cpGetProjectedVolumeSubsetBufferSize
* Signature: (JJ[I[IIIII)J
*/
JNIEXPORT jlong JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpGetProjectedVolumeSubsetBufferSize
        (JNIEnv *env, jclass, jlong managerHandle, jlong layoutHandle, jintArray minVoxelCoordinates,
         jintArray maxVoxelCoordinates,
         jint projectedDimensions, jint format, jint lod, jint channel) {
    try {
        std::vector<int> minVoxelArray = JArrayToVector(env, minVoxelCoordinates);
        std::vector<int> maxVoxelArray = JArrayToVector(env, maxVoxelCoordinates);
        return GetManager(managerHandle)->GetProjectedVolumeSubsetBufferSize(GetLayout(layoutHandle),
                                                                             (Voxel &) *minVoxelArray.data(),
                                                                             (Voxel &) *maxVoxelArray.data(),
                                                                             (DimensionsND) projectedDimensions,
                                                                             (VolumeDataChannelDescriptor::Format) format,
                                                                             lod, channel);
    }
    CATCH_EXCEPTIONS_FOR_JAVA;
    return 0;
}

/*
 * Class:     org_opengroup_openvds_VolumeDataAccessManager
 * Method:    cpRequestProjectedVolumeSubset
 * Signature: (JLjava/nio/FloatBuffer;JIII[I[IFFFFIII)J
 */
JNIEXPORT jlong JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpRequestProjectedVolumeSubset
        (JNIEnv *env, jclass, jlong managerHandle, jobject jsampleBuffer, jlong layoutHandle,
         jint jDimensionsND, jint lod, jint channel, jintArray minVoxelCoordinates, jintArray maxVoxelCoordinates,
         jfloat jVp0, jfloat jVp1, jfloat jVp2, jfloat jVp3,  jint projectedDimensions, jint jFormat, jint interpolationMethod){
    try {
        VolumeDataAccessManager *manager = GetManager(managerHandle);
        VolumeDataLayout *layout = GetLayout(layoutHandle);
        std::vector<int> minVoxelArray = JArrayToVector(env, minVoxelCoordinates);
        std::vector<int> maxVoxelArray = JArrayToVector(env, maxVoxelCoordinates);
        Voxel &minVoxel = (Voxel &) *minVoxelArray.data();
        Voxel &maxVoxel = (Voxel &) *maxVoxelArray.data();
        DimensionsND dimensionsND = (DimensionsND) jDimensionsND;
        VolumeDataChannelDescriptor::Format format = (VolumeDataChannelDescriptor::Format) jFormat;
        FloatVector4 voxelPlane(jVp0, jVp1, jVp2, jVp3);
        auto output = static_cast<float *>(env->GetDirectBufferAddress(jsampleBuffer));
        return manager->RequestProjectedVolumeSubset(
                output, layout, dimensionsND, lod, channel,
                minVoxel, maxVoxel, voxelPlane,
                (DimensionsND) projectedDimensions, format, (InterpolationMethod) interpolationMethod);
    }
    CATCH_EXCEPTIONS_FOR_JAVA;
    return 0;
}

/*
 * Class:     org_opengroup_openvds_VolumeDataAccessManager
 * Method:    cpRequestProjectedVolumeSubsetR
 * Signature: (JJJJIII[I[I[FIIIF)J
 */
JNIEXPORT jlong JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpRequestProjectedVolumeSubsetR
        (JNIEnv *env, jclass, jlong managerHandle, jobject jsampleBuffer, jlong layoutHandle,
         jint jDimensionsND, jint lod, jint channel, jintArray minVoxelCoordinates, jintArray maxVoxelCoordinates,
         jfloat jVp0, jfloat jVp1, jfloat jVp2, jfloat jVp3,  jint projectedDimensions, jint jFormat, jint interpolationMethod, jfloat replacementNoValue) {
    try {
        VolumeDataAccessManager *manager = GetManager(managerHandle);
        VolumeDataLayout *layout = GetLayout(layoutHandle);
        std::vector<int> minVoxelArray = JArrayToVector(env, minVoxelCoordinates);
        std::vector<int> maxVoxelArray = JArrayToVector(env, maxVoxelCoordinates);
        Voxel &minVoxel = (Voxel &) *minVoxelArray.data();
        Voxel &maxVoxel = (Voxel &) *maxVoxelArray.data();
        DimensionsND dimensionsND = (DimensionsND) jDimensionsND;
        VolumeDataChannelDescriptor::Format format = (VolumeDataChannelDescriptor::Format) jFormat;
        FloatVector4 voxelPlane(jVp0, jVp1, jVp2, jVp3);
        auto output = static_cast<float *>(env->GetDirectBufferAddress(jsampleBuffer));
        return manager->RequestProjectedVolumeSubset(
                output, layout, dimensionsND, lod, channel,
                minVoxel, maxVoxel, voxelPlane,
                (DimensionsND) projectedDimensions, format, (InterpolationMethod) interpolationMethod,
                replacementNoValue);
    }
    CATCH_EXCEPTIONS_FOR_JAVA;
    return 0;
}

/*
 * Class:     org_opengroup_openvds_VolumeDataAccessManager
 * Method:    cpRequestVolumeSamples
 * Signature: (JLjava/nio/FloatBuffer;JIIILjava/nio/FloatBuffer;II)J
 */
JNIEXPORT jlong JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpRequestVolumeSamples
        (JNIEnv *env, jclass, jlong managerHandle, jobject jsampleBuffer, jlong layoutHandle, jint dimensionsND,
         jint lod, jint channel, jobject jpositionBuffer, jint sampleCount, jint interpolationMethod) {
    auto positions = reinterpret_cast<const float (*)[Dimensionality_Max]>(env->GetDirectBufferAddress(jpositionBuffer));
    auto output = static_cast<float *>(env->GetDirectBufferAddress(jsampleBuffer));
    try {
        return GetManager(managerHandle)->RequestVolumeSamples(output,
                                                               GetLayout(layoutHandle),
                                                               (DimensionsND) dimensionsND, lod, channel,
                                                               positions,
                                                               sampleCount, (InterpolationMethod) interpolationMethod);
    }
    CATCH_EXCEPTIONS_FOR_JAVA;
    return 0;
}

/*
* Class:     org_opengroup_openvds_VolumeDataAccessManager
* Method:    cpGetVolumeSamplesBufferSize
* Signature: (JJII)J
*/
JNIEXPORT jlong JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpGetVolumeSamplesBufferSize
(JNIEnv *env, jclass, jlong managerHandle, jlong layoutHandle, jint sampleCount, jint channel) {
  try {
    return GetManager(managerHandle)->GetVolumeSamplesBufferSize(GetLayout(layoutHandle), sampleCount, channel);
  }
  CATCH_EXCEPTIONS_FOR_JAVA;
  return 0;
}

/*
* Class:     org_opengroup_openvds_VolumeDataAccessManager
* Method:    cpRequestVolumeSamplesR
* Signature: (JLjava/nio/FloatBuffer;JIIILjava/nio/FloatBuffer;IIF)J
*/
JNIEXPORT jlong JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpRequestVolumeSamplesR
        (JNIEnv *env, jclass, jlong managerHandle, jobject jsampleBuffer, jlong layoutHandle, jint dimensionsND,
         jint lod, jint channel, jobject jpositionBuffer, jint sampleCount, jint interpolationMethod,
         jfloat replacementValue) {
    auto positions = reinterpret_cast<const float (*)[Dimensionality_Max]>(env->GetDirectBufferAddress(jpositionBuffer));
    auto output = static_cast<float *>(env->GetDirectBufferAddress(jsampleBuffer));
    try {
        return GetManager(managerHandle)->RequestVolumeSamples(output,
                                                               GetLayout(layoutHandle),
                                                               (DimensionsND) dimensionsND, lod, channel,
                                                               positions,
                                                               sampleCount, (InterpolationMethod) interpolationMethod,
                                                               replacementValue);
    }
    CATCH_EXCEPTIONS_FOR_JAVA;
    return 0;
}

/*
* Class:     org_opengroup_openvds_VolumeDataAccessManager
* Method:    cpGetVolumeTracesBufferSize
* Signature: (JJIIII)J
*/
JNIEXPORT jlong JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpGetVolumeTracesBufferSize
        (JNIEnv *env, jclass, jlong managerHandle, jlong layoutHandle, jint traceCount, jint traceDimension, jint lod, jint channel) {
    try {
        return GetManager(managerHandle)->GetVolumeTracesBufferSize(GetLayout(layoutHandle), traceCount, traceDimension,
                                                                    lod, channel);
    }
    CATCH_EXCEPTIONS_FOR_JAVA;
    return 0;
}

/*
 * Class:     org_opengroup_openvds_VolumeDataAccessManager
 * Method:    cpRequestVolumeTraces
 * Signature: (JLjava/nio/FloatBuffer;JIIILjava/nio/FloatBuffer;III)J
 */
JNIEXPORT jlong JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpRequestVolumeTraces__JLjava_nio_FloatBuffer_2JIIILjava_nio_FloatBuffer_2III
        (JNIEnv *env, jclass, jlong managerHandle, jobject jsampleBuffer, jlong layoutHandle,
            jint jDimensionsND, jint lod, jint channel, jobject jTracePositions, jint traceCount,
            jint interpolationMethod, jint traceDimension){
    auto tracePositions = reinterpret_cast<const float (*)[Dimensionality_Max]>(env->GetDirectBufferAddress(jTracePositions));
    auto output = static_cast<float*>(env->GetDirectBufferAddress(jsampleBuffer));
    auto pManager = GetManager(managerHandle);
    auto layout = GetLayout(layoutHandle);
    try {
        return pManager->RequestVolumeTraces(output,
                                             layout,
                                             (DimensionsND) jDimensionsND, lod, channel,
                                             tracePositions,
                                             traceCount, (InterpolationMethod) interpolationMethod,
                                             traceDimension);
    }
    CATCH_EXCEPTIONS_FOR_JAVA;
    return 0;
}

/*
 * Class:     org_opengroup_openvds_VolumeDataAccessManager
 * Method:    cpRequestVolumeTraces
 * Signature: (JLjava/nio/FloatBuffer;JIIILjava/nio/FloatBuffer;IIIF)J
 */
JNIEXPORT jlong JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpRequestVolumeTraces__JLjava_nio_FloatBuffer_2JIIILjava_nio_FloatBuffer_2IIIF
        (JNIEnv *env, jclass, jlong managerHandle, jobject jsampleBuffer, jlong layoutHandle,
         jint jDimensionsND, jint lod, jint channel, jobject jTracePositions, jint traceCount,
         jint interpolationMethod, jint traceDimension, jfloat replacementNoValue){
    auto tracePositions = reinterpret_cast<const float (*)[Dimensionality_Max]>(env->GetDirectBufferAddress(jTracePositions));
    auto output = static_cast<float*>(env->GetDirectBufferAddress(jsampleBuffer));
    auto pManager = GetManager(managerHandle);
    auto layout = GetLayout(layoutHandle);
    try {
        return pManager->RequestVolumeTraces(output,
                                             layout,
                                             (DimensionsND) jDimensionsND, lod, channel,
                                             tracePositions,
                                             traceCount, (InterpolationMethod) interpolationMethod,
                                             traceDimension, replacementNoValue);
    }
    CATCH_EXCEPTIONS_FOR_JAVA;
    return 0;
}

/*
* Class:     org_opengroup_openvds_VolumeDataAccessManager
* Method:    cpPrefetchVolumeChunk
* Signature: (JJIIIJ)J
*/
JNIEXPORT jlong JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpPrefetchVolumeChunk
        (JNIEnv *env, jclass, jlong managerHandle, jlong layoutHandle, jint dimensionsND, jint lod, jint channel,
         jlong chunk) {
    try {
        return GetManager(managerHandle)->PrefetchVolumeChunk(GetLayout(layoutHandle), (DimensionsND) dimensionsND, lod,
                                                              channel, chunk);
    }
    CATCH_EXCEPTIONS_FOR_JAVA;
    return 0;
}

/*
* Class:     org_opengroup_openvds_VolumeDataAccessManager
* Method:    cpIsCompleted
* Signature: (JJ)Z
*/
JNIEXPORT jboolean JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpIsCompleted
        (JNIEnv *env, jclass, jlong handle, jlong requestID) {
    try {
        return GetManager(handle)->IsCompleted(requestID);
    }
    CATCH_EXCEPTIONS_FOR_JAVA;
    return 0;
}

/*
* Class:     org_opengroup_openvds_VolumeDataAccessManager
* Method:    cpIsCanceled
* Signature: (JJ)Z
*/
JNIEXPORT jboolean JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpIsCanceled
        (JNIEnv *env, jclass, jlong handle, jlong requestID) {
    try {
        GetManager(handle)->IsCanceled(requestID);
    }
    CATCH_EXCEPTIONS_FOR_JAVA;
    return 0;
}

/*
* Class:     org_opengroup_openvds_VolumeDataAccessManager
* Method:    cpWaitForCompletion
* Signature: (JJI)Z
*/
JNIEXPORT jboolean JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpWaitForCompletion
        (JNIEnv *env, jclass, jlong handle, jlong requestID, jint millisBeforeTimeout) {
    try {
        return GetManager(handle)->WaitForCompletion(requestID, millisBeforeTimeout);
    }
    CATCH_EXCEPTIONS_FOR_JAVA;
    return false;
}

/*
* Class:     org_opengroup_openvds_VolumeDataAccessManager
* Method:    cpCancel
* Signature: (JJ)V
*/
JNIEXPORT void JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpCancel
        (JNIEnv *env, jclass, jlong handle, jlong requestID) {
    try {
        GetManager(handle)->Cancel(requestID);
    }
    CATCH_EXCEPTIONS_FOR_JAVA;
}

/*
* Class:     org_opengroup_openvds_VolumeDataAccessManager
* Method:    cpGetCompletionFactor
* Signature: (JJ)F
*/
JNIEXPORT jfloat JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpGetCompletionFactor
        (JNIEnv *env, jclass, jlong handle, jlong requestID) {
    try {
        return GetManager(handle)->GetCompletionFactor(requestID);
    }
    CATCH_EXCEPTIONS_FOR_JAVA;
    return 0;
}

/*
* Class:     org_opengroup_openvds_VolumeDataAccessManager
* Method:    cpFlushUploadQueue
* Signature: (J)V
*/
JNIEXPORT void JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpFlushUploadQueue__J
        (JNIEnv *env, jclass, jlong handle) {
    try {
        GetManager(handle)->FlushUploadQueue();
    }
    CATCH_EXCEPTIONS_FOR_JAVA;
}

/*
* Class:     org_opengroup_openvds_VolumeDataAccessManager
* Method:    cpFlushUploadQueue
* Signature: (JZ)V
*/
JNIEXPORT void JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpFlushUploadQueue__JZ
        (JNIEnv *env, jclass, jlong handle, jboolean writeUpdatedLayerStatus) {
    try {
        GetManager(handle)->FlushUploadQueue(writeUpdatedLayerStatus);
    }
    CATCH_EXCEPTIONS_FOR_JAVA;
}

/*
* Class:     org_opengroup_openvds_VolumeDataAccessManager
* Method:    cpClearUploadErrors
* Signature: (J)V
*/
JNIEXPORT void JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpClearUploadErrors
        (JNIEnv *env, jclass, jlong handle) {
    try {
        GetManager(handle)->ClearUploadErrors();
    }
    CATCH_EXCEPTIONS_FOR_JAVA;
}

/*
* Class:     org_opengroup_openvds_VolumeDataAccessManager
* Method:    cpForceClearAllUploadErrors
* Signature: (J)V
*/
JNIEXPORT void JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpForceClearAllUploadErrors
        (JNIEnv *env, jclass, jlong handle) {
    try {
        GetManager(handle)->ForceClearAllUploadErrors();
    }
    CATCH_EXCEPTIONS_FOR_JAVA;
}

/*
* Class:     org_opengroup_openvds_VolumeDataAccessManager
* Method:    cpUploadErrorCount
* Signature: (J)I
*/
JNIEXPORT jint JNICALL Java_org_opengroup_openvds_VolumeDataAccessManager_cpUploadErrorCount
        (JNIEnv *env, jclass, jlong handle) {
    try {
        return GetManager(handle)->UploadErrorCount();
    }
    CATCH_EXCEPTIONS_FOR_JAVA;
    return 0;
}

#ifdef __cplusplus
}
#endif
