
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

import org.opengroup.openvds.BufferUtils;
import org.opengroup.openvds.JniPointer;
import org.opengroup.openvds.experimental.VariousJavaTests;

import java.nio.FloatBuffer;
import java.time.Duration;
import java.time.Instant;
import java.util.LinkedList;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.stream.IntStream;

public class TestVariousAllocationAndTransmissionMethods {
    public static void main(String[] args) {
        System.out.println(JniPointer.libraryDescription());

        int QUERY_SIZE = 8000;
        int NB_ITERATION = 100000;

        // Using java array
        final Instant beginJavaArray = Instant.now();
        testJavaArray(QUERY_SIZE, NB_ITERATION, false);
        final Instant endJavaArray = Instant.now();
        System.out.println("testJavaArray " + Duration.between(beginJavaArray, endJavaArray).toMillis() + " ms");
        System.out.flush();

        // Using direct byte buffer
        final Instant beginDirectByteBuffer = Instant.now();
        testDirectByteBuffer(QUERY_SIZE, NB_ITERATION, false);
        final Instant endDirectByteBuffer = Instant.now();
        System.out.println("testDirectByteBuffer " + Duration.between(beginDirectByteBuffer, endDirectByteBuffer).toMillis() + " ms");
        System.out.flush();

        // Using direct byte buffer
        final Instant beginDirectByteBufferParallel = Instant.now();
        testDirectByteBufferParallel(QUERY_SIZE, NB_ITERATION);
        final Instant endDirectByteBufferParallel = Instant.now();
        System.out.println("testDirectByteBufferParallel " + Duration.between(beginDirectByteBufferParallel, endDirectByteBufferParallel).toMillis() + " ms");
        System.out.flush();

        // Using java array
        final Instant beginJavaArrayReallocate = Instant.now();
        testJavaArray(QUERY_SIZE, NB_ITERATION, true);
        final Instant endJavaArrayReallocate = Instant.now();
        System.out.println("testJavaArrayReallocate " + Duration.between(beginJavaArrayReallocate, endJavaArrayReallocate).toMillis() + " ms");
        System.out.flush();

        // Using direct byte buffer
        final Instant beginDirectByteBufferReallocate = Instant.now();
        testDirectByteBuffer(QUERY_SIZE, NB_ITERATION, true);
        final Instant endDirectByteBufferReallocate = Instant.now();
        System.out.println("testDirectByteBufferReallocate " + Duration.between(beginDirectByteBufferReallocate, endDirectByteBufferReallocate).toMillis() + " ms");
        System.out.flush();

//        // Using wrapped float array
//        final FloatBuffer wrappedQueryFloat = FloatBuffer.wrap(new float[QUERY_SIZE*6]);
//        final FloatBuffer wrappedSampleFloat = FloatBuffer.wrap(new float[QUERY_SIZE]);
//        for (int i = 0; i < QUERY_SIZE; i++) {
//            wrappedQueryFloat.put(i*6 + 0, i);
//            wrappedQueryFloat.put(i*6 + 1, i);
//            wrappedQueryFloat.put(i*6 + 2, i);
//        }
//        VariousJavaTests.requestUsingFloatBuffer(wrappedQueryFloat, wrappedSampleFloat);
//        for (int i = 0; i < QUERY_SIZE; i++) {
//            if(Float.compare(wrappedSampleFloat.get(i), wrappedQueryFloat.get(i * 6 + 0)) != 0)
//                throw new RuntimeException();
//        }
    }

//    private static void testIliaNativeBuffer(int QUERY_SIZE, int NB_ITERATION, boolean reallocate) {
//        NativeBuffer queryArray = null;
//        NativeBuffer sampleArray = null;
//
//        for (int it = 0; it < NB_ITERATION; it++) {
//            if(queryArray == null || reallocate){
//                if(queryArray != null && reallocate){
//                    queryArray.release();
//                    sampleArray.release();
//                }
//
//                queryArray = NativeBuffer.newFloatBuffer(6 * QUERY_SIZE);
//                sampleArray = NativeBuffer.newFloatBuffer(QUERY_SIZE);
//            }
//
////            for (int i = 0; i < QUERY_SIZE; i++) {
////                queryArray[i * 6 + 0] = it;
////                queryArray[i * 6 + 1] = it;
////                queryArray[i * 6 + 2] = it;
////            }
//
//            // Using java arrays
//            VariousJavaTests.requestUsingNativeBuffer(queryArray.handle(), sampleArray.handle(), QUERY_SIZE);
//            for (int i = 0; i < QUERY_SIZE; i++) {
//                if (Float.compare(sampleArray[i], it) != 0)
//                    throw new RuntimeException();
//            }
//        }
//
//        queryArray.release();
//        sampleArray.release();
//    }

