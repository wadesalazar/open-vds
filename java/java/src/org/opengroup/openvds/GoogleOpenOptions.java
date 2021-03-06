/*
 * Copyright 2020 The Open Group
** Copyright 2020 Bluware, Inc.
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
 * Options for opening a VDS in GS (Google Storage) cloud computing
 * platform.
 */
public class GoogleOpenOptions extends OpenOptions {

    public String bucket, pathPrefix;

    /**
     * Default constructor.
     */
    public GoogleOpenOptions() {
        super(ConnectionType.Google);
    }

    /**
     * Constructor.
     *
     * @param pBucket           the bucket of the VDS
     * @param pathPrefix        the prefix of the VDS
     */
    public GoogleOpenOptions(String pBucket, String pathPrefix) {
        super(ConnectionType.Google);
        bucket = pBucket;
        pathPrefix = pathPrefix;
    }

}
