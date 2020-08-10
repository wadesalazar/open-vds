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

#include <org_opengroup_openvds_MetadataReadAccess.h>
#include <CommonJni.h>
#include <OpenVDS/Metadata.h>

using namespace OpenVDS;

#ifdef __cplusplus
extern "C" {
#endif
    inline MetadataReadAccess* GetAccess( jlong handle ) {
        return (MetadataReadAccess*)CheckHandle( handle );
    }

    /*
     * Class:     org_opengroup_openvds_MetadataReadAccess
     * Method:    cpIsMetadataIntAvailable
     * Signature: (JLjava/lang/String;Ljava/lang/String;)Z
     */
    JNIEXPORT jboolean JNICALL Java_org_opengroup_openvds_MetadataReadAccess_cpIsMetadataIntAvailable
    ( JNIEnv *env, jclass, jlong handle, jstring category, jstring name )
    {
        try {
            return GetAccess( handle )->IsMetadataIntAvailable( JStringToString( env, category ).c_str(), JStringToString( env, name ).c_str() );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return false;
    }

    /*
     * Class:     org_opengroup_openvds_MetadataReadAccess
     * Method:    cpIsMetadataIntVector2Available
     * Signature: (JLjava/lang/String;Ljava/lang/String;)Z
     */
    JNIEXPORT jboolean JNICALL Java_org_opengroup_openvds_MetadataReadAccess_cpIsMetadataIntVector2Available
    ( JNIEnv *env, jclass, jlong handle, jstring category, jstring name )
    {
        try {
            return GetAccess( handle )->IsMetadataIntVector2Available( JStringToString( env, category ).c_str(), JStringToString( env, name ).c_str() );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return false;
    }

    /*
     * Class:     org_opengroup_openvds_MetadataReadAccess
     * Method:    cpIsMetadataIntVector3Available
     * Signature: (JLjava/lang/String;Ljava/lang/String;)Z
     */
    JNIEXPORT jboolean JNICALL Java_org_opengroup_openvds_MetadataReadAccess_cpIsMetadataIntVector3Available
    ( JNIEnv *env, jclass, jlong handle, jstring category, jstring name )
    {
        try {
            return GetAccess( handle )->IsMetadataIntVector3Available( JStringToString( env, category ).c_str(), JStringToString( env, name ).c_str() );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return false;
    }

    /*
     * Class:     org_opengroup_openvds_MetadataReadAccess
     * Method:    cpIsMetadataIntVector4Available
     * Signature: (JLjava/lang/String;Ljava/lang/String;)Z
     */
    JNIEXPORT jboolean JNICALL Java_org_opengroup_openvds_MetadataReadAccess_cpIsMetadataIntVector4Available
    ( JNIEnv *env, jclass, jlong handle, jstring category, jstring name )
    {
        try {
            return GetAccess( handle )->IsMetadataIntVector4Available( JStringToString( env, category ).c_str(), JStringToString( env, name ).c_str() );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return false;
    }

    /*
     * Class:     org_opengroup_openvds_MetadataReadAccess
     * Method:    cpIsMetadataFloatAvailable
     * Signature: (JLjava/lang/String;Ljava/lang/String;)Z
     */
    JNIEXPORT jboolean JNICALL Java_org_opengroup_openvds_MetadataReadAccess_cpIsMetadataFloatAvailable
    ( JNIEnv *env, jclass, jlong handle, jstring category, jstring name )
    {
        try {
            return GetAccess( handle )->IsMetadataFloatAvailable( JStringToString( env, category ).c_str(), JStringToString( env, name ).c_str() );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return false;
    }

    /*
     * Class:     org_opengroup_openvds_MetadataReadAccess
     * Method:    cpIsMetadataFloatVector2Available
     * Signature: (JLjava/lang/String;Ljava/lang/String;)Z
     */
    JNIEXPORT jboolean JNICALL Java_org_opengroup_openvds_MetadataReadAccess_cpIsMetadataFloatVector2Available
    ( JNIEnv *env, jclass, jlong handle, jstring category, jstring name )
    {
        try {
            return GetAccess( handle )->IsMetadataFloatVector2Available( JStringToString( env, category ).c_str(), JStringToString( env, name ).c_str() );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return false;
    }

    /*
     * Class:     org_opengroup_openvds_MetadataReadAccess
     * Method:    cpIsMetadataFloatVector3Available
     * Signature: (JLjava/lang/String;Ljava/lang/String;)Z
     */
    JNIEXPORT jboolean JNICALL Java_org_opengroup_openvds_MetadataReadAccess_cpIsMetadataFloatVector3Available
    ( JNIEnv *env, jclass, jlong handle, jstring category, jstring name )
    {
        try {
            return GetAccess( handle )->IsMetadataFloatVector3Available( JStringToString( env, category ).c_str(), JStringToString( env, name ).c_str() );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return false;
    }

    /*
     * Class:     org_opengroup_openvds_MetadataReadAccess
     * Method:    cpIsMetadataFloatVector4Available
     * Signature: (JLjava/lang/String;Ljava/lang/String;)Z
     */
    JNIEXPORT jboolean JNICALL Java_org_opengroup_openvds_MetadataReadAccess_cpIsMetadataFloatVector4Available
    ( JNIEnv *env, jclass, jlong handle, jstring category, jstring name )
    {
        try {
            return GetAccess( handle )->IsMetadataFloatVector4Available( JStringToString( env, category ).c_str(), JStringToString( env, name ).c_str() );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return false;
    }

    /*
     * Class:     org_opengroup_openvds_MetadataReadAccess
     * Method:    cpIsMetadataDoubleAvailable
     * Signature: (JLjava/lang/String;Ljava/lang/String;)Z
     */
    JNIEXPORT jboolean JNICALL Java_org_opengroup_openvds_MetadataReadAccess_cpIsMetadataDoubleAvailable
    ( JNIEnv *env, jclass, jlong handle, jstring category, jstring name )
    {
        try {
            return GetAccess( handle )->IsMetadataDoubleAvailable( JStringToString( env, category ).c_str(), JStringToString( env, name ).c_str() );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return false;
    }

    /*
     * Class:     org_opengroup_openvds_MetadataReadAccess
     * Method:    cpIsMetadataDoubleVector2Available
     * Signature: (JLjava/lang/String;Ljava/lang/String;)Z
     */
    JNIEXPORT jboolean JNICALL Java_org_opengroup_openvds_MetadataReadAccess_cpIsMetadataDoubleVector2Available
    ( JNIEnv *env, jclass, jlong handle, jstring category, jstring name )
    {
        try {
            return GetAccess( handle )->IsMetadataDoubleVector2Available( JStringToString( env, category ).c_str(), JStringToString( env, name ).c_str() );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return false;
    }

    /*
     * Class:     org_opengroup_openvds_MetadataReadAccess
     * Method:    cpIsMetadataDoubleVector3Available
     * Signature: (JLjava/lang/String;Ljava/lang/String;)Z
     */
    JNIEXPORT jboolean JNICALL Java_org_opengroup_openvds_MetadataReadAccess_cpIsMetadataDoubleVector3Available
    ( JNIEnv *env, jclass, jlong handle, jstring category, jstring name )
    {
        try {
            return GetAccess( handle )->IsMetadataDoubleVector3Available( JStringToString( env, category ).c_str(), JStringToString( env, name ).c_str() );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return false;
    }

    /*
     * Class:     org_opengroup_openvds_MetadataReadAccess
     * Method:    cpIsMetadataDoubleVector4Available
     * Signature: (JLjava/lang/String;Ljava/lang/String;)Z
     */
    JNIEXPORT jboolean JNICALL Java_org_opengroup_openvds_MetadataReadAccess_cpIsMetadataDoubleVector4Available
    ( JNIEnv *env, jclass, jlong handle, jstring category, jstring name )
    {
        try {
            return GetAccess( handle )->IsMetadataDoubleVector4Available( JStringToString( env, category ).c_str(), JStringToString( env, name ).c_str() );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return false;
    }

    /*
     * Class:     org_opengroup_openvds_MetadataReadAccess
     * Method:    cpIsMetadataStringAvailable
     * Signature: (JLjava/lang/String;Ljava/lang/String;)Z
     */
    JNIEXPORT jboolean JNICALL Java_org_opengroup_openvds_MetadataReadAccess_cpIsMetadataStringAvailable
    ( JNIEnv *env, jclass, jlong handle, jstring category, jstring name )
    {
        try {
            return GetAccess( handle )->IsMetadataStringAvailable( JStringToString( env, category ).c_str(), JStringToString( env, name ).c_str() );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return false;
    }

    /*
     * Class:     org_opengroup_openvds_MetadataReadAccess
     * Method:    cpIsMetadataBLOBAvailable
     * Signature: (JLjava/lang/String;Ljava/lang/String;)Z
     */
    JNIEXPORT jboolean JNICALL Java_org_opengroup_openvds_MetadataReadAccess_cpIsMetadataBLOBAvailable
    ( JNIEnv *env, jclass, jlong handle, jstring category, jstring name )
    {
        try {
            return GetAccess( handle )->IsMetadataBLOBAvailable( JStringToString( env, category ).c_str(), JStringToString( env, name ).c_str() );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return false;
    }

    /*
     * Class:     org_opengroup_openvds_MetadataReadAccess
     * Method:    cpGetMetadataInt
     * Signature: (JLjava/lang/String;Ljava/lang/String;)I
     */
    JNIEXPORT jint JNICALL Java_org_opengroup_openvds_MetadataReadAccess_cpGetMetadataInt
    ( JNIEnv *env, jclass, jlong handle, jstring category, jstring name )
    {
        try {
            return GetAccess( handle )->GetMetadataInt( JStringToString( env, category ).c_str(), JStringToString( env, name ).c_str() );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return 0;
    }

    /*
     * Class:     org_opengroup_openvds_MetadataReadAccess
     * Method:    cpGetMetadataIntVector2
     * Signature: (JLjava/lang/String;Ljava/lang/String;)[I
     */
    JNIEXPORT jintArray JNICALL Java_org_opengroup_openvds_MetadataReadAccess_cpGetMetadataIntVector2
    ( JNIEnv *env, jclass, jlong handle, jstring category, jstring name )
    {
        try {
            IntVector2 v = GetAccess( handle )->GetMetadataIntVector2( JStringToString( env, category ).c_str(), JStringToString( env, name ).c_str() );
            return NewJIntArray( env, v.data, v.element_count );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return NULL;
    }

    /*
     * Class:     org_opengroup_openvds_MetadataReadAccess
     * Method:    cpGetMetadataIntVector3
     * Signature: (JLjava/lang/String;Ljava/lang/String;)[I
     */
    JNIEXPORT jintArray JNICALL Java_org_opengroup_openvds_MetadataReadAccess_cpGetMetadataIntVector3
    ( JNIEnv *env, jclass, jlong handle, jstring category, jstring name )
    {
        try {
            IntVector3 v = GetAccess( handle )->GetMetadataIntVector3( JStringToString( env, category ).c_str(), JStringToString( env, name ).c_str() );
            return NewJIntArray( env, v.data, v.element_count );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return NULL;
    }

    /*
     * Class:     org_opengroup_openvds_MetadataReadAccess
     * Method:    cpGetMetadataIntVector4
     * Signature: (JLjava/lang/String;Ljava/lang/String;)[I
     */
    JNIEXPORT jintArray JNICALL Java_org_opengroup_openvds_MetadataReadAccess_cpGetMetadataIntVector4
    ( JNIEnv *env, jclass, jlong handle, jstring category, jstring name )
    {
        try {
            IntVector4 v = GetAccess( handle )->GetMetadataIntVector4( JStringToString( env, category ).c_str(), JStringToString( env, name ).c_str() );
            return NewJIntArray( env, v.data, v.element_count );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return NULL;
    }

    /*
     * Class:     org_opengroup_openvds_MetadataReadAccess
     * Method:    cpGetMetadataFloat
     * Signature: (JLjava/lang/String;Ljava/lang/String;)F
     */
    JNIEXPORT jfloat JNICALL Java_org_opengroup_openvds_MetadataReadAccess_cpGetMetadataFloat
    ( JNIEnv *env, jclass, jlong handle, jstring category, jstring name )
    {
        try {
            return GetAccess( handle )->GetMetadataFloat( JStringToString( env, category ).c_str(), JStringToString( env, name ).c_str() );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return 0;
    }

    /*
     * Class:     org_opengroup_openvds_MetadataReadAccess
     * Method:    cpGetMetadataFloatVector2
     * Signature: (JLjava/lang/String;Ljava/lang/String;)[F
     */
    JNIEXPORT jfloatArray JNICALL Java_org_opengroup_openvds_MetadataReadAccess_cpGetMetadataFloatVector2
    ( JNIEnv *env, jclass, jlong handle, jstring category, jstring name )
    {
        try {
            FloatVector2 v = GetAccess( handle )->GetMetadataFloatVector2( JStringToString( env, category ).c_str(), JStringToString( env, name ).c_str() );
            return NewJFloatArray( env, v.data, v.element_count );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return NULL;
    }

    /*
     * Class:     org_opengroup_openvds_MetadataReadAccess
     * Method:    cpGetMetadataFloatVector3
     * Signature: (JLjava/lang/String;Ljava/lang/String;)[F
     */
    JNIEXPORT jfloatArray JNICALL Java_org_opengroup_openvds_MetadataReadAccess_cpGetMetadataFloatVector3
    ( JNIEnv *env, jclass, jlong handle, jstring category, jstring name )
    {
        try {
            FloatVector3 v = GetAccess( handle )->GetMetadataFloatVector3( JStringToString( env, category ).c_str(), JStringToString( env, name ).c_str() );
            return NewJFloatArray( env, v.data, v.element_count );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return NULL;
    }

    /*
     * Class:     org_opengroup_openvds_MetadataReadAccess
     * Method:    cpGetMetadataFloatVector4
     * Signature: (JLjava/lang/String;Ljava/lang/String;)[F
     */
    JNIEXPORT jfloatArray JNICALL Java_org_opengroup_openvds_MetadataReadAccess_cpGetMetadataFloatVector4
    ( JNIEnv *env, jclass, jlong handle, jstring category, jstring name )
    {
        try {
            FloatVector4 v = GetAccess( handle )->GetMetadataFloatVector4( JStringToString( env, category ).c_str(), JStringToString( env, name ).c_str() );
            return NewJFloatArray( env, v.data, v.element_count );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return NULL;
    }

    /*
     * Class:     org_opengroup_openvds_MetadataReadAccess
     * Method:    cpGetMetadataDouble
     * Signature: (JLjava/lang/String;Ljava/lang/String;)D
     */
    JNIEXPORT jdouble JNICALL Java_org_opengroup_openvds_MetadataReadAccess_cpGetMetadataDouble
    ( JNIEnv *env, jclass, jlong handle, jstring category, jstring name )
    {
        try {
            return GetAccess( handle )->GetMetadataDouble( JStringToString( env, category ).c_str(), JStringToString( env, name ).c_str() );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return 0;
    }

    /*
     * Class:     org_opengroup_openvds_MetadataReadAccess
     * Method:    cpGetMetadataDoubleVector2
     * Signature: (JLjava/lang/String;Ljava/lang/String;)[D
     */
    JNIEXPORT jdoubleArray JNICALL Java_org_opengroup_openvds_MetadataReadAccess_cpGetMetadataDoubleVector2
    ( JNIEnv *env, jclass, jlong handle, jstring category, jstring name )
    {
        try {
            DoubleVector2 v = GetAccess( handle )->GetMetadataDoubleVector2( JStringToString( env, category ).c_str(), JStringToString( env, name ).c_str() );
            return NewJDoubleArray( env, v.data, v.element_count );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return NULL;
    }

    /*
     * Class:     org_opengroup_openvds_MetadataReadAccess
     * Method:    cpGetMetadataDoubleVector3
     * Signature: (JLjava/lang/String;Ljava/lang/String;)[D
     */
    JNIEXPORT jdoubleArray JNICALL Java_org_opengroup_openvds_MetadataReadAccess_cpGetMetadataDoubleVector3
    ( JNIEnv *env, jclass, jlong handle, jstring category, jstring name )
    {
        try {
            DoubleVector3 v = GetAccess( handle )->GetMetadataDoubleVector3( JStringToString( env, category ).c_str(), JStringToString( env, name ).c_str() );
            return NewJDoubleArray( env, v.data, v.element_count );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return NULL;
    }

    /*
     * Class:     org_opengroup_openvds_MetadataReadAccess
     * Method:    cpGetMetadataDoubleVector4
     * Signature: (JLjava/lang/String;Ljava/lang/String;)[D
     */
    JNIEXPORT jdoubleArray JNICALL Java_org_opengroup_openvds_MetadataReadAccess_cpGetMetadataDoubleVector4
    ( JNIEnv *env, jclass, jlong handle, jstring category, jstring name )
    {
        try {
            DoubleVector4 v = GetAccess( handle )->GetMetadataDoubleVector4( JStringToString( env, category ).c_str(), JStringToString( env, name ).c_str() );
            return NewJDoubleArray( env, v.data, v.element_count );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return NULL;
    }

    /*
     * Class:     org_opengroup_openvds_MetadataReadAccess
     * Method:    cpGetMetadataString
     * Signature: (JLjava/lang/String;Ljava/lang/String;)Ljava/lang/String;
     */
    JNIEXPORT jstring JNICALL Java_org_opengroup_openvds_MetadataReadAccess_cpGetMetadataString
    ( JNIEnv *env, jclass, jlong handle, jstring category, jstring name )
    {
        try {
            return NewJString( env, GetAccess( handle )->GetMetadataString(
                JStringToString( env, category ).c_str(), JStringToString( env, name ).c_str() ) );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return NULL;
    }

    /*
    * Class:     org_opengroup_openvds_MetadataReadAccess
    * Method:    cpGetMetadataKeys
    * Signature: (J)[Lorg/opengroup/openvds/MetadataKey;
    */
    JNIEXPORT jobjectArray JNICALL Java_org_opengroup_openvds_MetadataReadAccess_cpGetMetadataKeys
    ( JNIEnv *env, jclass, jlong handle )
    {
        try {
            jclass keyClass = env->FindClass( "Lorg/opengroup/openvds/MetadataKey;" );
            MetadataKeyRange keys = GetAccess( handle )->GetMetadataKeys();
            int nKeys = keys.end() - keys.begin();

            jmethodID initMethod = env->GetMethodID( keyClass, "<init>", "(ILjava/lang/String;Ljava/lang/String;)V" );
            jobjectArray jKeysArray = env->NewObjectArray( nKeys, keyClass, (jobject)NULL );

            int i = 0;
            for( MetadataKey key : keys ) {
                jobject jKey = env->NewObject( keyClass, initMethod,
                        (jint) key.GetType(),
                        NewJString( env, key.GetCategory() ),
                        NewJString( env, key.GetName() ) );
                env->SetObjectArrayElement( jKeysArray, i, jKey );
                ++i;
            }

            return jKeysArray;
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return 0;
    }

#ifdef __cplusplus
}
#endif
