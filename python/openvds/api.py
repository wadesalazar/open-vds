import openvds.core
from openvds.core import *
from .volumedataaccess import VolumeDataAccessManager

def getAccessManager(handle: int):
    """Get the VolumeDataAccessManager for a VDS
    
    Parameter `handle`:
        The handle of the VDS
    
    Returns:
        The VolumeDataAccessManager of the VDS
    """
    return VolumeDataAccessManager(handle)
