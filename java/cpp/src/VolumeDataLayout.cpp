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

#include <org_opengroup_openvds_VolumeDataLayout.h>
#include <CommonJni.h>
#include <OpenVDS/VolumeDataLayout.h>

using namespace OpenVDS;

#ifdef __cplusplus
extern "C" {
#endif

    inline VolumeDataLayout* GetLayout( jlong handle ) {
        return (VolumeDataLayout*)CheckHandle( handle );
    }

    /*
     * Class:     org_opengroup_openvds_VolumeDataLayout
     * Method:    cpGetContentsHash
     * Signature: (J)J
     */
    JNIEXPORT jlong JNICALL Java_org_opengroup_openvds_VolumeDataLayout_cpGetContentsHash
    ( JNIEnv *env, jclass, jlong handle )
    {
        try {
            return GetLayout( handle )->GetContentsHash();
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return 0;
    }

    /*
     * Class:     org_opengroup_openvds_VolumeDataLayout
     * Method:    cpGetDimensionality
     * Signature: (J)I
     */
    JNIEXPORT jint JNICALL Java_org_opengroup_openvds_VolumeDataLayout_cpGetDimensionality
    ( JNIEnv *env, jclass, jlong handle )
    {
        try {
            return GetLayout( handle )->GetDimensionality();
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return 0;
    }

    /*
     * Class:     org_opengroup_openvds_VolumeDataLayout
     * Method:    cpGetChannelCount
     * Signature: (J)I
     */
    JNIEXPORT jint JNICALL Java_org_opengroup_openvds_VolumeDataLayout_cpGetChannelCount
    ( JNIEnv *env, jclass, jlong handle )
    {
        try {
            return GetLayout( handle )->GetChannelCount();
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return 0;
    }

    /*
     * Class:     org_opengroup_openvds_VolumeDataLayout
     * Method:    cpIsChannelAvailable
     * Signature: (JLjava/lang/String;)Z
     */
    JNIEXPORT jboolean JNICALL Java_org_opengroup_openvds_VolumeDataLayout_cpIsChannelAvailable
    ( JNIEnv *env, jclass, jlong handle, jstring jChannelName )
    {
        try {
            std::string channelName = JStringToString( env, jChannelName );
            return GetLayout( handle )->IsChannelAvailable( channelName.c_str() );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return false;
    }

    /*
     * Class:     org_opengroup_openvds_VolumeDataLayout
     * Method:    cpGetChannelIndex
     * Signature: (JLjava/lang/String;)I
     */
    JNIEXPORT jint JNICALL Java_org_opengroup_openvds_VolumeDataLayout_cpGetChannelIndex
    ( JNIEnv *env, jclass, jlong handle, jstring jChannelName )
    {
        try {
            std::string channelName = JStringToString( env, jChannelName );
            return GetLayout( handle )->GetChannelIndex( channelName.c_str() );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return 0;
    }

    /*
    * Class:     org_opengroup_openvds_VolumeDataLayout
    * Method:    cpGetLayoutDescriptor
    * Signature: (J)[I
    */
    JNIEXPORT jintArray JNICALL Java_org_opengroup_openvds_VolumeDataLayout_cpGetLayoutDescriptor
    ( JNIEnv *env, jclass, jlong handle )
    {
        try {
            VolumeDataLayoutDescriptor descr = GetLayout( handle )->GetLayoutDescriptor();
            int arr[8];
            arr[0] = descr.GetBrickSize();
            arr[1] = descr.GetNegativeMargin();
            arr[2] = descr.GetPositiveMargin();
            arr[3] = descr.GetBrickSizeMultiplier2D();
            arr[4] = descr.GetLODLevels();
            arr[5] = descr.IsCreate2DLODs();
            arr[6] = descr.IsForceFullResolutionDimension();
            arr[7] = descr.GetFullResolutionDimension();
            return NewJIntArray( env, arr, sizeof( arr ) / sizeof( int ) );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return 0;
    }

    /*
    * Class:     org_opengroup_openvds_VolumeDataLayout
    * Method:    cpGetChannelDescriptor
    * Signature: (JI)Lorg/opengroup/openvds/VolumeDataChannelDescriptor;
    */
    JNIEXPORT jobject JNICALL Java_org_opengroup_openvds_VolumeDataLayout_cpGetChannelDescriptor
    ( JNIEnv *env, jclass, jlong handle, jint channel )
    {
        try {
            VolumeDataChannelDescriptor descr = GetLayout( handle )->GetChannelDescriptor( channel );
            jclass cls = env->FindClass( "Lorg/opengroup/openvds/VolumeDataChannelDescriptor;" );
            jobject result = env->NewObject( cls, env->GetMethodID( cls, "<init>", "()V" ) );

            env->SetIntField( result, env->GetFieldID( cls, "format", "I" ), descr.GetFormat() );
            env->SetIntField( result, env->GetFieldID( cls, "components", "I" ), descr.GetComponents() );
            env->SetObjectField( result, env->GetFieldID( cls, "name", StringClassId ), NewJString( env, descr.GetName() ) );
            env->SetObjectField( result, env->GetFieldID( cls, "unit", StringClassId ), NewJString( env, descr.GetUnit() ) );
            env->SetFloatField( result, env->GetFieldID( cls, "valueRangeMin", "F" ), descr.GetValueRangeMin() );
            env->SetFloatField( result, env->GetFieldID( cls, "valueRangeMax", "F" ), descr.GetValueRangeMax() );
            env->SetLongField( result, env->GetFieldID( cls, "mapping", "J" ), (jlong)descr.GetMapping() );
            env->SetIntField( result, env->GetFieldID( cls, "mappedValueCount", "I" ), descr.GetMappedValueCount() );

            // bits of m_flags
            env->SetBooleanField( result, env->GetFieldID( cls, "isDiscrete", "Z" ), descr.IsDiscrete() );
            env->SetBooleanField( result, env->GetFieldID( cls, "isRenderable", "Z" ), descr.IsRenderable() );
            env->SetBooleanField( result, env->GetFieldID( cls, "isAllowLossyCompression", "Z" ), descr.IsAllowLossyCompression() );
            env->SetBooleanField( result, env->GetFieldID( cls, "isUseZipForLosslessCompression", "Z" ), descr.IsUseZipForLosslessCompression() );

            env->SetBooleanField( result, env->GetFieldID( cls, "useNoValue", "Z" ), descr.IsUseNoValue() );
            env->SetFloatField( result, env->GetFieldID( cls, "noValue", "F" ), descr.GetNoValue() );
            env->SetFloatField( result, env->GetFieldID( cls, "integerScale", "F" ), descr.GetIntegerScale() );
            env->SetFloatField( result, env->GetFieldID( cls, "integerOffset", "F" ), descr.GetIntegerOffset() );

            return result;
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return 0;
    }

    /*
    * Class:     org_opengroup_openvds_VolumeDataLayout
    * Method:    cpGetAxisDescriptor
    * Signature: (JI)Lorg/opengroup/openvds/VolumeDataAxisDescriptor;
    */
    JNIEXPORT jobject JNICALL Java_org_opengroup_openvds_VolumeDataLayout_cpGetAxisDescriptor
    ( JNIEnv *env, jclass, jlong handle, jint dimension )
    {
        try {
            VolumeDataAxisDescriptor descr = GetLayout( handle )->GetAxisDescriptor( dimension );
            jclass cls = env->FindClass( "Lorg/opengroup/openvds/VolumeDataAxisDescriptor;" );
            jobject result = env->NewObject( cls, env->GetMethodID( cls, "<init>", "()V" ) );

            env->SetIntField( result, env->GetFieldID( cls, "numSamples", "I" ), descr.GetNumSamples() );
            env->SetObjectField( result, env->GetFieldID( cls, "name", StringClassId ), NewJString( env, descr.GetName() ) );
            env->SetObjectField( result, env->GetFieldID( cls, "unit", StringClassId ), NewJString( env, descr.GetUnit() ) );
            env->SetFloatField( result, env->GetFieldID( cls, "coordinateMin", "F" ), descr.GetCoordinateMin() );
            env->SetFloatField( result, env->GetFieldID( cls, "coordinateMax", "F" ), descr.GetCoordinateMax() );

            return result;
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return 0;
    }

    /*
     * Class:     org_opengroup_openvds_VolumeDataLayout
     * Method:    cpGetChannelFormat
     * Signature: (JI)I
     */
    JNIEXPORT jint JNICALL Java_org_opengroup_openvds_VolumeDataLayout_cpGetChannelFormat
    ( JNIEnv *env, jclass, jlong handle, jint channel )
    {
        try {
            return GetLayout( handle )->GetChannelFormat( channel );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return 0;
    }

    /*
     * Class:     org_opengroup_openvds_VolumeDataLayout
     * Method:    cpGetChannelComponents
     * Signature: (JI)I
     */
    JNIEXPORT jint JNICALL Java_org_opengroup_openvds_VolumeDataLayout_cpGetChannelComponents
    ( JNIEnv *env, jclass, jlong handle, jint channel )
    {
        try {
            return GetLayout( handle )->GetChannelComponents( channel );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return 0;
    }

    /*
     * Class:     org_opengroup_openvds_VolumeDataLayout
     * Method:    cpGetChannelName
     * Signature: (JI)Ljava/lang/String;
     */
    JNIEXPORT jstring JNICALL Java_org_opengroup_openvds_VolumeDataLayout_cpGetChannelName
    ( JNIEnv *env, jclass, jlong handle, jint channel )
    {
        try {
            return NewJString( env, GetLayout( handle )->GetChannelName( channel ) );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return NULL;
    }

    /*
     * Class:     org_opengroup_openvds_VolumeDataLayout
     * Method:    cpGetChannelUnit
     * Signature: (JI)Ljava/lang/String;
     */
    JNIEXPORT jstring JNICALL Java_org_opengroup_openvds_VolumeDataLayout_cpGetChannelUnit
    ( JNIEnv *env, jclass, jlong handle, jint channel )
    {
        try {
            return NewJString( env, GetLayout( handle )->GetChannelUnit( channel ) );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return NULL;
    }

    /*
     * Class:     org_opengroup_openvds_VolumeDataLayout
     * Method:    cpGetChannelValueRangeMin
     * Signature: (JI)F
     */
    JNIEXPORT jfloat JNICALL Java_org_opengroup_openvds_VolumeDataLayout_cpGetChannelValueRangeMin
    ( JNIEnv *env, jclass, jlong handle, jint channel )
    {
        try {
            return GetLayout( handle )->GetChannelValueRangeMin( channel );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return 0;
    }

    /*
     * Class:     org_opengroup_openvds_VolumeDataLayout
     * Method:    cpGetChannelValueRangeMax
     * Signature: (JI)F
     */
    JNIEXPORT jfloat JNICALL Java_org_opengroup_openvds_VolumeDataLayout_cpGetChannelValueRangeMax
    ( JNIEnv *env, jclass, jlong handle, jint channel )
    {
        try {
            return GetLayout( handle )->GetChannelValueRangeMax( channel );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return 0;
    }

    /*
     * Class:     org_opengroup_openvds_VolumeDataLayout
     * Method:    cpIsChannelDiscrete
     * Signature: (JI)Z
     */
    JNIEXPORT jboolean JNICALL Java_org_opengroup_openvds_VolumeDataLayout_cpIsChannelDiscrete
    ( JNIEnv *env, jclass, jlong handle, jint channel )
    {
        try {
            return GetLayout( handle )->IsChannelDiscrete( channel );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return false;
    }

    /*
     * Class:     org_opengroup_openvds_VolumeDataLayout
     * Method:    cpIsChannelRenderable
     * Signature: (JI)Z
     */
    JNIEXPORT jboolean JNICALL Java_org_opengroup_openvds_VolumeDataLayout_cpIsChannelRenderable
    ( JNIEnv *env, jclass, jlong handle, jint channel )
    {
        try {
            return GetLayout( handle )->IsChannelRenderable( channel );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return false;
    }

    /*
     * Class:     org_opengroup_openvds_VolumeDataLayout
     * Method:    cpIsChannelAllowingLossyCompression
     * Signature: (JI)Z
     */
    JNIEXPORT jboolean JNICALL Java_org_opengroup_openvds_VolumeDataLayout_cpIsChannelAllowingLossyCompression
    ( JNIEnv *env, jclass, jlong handle, jint channel )
    {
        try {
            return GetLayout( handle )->IsChannelAllowingLossyCompression( channel );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return false;
    }

    /*
     * Class:     org_opengroup_openvds_VolumeDataLayout
     * Method:    cpIsChannelUseZipForLosslessCompression
     * Signature: (JI)Z
     */
    JNIEXPORT jboolean JNICALL Java_org_opengroup_openvds_VolumeDataLayout_cpIsChannelUseZipForLosslessCompression
    ( JNIEnv *env, jclass, jlong handle, jint channel )
    {
        try {
            return GetLayout( handle )->IsChannelUseZipForLosslessCompression( channel );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return false;
    }

    /*
     * Class:     org_opengroup_openvds_VolumeDataLayout
     * Method:    cpGetChannelMapping
     * Signature: (JI)J
     */
    JNIEXPORT jlong JNICALL Java_org_opengroup_openvds_VolumeDataLayout_cpGetChannelMapping
    ( JNIEnv *env, jclass, jlong handle, jint channel )
    {
        try {
            return (jlong)GetLayout( handle )->GetChannelMapping( channel );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return 0;
    }

    /*
     * Class:     org_opengroup_openvds_VolumeDataLayout
     * Method:    cpGetDimensionNumSamples
     * Signature: (JI)I
     */
    JNIEXPORT jint JNICALL Java_org_opengroup_openvds_VolumeDataLayout_cpGetDimensionNumSamples
    ( JNIEnv *env, jclass, jlong handle, jint dimension )
    {
        try {
            return GetLayout( handle )->GetDimensionNumSamples( dimension );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return 0;
    }

    /*
     * Class:     org_opengroup_openvds_VolumeDataLayout
     * Method:    cpGetDimensionName
     * Signature: (JI)Ljava/lang/String;
     */
    JNIEXPORT jstring JNICALL Java_org_opengroup_openvds_VolumeDataLayout_cpGetDimensionName
    ( JNIEnv *env, jclass, jlong handle, jint dimension )
    {
        try {
            return NewJString( env, GetLayout( handle )->GetDimensionName( dimension ) );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return NULL;
    }

    /*
     * Class:     org_opengroup_openvds_VolumeDataLayout
     * Method:    cpGetDimensionUnit
     * Signature: (JI)Ljava/lang/String;
     */
    JNIEXPORT jstring JNICALL Java_org_opengroup_openvds_VolumeDataLayout_cpGetDimensionUnit
    ( JNIEnv *env, jclass, jlong handle, jint dimension )
    {
        try {
            return NewJString( env, GetLayout( handle )->GetDimensionUnit( dimension ) );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return NULL;
    }

    /*
     * Class:     org_opengroup_openvds_VolumeDataLayout
     * Method:    cpGetDimensionMin
     * Signature: (JI)F
     */
    JNIEXPORT jfloat JNICALL Java_org_opengroup_openvds_VolumeDataLayout_cpGetDimensionMin
    ( JNIEnv *env, jclass, jlong handle, jint dimension )
    {
        try {
            return GetLayout( handle )->GetDimensionMin( dimension );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return 0;
    }

    /*
     * Class:     org_opengroup_openvds_VolumeDataLayout
     * Method:    cpGetDimensionMax
     * Signature: (JI)F
     */
    JNIEXPORT jfloat JNICALL Java_org_opengroup_openvds_VolumeDataLayout_cpGetDimensionMax
    ( JNIEnv *env, jclass, jlong handle, jint dimension )
    {
        try {
            return GetLayout( handle )->GetDimensionMax( dimension );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return 0;
    }

    /*
     * Class:     org_opengroup_openvds_VolumeDataLayout
     * Method:    cpIsChannelUseNoValue
     * Signature: (JI)Z
     */
    JNIEXPORT jboolean JNICALL Java_org_opengroup_openvds_VolumeDataLayout_cpIsChannelUseNoValue
    ( JNIEnv *env, jclass, jlong handle, jint channel )
    {
        try {
            return GetLayout( handle )->IsChannelUseNoValue( channel );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return false;
    }

    /*
     * Class:     org_opengroup_openvds_VolumeDataLayout
     * Method:    cpGetChannelNoValue
     * Signature: (JI)F
     */
    JNIEXPORT jfloat JNICALL Java_org_opengroup_openvds_VolumeDataLayout_cpGetChannelNoValue
    ( JNIEnv *env, jclass, jlong handle, jint channel )
    {
        try {
            return GetLayout( handle )->GetChannelNoValue( channel );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return 0;
    }

    /*
     * Class:     org_opengroup_openvds_VolumeDataLayout
     * Method:    cpGetChannelIntegerScale
     * Signature: (JI)F
     */
    JNIEXPORT jfloat JNICALL Java_org_opengroup_openvds_VolumeDataLayout_cpGetChannelIntegerScale
    ( JNIEnv *env, jclass, jlong handle, jint channel )
    {
        try {
            return GetLayout( handle )->GetChannelIntegerScale( channel );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return 0;
    }

    /*
     * Class:     org_opengroup_openvds_VolumeDataLayout
     * Method:    cpGetChannelIntegerOffset
     * Signature: (JI)F
     */
    JNIEXPORT jfloat JNICALL Java_org_opengroup_openvds_VolumeDataLayout_cpGetChannelIntegerOffset
    ( JNIEnv *env, jclass, jlong handle, jint channel )
    {
        try {
            return GetLayout( handle )->GetChannelIntegerOffset( channel );
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return 0;
    }

#ifdef __cplusplus
}
#endif
