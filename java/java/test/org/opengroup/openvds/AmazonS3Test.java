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

import org.testng.annotations.Test;

import java.io.IOException;
import java.nio.FloatBuffer;
import java.util.stream.IntStream;

import static org.testng.Assert.*;

public class AmazonS3Test {
    @Test
    void testAWSFail() {
        assertThrows(IOException.class, () -> OpenVDS.open(new AWSOpenOptions()));
    }

    @Test
    void testAzureFail() {
        assertThrows(IOException.class, () -> OpenVDS.open(new AzureOpenOptions()));
    }

    @Test
    void testAzurePresignedFail() {
        assertThrows(IOException.class, () -> OpenVDS.open(new AzurePresignedOpenOptions()));
    }

//    @Test
    void testVolumeSubsetRequestFloatLeak() throws IOException {
        AWSOpenOptions o = new AWSOpenOptions();
        o.key = "vds/alwynDepth_w_IL_XL_IEEEFloat";
        o.bucket = "openvds-test-int";
        o.region = "eu-west-3";
        try (OpenVDS openVDS = OpenVDS.open(o)) {
            assertTrue(!openVDS.isNull());
            assertTrue(openVDS.ownHandle());

            final VolumeDataLayout layout = openVDS.getLayout();
            assertTrue(!layout.isNull());

            int nXSamples = layout.getDimensionNumSamples(2);
            int nYSamples = layout.getDimensionNumSamples(1);
            int nZSamples = layout.getDimensionNumSamples(0);

            final int NB_TEST = 1000000;

            IntStream
                    .range(0, NB_TEST)
                    .mapToObj(iTest -> {
                        int iSlice = iTest % 10;
                        return new NDBox(
                                0, 0, iSlice, 0, 0, 0,
                                nZSamples, nYSamples, iSlice + 1, 0, 0, 0);
                    })
                    .forEach(box -> {
                                final FloatBuffer floatBuffer1 = BufferUtils.createFloatBuffer(nZSamples * nYSamples);
                                final FloatBuffer floatBuffer0 = BufferUtils.createFloatBuffer(nZSamples * nYSamples);
                                final VolumeDataAccessManager accessManager = openVDS.getAccessManager();
                                assertTrue(!accessManager.isNull());
                                final long requestID1 = accessManager.requestVolumeSubset(
                                        floatBuffer1, DimensionsND.DIMENSIONS_012, 0, 0, box);
                                final long requestID0 = accessManager.requestVolumeSubset(
                                        floatBuffer0, DimensionsND.DIMENSIONS_012, 0, 0, box, layout.getChannelNoValue(0));

                                System.out.println("Wait for request completion " + requestID1 + " ...");
                                waitAndDisplayProgress(accessManager, requestID1);
                                System.out.println("Wait for request completion " + requestID1 + " ...");
                                waitAndDisplayProgress(accessManager, requestID0);

                                for (int i = 0; i < nZSamples * nYSamples; i++) {
                                    final float f1 = floatBuffer0.get(i);
                                    final float f2 = floatBuffer1.get(i);
                                    assertEquals(0, Float.compare(f1, f2), " value " + i + " is different");
                                }

                                B.release(floatBuffer0, floatBuffer1);
                            }
                    );
        }
    }

    private static void waitAndDisplayProgress(VolumeDataAccessManager accessManager, long request) {
        while (!accessManager.waitForCompletion(request, 1000)) {
            if (accessManager.isCanceled(request)) throw new RuntimeException("Cancelled job");

            // Timeout, so let display progress
            final float completionFactor = accessManager.getCompletionFactor(request);
            System.out.println("Completion (" + request + ") " + completionFactor * 100. + " %");
        }
    }
}
