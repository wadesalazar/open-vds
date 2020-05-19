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
import java.nio.ByteBuffer;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import java.util.LinkedList;
import java.util.Random;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.stream.IntStream;

import static org.opengroup.openvds.Dimensionality.DIMENSIONALITY_MAX;
import static org.opengroup.openvds.VolumeDataChannelDescriptor.Format.*;
import static org.opengroup.openvds.VolumeDataLayoutDescriptor.BrickSize.BRICK_SIZE_32;
import static org.opengroup.openvds.VolumeDataLayoutDescriptor.LODLevels.LOD_LEVELS_NONE;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertTrue;

public class MemoryVdsGeneratorTest {

    @Test
    void testOpenClose() throws IOException {
        int nXSamples = 60, nYSamples = 60, nZSamples = 60;
        VolumeDataChannelDescriptor.Format format = FORMAT_U8;
        MemoryVdsGenerator generator = new MemoryVdsGenerator(nXSamples, nYSamples, nZSamples, format);
        assertTrue(!generator.isNull());
        assertTrue(generator.ownHandle());

        assertTrue(!generator.getLayout().isNull());
        assertTrue(!generator.getAccessManager().isNull());

        generator.close();
        assertTrue(generator.isNull());
    }

    @Test
    void testVolumeTracesVsSampleRequest() throws IOException {
        int nXSamples = 60, nYSamples = 60, nZSamples = 60;
        VolumeDataChannelDescriptor.Format format = FORMAT_R32;
        MemoryVdsGenerator generator = new MemoryVdsGenerator(nZSamples, nYSamples, nXSamples, format);
        assertTrue(!generator.isNull());
        assertTrue(generator.ownHandle());

        final VolumeDataLayout layout = generator.getLayout();
        assertTrue(!layout.isNull());

        final VolumeDataAccessManager accessManager = generator.getAccessManager();
        assertTrue(!accessManager.isNull());

        final FloatBuffer tracePositions = B.createFloatBuffer(nYSamples * DIMENSIONALITY_MAX.getNbDimension());
        final FloatBuffer samplePositions = B.createFloatBuffer(nZSamples * nYSamples * DIMENSIONALITY_MAX.getNbDimension());
        for (int j = 0; j < nYSamples; j++) {
            tracePositions.put(0.5f);
            tracePositions.put(j + 0.5f);
            tracePositions.put(0.5f);
            tracePositions.put(0.5f);
            tracePositions.put(0.5f);
            tracePositions.put(0.5f);

            for (int k = 0; k < nZSamples; k++) {
                samplePositions.put(k + 0.5f);
                samplePositions.put(j + 0.5f);
                samplePositions.put(0.5f);
                samplePositions.put(0.5f);
                samplePositions.put(0.5f);
                samplePositions.put(0.5f);
            }
        }

        final FloatBuffer floatBuffer1 = B.createFloatBuffer(nZSamples * nYSamples);
        final long requestSubsetId = accessManager.requestVolumeTraces(floatBuffer1, layout, DimensionsND.DIMENSIONS_012, 0, 0,
                tracePositions, nYSamples, InterpolationMethod.NEAREST, 0);
        accessManager.waitForCompletion(requestSubsetId);

        final FloatBuffer floatBuffer0 = B.createFloatBuffer(nZSamples * nYSamples);
        final long requestId0 = accessManager.requestVolumeSamples(
                floatBuffer0, layout, DimensionsND.DIMENSIONS_012, 0, 0,
                samplePositions, nZSamples * nYSamples, InterpolationMethod.NEAREST);
        accessManager.waitForCompletion(requestId0);

        B.release(samplePositions, tracePositions);

        for (int i = 0; i < nZSamples * nYSamples; i++) {
            final float f1 = floatBuffer0.get(i);
            final float f2 = floatBuffer1.get(i);
            assertEquals(0, Float.compare(f1, f2), " value " + i + " is different");
        }

        B.release(floatBuffer0, floatBuffer1);

        generator.close();
        assertTrue(generator.isNull());
    }

    @Test
    void testVolumeTraces() throws IOException {
        int nXSamples = 60, nYSamples = 60, nZSamples = 60;
        VolumeDataChannelDescriptor.Format format = FORMAT_R32;
        MemoryVdsGenerator generator = new MemoryVdsGenerator(nZSamples, nYSamples, nXSamples, format);
        assertTrue(!generator.isNull());
        assertTrue(generator.ownHandle());

        final VolumeDataLayout layout = generator.getLayout();
        assertTrue(!layout.isNull());

        final VolumeDataAccessManager accessManager = generator.getAccessManager();
        assertTrue(!accessManager.isNull());

        final long volumeTracesBufferSize = accessManager.getVolumeTracesBufferSize(layout, nYSamples, 0, 0);
        assertEquals(nZSamples * nYSamples * Float.BYTES, volumeTracesBufferSize);

        final FloatBuffer tracePositions = B.createFloatBuffer(nYSamples * DIMENSIONALITY_MAX.getNbDimension());
        for (int j = 0; j < nYSamples; j++) {
            tracePositions.put(0.5f);
            tracePositions.put(j + 0.5f);
            tracePositions.put(0.5f);
            tracePositions.put(0.5f);
            tracePositions.put(0.5f);
            tracePositions.put(0.5f);
        }

        final FloatBuffer floatBuffer1 = B.createFloatBuffer(nZSamples * nYSamples);
        final long requestSubsetId = accessManager.requestVolumeTraces(floatBuffer1, layout, DimensionsND.DIMENSIONS_012, 0, 0,
                tracePositions, nYSamples, InterpolationMethod.NEAREST, 0);
        accessManager.waitForCompletion(requestSubsetId);

        final FloatBuffer floatBuffer0 = B.createFloatBuffer(nZSamples * nYSamples);
        final long requestId0 = accessManager.requestVolumeTraces(floatBuffer0, layout, DimensionsND.DIMENSIONS_012, 0, 0,
                tracePositions, nYSamples, InterpolationMethod.NEAREST, 0, layout.getChannelNoValue(0));
        accessManager.waitForCompletion(requestId0);

        B.release(tracePositions);

        for (int i = 0; i < nZSamples * nYSamples; i++) {
            final float f1 = floatBuffer0.get(i);
            final float f2 = floatBuffer1.get(i);
            assertEquals(0, Float.compare(f1, f2), " value " + i + " is different");
        }

        B.release(floatBuffer0, floatBuffer1);

        generator.close();
        assertTrue(generator.isNull());
    }

