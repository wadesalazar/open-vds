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
