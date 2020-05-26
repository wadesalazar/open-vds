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

#include <org_opengroup_openvds_QuantizingValueConverter_FloatToByte.h>
#include <CommonJni.h>
#include <OpenVDS/ValueConversion.h>

using namespace OpenVDS;

#ifdef __cplusplus
extern "C" {
#endif

    inline QuantizingValueConverterWithNoValue<uint8_t, float, false>* GetConvertor( jlong handle ) {
        return (QuantizingValueConverterWithNoValue<uint8_t, float, false>*)CheckHandle( handle );
    }

    /*
     * Class:     org_opengroup_openvds_QuantizingValueConverter_FloatToByte
     * Method:    cpCreateHandle
     * Signature: (FFFF)J
     */
    JNIEXPORT jlong JNICALL Java_org_opengroup_openvds_QuantizingValueConverter_1FloatToByte_cpCreateHandle
    ( JNIEnv *env, jclass, jfloat minValue, jfloat maxValue, jfloat intScale, jfloat intLayout )
    {
        try {
            return (jlong)new QuantizingValueConverterWithNoValue<uint8_t, float, false>(minValue, maxValue, intScale, intLayout, 0.f, 0.f);
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return 0;
    }

    /*
     * Class:     org_opengroup_openvds_QuantizingValueConverter_FloatToByte
     * Method:    cpDeleteHandle
     * Signature: (J)V
     */
    JNIEXPORT void JNICALL Java_org_opengroup_openvds_QuantizingValueConverter_1FloatToByte_cpDeleteHandle
    ( JNIEnv *env, jclass, jlong handle )
    {
        try {
            delete GetConvertor(handle);
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
    }

    /*
     * Class:     org_opengroup_openvds_QuantizingValueConverter_FloatToByte
     * Method:    cpConvertValue
     * Signature: (JF)B
     */
    JNIEXPORT jbyte JNICALL Java_org_opengroup_openvds_QuantizingValueConverter_1FloatToByte_cpConvertValue
    ( JNIEnv *env, jclass, jlong handle, jfloat val )
    {
        try {
            return GetConvertor(handle)->ConvertValue(val);
        }
        CATCH_EXCEPTIONS_FOR_JAVA;
        return 0;
    }

#ifdef __cplusplus
}
#endif