    @Test
    void testVolumeSubsetBufferVsToFloat() throws IOException {
        int nXSamples = 60, nYSamples = 60, nZSamples = 60;
        VolumeDataChannelDescriptor.Format format = FORMAT_R32;
        MemoryVdsGenerator generator = new MemoryVdsGenerator(nZSamples, nYSamples, nXSamples, format);
        assertTrue(!generator.isNull());
        assertTrue(generator.ownHandle());

        final VolumeDataLayout layout = generator.getLayout();
        assertTrue(!layout.isNull());

        final VolumeDataAccessManager accessManager = generator.getAccessManager();
        assertTrue(!accessManager.isNull());

        NDBox box = new NDBox(0, 0, 0, 0, 0, 0, nZSamples, nYSamples, 1, 0, 0, 0);

        final FloatBuffer floatBuffer1 = B.createFloatBuffer(nZSamples * nYSamples);
        final long requestSubsetId1 = accessManager.requestVolumeSubset(floatBuffer1, layout, DimensionsND.DIMENSIONS_012, 0, 0, box);
        accessManager.waitForCompletion(requestSubsetId1);

        final FloatBuffer floatBuffer0 = B.toBuffer(new float[nZSamples * nYSamples]);
        final long requestSubsetId0 = accessManager.requestVolumeSubset(floatBuffer0, layout, DimensionsND.DIMENSIONS_012, 0, 0, box);
        accessManager.waitForCompletion(requestSubsetId0);

        for (int i = 0; i < nZSamples * nYSamples; i++) {
            final float f1 = floatBuffer0.get(i);
            final float f2 = floatBuffer1.get(i);
            assertEquals(0, Float.compare(f1, f2), " value " + i + " is different");
        }

        B.release(floatBuffer0, floatBuffer1);

        generator.close();
        assertTrue(generator.isNull());
    }

    @Test
    void testVolumeSubsetBufferToArrayAndRelease() throws IOException {
        int nXSamples = 60, nYSamples = 60, nZSamples = 60;
        VolumeDataChannelDescriptor.Format format = FORMAT_R32;
        MemoryVdsGenerator generator = new MemoryVdsGenerator(nZSamples, nYSamples, nXSamples, format);
        assertTrue(!generator.isNull());
        assertTrue(generator.ownHandle());

        final VolumeDataLayout layout = generator.getLayout();
        assertTrue(!layout.isNull());

        final VolumeDataAccessManager accessManager = generator.getAccessManager();
        assertTrue(!accessManager.isNull());

        NDBox box = new NDBox(0, 0, 0, 0, 0, 0, nZSamples, nYSamples, 1, 0, 0, 0);

        final FloatBuffer floatBuffer1 = B.createFloatBuffer(nZSamples * nYSamples);
        final long requestSubsetId1 = accessManager.requestVolumeSubset(floatBuffer1, layout, DimensionsND.DIMENSIONS_012, 0, 0, box);
        accessManager.waitForCompletion(requestSubsetId1);

        final FloatBuffer floatBuffer0 = B.toBuffer(new float[nZSamples * nYSamples]);
        final long requestSubsetId0 = accessManager.requestVolumeSubset(floatBuffer0, layout, DimensionsND.DIMENSIONS_012, 0, 0, box);
        accessManager.waitForCompletion(requestSubsetId0);
        float[] floatArray = B.toArrayAndRelease(floatBuffer0);

        for (int i = 0; i < nZSamples * nYSamples; i++) {
            final float f1 = floatArray[i];
            final float f2 = floatBuffer1.get(i);
            assertEquals(0, Float.compare(f1, f2), " value " + i + " is different");
        }

        B.release(floatBuffer0, floatBuffer1);

        generator.close();
        assertTrue(generator.isNull());
    }

