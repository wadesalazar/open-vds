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

#ifndef _COMMONJNI_H_
#define _COMMONJNI_H_

#include <jni.h>
#include <string>
#include <vector>

extern const char StringClassId[], ExceptionClassId[], IOExceptionClassId[];

void* CheckHandle( jlong handle );

void ThrowJavaIOException( JNIEnv *env, const char* message);
void ThrowJavaException( JNIEnv *env, const char* format, ... );
void ThrowJavaExceptionUnknown( JNIEnv *env );

#define CATCH_EXCEPTIONS_FOR_JAVA \
catch( const std::exception& se ) { ThrowJavaException( env, se.what() ); } \
catch( ... ) { ThrowJavaExceptionUnknown( env ); }

std::string JStringToString( JNIEnv* env, jstring jstr );

std::vector<char> JArrayToVector( JNIEnv* env, jbyteArray jArr );
std::vector<char> JArrayToVector( JNIEnv* env, jbyteArray jArr, int start, int len );
std::vector<short> JArrayToVector( JNIEnv* env, jshortArray jArr );
std::vector<int> JArrayToVector( JNIEnv* env, jintArray jArr );
std::vector<float> JArrayToVector( JNIEnv* env, jfloatArray jArr );

jbyteArray NewJByteArray( JNIEnv* env, const std::string& str );
jstring NewJString( JNIEnv* env, const char* str );
jstring NewJString( JNIEnv* env, const std::string& str );

jobjectArray NewJStringArray( JNIEnv* env, const std::string* strings, size_t nStrings );
jobjectArray NewJStringArray( JNIEnv* env, const std::vector<std::string>& strings );

jbyteArray NewJByteArray( JNIEnv* env, const char* data, size_t count );
jshortArray NewJShortArray( JNIEnv* env, const short* data, size_t count );
jintArray NewJIntArray( JNIEnv* env, const int* data, size_t count );
jlongArray NewJLongArray( JNIEnv* env, const int64_t* data, size_t count );
jfloatArray NewJFloatArray( JNIEnv* env, const float* data, size_t count );
jdoubleArray NewJDoubleArray( JNIEnv* env, const double* data, size_t count );

#endif