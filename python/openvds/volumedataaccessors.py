
from openvds.core import VolumeDataAccessManager, VolumeDataPageAccessor, InterpolationMethod

class VolmeDataAccessorManager(object):
    def __init__(self, factoryName: str, manager: VolumeDataAccessManager, pageAccessor: VolumeDataPageAccessor, replacementNoValue=None, interpolationMethod: InterpolationMethod=None):
        self.factory                = getattr(manager, factoryName)
        self.manager                = manager
        self.pageAccessor           = pageAccessor
        self.replacementNoValue     = replacementNoValue or 0.0
        self.interpolationMethod    = interpolationMethod
        self.accessor               = None
        
    def __enter__(self): 
        if self.interpolationMethod:
            self.accessor = self.factory(self.pageAccessor, self.replacementNoValue, self.interpolationMethod)
        else:
            self.accessor = self.factory(self.pageAccessor, self.replacementNoValue)
        return self.accessor
      
    def __exit__(self, exc_type, exc_value, exc_traceback): 
        if self.accessor:
            self.manager.destroyVolumeDataAccessor(self.accessor)
            self.accessor = None
      
    