    @Test
    void testVolumeSubsetVsSampleRequest() throws IOException {
        int nXSamples = 60, nYSamples = 60, nZSamples = 60;
        VolumeDataChannelDescriptor.Format format = FORMAT_R32;
        MemoryVdsGenerator generator = new MemoryVdsGenerator(nZSamples, nYSamples, nXSamples, format);
        assertTrue(!generator.isNull());
        assertTrue(generator.ownHandle());

        final VolumeDataLayout layout = generator.getLayout();
        assertTrue(!layout.isNull());

        final VolumeDataAccessManager accessManager = generator.getAccessManager();
        assertTrue(!accessManager.isNull());

        final FloatBuffer samplePositions = B.createFloatBuffer(nZSamples * nYSamples * DIMENSIONALITY_MAX.getNbDimension());
        for (int j = 0; j < nYSamples; j++) {
            for (int k = 0; k < nZSamples; k++) {
                samplePositions.put(k + 0.5f);
                samplePositions.put(j + 0.5f);
                samplePositions.put(0.5f);
                samplePositions.put(0.5f);
                samplePositions.put(0.5f);
                samplePositions.put(0.5f);
            }
        }

        NDBox box = new NDBox(0, 0, 0, 0, 0, 0, nZSamples, nYSamples, 1, 0, 0, 0);

        final FloatBuffer floatBuffer1 = B.createFloatBuffer(nZSamples * nYSamples);
        final long requestSubsetId = accessManager.requestVolumeSubset(floatBuffer1, layout, DimensionsND.DIMENSIONS_012, 0, 0, box);
        accessManager.waitForCompletion(requestSubsetId);

        final FloatBuffer floatBuffer0 = B.createFloatBuffer(nZSamples * nYSamples);
        final long requestId0 = accessManager.requestVolumeSamples(
                floatBuffer0, layout, DimensionsND.DIMENSIONS_012, 0, 0,
                samplePositions, nZSamples * nYSamples, InterpolationMethod.NEAREST);
        accessManager.waitForCompletion(requestId0);

        B.release(samplePositions);

        for (int i = 0; i < nZSamples * nYSamples; i++) {
            final float f1 = floatBuffer0.get(i);
            final float f2 = floatBuffer1.get(i);
            assertEquals(0, Float.compare(f1, f2), " value " + i + " is different");
        }

        B.release(floatBuffer0, floatBuffer1);

        generator.close();
        assertTrue(generator.isNull());
    }

    @Test
    void testVolumeProjectedSubsetRequestFloat() throws IOException {
        int nXSamples = 60, nYSamples = 60, nZSamples = 60;
        VolumeDataChannelDescriptor.Format format = FORMAT_R32;
        MemoryVdsGenerator generator = new MemoryVdsGenerator(nZSamples, nYSamples, nXSamples, format);
        assertTrue(!generator.isNull());
        assertTrue(generator.ownHandle());

        final VolumeDataLayout layout = generator.getLayout();
        assertTrue(!layout.isNull());

        final VolumeDataAccessManager accessManager = generator.getAccessManager();
        assertTrue(!accessManager.isNull());

        NDBox wholeCube = new NDBox(0, 0, 0, 0, 0, 0, nZSamples, nYSamples, nXSamples, 0, 0, 0);

        final FloatBuffer floatBuffer1 = B.createFloatBuffer(nZSamples * nYSamples);
        final long requestSubsetId = accessManager.requestProjectedVolumeSubset(floatBuffer1, layout, DimensionsND.DIMENSIONS_012, 0, 0,
                wholeCube, 0.5f, 0.1f, 0.9f, 1.f, DimensionsND.DIMENSIONS_01, InterpolationMethod.NEAREST);
        accessManager.waitForCompletion(requestSubsetId);

        final FloatBuffer floatBuffer0 = B.createFloatBuffer(nZSamples * nYSamples);
        final long requestSubsetId0 = accessManager.requestProjectedVolumeSubset(floatBuffer0, layout, DimensionsND.DIMENSIONS_012, 0, 0,
                wholeCube, 0.5f, 0.1f, 0.9f, 1.f, DimensionsND.DIMENSIONS_01, InterpolationMethod.NEAREST, layout.getChannelNoValue(0));
        accessManager.waitForCompletion(requestSubsetId0);

        for (int i = 0; i < nZSamples * nYSamples; i++) {
            final float f1 = floatBuffer0.get(i);
            final float f2 = floatBuffer1.get(i);
            assertEquals(0, Float.compare(f1, f2), " value " + i + " is different");
        }

        B.release(floatBuffer0, floatBuffer1);

        generator.close();
        assertTrue(generator.isNull());
    }

