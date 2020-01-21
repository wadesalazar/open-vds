import openvds.core
from openvds.core import *
from .volumedataaccess import VolumeDataAccess

def open(options: OpenOptions = None, ioManager: IOManager = None, err: Error = None) -> int: 
    """Open an existing VDS
    
    Parameter `options`:
        The options for the connection
    
    Parameter `ioManager`:
        The IOManager for the connection, it will be deleted automatically
        when the VDS handle is closed

    Either `options` or `ioManager` must be specified.
    
    Parameter `error`:
        If an error occured, the error code and message will be written to
        this output parameter
    
    Returns:
        The VDS handle that can be used to get the VolumeDataLayout and
        the VolumeDataAccessManager
    """
    if options:
        return openvds.core.open(options, err or openvds.core.Error())
    else:
        return openvds.core.open(ioManager, err or openvds.core.Error())

def close(handle: int):
    """Close a VDS and free up all associated resources

    Parameter `handle`:
        The handle of the VDS
    """
    openvds.core.close(handle)
    