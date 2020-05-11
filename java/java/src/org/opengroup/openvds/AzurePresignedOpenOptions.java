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
 * Options for opening a VDS in Microsoft Azure cloud computing platform.
 */
public class AzurePresignedOpenOptions extends OpenOptions {

    public String baseUrl;
    public String urlSuffix;

    /**
     * Default constructor.
     */
    public AzurePresignedOpenOptions() {
        super(ConnectionType.Azure);
    }

    /**
     * Constructor.
     *
     * @param baseUrl the connectionString for the VDS
     * @param urlSuffix the container of the VDS
     */
    public AzurePresignedOpenOptions(String baseUrl, String urlSuffix) {
        super(ConnectionType.AzurePresigned);
        this.baseUrl = baseUrl;
        this.urlSuffix = urlSuffix;
    }
}