    @Test
    void testVolumeProjectedSubsetVSSubsetRequestFloat() throws IOException {
        int nXSamples = 60, nYSamples = 60, nZSamples = 60;
        VolumeDataChannelDescriptor.Format format = FORMAT_R32;
        MemoryVdsGenerator generator = new MemoryVdsGenerator(nZSamples, nYSamples, nXSamples, format);
        assertTrue(!generator.isNull());
        assertTrue(generator.ownHandle());

        final VolumeDataLayout layout = generator.getLayout();
        assertTrue(!layout.isNull());

        final VolumeDataAccessManager accessManager = generator.getAccessManager();
        assertTrue(!accessManager.isNull());

        NDBox wholeCube = new NDBox(0, 0, 0, 0, 0, 0, nZSamples, nYSamples, nXSamples, 0, 0, 0);

        final long projectedVolumeSubsetBufferSize = accessManager.getProjectedVolumeSubsetBufferSize(layout, wholeCube, DimensionsND.DIMENSIONS_01, FORMAT_R32, 0);
        assertEquals(nZSamples * nYSamples * Float.BYTES, projectedVolumeSubsetBufferSize);

        final FloatBuffer floatBuffer1 = B.createFloatBuffer(nZSamples * nYSamples);
        final long requestSubsetId = accessManager.requestProjectedVolumeSubset(floatBuffer1, layout, DimensionsND.DIMENSIONS_012, 0, 0,
                wholeCube, 0.5f, 0.1f, 0.9f, 1.f, DimensionsND.DIMENSIONS_01, InterpolationMethod.NEAREST);
        accessManager.waitForCompletion(requestSubsetId);

        NDBox boxSlice = new NDBox(0, 0, 0, 0, 0, 0, nZSamples, nYSamples, 1, 0, 0, 0);
        final FloatBuffer floatBuffer0 = B.createFloatBuffer(nZSamples * nYSamples);
        final float channelNoValue = layout.getChannelNoValue(0);
        final long requestId0 = accessManager.requestVolumeSubset(floatBuffer0, layout, DimensionsND.DIMENSIONS_012, 0, 0,
                boxSlice, channelNoValue);
        accessManager.waitForCompletion(requestId0);

        for (int i = 0; i < nZSamples * nYSamples; i++) {
            final float f1 = floatBuffer0.get(i);
            final float f2 = floatBuffer1.get(i);
            assertEquals(0, Float.compare(f1, f2), " value " + i + " is different");
        }

        B.release(floatBuffer0, floatBuffer1);

        generator.close();
        assertTrue(generator.isNull());
    }

    //    @Test
    void testVolumeSubsetRequestFloatMultiThread2() throws IOException {
        int nXSamples = 60, nYSamples = 60, nZSamples = 60;
        VolumeDataChannelDescriptor.Format format = FORMAT_R32;
        final MemoryVdsGenerator generator = new MemoryVdsGenerator(nZSamples, nYSamples, nXSamples, format);
        assertTrue(!generator.isNull());
        assertTrue(generator.ownHandle());

        final VolumeDataLayout layout = generator.getLayout();
        assertTrue(!layout.isNull());

        IntStream
                .range(0, nXSamples)
                .mapToObj(iSlice -> new NDBox(
                        0, 0, iSlice, 0, 0, 0,
                        nZSamples, nYSamples, iSlice + 1, 0, 0, 0))
                .parallel().forEach(box -> {
            final FloatBuffer floatBuffer1 = B.createFloatBuffer(nZSamples * nYSamples);
            final FloatBuffer floatBuffer0 = B.createFloatBuffer(nZSamples * nYSamples);
            final VolumeDataAccessManager accessManager = generator.getAccessManager();
            assertTrue(!accessManager.isNull());
            final long requestId1 = accessManager.requestVolumeSubset(
                    floatBuffer1, layout, DimensionsND.DIMENSIONS_012, 0, 0, box);
            final long requestId0 = accessManager.requestVolumeSubset(
                    floatBuffer0, layout, DimensionsND.DIMENSIONS_012, 0, 0, box, layout.getChannelNoValue(0));

            accessManager.waitForCompletion(requestId1);
            accessManager.waitForCompletion(requestId0);

            for (int i = 0; i < nZSamples * nYSamples; i++) {
                final float f1 = floatBuffer0.get(i);
                final float f2 = floatBuffer1.get(i);
                assertEquals(0, Float.compare(f1, f2), " value " + i + " is different");
            }
            B.release(floatBuffer0, floatBuffer1);
        });

        generator.close();
        assertTrue(generator.isNull());
    }

    //    @Test
    void testVolumeSubsetRequestFloatLeak() throws IOException {
        int nXSamples = 60, nYSamples = 60, nZSamples = 60;
        VolumeDataChannelDescriptor.Format format = FORMAT_R32;
        final MemoryVdsGenerator generator = new MemoryVdsGenerator(nZSamples, nYSamples, nXSamples, format);
        assertTrue(!generator.isNull());
        assertTrue(generator.ownHandle());

        final VolumeDataLayout layout = generator.getLayout();
        assertTrue(!layout.isNull());

        final int NB_TEST = 10000;

        IntStream
                .range(0, NB_TEST)
                .mapToObj(iTest -> {
                    int iSlice = iTest % 10;
                    return new NDBox(
                            0, 0, iSlice, 0, 0, 0,
                            nZSamples, nYSamples, iSlice + 1, 0, 0, 0);
                })
                .forEach(box -> {
//                                final FloatBuffer floatBuffer1 = samplesBuffers0[box.getMin()[2] % NB_THREAD];
//                                final FloatBuffer floatBuffer0 = samplesBuffers1[box.getMin()[2] % NB_THREAD];
                            final FloatBuffer floatBuffer1 = BufferUtils.createFloatBuffer(nZSamples * nYSamples);
                            final FloatBuffer floatBuffer0 = BufferUtils.createFloatBuffer(nZSamples * nYSamples);
                            final VolumeDataAccessManager accessManager = generator.getAccessManager();
                            assertTrue(!accessManager.isNull());
                            final long requestId1 = accessManager.requestVolumeSubset(
                                    floatBuffer1, layout, DimensionsND.DIMENSIONS_012, 0, 0, box);
                            final long requestId0 = accessManager.requestVolumeSubset(
                                    floatBuffer0, layout, DimensionsND.DIMENSIONS_012, 0, 0, box, layout.getChannelNoValue(0));

                            accessManager.waitForCompletion(requestId1);
                            accessManager.waitForCompletion(requestId0);

                            for (int i = 0; i < nZSamples * nYSamples; i++) {
                                final float f1 = floatBuffer0.get(i);
                                final float f2 = floatBuffer1.get(i);
                                assertEquals(0, Float.compare(f1, f2), " value " + i + " is different");
                            }

                            B.release(floatBuffer0, floatBuffer1);
                        }
                );

        generator.close();
        assertTrue(generator.isNull());
    }

