## OpenVDS
OpenVDS is a specification and an open source reference implementation of a
storage format for fast random access to multi-dimensional (up to 6D)
volumetric data stored in an object storage cloud service (e.g.
Amazon S3, Azure Blob storage or Google Cloud Storage). The specification is
based on, but not similar to, the existing Volume Data Store (VDS) file format.
The VDS format is a Bluware Inc. proprietary format, which has seen extensive
industrial deployments over the last two decades. The design of the VDS format
is contributed by Bluware Inc. to the Open Group Open Subsurface Data Universe
Forum (OSDU) (The Open Group, u.d.).

OpenVDS has been designed to handle extremely large volumes, up to petabytes in
size, with variable sized compressed bricks. The OpenVDS format is very
flexible and can store any kind data representable as arrays with
key/value-pair metadata. In particular, data commonly used in seismic
processing can be stored along with all necessary metadata. This makes it
possible to go from legacy formats to OpenVDS and back, while retaining all
metadata.

OpenVDS may be used to store E&P data types such as regularized single-Z
horizons/height-maps (2D), seismic lines (2D), pre-stack volumes (3D-5D),
post-stack volumes (3D), geobody volumes (3D-5D), and attribute volumes of any
dimensionality up to 6D.

The format has been designed primarily to support random access and on-demand
fetching of data, this enables applications that are responsive and interactive
as well as efficient I/O for high-performance computing or machine learning
workloads.

The OpenVDS implementation is made up of the following components:
- VolumeDataAccess C++ API for direct access to volume data stored in a VDS
- Python bindings for the VolumeDataAccess API
- Java bindings for the VolumeDataAccess API
- SEGYImport tool (import a SEG-Y file to a VDS)
- SEGYExport tool (export a SEG-Y file from a VDS)
- VDSInfo tool (transfer from object storage)

In order to implement these components there are a number of internal components:
- Decompression (Zip, Run-length encoding, Bluware Inc. proprietary Wavelet compression)
- VolumeDataLayout (manages how the volume is divided into chunks)
- IOManagers for connecting to the various cloud providers' object storage solutions
- File (UTF-8 filenames, thread-safe read/write, possibility to create memory-mapped file views)

