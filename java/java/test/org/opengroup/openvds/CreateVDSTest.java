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

import static org.testng.Assert.*;
import org.testng.annotations.*;

public class CreateVDSTest {
    @BeforeClass
    public void init() {
        vds = new MemoryVdsGenerator(16, 16, 16, VolumeDataChannelDescriptor.Format.FORMAT_U8);
        o = new AzureOpenOptions();
        o.connectionString = System.getenv("CONNECTION_STRING");
        o.container = "test";
        o.blob = "test_vds_16_16_16";
        VolumeDataLayout volumeDataLayout = vds.getLayout();

        int nbChannel = volumeDataLayout.getChannelCount();
        VolumeDataAccessManager accessManager = vds.getAccessManager();

        for (VolumeDataLayoutDescriptor.LODLevels l : VolumeDataLayoutDescriptor.LODLevels.values()) {
            for (int channel = 0; channel < nbChannel; channel++) {
                for (DimensionsND dimGroup : DimensionsND.values()) {
                    VDSProduceStatus vdsProduceStatus = accessManager.getVDSProduceStatus(volumeDataLayout, dimGroup, l.ordinal(), channel);
                }
            }
        }

        vda = new VolumeDataAxisDescriptor[] {volumeDataLayout.getAxisDescriptor(0),
                                                volumeDataLayout.getAxisDescriptor(1),
                                                volumeDataLayout.getAxisDescriptor(2)};
        vdc = new VolumeDataChannelDescriptor[] {volumeDataLayout.getChannelDescriptor(0)};

        md = volumeDataLayout;
        ld = volumeDataLayout.getLayoutDescriptor();
    }


    @Test
    public void testCreateVDS() {
        try {
            if (o.connectionString == null) {
                return;
            }

            OpenVDS openvds1 = OpenVDS.create(o, ld, vda, vdc, md);
            openvds1.getAccessManager().flushUploadQueue();

            OpenVDS openvds2 = OpenVDS.open(o);

            VolumeDataLayout layout = openvds2.getLayout();

            assertEquals(layout.getDimensionality(), Dimensionality.DIMENSIONALITY_3);
            assertEquals(layout.getChannelCount(), 1);
            assertEquals(layout.getChannelFormat(0), VolumeDataChannelDescriptor.Format.FORMAT_U8);
            assertEquals(layout.getDimensionName(1), openvds1.getLayout().getDimensionName(1));
        } catch (java.io.IOException e) {
            System.out.println(e.getMessage());
            fail();
        }
    }

    @Test(expectedExceptions = IllegalArgumentException.class)
    public void testException1() {
        try {
           OpenVDS.create(null, ld, vda, vdc, md);
        } catch (java.io.IOException e) {
            fail();
        }
    }


    @Test(expectedExceptions = IllegalArgumentException.class)
    public void testException2() {
        try {
            OpenVDS.create(o, null, vda, vdc, md);
        } catch (java.io.IOException e) {
            fail();
        }
    }


    @Test(expectedExceptions = IllegalArgumentException.class)
    public void testException3() {
        try {
            OpenVDS.create(o, ld, null, vdc, md);
        } catch (java.io.IOException e) {
            fail();
        }
    }


    @Test(expectedExceptions = IllegalArgumentException.class)
    public void testException4() {
        try {
            OpenVDS.create(o, ld, vda, null, md);
        } catch (java.io.IOException e) {
            fail();
        }
    }


    @Test(expectedExceptions = IllegalArgumentException.class)
    public void testException5() {
        try {
            OpenVDS.create(o, ld, vda, vdc, null);
        } catch (java.io.IOException e) {
            fail();
        }
    }

    public AzureOpenOptions o;
    public VolumeDataLayoutDescriptor ld;
    public VolumeDataAxisDescriptor[] vda;
    public VolumeDataChannelDescriptor[] vdc;
    public MetadataReadAccess md;
    public MemoryVdsGenerator vds;
}
