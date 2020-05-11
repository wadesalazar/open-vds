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
public class AzureOpenOptions extends OpenOptions {

    public String connectionString, container, blob;
    public int parallelism_factor = 4;
    public int max_execution_time = 100000;

    /**
     * Default constructor.
     */
    public AzureOpenOptions() {
        super(ConnectionType.Azure);
    }

    /**
     * Constructor.
     *
     * @param pConnectionString the connectionString for the VDS
     * @param pContainer the container of the VDS
     * @param pBlob the blob prefix of the VDS
     */
    public AzureOpenOptions(String pConnectionString, String pContainer, String pBlob) {
        super(ConnectionType.Azure);
        connectionString = pConnectionString;
        container = pContainer;
        blob = pBlob;
    }

    /**
     * Constructor.
     *
     * @param pConnectionString the connectionString for the VDS
     * @param pContainer the container of the VDS
     * @param pBlob the blob prefix of the VDS
     * @param pParallelismFactor the parallelism factor setting for the Azure
     * Blob Storage library
     * @param pMaxExecutionTime the max execution time setting for the Azure
     * Blob Storage library
     */
    public AzureOpenOptions(String pConnectionString, String pContainer, String pBlob,
            int pParallelismFactor, int pMaxExecutionTime) {
        super(ConnectionType.Azure);
        connectionString = pConnectionString;
        container = pContainer;
        blob = pBlob;
        parallelism_factor = pParallelismFactor;
        max_execution_time = pMaxExecutionTime;
    }
}
