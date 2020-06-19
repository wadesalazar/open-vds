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

#include <org_opengroup_openvds_OpenVDS.h>
#include <OpenVDS/OpenVDS.h>
#include <OpenVDS/VolumeDataLayoutDescriptor.h>
#include <OpenVDS/VolumeDataChannelDescriptor.h>
#include <OpenVDS/VolumeDataAxisDescriptor.h>

#include <CommonJni.h>
#include <iostream>
#include <unistd.h>
#include <set>
#include <string>
#include <cassert>

void throwJavaIOException(JNIEnv *env, const OpenVDS::Error &error) {
    std::string message = error.string + " ";
    message += "code : " + std::to_string(error.code);
    ThrowJavaIOException(env, message.c_str());
}

jlong openVDSOrThrowJavaIOException(JNIEnv *env, const OpenVDS::OpenOptions &openOptions) {
    OpenVDS::Error error;
    OpenVDS::VDSHandle pVds = OpenVDS::Open(openOptions, error);
    if (pVds == nullptr) {
        throwJavaIOException(env, error);
    }
    return (jlong) pVds;
}

/*
 * Class:     org_opengroup_openvds_OpenVDS
 * Method:    cpOpenAWS
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)J
 */
jlong JNICALL Java_org_opengroup_openvds_OpenVDS_cpOpenAWS
        (JNIEnv *env, jclass, jstring jbucket, jstring jkey, jstring jregion, jstring jEndPointOverride) {
    OpenVDS::AWSOpenOptions openOptions;

    openOptions.key = JStringToString(env, jkey);
    openOptions.bucket = JStringToString(env, jbucket);
    openOptions.region = JStringToString(env, jregion);
    openOptions.endpointOverride = JStringToString(env, jEndPointOverride);

    return openVDSOrThrowJavaIOException(env, openOptions);
}

jlong JNICALL Java_org_opengroup_openvds_OpenVDS_cpOpenAzure
        (JNIEnv *env, jclass, jstring jConnectionString, jstring jContainer, jstring jBlob, jint jParallelismFactor,
         jint jMaxExecutionTime) {

    OpenVDS::AzureOpenOptions openOptions;

    openOptions.connectionString = JStringToString(env, jConnectionString);
    openOptions.container = JStringToString(env, jContainer);
    openOptions.blob = JStringToString(env, jBlob);
    openOptions.parallelism_factor = jParallelismFactor;
    openOptions.max_execution_time = jMaxExecutionTime;

    return openVDSOrThrowJavaIOException(env, openOptions);
}

