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

#include <org_opengroup_openvds_AzureVdsGenerator.h>
#include <CommonJni.h>

#include <OpenVDS/VolumeDataChannelDescriptor.h>


// Begin of stuff from open-vds-master/examples/SliceDump/GenerateVDS.h

#include <OpenVDS/OpenVDS.h>
#include <OpenVDS/VolumeDataLayoutDescriptor.h>
#include <OpenVDS/VolumeDataAxisDescriptor.h>
#include <OpenVDS/VolumeDataChannelDescriptor.h>
#include <OpenVDS/KnownMetadata.h>
#include <OpenVDS/GlobalMetadataCommon.h>
#include <OpenVDS/VolumeDataLayout.h>
#include <OpenVDS/VolumeDataAccess.h>


static OpenVDS::VDS *generate(const OpenVDS::AzureOpenOptions& opts, int32_t samplesX, int32_t samplesY, int32_t samplesZ,
                              OpenVDS::VolumeDataChannelDescriptor::Format format)
{
    auto brickSize = OpenVDS::VolumeDataLayoutDescriptor::BrickSize_32;
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
    axisDescriptors.emplace_back(samplesZ, KNOWNMETADATA_SURVEYCOORDINATE_INLINECROSSLINE_AXISNAME_INLINE,    "", 9985.f, 10369.f);

    std::vector<OpenVDS::VolumeDataChannelDescriptor> channelDescriptors;
    float rangeMin = -0.1234f;
    float rangeMax = 0.1234f;
    float intScale = 1.f;
    float intOffset = 0.f;
    channelDescriptors.emplace_back(format, OpenVDS::VolumeDataChannelDescriptor::Components_1,
            AMPLITUDE_ATTRIBUTE_NAME, "", rangeMin, rangeMax, OpenVDS::VolumeDataMapping::Direct, 1,
            OpenVDS::VolumeDataChannelDescriptor::Default, 0.f, intScale, intOffset);

    OpenVDS::MetadataContainer metadataContainer;
    OpenVDS::Error error;

    return OpenVDS::Create(opts, layoutDescriptor, axisDescriptors, channelDescriptors, metadataContainer, error);
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
            jobject azureOptions, jint nXSamples, jint nYSamples, jint nZSamples, jint format )
    {
        try {
            jclass obj_class = env->GetObjectClass(azureOptions);
            jfieldID conn_fid = env->GetFieldID(obj_class, "connectionString", "Ljava/lang/String;");
            jfieldID cont_fid = env->GetFieldID(obj_class, "container", "Ljava/lang/String;");
            jfieldID blob_fid = env->GetFieldID(obj_class, "blob", "Ljava/lang/String;");

            auto conn_str = JStringToString(env, (jstring) env->GetObjectField(azureOptions, conn_fid));
            auto cont_str = JStringToString(env, (jstring) env->GetObjectField(azureOptions, cont_fid));
            auto blob_str = JStringToString(env, (jstring) env->GetObjectField(azureOptions, blob_fid));

            OpenVDS::AzureOpenOptions opts(conn_str, cont_str, blob_str);
            OpenVDS::VDSHandle handle = generate(opts, nXSamples, nYSamples,
                    nZSamples, (OpenVDS::VolumeDataChannelDescriptor::Format)format );
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
