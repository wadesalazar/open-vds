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

#include <org_opengroup_openvds_VdsHandle.h>
#include <CommonJni.h>
#include <OpenVDS/OpenVDS.h>

using namespace OpenVDS;

#ifdef __cplusplus
extern "C" {
#endif

    inline VDSHandle GetVds( jlong handle ) {
        return (VDSHandle)CheckHandle( handle );
    }

    /*
    * Class:     org_opengroup_openvds_VdsHandle
    * Method:    cpDeleteHandle
    * Signature: (J)V
    */
    JNIEXPORT void JNICALL Java_org_opengroup_openvds_VdsHandle_cpDeleteHandle
    ( JNIEnv *env, jclass, jlong handle )
    {
        try {
            OpenVDS::Close( GetVds( handle ) );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
    }

    /*
    * Class:     org_opengroup_openvds_VdsHandle
    * Method:    cpGetLayoutHandle
    * Signature: (J)J
    */
    JNIEXPORT jlong JNICALL Java_org_opengroup_openvds_VdsHandle_cpGetLayoutHandle
    ( JNIEnv *env, jclass, jlong handle )
    {
        try {
            return  (jlong)OpenVDS::GetLayout( GetVds( handle ) );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return 0;
    }

    /*
    * Class:     org_opengroup_openvds_VdsHandle
    * Method:    cpGetAccessManagerHandle
    * Signature: (J)J
    */
    JNIEXPORT jlong JNICALL Java_org_opengroup_openvds_VdsHandle_cpGetAccessManagerHandle
    ( JNIEnv *env, jclass, jlong handle )
    {
        try {
            return (jlong)OpenVDS::GetAccessManager( GetVds( handle ) );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return 0;
    }

#ifdef __cplusplus
}
#endif
