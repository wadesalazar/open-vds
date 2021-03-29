import openvds
import numpy as np
from connection_defs import *

if not TEST_URL:
  quit()

handle = openvds.open(TEST_URL, TEST_CONNECTION)
acc = openvds.VolumeDataAccessManager(handle)
r = acc.requestVolumeSubset((0,0,0),(100,100,100))
r.waitForCompletion()

pacc = acc.manager.createVolumeDataPageAccessor(acc.layout, openvds.DimensionsND.Dimensions_012, 0, 0, 256, acc.manager.AccessMode.AccessMode_ReadOnly)
racc = acc.manager.create3DVolumeDataAccessorR32(pacc, -1000)
v = racc.getValue((100,100,100))

openvds.close(handle)
