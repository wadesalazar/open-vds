import openvds
import numpy as np
TEST_AWS_BUCKET = ""
TEST_AWS_OBJECTID = ""
TEST_AWS_REGION = ""

quit()

err = openvds.Error()
opt = openvds.AWSOpenOptions(TEST_AWS_BUCKET, TEST_AWS_OBJECTID, TEST_AWS_REGION)
handle = openvds.open(opt, err)
acc = openvds.VolumeDataAccessManager(handle)
r = acc.requestVolumeSubset((0,0,0),(100,100,100))

pacc = acc.manager.createVolumeDataPageAccessor(acc.layout, openvds.DimensionsND.Dimensions_012, 0, 0, 256, acc.manager.AccessMode.AccessMode_ReadOnly)
racc = acc.manager.create3DVolumeDataAccessorR32(pacc, -1000)
v = racc.getValue((100,100,100))

