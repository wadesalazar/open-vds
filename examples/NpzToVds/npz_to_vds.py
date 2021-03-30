import openvds
import numpy as np

import argparse
import sys
import math

parser = argparse.ArgumentParser(description="Converter converting numpy array file to VDS.")
parser.add_argument("--url", help="The url for where to put the VDS")
parser.add_argument("--connection", default="", help="The connection string for the url.")
parser.add_argument("--value-range-min", default=0, help="The lower bounds of the value range for the data.")
parser.add_argument("--value-range-max", default=0, help="The upper bounds of the value range for the data.")
parser.add_argument("npz_file", metavar="file", help="The npz file")

if len(sys.argv) == 1:
    parser.print_usage()
    sys.exit(0)

args = parser.parse_args()

print("Opening file {}:".format(args.npz_file))
data = np.load(args.npz_file,mmap_mode='r')['data']
data=  data.transpose()
print("Done reading file {}".format(args.npz_file))

(z_samples, y_samples, x_samples) = data.shape

value_range_min = args.value_range_min
value_range_max = args.value_range_max
if value_range_min == value_range_max: #invalid value range. Bruteforce valuerange
    print("Finding value range")
    value_range_min = 2**53
    value_range_max = -2**53
    x = data[:, math.floor(x_samples / 2), :]
    for y in x:
        for val in y:
            if val < value_range_min:
                value_range_min= val
            if value_range_max < val:
                value_range_max = val
    print("Using value range: [{} <-> {}]".format(value_range_min, value_range_max))


layoutDescriptor = openvds.VolumeDataLayoutDescriptor(openvds.VolumeDataLayoutDescriptor.BrickSize.BrickSize_64,
                                                      0, 0, 4,
                                                      openvds.VolumeDataLayoutDescriptor.LODLevels.LODLevels_None,
                                                      openvds.VolumeDataLayoutDescriptor.Options.Options_None)
compressionMethod = openvds.CompressionMethod(0)
compressionTolerance = 0.01

if openvds.isCompressionMethodSupported(openvds.CompressionMethod.WaveletLossless):
    layoutDescriptor = openvds.VolumeDataLayoutDescriptor(openvds.VolumeDataLayoutDescriptor.BrickSize.BrickSize_128,
                                                          2, 2, 4,
                                                          openvds.VolumeDataLayoutDescriptor.LODLevels.LODLevels_None,
                                                          openvds.VolumeDataLayoutDescriptor.Options.Options_None)
    compressionMethod = openvds.CompresionMethod.WaveletLossless
        
axisDescriptors = [ openvds.VolumeDataAxisDescriptor(x_samples, openvds.KnownAxisNames.x(), openvds.KnownUnitNames.meter(), 0.0, 2000.0),
                    openvds.VolumeDataAxisDescriptor(y_samples, openvds.KnownAxisNames.y(), openvds.KnownUnitNames.meter(), 0.0, 2000.0),
                    openvds.VolumeDataAxisDescriptor(z_samples, openvds.KnownAxisNames.z(), openvds.KnownUnitNames.meter(), 0.0, 2000.0),
                  ]
channelDescriptors = [ openvds.VolumeDataChannelDescriptor(openvds.VolumeDataChannelDescriptor.Format.Format_R32,
                                                           openvds.VolumeDataChannelDescriptor.Components.Components_1,
                                                           "Value", openvds.KnownUnitNames.metersPerSecond(), value_range_min, value_range_max)
                     ]

metaData = openvds.MetadataContainer()
metaData.setMetadataDoubleVector2(openvds.KnownMetadata.surveyCoordinateSystemOrigin().category, openvds.KnownMetadata.surveyCoordinateSystemOrigin().name, (1234.0, 4321.0))

vds = openvds.create(args.url, args.connection, layoutDescriptor, axisDescriptors, channelDescriptors, metaData, compressionMethod, compressionTolerance)
layout = openvds.getLayout(vds)

shape = (layout.getDimensionNumSamples(0), layout.getDimensionNumSamples(1), layout.getDimensionNumSamples(2))
manager = openvds.getAccessManager(vds)
accessor = manager.createVolumeDataPageAccessor(openvds.DimensionsND.Dimensions_012, 0, 0, 8, openvds.IVolumeDataAccessManager.AccessMode.AccessMode_Create, 1024)

print("Converting data")
for c in range(accessor.getChunkCount()):
    page = accessor.createPage(c)
    buf = np.array(page.getWritableBuffer(), copy = False)
    (min, max) = page.getMinMax()
    buf[:,:,:] = data[min[2]:max[2],min[1]:max[1],min[0]:max[0]]
    page.release()
accessor.commit()
openvds.close(vds)
print("Done processing into {}".format(args.url))
