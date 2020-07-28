/*
 * Copyright 2020 The Open Group
 * Copyright 2020 Bluware, Inc.
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


#include <OpenVDS/OpenVDS.h>
#include <OpenVDS/VolumeDataLayout.h>
#include <OpenVDS/VolumeDataAccess.h>
#include <OpenVDS/VolumeData.h>
#include <OpenVDS/VolumeDataChannelDescriptor.h>

#include <CommonJni.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <type_traits>


using OpenVDS::VDSHandle;

// Work-around for MSVC
template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, bool>::type check_isfinite(T arg) {
    return std::isfinite(arg);
}

template <typename T>
typename std::enable_if<std::is_integral<T>::value, bool>::type check_isfinite(T arg) {
    return true;
}

template <typename T_src, typename T_dst>
static void copy_data_to_chunk_2d(OpenVDS::VolumeDataPageAccessor *p_access, const T_src *src,
                               const OpenVDS::VolumeDataLayout *layout, int chunk)
{
    const T_dst no_value = p_access->GetChannelDescriptor().GetNoValue();

    int min[OpenVDS::Dimensionality_Max];
    int max[OpenVDS::Dimensionality_Max];
    int chunk_pitch[OpenVDS::Dimensionality_Max];
    OpenVDS::VolumeDataPage *page(p_access->CreatePage(chunk));

    T_dst *dest = reinterpret_cast<T_dst *>(page->GetWritableBuffer(chunk_pitch));
    p_access->GetChunkMinMax(chunk, min, max);

    int chunk_offset_x = min[1];
    int chunk_offset_z = min[0];
    int chunk_size_x = max[1] - min[1];
    int chunk_size_z = max[0] - min[0];
    int size_z = layout->GetAxisDescriptor(0).GetNumSamples();

    for (int j = 0; j < chunk_size_x; j++) {
        for (int i = 0; i < chunk_size_z; i++) {
            std::ptrdiff_t i_src = size_z * (j + chunk_offset_x) + i + chunk_offset_z;
            std::ptrdiff_t i_dest = chunk_pitch[1] * j + i;
            T_src val = check_isfinite((double) src[i_src]) ? src[i_src] : no_value;
            dest[i_dest] = static_cast<T_dst>(val);
        }
    }

    page->Release();
}

template <typename T_src, typename T_dst>
static void copy_data_to_chunk_3d(OpenVDS::VolumeDataPageAccessor *p_access, const T_src *src,
                               const OpenVDS::VolumeDataLayout *layout, int chunk)
{
    const T_dst no_value = p_access->GetChannelDescriptor().GetNoValue();

    int min[OpenVDS::Dimensionality_Max];
    int max[OpenVDS::Dimensionality_Max];
    int chunk_pitch[OpenVDS::Dimensionality_Max];
    OpenVDS::VolumeDataPage *page(p_access->CreatePage(chunk));

    T_dst *dest = reinterpret_cast<T_dst *>(page->GetWritableBuffer(chunk_pitch));
    p_access->GetChunkMinMax(chunk, min, max);

    int chunk_offset_i = min[2];
    int chunk_offset_x = min[1];
    int chunk_offset_z = min[0];
    int chunk_size_i = max[2] - min[2];
    int chunk_size_x = max[1] - min[1];
    int chunk_size_z = max[0] - min[0];
    int size_z = layout->GetAxisDescriptor(0).GetNumSamples();
    int size_x = layout->GetAxisDescriptor(1).GetNumSamples();

    for (int k = 0; k < chunk_size_i; k++) {
        for (int j = 0; j < chunk_size_x; j++) {
            for (int i = 0; i < chunk_size_z; i++) {
                std::ptrdiff_t i_src = chunk_offset_z + i + size_z * (chunk_offset_x + j) + size_z * size_x * (chunk_offset_i + k);
                std::ptrdiff_t i_dest = chunk_pitch[2] * k + chunk_pitch[1] * j + i;
                T_src val = check_isfinite((double) src[i_src]) ? src[i_src] : no_value;
                dest[i_dest] = static_cast<T_dst>(val);
            }
        }
    }

    page->Release();
}


template <typename T>
using copy_fcn_t = std::function<void(OpenVDS::VolumeDataPageAccessor *, const T *, const OpenVDS::VolumeDataLayout *, int)>;

template <typename T>
static copy_fcn_t<T> getCopyFunction_2d(OpenVDS::VolumeDataChannelDescriptor::Format format)
{
    using OpenVDS::VolumeDataChannelDescriptor;

    switch(format) {
        case VolumeDataChannelDescriptor::Format::Format_1Bit:
            // [[fallthrough]]
        case VolumeDataChannelDescriptor::Format::Format_U8:
            return &copy_data_to_chunk_2d<T, std::uint8_t>;
        case VolumeDataChannelDescriptor::Format::Format_U16:
            return &copy_data_to_chunk_2d<T, std::uint16_t>;
        case VolumeDataChannelDescriptor::Format::Format_R32:
            return &copy_data_to_chunk_2d<T, float>;
        case VolumeDataChannelDescriptor::Format::Format_U32:
            return &copy_data_to_chunk_2d<T, std::uint32_t>;
        case VolumeDataChannelDescriptor::Format::Format_R64:
            return &copy_data_to_chunk_2d<T, double>;
        case VolumeDataChannelDescriptor::Format::Format_U64:
            return &copy_data_to_chunk_2d<T, std::uint64_t>;
        case VolumeDataChannelDescriptor::Format::Format_Any:
            // [[fallthrough]]
        default:
            throw std::runtime_error("Cannot process format 'any'");
    }
}

template <typename T>
static copy_fcn_t<T> getCopyFunction_3d(OpenVDS::VolumeDataChannelDescriptor::Format format)
{
    using OpenVDS::VolumeDataChannelDescriptor;

    switch(format) {
        case VolumeDataChannelDescriptor::Format::Format_1Bit:
            // [[fallthrough]]
        case VolumeDataChannelDescriptor::Format::Format_U8:
            return &copy_data_to_chunk_3d<T, std::uint8_t>;
        case VolumeDataChannelDescriptor::Format::Format_U16:
            return &copy_data_to_chunk_3d<T, std::uint16_t>;
        case VolumeDataChannelDescriptor::Format::Format_R32:
            return &copy_data_to_chunk_3d<T, float>;
        case VolumeDataChannelDescriptor::Format::Format_U32:
            return &copy_data_to_chunk_3d<T, std::uint32_t>;
        case VolumeDataChannelDescriptor::Format::Format_R64:
            return &copy_data_to_chunk_3d<T, double>;
        case VolumeDataChannelDescriptor::Format::Format_U64:
            return &copy_data_to_chunk_3d<T, std::uint64_t>;
        case VolumeDataChannelDescriptor::Format::Format_Any:
            // [[fallthrough]]
        default:
            throw std::runtime_error("Cannot process format 'any'");
    }
}


template <class T>
void copy_data(const VDSHandle handle, const T *src, const std::string& channelName)
{
    auto *accessManager = OpenVDS::GetAccessManager(handle);
    const auto *layout = accessManager->GetVolumeDataLayout();

    const int lod_level = 0;
    const int max_pages = 8;
    const int channel = layout->GetChannelIndex(channelName.c_str());

    OpenVDS::DimensionsND dim;
    copy_fcn_t<T> copy_fcn;

    switch (layout->GetDimensionality()) {
    case 2:
        copy_fcn = getCopyFunction_2d<T>(layout->GetChannelFormat(channel));
        dim = OpenVDS::DimensionsND::Dimensions_01;
        break;
    case 3:
        copy_fcn = getCopyFunction_3d<T>(layout->GetChannelFormat(channel));
        dim = OpenVDS::DimensionsND::Dimensions_012;
        break;
    default:
        throw std::domain_error("Only 2D or 3D data can be written");
    }

    auto pageAccessor = accessManager->CreateVolumeDataPageAccessor(layout,
            dim, lod_level, channel, max_pages,
            OpenVDS::VolumeDataAccessManager::AccessMode_Create);

    for (int chunk = 0; chunk < pageAccessor->GetChunkCount(); chunk++) {
        copy_fcn(pageAccessor, src, layout, chunk);
    }

    pageAccessor->Commit();
}


static VDSHandle getVdsHandle(JNIEnv *env, jobject jni_ptr)
{
    jclass obj_class = env->GetObjectClass(jni_ptr);
    jmethodID mid = env->GetMethodID(obj_class, "handle", "()J");
    jlong ptr = env->CallLongMethod(jni_ptr, mid);

    return reinterpret_cast<VDSHandle>(ptr);
}


#ifdef __cplusplus
extern "C" {
#endif
    /*
     * Class:     org_opengroup_openvds_OpenVDS
     * Method:    cpWriteData_r32
     * Signature: (Lorg/opengroup/openvds/JniPointer;[DLjava/lang/String;)V
     */
   JNIEXPORT void JNICALL Java_org_opengroup_openvds_OpenVDS_cpWriteData_1r32(JNIEnv *env, jclass, jobject jJNI_ptr,
                                                                              jfloatArray jSrc, jstring jChannel)
    {
        try {
            VDSHandle vds = getVdsHandle(env, jJNI_ptr);
            float* src = reinterpret_cast<float*>(env->GetFloatArrayElements(jSrc, 0));
            std::string channelName = JStringToString(env, jChannel);
            copy_data(vds, src, channelName);
            env->ReleaseFloatArrayElements(jSrc, src, 0);
        }

        CATCH_EXCEPTIONS_FOR_JAVA;
    }

    /*
     * Class:     org_opengroup_openvds_OpenVDS
     * Method:    cpWriteData_r64
     * Signature: (Lorg/opengroup/openvds/JniPointer;[DLjava/lang/String;)V
     */
    JNIEXPORT void JNICALL Java_org_opengroup_openvds_OpenVDS_cpWriteData_1r64(JNIEnv *env, jclass, jobject jJNI_ptr,
                                                                               jdoubleArray jSrc, jstring jChannel)
    {
        try {
            VDSHandle vds = getVdsHandle(env, jJNI_ptr);
            double* src = reinterpret_cast<double*>(env->GetDoubleArrayElements(jSrc, 0));
            std::string channelName = JStringToString(env, jChannel);
            copy_data(vds, src, channelName);
            env->ReleaseDoubleArrayElements(jSrc, src, 0);
        }

        CATCH_EXCEPTIONS_FOR_JAVA;
    }

    /*
     * Class:     org_opengroup_openvds_OpenVDS
     * Method:    cpWriteData_bool
     * Signature: (Lorg/opengroup/openvds/JniPointer;[ZLjava/lang/String;)V
     */
    JNIEXPORT void JNICALL Java_org_opengroup_openvds_OpenVDS_cpWriteData_1bool(JNIEnv *env, jclass, jobject jJNI_ptr,
                                                                                jbooleanArray jSrc, jstring jChannel)
    {
        try {
            VDSHandle vds = getVdsHandle(env, jJNI_ptr);
            std::uint8_t* src = reinterpret_cast<std::uint8_t *>(env->GetBooleanArrayElements(jSrc, 0));
            std::string channelName = JStringToString(env, jChannel);
            copy_data(vds, src, channelName);
            env->ReleaseBooleanArrayElements(jSrc, src, 0);
        }

        CATCH_EXCEPTIONS_FOR_JAVA;
    }

    /*
     * Class:     org_opengroup_openvds_OpenVDS
     * Method:    cpWriteData_u8
     * Signature: (Lorg/opengroup/openvds/JniPointer;[BLjava/lang/String;)V
     */
    JNIEXPORT void JNICALL Java_org_opengroup_openvds_OpenVDS_cpWriteData_1u8(JNIEnv *env, jclass, jobject jJNI_ptr,
                                                                              jbyteArray jSrc, jstring jChannel)
    {
        try {
            VDSHandle vds = getVdsHandle(env, jJNI_ptr);
            std::int8_t* src = reinterpret_cast<std::int8_t *>(env->GetByteArrayElements(jSrc, 0));
            std::string channelName = JStringToString(env, jChannel);
            copy_data(vds, src, channelName);
            env->ReleaseByteArrayElements(jSrc, src, 0);
        }

        CATCH_EXCEPTIONS_FOR_JAVA;
    }

    /*
     * Class:     org_opengroup_openvds_OpenVDS
     * Method:    cpWriteData_u16
     * Signature: (Lorg/opengroup/openvds/JniPointer;[SLjava/lang/String;)V
     */
    JNIEXPORT void JNICALL Java_org_opengroup_openvds_OpenVDS_cpWriteData_1u16(JNIEnv *env, jclass, jobject jJNI_ptr,
                                                                               jshortArray jSrc, jstring jChannel)
    {
        try {
            VDSHandle vds = getVdsHandle(env, jJNI_ptr);
            std::int16_t* src = reinterpret_cast<std::int16_t *>(env->GetShortArrayElements(jSrc, 0));
            std::string channelName = JStringToString(env, jChannel);
            copy_data(vds, src, channelName);
            env->ReleaseShortArrayElements(jSrc, src, 0);
        }

        CATCH_EXCEPTIONS_FOR_JAVA;
    }

    /*
     * Class:     org_opengroup_openvds_OpenVDS
     * Method:    cpWriteData_u32
     * Signature: (Lorg/opengroup/openvds/JniPointer;[ILjava/lang/String;)V
     */
    JNIEXPORT void JNICALL Java_org_opengroup_openvds_OpenVDS_cpWriteData_1u32(JNIEnv *env, jclass, jobject jJNI_ptr,
                                                                               jintArray jSrc, jstring jChannel)
    {
        try {
            VDSHandle vds = getVdsHandle(env, jJNI_ptr);
            std::int32_t* src = reinterpret_cast<std::int32_t *>(env->GetIntArrayElements(jSrc, 0));
            std::string channelName = JStringToString(env, jChannel);
            copy_data(vds, src, channelName);
            env->ReleaseIntArrayElements(jSrc, (jint *) src, 0);
        }

        CATCH_EXCEPTIONS_FOR_JAVA;
    }

    /*
     * Class:     org_opengroup_openvds_OpenVDS
     * Method:    cpWriteData_u64
     * Signature: (Lorg/opengroup/openvds/JniPointer;[JLjava/lang/String;)V
     */
    JNIEXPORT void JNICALL Java_org_opengroup_openvds_OpenVDS_cpWriteData_1u64(JNIEnv *env, jclass, jobject jJNI_ptr,
                                                                               jlongArray jSrc, jstring jChannel)
    {
        try {
            VDSHandle vds = getVdsHandle(env, jJNI_ptr);
            std::int64_t* src = reinterpret_cast<std::int64_t *>(env->GetLongArrayElements(jSrc, 0));
            std::string channelName = JStringToString(env, jChannel);
            copy_data(vds, src, channelName);
            env->ReleaseLongArrayElements(jSrc, src, 0);
        }

        CATCH_EXCEPTIONS_FOR_JAVA;
    }
#ifdef __cplusplus
}
#endif