    @Test
    void testVolumeSubsetRequestFloatMultiThread() throws IOException {

        VolumeDataChannelDescriptor.Format format = FORMAT_R32;
        final MemoryVdsGenerator generator = new MemoryVdsGenerator(60, 60, 60, format);

        assertTrue(!generator.isNull());
        assertTrue(generator.ownHandle());

        final VolumeDataLayout layout = generator.getLayout();
        assertTrue(!layout.isNull());

        LinkedList<Future> jobs = new LinkedList();
        final int NB_TEST = 10000;
        final int NB_THREAD = 16;
//        FloatBuffer[] samplesBuffers0 = IntStream.range(0, NB_THREAD).mapToObj(i -> BufferUtils.createFloatBuffer(nZSamples * nYSamples)).toArray(FloatBuffer[]::new);
//        FloatBuffer[] samplesBuffers1 = IntStream.range(0, NB_THREAD).mapToObj(i -> BufferUtils.createFloatBuffer(nZSamples * nYSamples)).toArray(FloatBuffer[]::new);

        int nZSamples = layout.getDimensionNumSamples(0);
        int nYSamples = layout.getDimensionNumSamples(1);
        int nXSamples = layout.getDimensionNumSamples(2);

        final ExecutorService executorService = Executors.newFixedThreadPool(NB_THREAD);
        IntStream
                .range(0, NB_TEST)
                .mapToObj(iTest -> {
                    int iSlice = iTest % nXSamples;
                    return new NDBox(
                            0, 0, iSlice, 0, 0, 0,
                            nZSamples, nYSamples, iSlice + 1, 0, 0, 0);
                })
                .forEach(box -> {
                            if (jobs.size() >= NB_THREAD) {
                                final Future future = jobs.pollFirst();
                                try {
                                    future.get();
                                } catch (InterruptedException | ExecutionException e) {
                                    throw new RuntimeException(e);
                                }
                            }
                            jobs.addLast(executorService.submit(() -> {
//                                final FloatBuffer floatBuffer1 = samplesBuffers0[box.getMin()[2] % NB_THREAD];
//                                final FloatBuffer floatBuffer0 = samplesBuffers1[box.getMin()[2] % NB_THREAD];
                                final FloatBuffer floatBuffer1 = BufferUtils.createFloatBuffer(nZSamples * nYSamples);
                                final FloatBuffer floatBuffer0 = BufferUtils.createFloatBuffer(nZSamples * nYSamples);
                                final VolumeDataAccessManager accessManager = generator.getAccessManager();
                                assertTrue(!accessManager.isNull());
//                                synchronized (accessManager) {
                                final long requestId1 = accessManager.requestVolumeSubset(
                                        floatBuffer1, layout, DimensionsND.DIMENSIONS_012, 0, 0, box);
                                final long requestId0 = accessManager.requestVolumeSubset(
                                        floatBuffer0, layout, DimensionsND.DIMENSIONS_012, 0, 0, box, layout.getChannelNoValue(0));

                                waitComplationAndDisplayProgress(accessManager, requestId1);
                                waitComplationAndDisplayProgress(accessManager, requestId0);

                                for (int i = 0; i < nZSamples * nYSamples; i++) {
                                    final float f1 = floatBuffer0.get(i);
                                    final float f2 = floatBuffer1.get(i);
                                    assertEquals(0, Float.compare(f1, f2), " value " + i + " is different");
                                }

                                B.release(floatBuffer0, floatBuffer1);

//                                System.out.println("Done " + requestId0 + " " + requestId1);
//                                System.out.flush();
                                return null;
                            }));
                        }
                );
        jobs.forEach(future -> {
            try {
                future.get();
            } catch (InterruptedException | ExecutionException e) {
                throw new RuntimeException(e);
            }
        });

        executorService.shutdown();
        generator.close();
        assertTrue(generator.isNull());
    }

    private static void waitComplationAndDisplayProgress(VolumeDataAccessManager accessManager, long request) {
        while (!accessManager.waitForCompletion(request, 1000)) {
            if (accessManager.isCanceled(request)) throw new RuntimeException("Cancelled job");

            // Timeout, so let display progress
            final float completionFactor = accessManager.getCompletionFactor(request);
            System.out.println("Completion " + request + " : " + completionFactor * 100. + " %");
        }
    }

