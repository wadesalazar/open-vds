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

import openvds.core
from openvds.core import *
from .volumedataaccess import VolumeDataAccessManager

openvds.core.IVolumeDataAccessManager.AccessMode = openvds.core.VolumeDataPageAccessor.AccessMode
openvds.core.VolumeDataAccessManager.AccessMode = openvds.core.VolumeDataPageAccessor.AccessMode
VolumeDataAccessManager.AccessMode = openvds.core.VolumeDataPageAccessor.AccessMode

def getAccessManager(handle: int):
    """Get the VolumeDataAccessManager for a VDS
    
    Parameter `handle`:
        The handle of the VDS
    
    Returns:
        The VolumeDataAccessManager of the VDS
    """
    return VolumeDataAccessManager(handle)
