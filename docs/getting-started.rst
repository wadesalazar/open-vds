.. _getting-started:

Getting Started Tutorial
************************

This tutorial walks you through opening an existing VDS and requesting a slice of data from it.

Opening a VDS
-------------

To open a VDS we set up the :cpp:struct:`OpenOptions` for the object store the
VDS. We can create this directly or we can use a url string and a connection
parameter that will create a OpenOptions for us. In this tutorial we use the
url string and connection string.

The url string will have a shceme referring to the cloud provider and a path such as:
.. code-block:: none

  s3://bucket/vdsKey

or

.. code-block:: none

  azure://container/vdsKey

The connection string for s3 could look like this:

.. code-block:: none

  AccessKeyId=xxx;SecretKey=xxx;SessionToken=xxx;Region=eu-north-1

while the azure connection string could look like this:

.. code-block:: none

  DefaultEndpointsProtocol=https;AccountName=developer;AccountKey=xxx;EndpointSuffix=core.windows.net

.. code-block:: cpp

  std::string url = TEST_URL;
  std::string connectionString = TEST_CONNECTION;

We will then call :cpp:func:`Open` to get the :cpp:type:`VDSHandle`:

.. code-block:: cpp

  OpenVDS::Error error;
  OpenVDS::VDSHandle handle = OpenVDS::Open(url, connectionString, error);

  if(error.code != 0)
  {
    std::cerr << "Could not open VDS: " << error.string << std::endl;
    exit(1);
  }

If everything went well we can now get the :cpp:class:`VolumeDataAccessManager` that we will use to request data from the VDS and the :cpp:class:`VolumeDataLayout` that we can use to get information about the layout of the VDS:

.. code-block:: cpp

  OpenVDS::VolumeDataAccessManager accessManager = OpenVDS::GetAccessManager(handle);
  OpenVDS::VolumeDataLayout const *layout = accessManager.GetVolumeDataLayout();

Using the VolumeDataLayout
-------------
We can now use the VolumeDataLayout to find the inline number in the middle of the dataset and transform that inline number to an index in the VDS data:

.. code-block:: cpp

  const int sampleDimension = 0, crosslineDimension = 1, inlineDimension = 2;
  OpenVDS::VolumeDataAxisDescriptor inlineAxisDescriptor = layout->GetAxisDescriptor(inlineDimension);
  int inlineNumber = int((inlineAxisDescriptor.GetCoordinateMin() + inlineAxisDescriptor.GetCoordinateMax()) / 2);
  int inlineIndex = inlineAxisDescriptor.CoordinateToSampleIndex((float)inlineNumber);

The VolumeDataLayout can be used to find out which data channels are available, the names and units of channels and axes, the estimated value range and data types for the channels and the metadata of the VDS (containing e.g. the UTM coordinates).

Requesting a slice of data from a VDS
-------------
To request data we need to set up the index region that we want to read:

.. code-block:: cpp

  int voxelMin[OpenVDS::Dimensionality_Max] = { 0, 0, 0, 0, 0, 0};
  int voxelMax[OpenVDS::Dimensionality_Max] = { 1, 1, 1, 1, 1, 1};

  voxelMin[sampleDimension] = 0;
  voxelMax[sampleDimension] = layout->GetDimensionNumSamples(sampleDimension);
  voxelMin[crosslineDimension] = 0;
  voxelMax[crosslineDimension] = layout->GetDimensionNumSamples(crosslineDimension);
  voxelMin[inlineDimension] = inlineIndex;
  voxelMax[inlineDimension] = inlineIndex + 1;

Then we can make the request for data, the data is automatically converted to the type of the buffer (there is also an overload with a void-pointer and a format):

.. code-block:: cpp

  std::vector<float> buffer(layout->GetDimensionNumSamples(sampleDimension) * layout->GetDimensionNumSamples(crosslineDimension));

  auto request = accessManager.RequestVolumeSubset(buffer.data(), buffer.size() * sizeof(float), OpenVDS::Dimensions_012, 0, 0, voxelMin, voxelMax);

Because all requests in OpenVDS are asynchronous we need to wait for the request to complete before we can access the data in the buffer:

.. code-block:: cpp

  bool success = request->WaitForCompletion();

Alternatively you can let OpenVDS allocate the buffer for you, getting the data will block until the request has completed or throw an exception:

.. code-block:: cpp

  auto request = accessManager.RequestVolumeSubset<float>(OpenVDS::Dimensions_012, 0, 0, voxelMin, voxelMax);
  std::vector<float> data = std::move(request->Data());

The complete code for this tutorial can be found in examples/GettingStarted.
