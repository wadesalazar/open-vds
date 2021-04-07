import openvds
import numpy as np
from connection_defs import *

if not TEST_URL:
  quit()

handle = openvds.open(TEST_URL, TEST_CONNECTION)
acc = openvds.VolumeDataAccessManager(handle)
r = acc.requestVolumeSubset((0,0,0),(100,100,100))

accessor = acc.createVolumeData3DReadAccessorR32(openvds.DimensionsND.Dimensions_012)
v = accessor.getValue((100,100,100))

openvds.close(handle)
