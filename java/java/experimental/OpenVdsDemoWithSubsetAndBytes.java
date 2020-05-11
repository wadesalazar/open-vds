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

import org.opengroup.openvds.*;

import java.io.FileOutputStream;
import java.nio.ByteBuffer;
import java.nio.FloatBuffer;

public class OpenVdsDemoWithSubsetAndBytes {

    public static void main(String[] args) {
        try {
            process(args);
        } catch (Throwable t) {
            System.out.println();
            t.printStackTrace();
        }
    }

    static void process(String[] args) throws Exception {
        System.out.println("Library: " + JniPointer.libraryDescription());
        int nXSamples = 1, nYSamples = 1000, nZSamples = 1000;
        VolumeDataChannelDescriptor.Format format = VolumeDataChannelDescriptor.Format.FORMAT_U8;

        System.out.println("Create MemoryVdsGenerator...");
        MemoryVdsGenerator generator = new MemoryVdsGenerator(nZSamples, nYSamples, nXSamples, format);

        VolumeDataLayout layout = generator.getLayout();
        VolumeDataAccessManager accessManager = generator.getAccessManager();

        System.out.println("\nCreate request for a slice...");
        NDBox box = new NDBox(
                0, 0, 0, 0, 0, 0,
                nZSamples, nYSamples, 1, 0, 0, 0);

        final ByteBuffer data = BufferUtils.createByteBuffer(nZSamples * nYSamples);

        final long request = accessManager.requestVolumeSubset(data, layout, DimensionsND.DIMENSIONS_012, 0, 0, box);
        System.out.println("Wait for request completion...");
        while (!accessManager.waitForCompletion(request, 100)) {
            if (accessManager.isCanceled(request)) throw new RuntimeException("Cancelled job");

            // Timeout, so let display progress
            final float completionFactor = accessManager.getCompletionFactor(request);
            System.out.println("Completion : " + completionFactor * 100. + " %");
        }

        System.out.println("Create bitmap " + nZSamples + "x" + nYSamples + " from samples...");
        String outFileName = "OpenVdsDemo_Output.bmp";
        writeBitmap(outFileName, layout, BufferUtils.toArray(data), nZSamples, nYSamples);
        BufferUtils.release(data);
        System.out.println("Picture is written to file: " + outFileName);

        // Test of finalization. Normally manual call of release() is not needed:
        // it is called from JniPointer.finalize()
        System.out.println("Release objects...");
        generator.release();
        System.out.println("Finished");
    }

    static void writeBitmap(String fileName, VolumeDataLayout layout,
                            byte[] data, int output_width, int output_height) throws Exception {

        final int colorComponentsCount = 3;
        int dataSize = output_width * output_height * colorComponentsCount;
        byte[] fileData = new byte[dataSize];

        for (int y = 0; y < output_height; y++) {
            for (int x = 0; x < output_width; x++) {
                int inOffset = y * output_width + x;
                byte value = data[inOffset];
                int outOffset = inOffset * colorComponentsCount;
                fileData[outOffset] = value;
                fileData[outOffset + 1] = value;
                fileData[outOffset + 2] = value;
            }
        }

        long filesize = 54 + dataSize;
        byte[] bmpinfoheader = new byte[40];
        bmpinfoheader[0] = 40;
        bmpinfoheader[12] = 1;
        bmpinfoheader[14] = 24;
        byte[] bmppad = {0, 0, 0};

        byte[] bmpfileheader = {'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0};
        bmpfileheader[2] = (byte) (filesize);
        bmpfileheader[3] = (byte) (filesize >> 8);
        bmpfileheader[4] = (byte) (filesize >> 16);
        bmpfileheader[5] = (byte) (filesize >> 24);

        bmpinfoheader[4] = (byte) (output_width);
        bmpinfoheader[5] = (byte) (output_width >> 8);
        bmpinfoheader[6] = (byte) (output_width >> 16);
        bmpinfoheader[7] = (byte) (output_width >> 24);
        bmpinfoheader[8] = (byte) (output_height);
        bmpinfoheader[9] = (byte) (output_height >> 8);
        bmpinfoheader[10] = (byte) (output_height >> 16);
        bmpinfoheader[11] = (byte) (output_height >> 24);

        FileOutputStream file = new FileOutputStream(fileName);

        file.write(bmpfileheader);
        file.write(bmpinfoheader);

        int scanLineSize = output_width * colorComponentsCount;

        for (int i = 0; i < output_height; i++) {
            // In bmp file, lines go from bottom to top
            int offset = (output_height - i - 1) * scanLineSize;
            file.write(fileData, offset, scanLineSize);
            file.write(bmppad, 0, (-scanLineSize) & 3);
        }

        file.close();
    }
}
