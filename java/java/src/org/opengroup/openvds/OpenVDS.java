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

package org.opengroup.openvds;

import java.io.IOException;

public class OpenVDS extends VdsHandle{
    private static native long cpOpenAWS(String bucket, String key, String region, String endpointoverhide) throws IOException;

    private static native long cpOpenAzure(String pConnectionString, String pContainer, String pBlob,
                                           int pParallelismFactor, int pMaxExecutionTime) throws IOException;

    private static native long cpOpenGoogle(String bucket, String pathPrefix) throws IOException;

    private static native long cpOpenAzurePresigned(String baseUrl, String urlSuffix) throws IOException;

    private static native long cpOpenConnection(String url, String connectionString) throws IOException;

    private static native long cpCreateAzure(String pConnectionString, String pContainer, String pBlob,
                                             int pParallelismFactor, int pMaxExecutionTime,
                                             VolumeDataLayoutDescriptor ld, VolumeDataAxisDescriptor[] vda,
                                             VolumeDataChannelDescriptor[] vdc, MetadataReadAccess md, int compressionMethod, float compressionTolerance) throws IOException;


    private static native long cpCreateAws(String bucket, String key, String region, String endpointoverhide, String accessKeyId, String secretKey, String sessionToken, String expiration,
                                           VolumeDataLayoutDescriptor ld, VolumeDataAxisDescriptor[] vda,
                                           VolumeDataChannelDescriptor[] vdc, MetadataReadAccess md, int compressionMethod, float compressionTolerance) throws IOException;
    
    private static native long cpCreateGoogle(String bucket, String key,
                                              VolumeDataLayoutDescriptor ld, VolumeDataAxisDescriptor[] vda,
                                              VolumeDataChannelDescriptor[] vdc, MetadataReadAccess md, int compressionMethod, float compressionTolerance) throws IOException;
    
    private static native long cpCreateAzurePresigned(String baseUrl, String urlSuffix,
                                                      VolumeDataLayoutDescriptor ld, VolumeDataAxisDescriptor[] vda,
                                                      VolumeDataChannelDescriptor[] vdc, MetadataReadAccess md, int compressionMethod, float compressionTolerance) throws IOException;

    private static native long cpCreateConnection(String url, String connectionString,
                                                  VolumeDataLayoutDescriptor ld, VolumeDataAxisDescriptor[] vda,
                                                  VolumeDataChannelDescriptor[] vdc, MetadataReadAccess md, int compressionMethod, float compressionTolerance) throws IOException;

    private static native void cpWriteData_r64(JniPointer ptr, double[] src_data, String channel);
    private static native void cpWriteData_r32(JniPointer ptr, float[] src_data, String channel);
    private static native void cpWriteData_u32(JniPointer ptr, int[] src_data, String channel);
    private static native void cpWriteData_u8(JniPointer ptr, byte[] src_data, String channel);
    private static native void cpWriteData_bool(JniPointer ptr, boolean[] src_data, String channel);


    private OpenVDS(long handle, boolean ownHandle) {
        super(handle, ownHandle);
    }

    public static OpenVDS open(AWSOpenOptions o) throws IOException {
        if (o == null) throw new IllegalArgumentException("open option can't be null");
        return new OpenVDS(cpOpenAWS(o.bucket, o.key, o.region, o.endpointoverhide), true);
    }

    public static OpenVDS open(AzureOpenOptions o) throws IOException {
        if (o == null) throw new IllegalArgumentException("open option can't be null");
        return new OpenVDS(cpOpenAzure(o.connectionString, o.container, o.blob, o.parallelism_factor, o.max_execution_time), true);
    }

    public static OpenVDS open(AzurePresignedOpenOptions o) throws IOException {
        if (o == null) throw new IllegalArgumentException("open option can't be null");
        return new OpenVDS(cpOpenAzurePresigned(o.baseUrl, o.urlSuffix), true);
    }

    public static OpenVDS open(String url, String connectionString) throws IOException {
        if ("".equals(url)) throw new IllegalArgumentException("url can't be empty");
        return new OpenVDS(cpOpenConnection(url, connectionString), true);
    }
    
