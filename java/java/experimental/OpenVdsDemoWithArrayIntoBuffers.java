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
import org.opengroup.openvds.VolumeDataChannelDescriptor.Format;

import java.io.FileOutputStream;
import java.nio.FloatBuffer;

public class OpenVdsDemoWithArrayIntoBuffers {
    public static void main(String[] args) throws Exception {
        System.out.println(JniPointer.libraryDescription());

        process();
    }

    public static void process() throws Exception {
        int output_width = 500;
        int output_height = 500;
        int axis_position = Integer.MIN_VALUE;

        System.out.println("Library: " + JniPointer.libraryDescription());
        int nXSamples = 60, nYSamples = 60, nZSamples = 60;
        Format format = Format.FORMAT_U8;

        System.out.println("Create MemoryVdsGenerator...");
        MemoryVdsGenerator generator = new MemoryVdsGenerator(nXSamples, nYSamples, nZSamples, format);


        VolumeDataLayout layout = generator.getLayout();
        //printLayout(layout);

        VolumeDataAccessManager accessManager = generator.getAccessManager();

        int[] axis_mapper = {0, 1, 2};
        int[] sampleCount = new int[3];
        sampleCount[0] = layout.getDimensionNumSamples(axis_mapper[0]);
        sampleCount[1] = layout.getDimensionNumSamples(axis_mapper[1]);
        sampleCount[2] = layout.getDimensionNumSamples(axis_mapper[2]);

        System.out.println("\nFound data set with sample count "
                + sampleCount[0] + "x" + sampleCount[1] + "x" + sampleCount[2]);

        System.out.println("\nCreate request for samples...");

        axis_position = Math.max(0, axis_position);
        axis_position = Math.min(sampleCount[0], axis_position);

        float x_sample_shift = (float) sampleCount[1] / output_width;
        float y_sample_shift = (float) sampleCount[2] / output_height;
        final int elemSize = VolumeDataAccessManager.Dimensionality_Max;
        final int elemCount = output_width * output_height;
        float[] samples = new float[elemCount * elemSize];

        for (int y = 0; y < output_height; y++) {
            float y_pos = y * y_sample_shift + 0.5f;
            for (int x = 0; x < output_width; x++) {
                float x_pos = x * x_sample_shift + 0.5f;
                int offset = (y * output_width + x) * elemSize;
                samples[offset + axis_mapper[0]] = axis_position + 0.5f;
                samples[offset + axis_mapper[1]] = x_pos;
                samples[offset + axis_mapper[2]] = y_pos;
            }
        }

        System.out.println("Request samples from VolumeDataAccessManager...");

//        for (int i = 0; i < 100000; i++) {
            final FloatBuffer positions = BufferUtils.toBuffer(samples);
            final FloatBuffer sampleBuffer = BufferUtils.createFloatBuffer(output_width * output_height);

            long request = accessManager.requestVolumeSamples(sampleBuffer, layout, DimensionsND.DIMENSIONS_012, 0, 0,
                    positions, elemCount, InterpolationMethod.LINEAR);

            System.out.println("Wait for request completion...");
            boolean finished = accessManager.waitForCompletion(request);
            if (!finished) {
                throw new Exception("Failed to download request");
            }
            System.out.println("End of request " + request);

//            BufferUtils.release(positions);
//            BufferUtils.release(sampleBuffer);
//        }

        System.out.println("Create bitmap " + output_width + "x" + output_height + " from samples...");
        String outFileName = "/tmp/OpenVdsDemo_Output_" + axis_position + ".bmp";

        writeBitmap(outFileName, layout, BufferUtils.toArrayAndRelease(sampleBuffer), output_width, output_height);
        System.out.println("Picture is written to file: " + outFileName);

        // Test of finalization. Normally manual call of release() is not needed:
        // it is called from JniPointer.finalize()
        System.out.println("Release objects...");
        generator.release();
        System.out.println("Finished");
    }

    static void writeBitmap(String fileName, VolumeDataLayout layout,
                            float[] data, int output_width, int output_height) throws Exception {

        float minValue = layout.getChannelValueRangeMin(0);
        float maxValue = layout.getChannelValueRangeMax(0);
        float intScale = layout.getChannelIntegerScale(0);
        float intOffset = layout.getChannelIntegerOffset(0);

        QuantizingValueConverter_FloatToByte converter
                = new QuantizingValueConverter_FloatToByte(minValue, maxValue, intScale, intOffset);
        final int colorComponentsCount = 3;
        int dataSize = output_width * output_height * colorComponentsCount;
        byte[] fileData = new byte[dataSize];

        for (int y = 0; y < output_height; y++) {
            for (int x = 0; x < output_width; x++) {
                int inOffset = y * output_width + x;
                byte value = converter.convertValue(data[inOffset]);
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
        converter.release();
    }
}
