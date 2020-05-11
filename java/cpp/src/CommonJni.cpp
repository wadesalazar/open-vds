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
#include "Common.h"
#include "CommonJni.h"
#include <assert.h>
#include <exception>
#include <cstring>


const char
StringClassId[] = "Ljava/lang/String;",
ExceptionClassId[] = "java/lang/Exception",
IOExceptionClassId[] = "java/io/IOException";

void* CheckHandle( jlong handle ) {
    if( !handle )
        ThrowCppException( "Handle is NULL" );
    return (void*)handle;
}

void ThrowJavaIOException( JNIEnv *env, const char* message) {
    env->ThrowNew( env->FindClass( IOExceptionClassId ), message );
}

void ThrowJavaException( JNIEnv *env, const char* format, ... ) {
    if( strchr( format, '%' ) ) {
        char buf[1000];
        va_list arg;
        va_start( arg, format );
        vsnprintf( buf, sizeof( buf ), format, arg );
        buf[sizeof( buf ) - 1] = 0;
        format = buf;
    }
    env->ThrowNew( env->FindClass( ExceptionClassId ), format );
}

void ThrowJavaExceptionUnknown( JNIEnv *env ) {
    env->ThrowNew( env->FindClass( ExceptionClassId ), "Unknown exception occured in C++ code" );
}

std::string JStringToString( JNIEnv* env, jstring jStr )
{
    if( jStr == NULL )
        return std::string();
    const char* chars = (env)->GetStringUTFChars( jStr, 0 );
    std::string str( chars );
    env->ReleaseStringUTFChars( jStr, chars );
    return str;
}


std::vector<char> JArrayToVector( JNIEnv* env, jbyteArray jArr ) {
    if( !jArr )
        return std::vector<char>();
    if( sizeof( jbyte ) != sizeof( char ) )
        ThrowJavaException( env, "sizeof(jbyte)!=sizeof(char)" );
    jsize len = env->GetArrayLength( jArr );
    std::vector<char> arr( len );
    env->GetByteArrayRegion( jArr, 0, len, (jbyte*)arr.data() );
    return arr;
}

std::vector<char> JArrayToVector( JNIEnv* env, jbyteArray jArr, int start, int len ) {
    if( !jArr )
        return std::vector<char>();
    if( sizeof( jbyte ) != sizeof( char ) )
        ThrowJavaException( env, "sizeof(jbyte)!=sizeof(char)" );
    std::vector<char> arr( len );
    env->GetByteArrayRegion( jArr, start, len, (jbyte*)arr.data() );
    return arr;
}

std::vector<short> JArrayToVector( JNIEnv* env, jshortArray jArr ) {
    if( !jArr )
        return std::vector<short>();
    if( sizeof( jshort ) != sizeof( short ) )
        ThrowJavaException( env, "sizeof(jshort)!=sizeof(short)" );
    jsize len = env->GetArrayLength( jArr );
    std::vector<short> arr( len );
    env->GetShortArrayRegion( jArr, 0, len, (jshort*)arr.data() );
    return arr;
}

std::vector<int> JArrayToVector( JNIEnv* env, jintArray jArr ) {
    if( !jArr )
        return std::vector<int>();
    if( sizeof( jint ) != sizeof( int ) )
        ThrowJavaException( env, "sizeof(jint)!=sizeof(int)" );
    jsize len = env->GetArrayLength( jArr );
    std::vector<int> arr( len );
    env->GetIntArrayRegion( jArr, 0, len, (jint*)arr.data() );
    return arr;
}

std::vector<float> JArrayToVector( JNIEnv* env, jfloatArray jArr ) {
    if( !jArr )
        return std::vector<float>();
    if( sizeof( jfloat ) != sizeof( float ) )
        ThrowJavaException( env, "sizeof(jfloat)!=sizeof(float)" );
    jsize len = env->GetArrayLength( jArr );
    std::vector<float> arr( len );
    env->GetFloatArrayRegion( jArr, 0, len, (jfloat*)arr.data() );
    return arr;
}


jstring NewJString( JNIEnv* env, const char* str ) {
    if( !str || !*str )
        return NULL;
    return env->NewStringUTF( str );
}

