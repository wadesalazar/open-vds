import openvds
import numpy as np
from connection_defs import *

if not TEST_URL:
  quit()

handle = openvds.open(TEST_URL, TEST_CONNECTION)
layout = openvds.getLayout(handle)
acc = openvds.VolumeDataAccessManager(handle)
r = acc.requestVolumeSubset((0,0,0),(100,100,100))

accessor = acc.createVolumeData3DReadAccessorR32(openvds.DimensionsND.Dimensions_012)
v = accessor.getValue((int(layout.getDimensionNumSamples(2) / 1.4), int(layout.getDimensionNumSamples(1) / 2.2), int(layout.getDimensionNumSamples(0) / 1.55)))

openvds.close(handle)