    @Test
    void testVolumeSubsetRequestFloat() throws IOException {
        int nXSamples = 60, nYSamples = 60, nZSamples = 60;
        VolumeDataChannelDescriptor.Format format = FORMAT_R32;
        MemoryVdsGenerator generator = new MemoryVdsGenerator(nZSamples, nYSamples, nXSamples, format);
        assertTrue(!generator.isNull());
        assertTrue(generator.ownHandle());

        final VolumeDataLayout layout = generator.getLayout();
        assertTrue(!layout.isNull());

        final VolumeDataAccessManager accessManager = generator.getAccessManager();
        assertTrue(!accessManager.isNull());

        NDBox box = new NDBox(0, 0, 0, 0, 0, 0, nZSamples, nYSamples, 1, 0, 0, 0);

        final FloatBuffer floatBuffer1 = B.createFloatBuffer(nZSamples * nYSamples);
        final long requestSubsetId = accessManager.requestVolumeSubset(floatBuffer1, layout, DimensionsND.DIMENSIONS_012, 0, 0,
                box);
        accessManager.waitForCompletion(requestSubsetId);

        final FloatBuffer floatBuffer0 = B.createFloatBuffer(nZSamples * nYSamples);
        final float channelNoValue = layout.getChannelNoValue(0);
        final long requestId0 = accessManager.requestVolumeSubset(floatBuffer0, layout, DimensionsND.DIMENSIONS_012, 0, 0,
                box, channelNoValue);
        accessManager.waitForCompletion(requestId0);

        for (int i = 0; i < nZSamples * nYSamples; i++) {
            final float f1 = floatBuffer0.get(i);
            final float f2 = floatBuffer1.get(i);
            assertEquals(0, Float.compare(f1, f2), " value " + i + " is different");
        }

        B.release(floatBuffer0, floatBuffer1);

        generator.close();
        assertTrue(generator.isNull());
    }

    @Test
    void testVolumeSubsetRequestByte() throws IOException {
        int nXSamples = 60, nYSamples = 60, nZSamples = 60;
        VolumeDataChannelDescriptor.Format format = FORMAT_U8;
        MemoryVdsGenerator generator = new MemoryVdsGenerator(nZSamples, nYSamples, nXSamples, format);
        assertTrue(!generator.isNull());
        assertTrue(generator.ownHandle());

        final VolumeDataLayout layout = generator.getLayout();
        assertTrue(!layout.isNull());

        final VolumeDataAccessManager accessManager = generator.getAccessManager();
        assertTrue(!accessManager.isNull());

        NDBox box = new NDBox(0, 0, 0, 0, 0, 0, nZSamples, nYSamples, 1, 0, 0, 0);

        final ByteBuffer floatBuffer1 = B.createByteBuffer(nZSamples * nYSamples);
        final long requestSubsetId = accessManager.requestVolumeSubset(floatBuffer1, layout, DimensionsND.DIMENSIONS_012, 0, 0,
                box);
        accessManager.waitForCompletion(requestSubsetId);

        final ByteBuffer floatBuffer0 = B.createByteBuffer(nZSamples * nYSamples);
        final float channelNoValue = layout.getChannelNoValue(0);
        final long requestId0 = accessManager.requestVolumeSubset(floatBuffer0, layout, DimensionsND.DIMENSIONS_012, 0, 0,
                box, channelNoValue);
        accessManager.waitForCompletion(requestId0);

        for (int i = 0; i < nZSamples * nYSamples; i++) {
            final float f1 = floatBuffer0.get(i);
            final float f2 = floatBuffer1.get(i);
            assertEquals(0, Float.compare(f1, f2), " value " + i + " is different");
        }

        B.release(floatBuffer0, floatBuffer1);

        generator.close();
        assertTrue(generator.isNull());
    }

    @Test
    void testVolumeSubsetRequestInteger() throws IOException {
        int nXSamples = 60, nYSamples = 60, nZSamples = 60;
        VolumeDataChannelDescriptor.Format format = FORMAT_U32;
        MemoryVdsGenerator generator = new MemoryVdsGenerator(nZSamples, nYSamples, nXSamples, format);
        assertTrue(!generator.isNull());
        assertTrue(generator.ownHandle());

        final VolumeDataLayout layout = generator.getLayout();
        assertTrue(!layout.isNull());

        final VolumeDataAccessManager accessManager = generator.getAccessManager();
        assertTrue(!accessManager.isNull());

        NDBox box = new NDBox(0, 0, 0, 0, 0, 0, nZSamples, nYSamples, 1, 0, 0, 0);

        final IntBuffer floatBuffer1 = B.createIntBuffer(nZSamples * nYSamples);
        final long requestSubsetId = accessManager.requestVolumeSubset(floatBuffer1, layout, DimensionsND.DIMENSIONS_012, 0, 0,
                box);
        accessManager.waitForCompletion(requestSubsetId);

        final IntBuffer floatBuffer0 = B.createIntBuffer(nZSamples * nYSamples);
        final float channelNoValue = layout.getChannelNoValue(0);
        final long requestId0 = accessManager.requestVolumeSubset(floatBuffer0, layout, DimensionsND.DIMENSIONS_012, 0, 0,
                box, channelNoValue);
        accessManager.waitForCompletion(requestId0);

        for (int i = 0; i < nZSamples * nYSamples; i++) {
            final float f1 = floatBuffer0.get(i);
            final float f2 = floatBuffer1.get(i);
            assertEquals(0, Float.compare(f1, f2), " value " + i + " is different");
        }

        B.release(floatBuffer0, floatBuffer1);

        generator.close();
        assertTrue(generator.isNull());
    }

