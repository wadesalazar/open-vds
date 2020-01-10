import numpy as np
arr = np.ones((6), dtype=np.int)
cop = np.empty((6), dtype=np.int)
from openvds import core
t = core.TestArray()
t.setValues(arr)
t.getValues(cop)
assert not any(arr!=cop)
smallarr = np.ones((5), dtype=np.int)
