############################################################################
# Copyright 2019 The Open Group
# Copyright 2019 Bluware, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
###########################################################################/

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