    private static void validateCreateArguments(VolumeDataLayoutDescriptor ld,
                                 VolumeDataAxisDescriptor[] vda, VolumeDataChannelDescriptor[] vdc,
                                 MetadataReadAccess md) throws IOException {

        if (ld == null) {
            throw new IllegalArgumentException("VolumeDataLayoutDescriptor can't be null");
        }

        if (vda == null || java.util.Arrays.stream(vda).allMatch(a -> {return a == null;})) {
            throw new IllegalArgumentException("VolumeDataLayoutDescriptor or its elements can't be null");
        }

        if (vdc == null || java.util.Arrays.stream(vdc).allMatch(a -> {return a == null;})) {
            throw new IllegalArgumentException("VolumeDataChannelDescriptor or its elements can't be null");
        }

        if (md == null) {
            throw new IllegalArgumentException("MetadataReadAccess can't be null");
        }
    }

    private static <OpenOpt> void validateCreateArguments(OpenOpt o, VolumeDataLayoutDescriptor ld,
                                 VolumeDataAxisDescriptor[] vda, VolumeDataChannelDescriptor[] vdc,
                                 MetadataReadAccess md) throws IOException {

        if (o == null) {
            throw new IllegalArgumentException("open option can't be null");
        }
        validateCreateArguments(ld, vda, vdc, md);
    }

    public static OpenVDS create(AzureOpenOptions o, VolumeDataLayoutDescriptor ld,
                                 VolumeDataAxisDescriptor[] vda, VolumeDataChannelDescriptor[] vdc,
                                 MetadataReadAccess md) throws IOException {
        validateCreateArguments(o, ld, vda, vdc, md);

        return new OpenVDS(cpCreateAzure(o.connectionString, o.container, o.blob, 
                                         o.parallelism_factor, o.max_execution_time,
                                         ld, vda, vdc, md, 0, 0), true);
    }

    public static OpenVDS create(AzureOpenOptions o, VolumeDataLayoutDescriptor ld,
                                 VolumeDataAxisDescriptor[] vda, VolumeDataChannelDescriptor[] vdc,
                                 MetadataReadAccess md, CompressionMethod compressionMethod, float compressionTolerance) throws IOException {
        validateCreateArguments(o, ld, vda, vdc, md);

        return new OpenVDS(cpCreateAzure(o.connectionString, o.container, o.blob, 
                                         o.parallelism_factor, o.max_execution_time,
                                         ld, vda, vdc, md, compressionMethod.ordinal(), compressionTolerance), true);
    }

    public static OpenVDS create(AWSOpenOptions o, VolumeDataLayoutDescriptor ld,
                                 VolumeDataAxisDescriptor[] vda, VolumeDataChannelDescriptor[] vdc,
                                 MetadataReadAccess md) throws IOException {
        validateCreateArguments(o, ld, vda, vdc, md);

        return new OpenVDS(cpCreateAws(o.bucket, o.key, o.region, o.endpointoverhide, o.accessKeyId, o.secretKey, o.sessionToken, o.expiration,
                                         ld, vda, vdc, md, 0, 0), true);
    }

    public static OpenVDS create(AWSOpenOptions o, VolumeDataLayoutDescriptor ld,
                                 VolumeDataAxisDescriptor[] vda, VolumeDataChannelDescriptor[] vdc,
                                 MetadataReadAccess md, CompressionMethod compressionMethod, float compressionTolerance) throws IOException {
        validateCreateArguments(o, ld, vda, vdc, md);

        return new OpenVDS(cpCreateAws(o.bucket, o.key, o.region, o.endpointoverhide, o.accessKeyId, o.secretKey, o.sessionToken, o.expiration,
                                         ld, vda, vdc, md, compressionMethod.ordinal(), compressionTolerance), true);
    }

