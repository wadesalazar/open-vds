import openvds.core
import numpy as np

from typing import Dict, Tuple, Sequence

class VolumeDataLayout(object):
    def __init__(self, handle: int):
        self.handle         = handle
        self._layout        = openvds.core.getLayout(handle)

    @property
    def dimensions(self):
        return self._layout.getDimensionality()
    
    @property
    def shape(self):
        return tuple([self._layout.getDimensionNumSamples(i) for i in range(self.dimensions)])
        
        