    @Test
    void testSampleRequest() throws IOException {
        int nXSamples = 60, nYSamples = 60, nZSamples = 60;
        VolumeDataChannelDescriptor.Format format = FORMAT_U8;
        MemoryVdsGenerator generator = new MemoryVdsGenerator(nZSamples, nYSamples, nXSamples, format);
        assertTrue(!generator.isNull());
        assertTrue(generator.ownHandle());

        final VolumeDataLayout layout = generator.getLayout();
        assertTrue(!layout.isNull());

        final VolumeDataAccessManager accessManager = generator.getAccessManager();
        assertTrue(!accessManager.isNull());

        final int NB_SAMPLE_REQUESTED = 1000;
        final FloatBuffer samplePositions = BufferUtils.createFloatBuffer(NB_SAMPLE_REQUESTED * DIMENSIONALITY_MAX.getNbDimension());
        Random r = new Random(0);
        for (int i = 0; i < NB_SAMPLE_REQUESTED; i++) {
            samplePositions.put(r.nextFloat() * nZSamples + 0.5f);
            samplePositions.put(r.nextFloat() * nYSamples + 0.5f);
            samplePositions.put(r.nextFloat() * nXSamples + 0.5f);
            samplePositions.put(0.5f);
            samplePositions.put(0.5f);
            samplePositions.put(0.5f);
        }

        final FloatBuffer floatBuffer0 = BufferUtils.createFloatBuffer(NB_SAMPLE_REQUESTED);
        final long requestId0 = accessManager.requestVolumeSamples(
                floatBuffer0, layout, DimensionsND.DIMENSIONS_012, 0, 0,
                samplePositions, NB_SAMPLE_REQUESTED, InterpolationMethod.LINEAR);
        accessManager.waitForCompletion(requestId0);

        final FloatBuffer floatBuffer1 = BufferUtils.createFloatBuffer(NB_SAMPLE_REQUESTED);
        final long requestId1 = accessManager.requestVolumeSamples(
                floatBuffer1, layout, DimensionsND.DIMENSIONS_012, 0, 0,
                samplePositions, NB_SAMPLE_REQUESTED, InterpolationMethod.LINEAR, Float.NaN);
        accessManager.waitForCompletion(requestId1);

        for (int i = 0; i < NB_SAMPLE_REQUESTED; i++) {
            final float f1 = floatBuffer0.get(i);
            final float f2 = floatBuffer1.get(i);
            assertEquals(0, Float.compare(f1, f2), " value " + i + " is different");
        }

        generator.close();
        assertTrue(generator.isNull());
    }