    public static OpenVDS create(GoogleOpenOptions o, VolumeDataLayoutDescriptor ld,
                                 VolumeDataAxisDescriptor[] vda, VolumeDataChannelDescriptor[] vdc,
                                 MetadataReadAccess md) throws IOException {
        validateCreateArguments(o, ld, vda, vdc, md);

        return new OpenVDS(cpCreateGoogle(o.bucket, o.pathPrefix, 
                                         ld, vda, vdc, md, 0, 0), true);
    }

    public static OpenVDS create(GoogleOpenOptions o, VolumeDataLayoutDescriptor ld,
                                 VolumeDataAxisDescriptor[] vda, VolumeDataChannelDescriptor[] vdc,
                                 MetadataReadAccess md, CompressionMethod compressionMethod, float compressionTolerance) throws IOException {
        validateCreateArguments(o, ld, vda, vdc, md);

        return new OpenVDS(cpCreateGoogle(o.bucket, o.pathPrefix, 
                                         ld, vda, vdc, md, compressionMethod.ordinal(), compressionTolerance), true);
    }

    public static OpenVDS create(AzurePresignedOpenOptions o, VolumeDataLayoutDescriptor ld,
                                 VolumeDataAxisDescriptor[] vda, VolumeDataChannelDescriptor[] vdc,
                                 MetadataReadAccess md) throws IOException {
        validateCreateArguments(o, ld, vda, vdc, md);

        return new OpenVDS(cpCreateAzurePresigned(o.baseUrl, o.urlSuffix,
                                         ld, vda, vdc, md, 0, 0), true);
    }

    public static OpenVDS create(AzurePresignedOpenOptions o, VolumeDataLayoutDescriptor ld,
                                 VolumeDataAxisDescriptor[] vda, VolumeDataChannelDescriptor[] vdc,
                                 MetadataReadAccess md, CompressionMethod compressionMethod, float compressionTolerance) throws IOException {
        validateCreateArguments(o, ld, vda, vdc, md);

        return new OpenVDS(cpCreateAzurePresigned(o.baseUrl, o.urlSuffix,
                                         ld, vda, vdc, md, compressionMethod.ordinal(), compressionTolerance), true);
    }

    public static OpenVDS create(String url, String connectionString, VolumeDataLayoutDescriptor ld,
                                 VolumeDataAxisDescriptor[] vda, VolumeDataChannelDescriptor[] vdc,
                                 MetadataReadAccess md) throws IOException {
        if (url == null) {
            throw new IllegalArgumentException("url can't be null");
        }

        validateCreateArguments(ld, vda, vdc, md);

        return new OpenVDS(cpCreateConnection(url, connectionString,
                                         ld, vda, vdc, md, 0, 0), true);
    }

    public static OpenVDS create(String url, String connectionString, VolumeDataLayoutDescriptor ld,
                                 VolumeDataAxisDescriptor[] vda, VolumeDataChannelDescriptor[] vdc,
                                 MetadataReadAccess md, CompressionMethod compressionMethod, float compressionTolerance) throws IOException {
        if (url == null) {
            throw new IllegalArgumentException("url can't be null");
        }

        validateCreateArguments(ld, vda, vdc, md);

        return new OpenVDS(cpCreateConnection(url, connectionString,
                                         ld, vda, vdc, md, compressionMethod.ordinal(), compressionTolerance), true);
    }

    public static void writeArray(VdsHandle handle, double[] src_data, String channel_name) {
        cpWriteData_r64(handle, src_data, channel_name);
    }

    public static void writeArray(VdsHandle handle, float[] src_data, String channel_name) {
        cpWriteData_r32(handle, src_data, channel_name);
    }

    public static void writeArray(VdsHandle handle, int[] src_data, String channel_name) {
        cpWriteData_u32(handle, src_data, channel_name);
    }

    public static void writeArray(VdsHandle handle, byte[] src_data, String channel_name) {
        cpWriteData_u8(handle, src_data, channel_name);
    }

    public static void writeArray(VdsHandle handle, boolean[] src_data, String channel_name) {
        cpWriteData_bool(handle, src_data, channel_name);
    }
}
