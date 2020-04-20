## OpenVDS
OpenVDS is a specification and an open source reference implementation of a storage format for fast random access to multi-dimensional (up to 6D) volumetric data stored in an object storage cloud service (e.g. Amazon S3 or Azure Blob storage). The specification is based on, but not similar to, the existing Volume Data Store (VDS) file format. The VDS format is a Bluware Inc. proprietary format, which has seen extensive industrial deployments over the last two decades. The design of the VDS format is contributed by Bluware Inc. to the Open Group Open Subsurface Data Universe Forum (OSDU) (The Open Group, u.d.).

OpenVDS has been designed to handle extremely large volumes, up to petabytes in size, with variable sized compressed bricks. The OpenVDS format is very flexible and can store any kind data representable as arrays with key/value-pair metadata. In particular, data commonly used in seismic processing can be stored along with all necessary metadata. This makes it possible to go from legacy formats to OpenVDS and back, while retaining all metadata.

OpenVDS may be used to store E&P data types such as regularized single-Z horizons/height-maps (2D), seismic lines (2D), pre-stack volumes (3D-5D), post-stack volumes (3D), geobody volumes (3D-5D), and attribute volumes of any dimensionality up to 6D.

The format has been designed primarily to support random access and on-demand fetching of data, this enables applications that are responsive and interactive as well as efficient I/O for high-performance computing or machine learning workloads.

The OpenVDS implementation is made up of the following components:
- SEG-Y indexing (scanning the file to figure out where the traces go in the volume)
- SEG-Y loading (transfer to object storage)
- SEG-Y delivery (transfer from object storage)
- VolumeDataAccess API for direct access to volume data stored in the cloud
- Python bindings for the VolumeDataAccess API

In order to implement these components there are a number of internal components:
- Decompression (Zip, Run-lenght encoding, Bluware Inc. properitary Wavelet compression)
- VolumeDataLayout (manages how the volume is divided into chunks)
- File (UTF-8 filenames, thread-safe read/write, possibility to create memory-mapped file views)

Licensed under [**Apache 2.0**](https://gitlab.opengroup.org/osdu/open-vds/blob/master/LICENSE)

### Building
By default OpenVDS builds the Python 3 bindings. To install the required dependencies go into ${OpenVDSFolder}/python folder and run:
`$ python -m pip install -r requirements-dev.txt`
To disable building the Python 3 bindings use the -DBUILD_PYTHON=OFF cmake argument.

To install the python bindings as a site-package run:
`$ python setup.py install`

This will use the python executable as the target python distribution.

When building using cmake the cmake variable Python3_ROOT_DIR can be used to
specify a specific python installation.

Otherwise the cmake find_package
python rules will be used. Since CMake version 3.13 Python_FIND_REGISTRY can be
used to modify search order on windows. Ie. to disable searching the registry
pass the cmake option: -DPython_FIND_REGISTRY=NEVER.

#### Linux
Make a build directory in the OpenVDS folder and change current directory to the created folder.
Do:
`$ cmake ..`
to configure with default build settings. Now its possible to do `$ make -j8`
to build and `$ make install` to install the package. To specify a custom
install location specify the -DCMAKE_INSTALL_PREFIX=PATH cmake argument.
OpenVDS requires some dependencies on linux. These dependencies are transient
from the different network storage integrations. Currently these packages are required:
- curl
- openssl
- boost
- xml2
- zlib
- libuv
- uuid

#### Windows Visual Studio
Much like on Linux its possible to generate a project in a build folder and build it using native tools.
To generate a Visual Studio solution make a build directory in the OpenVDS folder and change current directory to the build folder. Either use `$ cmake ..` or launch cmake-gui and generate a visual studio solution.

OpenVDS also supports using the cmake integration in Visual Studio. Open Visual Studio and use the "Open a local folder" to open the OpenVDS folder.

This works with default settings with Visual Studio 2019, but there are some
limitations using Visual Studio 2017. Before opening the project for the first
time in Visual Studio 2017 do the following commands in the root OpenVDS project
folder:
`$ git checkout 0d7825df9c981f624b6e1197a1b90c74ddae6aa9 -- CMakeSettings.json`
`$ cmake -P CMake/Fetch3rdParty.cmake`
Then start like other cmake projects in Visual Studio


#### Build options
- BUILD_PYTHON (ON|OFF)
- BUILD_DOCS (ON|OFF) Default to OFF
- ENABLE_OPENMP (ON|OFF)
- BUILD_ZLIB (ON|OFF)
- CMAKE_INSTALL_PREFIX (PATH)
- CMAKE_BUILD_TYPE (Debug|Release|RelWithDebInfo|MinSizeRel)

Build options are arguments to cmake. `$ cmake -DBUILD_PYTHON=OFF ..` would turn off building python.

#### Building documentation
The following tools are needed to build the documentation:
- Doxygen
- Sphinx
- Breathe

On Windows download the Doxygen binary from: http://doxygen.nl/download.html
Then install Sphinx and Breathe with pip:
C:\> pip install -U sphinx breathe

Add the -DBUILD_DOCS=ON to the cmake argument list