Licensed under [**Apache 2.0**](https://gitlab.opengroup.org/osdu/open-vds/blob/master/LICENSE)

Latest build of the [**OpenVDS-Documentation**](http://osdu.pages.community.opengroup.org/platform/domain-data-mgmt-services/seismic/open-vds)

### Building
If OpenVDS is built with the AWS integration then git needs to be in the PATH.

NOTE: CMake will download 3rdparty dependencies the first time CMake is run on
the OpenVDS repository. This will take some time, so be patient. This will not
occur on subsequent builds.

By default OpenVDS builds the Python 3 bindings. To install the required
dependencies go into ${OpenVDSFolder}/python folder and run:
`$ python -m pip install -r requirements-dev.txt`
To disable building the Python 3 bindings use the -DBUILD_PYTHON=OFF cmake argument.

To install the python bindings as a site-package run:
`$ python setup.py install`

This will use the python executable as the target python distribution.

When building using cmake the cmake variable Python3_ROOT_DIR can be used to
specify a specific python installation.

Otherwise the cmake find_package
python rules will be used. Since CMake version 3.13 Python_FIND_REGISTRY can be
used to modify search order on windows. For example, to disable searching the 
registry pass the cmake option: -DPython_FIND_REGISTRY=NEVER.

NOTE: On Windows scikit-build will use Visual Studio 2017 if python version is
>= 3.6.  There is a mergerequest for scikit-build to support
[**Visual Studio 2019**](https://github.com/scikit-build/scikit-build/pull/526).

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
When building on Windows the Desktop C++ component for Visual Studio must be
installed. Install Visual Studio CMake integration if OpenVDS should be
compiled with the "Open a local folder" option.

Much like on Linux its possible to generate a project in a build folder and
build it using native tools.  To generate a Visual Studio solution make a build
directory in the OpenVDS folder and change current directory to the build
folder. Either use `$ cmake ..` or launch cmake-gui and generate a Visual
Studio solution.

OpenVDS also supports using the cmake integration in Visual Studio. Open Visual
Studio and use the "Open a local folder" to open the OpenVDS folder.

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

#### Building OSDU Docker image
The OSDU Docker image is built using `python3 build_osdu_image.py` in the root
of this repository. It uses a multistage Dockerfile to build OpenVDS and tools
from scratch, and then deploy them into a minimal size image on top of an
up-to-date Ubuntu 20.04 LTS base.

This single build will support all backends, which currently consists of
- Azure Blob Storage (azure, azureSAS)
- Google Cloud Storage (gs)
- AWS S3 (s3)
- OSDU/DELFI Seismic DMS (sd)

OpenVDS' URL scheme follows `protocol://resource/sub_path`, where the protocol
refers to the backend mentioned above. For `s3` and `gs` the resource will be
the bucket, while for `azure` or `azureSAS` the resource will be the container,
and for `sd` the resource will be the tenant name.

OpenVDS relies on the storage libraries from the various Cloud Service Providers,
and will support the environment variables these libraries themselves detect.

Azure Blob Storage [Azure Storage Client Library for C++ (7.5.0)]:
- None
- See https://github.com/Azure/azure-storage-cpp for more

Google Cloud Storage:
- `GOOGLE_CLOUD_PROJECT`
- `GOOGLE_CLOUD_CPP_ENABLE_CLOG`
- `GOOGLE_APPLICATION_CREDENTIALS`
- `GOOGLE_GCLOUD_ADC_PATH_OVERRIDE`
- `GOOGLE_RUNNING_ON_GCE_CHECK_OVERRIDE`
- `CLOUD_STORAGE_ENABLE_CLOG`
- `CLOUD_STORAGE_ENABLE_TRACING`
- `CLOUD_STORAGE_TESTBENCH_ENDPOINT`
- See https://googleapis.github.io/google-cloud-cpp/0.8.0/storage/client__options_8h_source.html for more

AWS S3:
- `AWS_ACCESS_KEY_ID`
- `AWS_CA_BUNDLE`
- `AWS_DEFAULT_OUTPUT`
- `AWS_CONFIG_FILE`
- `AWS_DEFAULT_REGION`
- `AWS_MAX_ATTEMPTS`
- `AWS_METADATA_SERVICE_NUM_ATTEMPTS`
- `AWS_METADATA_SERVICE_TIMEOUT`
- `AWS_PROFILE`
- `AWS_RETRY_MODE`
- `AWS_SECRET_ACCESS_KEY`
- `AWS_SESSION_TOKEN`
- `AWS_SHARED_CREDENTIALS_FILE`
- `AWS_STS_REGIONAL_ENDPOINTS`
- `CA_BUNDLE`
- See https://docs.aws.amazon.com/credref/latest/refdocs/environment-variables.html for more

OSDU/DELFI Seismic DMS:
- `SD_IMP_AUTH_TOKEN`
- `SD_SVC_API_KEY`
- `SD_SVC_ENV`
- `SD_SVC_URL`
- `SD_CURL_VERBOSE_RANDOM`
- `SD_SEISTORE_TELEMETRY`
- `SD_TIMER_REPORT_IF_SIZE`
- `SD_TIMER_REPORT_INTERVAL`
- `SD_TIMER_REPORT_RANDOM`
- `ENABLE_CURL_SIGNAL_HANDLERS`
- `DISABLE_SSL_CERTIFICATE_VERIFICATION`
- See https://community.opengroup.org/osdu/platform/domain-data-mgmt-services/seismic/seismic-dms-suite/seismic-store-cpp-lib/-/tree/master#environemnt-control-variables for more