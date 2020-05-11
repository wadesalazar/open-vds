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
#include <CommonJni.h>
#include <iostream>

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
        (JNIEnv *env, jclass, jstring jbucket, jstring jkey, jstring jregion) {
    OpenVDS::AWSOpenOptions openOptions;

    openOptions.key = JStringToString(env, jkey);
    openOptions.bucket = JStringToString(env, jbucket);
    openOptions.region = JStringToString(env, jregion);

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