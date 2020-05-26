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

/**
 * Options for opening a VDS in AWS (Amazon Web Services) cloud computing
 * platform.
 */
public class AWSOpenOptions extends OpenOptions {

    public String bucket, key, region, endpointoverhide;

    /**
     * Default constructor.
     */
    public AWSOpenOptions() {
        super(ConnectionType.AWS);
    }

    /**
     * Constructor.
     *
     * @param pBucket           the bucket of the VDS
     * @param pKey              the key prefix of the VDS
     * @param pRegion           the region of the bucket of the VDS
     * @param pEndpointOverride This parameter allows to override the endpoint url
     */
    public AWSOpenOptions(String pBucket, String pKey, String pRegion, String pEndpointOverride) {
        super(ConnectionType.AWS);
        bucket = pBucket;
        key = pKey;
        region = pRegion;
        endpointoverhide = pEndpointOverride;
    }

    /**
     * Constructor.
     *
     * @param pBucket the bucket of the VDS
     * @param pKey    the key prefix of the VDS
     * @param pRegion the region of the bucket of the VDS
     */
    public AWSOpenOptions(String pBucket, String pKey, String pRegion) {
        this(pBucket, pKey, pRegion, null);
    }
}