    private static void testDirectByteBufferParallel(int QUERY_SIZE, int NB_ITERATION) {
        int processors = Runtime.getRuntime().availableProcessors();
        FloatBuffer[] queries = IntStream.range(0, processors).mapToObj(i -> BufferUtils.createFloatBuffer(6 * QUERY_SIZE)).toArray(FloatBuffer[]::new);
        FloatBuffer[] samplesBuffers = IntStream.range(0, processors).mapToObj(i -> BufferUtils.createFloatBuffer(QUERY_SIZE)).toArray(FloatBuffer[]::new);
        final ExecutorService exec = Executors.newFixedThreadPool(processors);

        LinkedList<Future> tasks = new LinkedList();

        IntStream.range(0, NB_ITERATION).forEach(it -> {
            tasks.addLast(exec.submit(() -> {
                FloatBuffer directQueryFloat = queries[it % processors];
                FloatBuffer directSampleBuffer = samplesBuffers[it % processors];
                for (int i = 0; i < QUERY_SIZE; i++) {
                    directQueryFloat.put(i * 6 + 0, it);
                    directQueryFloat.put(i * 6 + 1, it);
                    directQueryFloat.put(i * 6 + 2, it);
                }
                VariousJavaTests.requestUsingFloatBuffer(directQueryFloat, directSampleBuffer);
                for (int i = 0; i < QUERY_SIZE; i++) {
                    if (Float.compare(directSampleBuffer.get(i), it) != 0)
                        throw new RuntimeException();
                }
            }));
            if(tasks.size() >= processors){
                final Future future = tasks.pollFirst();
                try {
                    future.get();
                } catch (InterruptedException | ExecutionException e) {
                    throw new RuntimeException(e);
                }
            }
        });
        tasks.forEach(future -> {
            try {
                future.get();
            } catch (InterruptedException | ExecutionException e) {
                throw new RuntimeException(e);
            }
        });

        exec.shutdown();
    }

    private static void testDirectByteBuffer(int QUERY_SIZE, int NB_ITERATION, boolean reallocate) {
        FloatBuffer directQueryFloat = null;
        FloatBuffer directSampleBuffer = null;

        for (int it = 0; it < NB_ITERATION; it++) {
            if (directQueryFloat == null || reallocate) {
                if(directQueryFloat != null){
//                    BufferUtils.cleanBuffer(directQueryFloat);
//                    BufferUtils.cleanBuffer(directSampleBuffer);
                    BufferUtils.release(directQueryFloat);
                    BufferUtils.release(directSampleBuffer);
                }
                directQueryFloat = BufferUtils.createFloatBuffer(6 * QUERY_SIZE);
                directSampleBuffer = BufferUtils.createFloatBuffer(QUERY_SIZE);
            }

            for (int i = 0; i < QUERY_SIZE; i++) {
                directQueryFloat.put(i * 6 + 0, it);
                directQueryFloat.put(i * 6 + 1, it);
                directQueryFloat.put(i * 6 + 2, it);
            }
            VariousJavaTests.requestUsingFloatBuffer(directQueryFloat, directSampleBuffer);
            for (int i = 0; i < QUERY_SIZE; i++) {
                if (Float.compare(directSampleBuffer.get(i), it) != 0)
                    throw new RuntimeException();
            }
        }
    }

    private static void testJavaArray(int QUERY_SIZE, int NB_ITERATION, boolean reallocate) {
        float[] queryArray = null;
        float[] sampleArray = null;

        for (int it = 0; it < NB_ITERATION; it++) {
            if (queryArray == null || reallocate) {
                queryArray = new float[6 * QUERY_SIZE];
                sampleArray = new float[QUERY_SIZE];
            }

            for (int i = 0; i < QUERY_SIZE; i++) {
                queryArray[i * 6 + 0] = it;
                queryArray[i * 6 + 1] = it;
                queryArray[i * 6 + 2] = it;
            }

            // Using java arrays
            VariousJavaTests.requestUsingJavaBuffer(queryArray, sampleArray);
            for (int i = 0; i < QUERY_SIZE; i++) {
                if (Float.compare(sampleArray[i], it) != 0)
                    throw new RuntimeException();
            }
        }
    }
}
