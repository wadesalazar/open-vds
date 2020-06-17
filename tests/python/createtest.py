import openvds
opt = openvds.InMemoryOpenOptions()

layoutDescriptor = openvds.VolumeDataLayoutDescriptor(openvds.VolumeDataLayoutDescriptor.BrickSize.BrickSize_256,
                                                     -1, 1, 1,
                                                      openvds.VolumeDataLayoutDescriptor.LODLevels.LODLevels_4,
                                                     openvds.VolumeDataLayoutDescriptor.Options.Options_None, 3)
        
axisDescriptors = [ openvds.VolumeDataAxisDescriptor(400, "X", "m", 0.0, 2000.0),
                    openvds.VolumeDataAxisDescriptor(400, "Y", "m", 0.0, 2000.0),
                    openvds.VolumeDataAxisDescriptor(400, "Z", "m", 0.0, 2000.0),
                  ]
channelDescriptors = [ openvds.VolumeDataChannelDescriptor(openvds.VolumeDataChannelDescriptor.Format.Format_R32,
                                                          openvds.VolumeDataChannelDescriptor.Components.Components_1,
                                                          "Amplitude", "void", 0.0, 100.0)
                     ]

metaData = openvds.MetadataContainer()
vds = openvds.create(opt, layoutDescriptor, axisDescriptors, channelDescriptors, metaData)