jstring NewJString( JNIEnv* env, const std::string& str ) {
    return NewJString( env, str.data() );
}


jobjectArray NewJStringArray( JNIEnv* env, const std::string* strings, size_t nStrings )
{
    if( !nStrings || !strings ) return NULL;

    jobjectArray jstrings = (jobjectArray)env->NewObjectArray(
        (jsize)nStrings, env->FindClass( "java/lang/String" ), NULL );

    for( size_t i = 0; i < nStrings; ++i ) {
        jstring jStr = NewJString( env, strings[i] );
        env->SetObjectArrayElement( jstrings, (jsize)i, jStr );
    }

    return jstrings;
}


jobjectArray NewJStringArray( JNIEnv* env, const std::vector<std::string>& ls )
{
    size_t n = ls.size();
    if( !n ) return NULL;

    jobjectArray jstrings = (jobjectArray)env->NewObjectArray(
        (jsize)n, env->FindClass( "java/lang/String" ), NULL );
    size_t i = 0;

    for( std::vector<std::string>::const_iterator it = ls.cbegin(); it != ls.cend() && i < n; ++it, ++i ) {
        const std::string& str = *it;
        jstring jStr = NewJString( env, str );
        env->SetObjectArrayElement( jstrings, (jsize)i, jStr );
    }

    return jstrings;
}


jbyteArray NewJByteArray( JNIEnv* env, const char* data, size_t count ) {
    jbyteArray jArr = env->NewByteArray( (jsize)count );
    if( sizeof( jbyte ) != sizeof( char ) )
        ThrowJavaException( env, "sizeof(jbyte)!=sizeof(char)" );
    env->SetByteArrayRegion( jArr, 0, (jsize)count, (jbyte*)data );
    return jArr;
}

jbyteArray NewJByteArray( JNIEnv* env, const std::string& str ) {
    if( str.empty() )
        return NULL;
    size_t len = str.length();
    jbyteArray arr = env->NewByteArray( (jsize)len );
    env->SetByteArrayRegion( arr, 0, (jsize)len, (jbyte*)str.c_str() );
    return arr;
}

jshortArray NewJShortArray( JNIEnv* env, const short* data, size_t count ) {
    jshortArray jArr = env->NewShortArray( (jsize)count );
    if( sizeof( jshort ) != sizeof( short ) )
        ThrowJavaException( env, "sizeof(jshort)!=sizeof(short)" );
    env->SetShortArrayRegion( jArr, 0, (jsize)count, data );
    return jArr;
}

jintArray NewJIntArray( JNIEnv* env, const int* data, size_t count ) {
    jintArray jArr = env->NewIntArray( (jsize)count );
    if( sizeof( jint ) != sizeof( int ) )
        ThrowJavaException( env, "sizeof(jint)!=sizeof(int)" );
    env->SetIntArrayRegion( jArr, 0, (jsize)count, (jint*)data );
    return jArr;
}

jlongArray NewJLongArray( JNIEnv* env, const int64_t* data, size_t count ) {
    jlongArray jArr = env->NewLongArray( (jsize)count );
    if( sizeof( jlong ) != sizeof( int64_t ) )
        ThrowJavaException( env, "sizeof(jint)!=sizeof(int)" );
    env->SetLongArrayRegion( jArr, 0, (jsize)count, (jlong*)data );
    return jArr;
}

jfloatArray NewJFloatArray( JNIEnv* env, const float* data, size_t count ) {
    jfloatArray jArr = env->NewFloatArray( (jsize)count );
    if( sizeof( jfloat ) != sizeof( float ) )
        ThrowJavaException( env, "sizeof(jfloat)!=sizeof(float)" );
    env->SetFloatArrayRegion( jArr, 0, (jsize)count, data );
    return jArr;
}

jdoubleArray NewJDoubleArray( JNIEnv* env, const double* data, size_t count ) {
    jdoubleArray jArr = env->NewDoubleArray( (jsize)count );
    if( sizeof( jfloat ) != sizeof( float ) )
        ThrowJavaException( env, "sizeof(jfloat)!=sizeof(float)" );
    env->SetDoubleArrayRegion( jArr, 0, (jsize)count, data );
    return jArr;
}
