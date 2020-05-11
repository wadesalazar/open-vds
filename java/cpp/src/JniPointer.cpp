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

#include <org_opengroup_openvds_JniPointer.h>
#include <CommonJni.h>
#include <Portability.h>

#ifdef __cplusplus
extern "C" {
#endif

    /*
    * Class:     org_opengroup_openvds_JniPointer
    * Method:    cpLibraryDescription
    * Signature: ()Ljava/lang/String;
    */
    JNIEXPORT jstring JNICALL Java_org_opengroup_openvds_JniPointer_cpLibraryDescription
    ( JNIEnv *env, jclass )
    {
        const char str[] =
#if __PLATFORM == __PLATFORM_SparcCompiler
            "SparcCompiler"
#elif __PLATFORM == __PLATFORM_SparcGcc
            "SparcGcc"
#elif __PLATFORM == __PLATFORM_EgcsLinux
            "EgcsLinux"
#elif __PLATFORM == __PLATFORM_Win32MSVC
            "Win32MSVC"
#ifdef NDEBUG
            " release"
#else
            " debug"
#endif
#elif __PLATFORM == __PLATFORM_Win32MinGW
            "Win32MinGW"
#elif __PLATFORM == __PLATFORM_SgiCompiler
            "SgiCompiler"
#elif __PLATFORM == __PLATFORM_IbmCompiler
            "IbmCompiler"
#elif __PLATFORM == __PLATFORM_HpCompiler
            "HpCompiler"
#elif __PLATFORM == __PLATFORM_LinuxIcc
            "LinuxIcc"
#elif __PLATFORM == __PLATFORM_MacIcc
            "MacIcc"
#else
            "Unknown platform"
#endif

            ". Built " __DATE__ " " __TIME__;

        return NewJString( env, str );
    }

#ifdef __cplusplus
}
#endif
