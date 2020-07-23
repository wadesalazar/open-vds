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
 * Options for opening a VDS in Google cloud computing platform.
 */
public class GoogleOpenOptions extends OpenOptions {

    public String bucket;
    public String pathPrefix;

    /**
     * Default constructor.
     */
    public GoogleOpenOptions() {
        super(ConnectionType.GoogleStorage);
    }

    /**
     * Constructor.
     *
     * @param bucket The bucket of the VDS
     * @param pathPrefix The prefix of the VDS
     */
    public GoogleOpenOptions(String bucket, String pathPrefix) {
        super(ConnectionType.GoogleStorage);
        this.bucket = bucket;
        this.pathPrefix = pathPrefix;
    }
}