/*
 * Class:     org_opengroup_openvds_OpenVDS
 * Method:    cpOpenAzurePresigned
 * Signature: (Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_org_opengroup_openvds_OpenVDS_cpOpenAzurePresigned
        (JNIEnv *env, jclass, jstring baseUrl, jstring urlSuffix) {
    OpenVDS::AzureOpenOptions openOptions;

    openOptions.connectionString = JStringToString(env, baseUrl);
    openOptions.container = JStringToString(env, urlSuffix);

    return openVDSOrThrowJavaIOException(env, openOptions);
}

/*
 * Class:     org_opengroup_openvds_OpenVDS
 * Method:    cpOpenConnection
 * Signature: (Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_org_opengroup_openvds_OpenVDS_cpOpenConnection
        (JNIEnv *env, jclass, jstring url, jstring connectionString) {
    OpenVDS::Error error;
    OpenVDS::VDSHandle pVds = OpenVDS::Open(JStringToString(env, url), JStringToString(env, connectionString), error);
    if (pVds == nullptr) {
        throwJavaIOException(env, error);
    }
    return (jlong) pVds;
}

static jlong
createVDSOrThrowJavaIOException(JNIEnv* env, const OpenVDS::OpenOptions& options,
                                const OpenVDS::VolumeDataLayoutDescriptor& layoutDescriptor,
                                OpenVDS::VectorWrapper<OpenVDS::VolumeDataAxisDescriptor> axisDescriptors,
                                OpenVDS::VectorWrapper<OpenVDS::VolumeDataChannelDescriptor> channelDescriptors,
                                const OpenVDS::MetadataReadAccess& metadata)
{
    OpenVDS::Error error;
    auto* pVds = OpenVDS::Create(options, layoutDescriptor, axisDescriptors,
            channelDescriptors, metadata, error);

    if (pVds == nullptr) {
        throwJavaIOException(env, error);
    }

    return (jlong) pVds;
}

static std::vector<OpenVDS::VolumeDataChannelDescriptor>
getVolumeDataChannelDescriptor(JNIEnv *env, jobjectArray obj, std::multiset<std::string>& string_buffer)
{
    jclass obj_class = env->GetObjectClass(obj);
    std::vector<OpenVDS::VolumeDataChannelDescriptor> result;

    for (jint i = 0; i < env->GetArrayLength(obj); i++) {
        jobject element = env->GetObjectArrayElement(obj, i);
        jclass elm_class = env->GetObjectClass(element);

        int format = env->CallIntMethod(element, env->GetMethodID(elm_class, "getFormat", "()I"));
        int components = env->CallIntMethod(element, env->GetMethodID(elm_class, "getComponents", "()I"));
        auto name = string_buffer.insert(JStringToString(env, (jstring) env->CallObjectMethod(element, env->GetMethodID(elm_class, "getName", "()Ljava/lang/String;"))));
        auto unit = string_buffer.insert(JStringToString(env, (jstring) env->CallObjectMethod(element, env->GetMethodID(elm_class, "getUnit", "()Ljava/lang/String;"))));
        float valuerangemin = env->CallFloatMethod(element, env->GetMethodID(elm_class, "getValueRangeMin", "()F"));
        float valuerangemax = env->CallFloatMethod(element, env->GetMethodID(elm_class, "getValueRangeMax", "()F"));
        long mapping = env->CallLongMethod(element, env->GetMethodID(elm_class, "getMapping", "()J"));
        int mappedvaluecount = env->CallIntMethod(element, env->GetMethodID(elm_class, "getMappedValueCount", "()I"));
        bool isdiscrete = env->CallBooleanMethod(element, env->GetMethodID(elm_class, "isDiscrete", "()Z"));
        bool isrenderable = env->CallBooleanMethod(element, env->GetMethodID(elm_class, "isRenderable", "()Z"));
        bool isallowlossycompression = env->CallBooleanMethod(element, env->GetMethodID(elm_class, "isAllowLossyCompression", "()Z"));
        bool isusezipforlosslesscompression = env->CallBooleanMethod(element, env->GetMethodID(elm_class, "isUseZipForLosslessCompression", "()Z"));
        //bool isusenovalue = env->CallBooleanMethod(element, env->GetMethodID(elm_class, " isUseNoValue", "()Z"));
        float novalue = env->CallFloatMethod(element, env->GetMethodID(elm_class, "getNoValue", "()F"));
        float integerscale = env->CallFloatMethod(element, env->GetMethodID(elm_class, "getIntegerScale", "()F"));
        float integeroffset = env->CallFloatMethod(element, env->GetMethodID(elm_class, "getIntegerOffset", "()F"));
        int flags = (isdiscrete << 0) | (!isallowlossycompression << 1) | (!isrenderable << 2) | (!isusezipforlosslesscompression << 3);

        OpenVDS::VolumeDataChannelDescriptor tmp(static_cast<OpenVDS::VolumeDataChannelDescriptor::Format>(format),
               static_cast<OpenVDS::VolumeDataChannelDescriptor::Components>(components), name->c_str(), unit->c_str(),
               valuerangemin, valuerangemax, static_cast<OpenVDS::VolumeDataMapping>(mapping), mappedvaluecount, 
               static_cast<OpenVDS::VolumeDataChannelDescriptor::Flags>(flags), novalue, integerscale, integeroffset);

        result.emplace_back(tmp);
    }

    return result;
}

static OpenVDS::MetadataReadAccess*
getMetadata(JNIEnv *env, jobject md)
{
    jclass obj_class = env->GetObjectClass(md);
    jlong handle = env->CallLongMethod(md, env->GetMethodID(obj_class, "handle", "()J"));
    return reinterpret_cast<OpenVDS::MetadataReadAccess*>(CheckHandle(handle));
}

static std::vector<OpenVDS::VolumeDataAxisDescriptor>
getValueDataAxisDescriptors(JNIEnv *env, jobjectArray obj, std::multiset<std::string>& string_buffer)
{
    std::vector<OpenVDS::VolumeDataAxisDescriptor> result;

    for (jint i = 0; i < env->GetArrayLength(obj); i++) {
        jobject element = env->GetObjectArrayElement(obj, i);
        jclass elm_class = env->GetObjectClass(element);

        int num_samples = env->CallIntMethod(element, env->GetMethodID(elm_class, "getNumSamples", "()I"));
        auto p1 = string_buffer.insert(JStringToString(env, (jstring) env->CallObjectMethod(element, env->GetMethodID(elm_class, "getName", "()Ljava/lang/String;"))));
        auto p2 = string_buffer.insert(JStringToString(env, (jstring) env->CallObjectMethod(element, env->GetMethodID(elm_class, "getUnit", "()Ljava/lang/String;"))));

        float min_coor = env->CallFloatMethod(element, env->GetMethodID(elm_class, "getCoordinateMin", "()F"));
        float max_coor = env->CallFloatMethod(element, env->GetMethodID(elm_class, "getCoordinateMax", "()F"));

        result.emplace_back(num_samples, p1->c_str(), p2->c_str(), min_coor, max_coor);
    }

    return result;
}



static enum OpenVDS::VolumeDataLayoutDescriptor::BrickSize
getBrickSize(JNIEnv* env, jobject obj)
{
    jclass obj_class = env->GetObjectClass(obj);
    jmethodID mid = env->GetMethodID(obj_class, "getBrickSize", "()Lorg/opengroup/openvds/VolumeDataLayoutDescriptor$BrickSize;");
    jobject bricksize = env->CallObjectMethod(obj, mid);
    int brick_size_ = env->CallIntMethod(bricksize, env->GetMethodID(env->GetObjectClass(bricksize), "ordinal", "()I"));

    switch (brick_size_) {
    case 0: 
        return OpenVDS::VolumeDataLayoutDescriptor::BrickSize_32;
    case 1: 
        return OpenVDS::VolumeDataLayoutDescriptor::BrickSize_64;
    case 2: 
        return OpenVDS::VolumeDataLayoutDescriptor::BrickSize_128;
    case 3: 
        return OpenVDS::VolumeDataLayoutDescriptor::BrickSize_256;
    case 4: 
        return OpenVDS::VolumeDataLayoutDescriptor::BrickSize_512;
    case 5: 
        return OpenVDS::VolumeDataLayoutDescriptor::BrickSize_1024;
    case 6: 
        return OpenVDS::VolumeDataLayoutDescriptor::BrickSize_2048;
    case 7: 
        return OpenVDS::VolumeDataLayoutDescriptor::BrickSize_4096;
    default:
        assert(false);
        return OpenVDS::VolumeDataLayoutDescriptor::BrickSize::BrickSize_32;
    }
}

static enum OpenVDS::VolumeDataLayoutDescriptor::LODLevels
getLODLevels(JNIEnv* env, jobject obj)
{
    jclass obj_class = env->GetObjectClass(obj);
    jobject jlodlevels = env->CallObjectMethod(obj, env->GetMethodID(obj_class, "getLODLevels", "()Lorg/opengroup/openvds/VolumeDataLayoutDescriptor$LODLevels;"));
    int lod_level = env->CallIntMethod(jlodlevels, env->GetMethodID(env->GetObjectClass(jlodlevels), "ordinal", "()I"));

    switch(lod_level) {
    case 0:
        return OpenVDS::VolumeDataLayoutDescriptor::LODLevels::LODLevels_None;
    case 1:
        return OpenVDS::VolumeDataLayoutDescriptor::LODLevels::LODLevels_1;
    case 2:
        return OpenVDS::VolumeDataLayoutDescriptor::LODLevels::LODLevels_2;
    case 3:
        return OpenVDS::VolumeDataLayoutDescriptor::LODLevels::LODLevels_3;
    case 4:
        return OpenVDS::VolumeDataLayoutDescriptor::LODLevels::LODLevels_4;
    case 5:
        return OpenVDS::VolumeDataLayoutDescriptor::LODLevels::LODLevels_5;
    case 6:
        return OpenVDS::VolumeDataLayoutDescriptor::LODLevels::LODLevels_6;
    case 7:
        return OpenVDS::VolumeDataLayoutDescriptor::LODLevels::LODLevels_7;
    case 8:
        return OpenVDS::VolumeDataLayoutDescriptor::LODLevels::LODLevels_8;
    case 9:
        return OpenVDS::VolumeDataLayoutDescriptor::LODLevels::LODLevels_9;
    case 10:
        return OpenVDS::VolumeDataLayoutDescriptor::LODLevels::LODLevels_10;
    case 11:
        return OpenVDS::VolumeDataLayoutDescriptor::LODLevels::LODLevels_11;
    case 12:
        return OpenVDS::VolumeDataLayoutDescriptor::LODLevels::LODLevels_12;
    default:
        assert(false);
        return OpenVDS::VolumeDataLayoutDescriptor::LODLevels::LODLevels_None;
    }
}

static OpenVDS::VolumeDataLayoutDescriptor
getDescriptor(JNIEnv *env, jobject obj)
{
    jclass obj_class = env->GetObjectClass(obj);

    auto brickSize = getBrickSize(env, obj);
    int negativeMargin = env->CallIntMethod(obj, env->GetMethodID(obj_class, "getNegativeMargin", "()I"));
    int positiveMargin = env->CallIntMethod(obj, env->GetMethodID(obj_class, "getPositiveMargin", "()I"));
    int brickSize2DMultiplier = env->CallIntMethod(obj, env->GetMethodID(obj_class, "getBrickSizeMultiplier2D", "()I"));
    auto lodLevels = getLODLevels(env, obj);

    int fullResolutionDimension = env->CallBooleanMethod(obj, env->GetMethodID(obj_class, "isForceFullResolutionDimension", "()Z"));

    int options = (env->CallBooleanMethod(obj, env->GetMethodID(obj_class, "isCreate2DLODs", "()Z")) << 1);

    return OpenVDS::VolumeDataLayoutDescriptor(brickSize, negativeMargin, positiveMargin, brickSize2DMultiplier,
                                               lodLevels, (OpenVDS::VolumeDataLayoutDescriptor::Options) options, fullResolutionDimension);
}

jlong JNICALL
Java_org_opengroup_openvds_OpenVDS_cpCreateAzure(JNIEnv *env, jclass, jstring jConnectionString,
        jstring jContainer, jstring jBlob, jint jParallelismFactor, jint jMaxExecutionTime, jobject ld, jobjectArray vda,
        jobjectArray vdc, jobject md)
{
    OpenVDS::AzureOpenOptions openOptions = OpenVDS::AzureOpenOptions();

    openOptions.connectionString = JStringToString(env, jConnectionString);
    openOptions.container = JStringToString(env, jContainer);
    openOptions.blob = JStringToString(env, jBlob);
    openOptions.parallelism_factor = jParallelismFactor;
    openOptions.max_execution_time = jMaxExecutionTime;

    auto descr = getDescriptor(env, ld);

    std::multiset<std::string> buffer;
    auto axisDescriptors = getValueDataAxisDescriptors(env, vda, buffer);
    auto channelDescriptors = getVolumeDataChannelDescriptor(env, vdc, buffer);
    auto* metadata = getMetadata(env, md);

    return createVDSOrThrowJavaIOException(env, openOptions, descr, axisDescriptors,
                                           channelDescriptors, *metadata);
}
