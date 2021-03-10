import openvds
import numpy as np

opt = openvds.InMemoryOpenOptions()

layoutDescriptor = openvds.VolumeDataLayoutDescriptor(openvds.VolumeDataLayoutDescriptor.BrickSize.BrickSize_64,
                                                      0, 0, 4,
                                                      openvds.VolumeDataLayoutDescriptor.LODLevels.LODLevels_None,
                                                      openvds.VolumeDataLayoutDescriptor.Options.Options_None)
        
axisDescriptors = [ openvds.VolumeDataAxisDescriptor(300, "X", "m", 0.0, 2000.0),
                    openvds.VolumeDataAxisDescriptor(200, "Y", "m", 0.0, 2000.0),
                    openvds.VolumeDataAxisDescriptor(100, "Z", "m", 0.0, 2000.0),
                  ]
channelDescriptors = [ openvds.VolumeDataChannelDescriptor(openvds.VolumeDataChannelDescriptor.Format.Format_R32,
                                                           openvds.VolumeDataChannelDescriptor.Components.Components_1,
                                                           "Value", "", 0.0, (300.0 * 200.0 * 100.0) - 1.0)
                     ]

metaData = openvds.MetadataContainer()
metaData.setMetadataDoubleVector2(openvds.KnownMetadata.surveyCoordinateSystemOrigin().category, openvds.KnownMetadata.surveyCoordinateSystemOrigin().name, (1234.0, 4321.0))
vds = openvds.create(opt, layoutDescriptor, axisDescriptors, channelDescriptors, metaData)
layout = openvds.getLayout(vds)
origin = layout.getMetadata(openvds.KnownMetadata.surveyCoordinateSystemOrigin())
if origin != (1234.0, 4321.0):
    raise "Origin not set correctly"

def writePages(accessor, data):
    for c in range(accessor.getChunkCount()):
        page = accessor.createPage(c)
        buf = np.array(page.getWritableBuffer(), copy = False)
#        print("shape {} strides {}".format(buf.shape, buf.strides))
        min = np.empty(6, dtype = np.int32)
        max = np.empty(6, dtype = np.int32)
        page.getMinMax(min, max)
#        print("min {} max {}".format(min, max))
        buf[:max[2],:max[1],:max[0]] = data[min[2]:max[2],min[1]:max[1],min[0]:max[0]]
        page.release()
    accessor.commit()

shape = (layout.getDimensionNumSamples(2), layout.getDimensionNumSamples(1), layout.getDimensionNumSamples(0))
data = np.arange(shape[0]*shape[1]*shape[2], step=1.0, dtype=np.float).reshape(shape)

manager = openvds.getAccessManager(vds)
accessor = manager.createVolumeDataPageAccessor(openvds.DimensionsND.Dimensions_012, 0, 0, 8, openvds.IVolumeDataAccessManager.AccessMode.AccessMode_Create, 1024)
writePages(accessor, data)

req = manager.requestVolumeSubset(min=(0,0,0), max=(shape[2], shape[1], shape[0]))
result = req.data.reshape(shape)
assert np.array_equal(data, result)
