/*
 * Copyright 2020 The Open Group
 * Copyright 2020 Bluware, Inc.
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


public class AzureVdsGenerator extends VdsHandle {

    private static native long cpCreateAzureHandle(AzureOpenOptions o, int nXSamples, int nYSamples, int nZSamples,
            int format, String[] channel);

    public AzureVdsGenerator(AzureOpenOptions o, int nXSamples, int nYSamples, int nZSamples,
            VolumeDataChannelDescriptor.Format format, String[] channel) {
        super(cpCreateAzureHandle(o, nXSamples, nYSamples, nZSamples, format.getCode(), channel), true);
    }

    public AzureVdsGenerator(AzureOpenOptions o, int nXSamples, int nYSamples,
            VolumeDataChannelDescriptor.Format format, String[] channel) {
        super(cpCreateAzureHandle(o, nXSamples, nYSamples, 0, format.getCode(), channel), true);
    }
}