    @Test
    void testLayout() throws IOException {
        int nXSamples = 60, nYSamples = 60, nZSamples = 60;
        VolumeDataChannelDescriptor.Format format = FORMAT_U8;
        MemoryVdsGenerator generator = new MemoryVdsGenerator(nXSamples, nYSamples, nZSamples, format);
        final VolumeDataLayout layout = generator.getLayout();

        int nbChannel = layout.getChannelCount();
        VolumeDataAccessManager accessManager = generator.getAccessManager();
        for (VolumeDataLayoutDescriptor.LODLevels l : VolumeDataLayoutDescriptor.LODLevels.values()) {
            for (int channel = 0; channel < nbChannel; channel++) {
                for (DimensionsND dimGroup : DimensionsND.values()) {
                    VDSProduceStatus vdsProduceStatus = accessManager.getVDSProduceStatus(layout, dimGroup, l.ordinal(), channel);
                    if (channel == 0 && LOD_LEVELS_NONE.equals(l) && DimensionsND.DIMENSIONS_012.equals(dimGroup))
                        assertEquals(VDSProduceStatus.NORMAL, vdsProduceStatus);
                    else
                        assertEquals(VDSProduceStatus.UNAVAILABLE, vdsProduceStatus);
                }
            }
        }

        assertEquals(Dimensionality.DIMENSIONALITY_3, layout.getDimensionality());

        int dimensionIndex = 0;
        assertEquals(60, layout.getDimensionNumSamples(dimensionIndex));
        assertEquals("Sample", layout.getDimensionName(dimensionIndex));
        assertEquals("ms", layout.getDimensionUnit(dimensionIndex));
        assertEquals(0f, layout.getDimensionMin(dimensionIndex));
        assertEquals(4f, layout.getDimensionMax(dimensionIndex));

        dimensionIndex = 1;
        assertEquals(60, layout.getDimensionNumSamples(dimensionIndex));
        assertEquals("Crossline", layout.getDimensionName(dimensionIndex));
        assertEquals(null, layout.getDimensionUnit(dimensionIndex));
        assertEquals(1932.f, layout.getDimensionMin(dimensionIndex));
        assertEquals(2536.f, layout.getDimensionMax(dimensionIndex));

        dimensionIndex = 2;
        assertEquals(60, layout.getDimensionNumSamples(dimensionIndex));
        assertEquals("Inline", layout.getDimensionName(dimensionIndex));
        assertEquals(null, layout.getDimensionUnit(dimensionIndex));
        assertEquals(9985.f, layout.getDimensionMin(dimensionIndex));
        assertEquals(10369.f, layout.getDimensionMax(dimensionIndex));

        final VolumeDataLayoutDescriptor descriptor = layout.getLayoutDescriptor();
        assertTrue(descriptor.isValid());
        assertEquals(BRICK_SIZE_32, descriptor.getBrickSize());
        assertEquals(4, descriptor.getNegativeMargin());
        assertEquals(4, descriptor.getPositiveMargin());
        assertEquals(4, descriptor.getBrickSizeMultiplier2D());
        assertEquals(LOD_LEVELS_NONE, descriptor.getLODLevels());
        assertTrue(!descriptor.isCreate2DLODs());
        assertTrue(descriptor.isForceFullResolutionDimension());
        assertEquals(0, descriptor.getFullResolutionDimension());

        assertEquals(1, layout.getChannelCount());

        int channelIndex = 0;
        String channelName = layout.getChannelName(channelIndex);
        assertEquals("Amplitude", channelName);
        assertEquals(null, layout.getChannelUnit(channelIndex));
        assertTrue(layout.isChannelAvailable(channelName));
        assertEquals(0, layout.getChannelIndex(channelName));
        assertEquals(FORMAT_U8, layout.getChannelFormat(channelIndex));
        assertEquals(1, layout.getChannelComponents(channelIndex));
        assertEquals(-0.1234f, layout.getChannelValueRangeMin(channelIndex));
        assertEquals(0.1234f, layout.getChannelValueRangeMax(channelIndex));
        assertTrue(!layout.isChannelDiscrete(channelIndex));
        assertTrue(layout.isChannelRenderable(channelIndex));
        assertTrue(layout.isChannelAllowingLossyCompression(channelIndex));
        assertTrue(!layout.isChannelUseZipForLosslessCompression(channelIndex));
        assertEquals(VolumeDataMapping.DIRECT, layout.getChannelMapping(channelIndex));
        assertTrue(layout.isChannelUseNoValue(channelIndex));
        assertEquals(0f, layout.getChannelNoValue(channelIndex));
        assertEquals(9.7165356E-4f, layout.getChannelIntegerScale(channelIndex));
        assertEquals(-0.1234f, layout.getChannelIntegerOffset(channelIndex));

        assertEquals(1, layout.getChannelDescriptor(0).getMappedValueCount());

        final MetadataKey[] metadataKeys = layout.getMetadataKeys();
        assertEquals(13, metadataKeys.length);

        assertTrue(layout.isMetadataIntAvailable("categoryInt", "Int"));
        assertEquals(123, layout.getMetadataInt("categoryInt", "Int"));
        assertTrue(layout.isMetadataIntVector2Available("categoryInt", "IntVector2"));
        assertEquals(new int[]{45, 78}, layout.getMetadataIntVector2("categoryInt", "IntVector2"));
        assertTrue(layout.isMetadataIntVector3Available("categoryInt", "IntVector3"));
        assertEquals(new int[]{45, 78, 72}, layout.getMetadataIntVector3("categoryInt", "IntVector3"));
        assertTrue(layout.isMetadataIntVector4Available("categoryInt", "IntVector4"));
        assertEquals(new int[]{45, 78, 72, 84}, layout.getMetadataIntVector4("categoryInt", "IntVector4"));

        assertTrue(layout.isMetadataFloatAvailable("categoryFloat", "Float"));
        assertEquals(123f, layout.getMetadataFloat("categoryFloat", "Float"));
        assertTrue(layout.isMetadataFloatVector2Available("categoryFloat", "FloatVector2"));
        assertEquals(new float[]{45.5f, 78.75f}, layout.getMetadataFloatVector2("categoryFloat", "FloatVector2"));
        assertTrue(layout.isMetadataFloatVector3Available("categoryFloat", "FloatVector3"));
        assertEquals(new float[]{45.5f, 78.75f, 72.75f}, layout.getMetadataFloatVector3("categoryFloat", "FloatVector3"));
        assertTrue(layout.isMetadataFloatVector4Available("categoryFloat", "FloatVector4"));
        assertEquals(new float[]{45.5f, 78.75f, 72.75f, 84.1f}, layout.getMetadataFloatVector4("categoryFloat", "FloatVector4"));

        assertTrue(layout.isMetadataDoubleAvailable("categoryDouble", "Double"));
        assertEquals(123., layout.getMetadataDouble("categoryDouble", "Double"));
        assertTrue(layout.isMetadataDoubleVector2Available("categoryDouble", "DoubleVector2"));
        assertEquals(new double[]{45.5, 78.75}, layout.getMetadataDoubleVector2("categoryDouble", "DoubleVector2"));
        assertTrue(layout.isMetadataDoubleVector3Available("categoryDouble", "DoubleVector3"));
        assertEquals(new double[]{45.5, 78.75, 72.75}, layout.getMetadataDoubleVector3("categoryDouble", "DoubleVector3"));
        assertTrue(layout.isMetadataDoubleVector4Available("categoryDouble", "DoubleVector4"));
        assertEquals(new double[]{45.5, 78.75, 72.75, 84.1}, layout.getMetadataDoubleVector4("categoryDouble", "DoubleVector4"));

        assertTrue(layout.isMetadataStringAvailable("categoryString", "String"));
        assertEquals("Test string", layout.getMetadataString("categoryString", "String"));

        generator.close();
    }
}