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

#include <org_opengroup_openvds_AzureVdsGenerator.h>
#include <CommonJni.h>

#include <OpenVDS/OpenVDS.h>
#include <OpenVDS/VolumeDataLayoutDescriptor.h>
#include <OpenVDS/VolumeDataAxisDescriptor.h>
#include <OpenVDS/VolumeDataChannelDescriptor.h>
#include <OpenVDS/KnownMetadata.h>

#include <limits>
#include <string>
#include <vector>


static OpenVDS::VDS *generate(const OpenVDS::AzureOpenOptions& opts, int32_t samplesX, int32_t samplesY, int32_t samplesZ,
                              OpenVDS::VolumeDataChannelDescriptor::Format format, const std::vector<std::string>& channels)
{
    OpenVDS::VolumeDataLayoutDescriptor::BrickSize brickSize;
    if (samplesZ == 0) {
        brickSize = OpenVDS::VolumeDataLayoutDescriptor::BrickSize_1024;
    } else {
        brickSize = OpenVDS::VolumeDataLayoutDescriptor::BrickSize_128;
    }

    int negativeMargin = 4;
    int positiveMargin = 4;
    int brickSize2DMultiplier = 4;
    auto lodLevels = OpenVDS::VolumeDataLayoutDescriptor::LODLevels_None;
    auto layoutOptions = OpenVDS::VolumeDataLayoutDescriptor::Options_None;
    OpenVDS::VolumeDataLayoutDescriptor layoutDescriptor(brickSize, negativeMargin, positiveMargin,
                                                         brickSize2DMultiplier, lodLevels, layoutOptions);

    std::vector<OpenVDS::VolumeDataAxisDescriptor> axisDescriptors;
    axisDescriptors.emplace_back(samplesX, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_SAMPLE, "ms", 0.0f, 4.f);
    axisDescriptors.emplace_back(samplesY, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_CROSSLINE, "", 1932.f, 2536.f);

    if (samplesZ != 0) {
        axisDescriptors.emplace_back(samplesZ, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_INLINE,    "", 9985.f, 10369.f);
    }

    std::vector<OpenVDS::VolumeDataChannelDescriptor> channelDescriptors;

    float rangeMin = -0.1234f;
    float rangeMax = 0.1234f;
    float intScale = 1.f;
    float intOffset = 0.f;

    for (auto& name : channels) {
        channelDescriptors.emplace_back(format, OpenVDS::VolumeDataChannelDescriptor::Components_1,
            name.c_str(), "", rangeMin, rangeMax, OpenVDS::VolumeDataMapping::Direct, 1,
            OpenVDS::VolumeDataChannelDescriptor::Default, std::numeric_limits<float>::lowest(), intScale, intOffset);
    }

    OpenVDS::MetadataContainer metadataContainer;
    OpenVDS::Error error;

    return OpenVDS::Create(opts, layoutDescriptor, axisDescriptors, channelDescriptors, metadataContainer, error);
}


static std::string getStringField(JNIEnv *env, jobject obj, const char *fieldName)
{
    jclass obj_class = env->GetObjectClass(obj);
    jfieldID fid = env->GetFieldID(obj_class, fieldName, "Ljava/lang/String;");
    return JStringToString(env, (jstring) env->GetObjectField(obj, fid));
}

static std::vector<std::string> convertStringArray(JNIEnv *env, jobjectArray obj)
{
    std::vector<std::string> res;

    for (int i = 0; i < env->GetArrayLength(obj); i++) {
        jstring jstr = (jstring) env->GetObjectArrayElement(obj, i);
        res.push_back(JStringToString(env, jstr));
    }

    return res;
}

#ifdef __cplusplus
extern "C" {
#endif

    /*
     * Class:     org_opengroup_openvds_AzureVdsGenerator
     * Method:    cpCreateAzureHandle
     * Signature: (Lorg/opengroup/openvds/AzureOpenOptions;IIII)J
     */
    jlong JNICALL Java_org_opengroup_openvds_AzureVdsGenerator_cpCreateAzureHandle(JNIEnv *env, jclass,
            jobject azureOptions, jint nXSamples, jint nYSamples, jint nZSamples, jint format,
            jobjectArray jChannelNames)
    {
        try {
            std::string conn_str = getStringField(env, azureOptions, "connectionString");
            std::string cont_str = getStringField(env, azureOptions, "container");
            std::string blob_str = getStringField(env, azureOptions, "blob");

            OpenVDS::AzureOpenOptions opts(conn_str, cont_str, blob_str);
            auto channel_names = convertStringArray(env, jChannelNames);


            OpenVDS::VDSHandle handle = generate(opts, nXSamples, nYSamples, nZSamples, 
                                                 (OpenVDS::VolumeDataChannelDescriptor::Format)format, channel_names);

            if (!handle) {
                ThrowJavaException(env,"OpenVDS::Create returned NULL");
                return 0;
            }

            return (jlong) handle; // convert struct VDS * to Java long
        }

        CATCH_EXCEPTIONS_FOR_JAVA;

        return 0;
    }

#ifdef __cplusplus
}
#endif
