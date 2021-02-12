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
import org.opengroup.openvds.*;



public class WriteDataTest {
    @Test
    void roundtrip_2d() {
        final int nz = 113;
        final int nx = 128;

        AzureOpenOptions options = new AzureOpenOptions();
        options.connectionString = System.getenv("CONNECTION_STRING");

        if (options.connectionString == null) {
            return;
        }

        options.container = "test";
        options.blob = "rtrip_2d";

        String[] names = new String[]{"chan1", "chan2"};
        VolumeDataChannelDescriptor.Format f = VolumeDataChannelDescriptor.Format.FORMAT_R32;
        VdsHandle vds = new org.opengroup.openvds.AzureVdsGenerator(options, nz, nx, f, names);

        java.util.Random r = new java.util.Random();

        double[] src1 = r.doubles(nx * nz).toArray();
        double[] src2 = r.doubles(nx * nz).toArray();

        OpenVDS.writeArray(vds, src1, "chan1");
        OpenVDS.writeArray(vds, src2, "chan2");

        vds.getAccessManager().flushUploadQueue();
        VdsHandle vds2;

        try {
            vds2 = OpenVDS.open(options);
        } catch (java.io.IOException e) {
            fail();
            return;
        }

        VolumeDataAccessManager access = vds2.getAccessManager();
        VolumeDataLayout layout = vds2.getLayout();

        NDBox box = new NDBox(0, 0, 0, 0, 0, 0, nz, nx, 0, 0, 0, 0);

        int channel = layout.getChannelIndex("chan1");
        long size = access.getVolumeSubsetBufferSize(box, f, 0, channel);

        java.nio.FloatBuffer outbuf = BufferUtils.createFloatBuffer((int) size / 4);
        DimensionsND dims = DimensionsND.DIMENSIONS_01;

        long t = access.requestVolumeSubset(outbuf, dims, 0, channel, box);
        access.waitForCompletion(t);
        float[] arr = new float[outbuf.remaining()];
        outbuf.get(arr);
        assertEquals(arr[0], src1[0], 1e-6);
        assertEquals(arr[1], src1[1], 1e-6);
        assertEquals(arr[nx], src1[nx], 1e-6);
    }


    @Test
    void roundtrip_3d() {
        final int nz = 113;
        final int ny = 107;
        final int nx = 108;

        AzureOpenOptions options = new AzureOpenOptions();
        options.connectionString = System.getenv("CONNECTION_STRING");

        if (options.connectionString == null) {
            return;
        }

        options.container = "test";
        options.blob = "rtrip_3d";

        String[] names = new String[]{"chan1", "chan2"};
        VolumeDataChannelDescriptor.Format f = VolumeDataChannelDescriptor.Format.FORMAT_R32;
        VdsHandle vds = new org.opengroup.openvds.AzureVdsGenerator(options, nz, ny, nx, f, names);

        java.util.Random r = new java.util.Random();

        double[] src1 = r.doubles(nx * ny * nz).toArray();

        float[] src2 = new float[src1.length];
        for (int i = 0; i < src2.length; i++)
            src2[i] = i;

        OpenVDS.writeArray(vds, src1, "chan1");
        OpenVDS.writeArray(vds, src2, "chan2");

        vds.getAccessManager().flushUploadQueue();
        VdsHandle vds2;

        try {
            vds2 = OpenVDS.open(options);
        } catch (java.io.IOException e) {
            fail();
            return;
        }

        VolumeDataAccessManager access = vds2.getAccessManager();
        VolumeDataLayout layout = vds2.getLayout();

        NDBox box = new NDBox(0, 0, 0, 0, 0, 0, nz, ny, nx, 0, 0, 0);

        int channel = layout.getChannelIndex("chan2");
        long size = access.getVolumeSubsetBufferSize(box, f, 0, channel);

        java.nio.FloatBuffer outbuf = BufferUtils.createFloatBuffer((int) size / 4);
        DimensionsND dims = DimensionsND.DIMENSIONS_012;

        long t = access.requestVolumeSubset(outbuf, dims, 0, channel, box);
        access.waitForCompletion(t);
        float[] arr = new float[outbuf.remaining()];
        outbuf.get(arr);
        assertEquals(arr[0], src2[0], 1e-6);
        assertEquals(arr[nx], src2[nx], 1e-6);
        assertEquals(arr[nx * ny], src2[nx * ny], 1e-6);
        assertEquals(arr[nx * ny * nz - 42], src2[nx * ny * nz - 42], 1e-6);
    }
}
