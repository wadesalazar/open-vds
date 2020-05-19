.. _cpp-api:

C++ API
*******

.. doxygenfunction:: OpenVDS::Open(const OpenOptions &options, Error &error)

.. doxygenfunction:: OpenVDS::Open(IOManager *ioManager, Error &error)

.. doxygenfunction:: OpenVDS::Create(const OpenOptions &options, VolumeDataLayoutDescriptor const &layoutDescriptor, VectorWrapper<VolumeDataAxisDescriptor> axisDescriptors, VectorWrapper<VolumeDataChannelDescriptor> channelDescriptors, MetadataReadAccess const &metadata, Error &error)

.. doxygenfunction:: OpenVDS::Create(IOManager *ioManager, VolumeDataLayoutDescriptor const &layoutDescriptor, VectorWrapper<VolumeDataAxisDescriptor> axisDescriptors, VectorWrapper<VolumeDataChannelDescriptor> channelDescriptors, MetadataReadAccess const &metadata, Error &error)

.. doxygenfunction:: OpenVDS::Close(VDSHandle handle)

.. doxygenstruct:: OpenVDS::AWSOpenOptions
  :members:

.. doxygenstruct:: OpenVDS::AzureOpenOptions
  :members:

.. doxygenfunction:: OpenVDS::GetAccessManager(VDSHandle handle)

.. doxygenfunction:: OpenVDS::GetLayout(VDSHandle handle)

.. doxygenclass:: OpenVDS::VolumeDataAccessManager
  :members:

.. doxygenclass:: OpenVDS::VolumeDataLayout
  :members:

.. doxygenclass:: OpenVDS::VolumeDataAxisDescriptor
  :members:

.. doxygenclass:: OpenVDS::VolumeDataChannelDescriptor
  :members:

.. doxygenclass:: OpenVDS::VolumeDataLayoutDescriptor
  :members:

.. doxygenclass:: OpenVDS::MetadataContainer
  :members:

.. doxygenclass:: OpenVDS::MetadataReadAccess
  :members:

.. doxygenclass:: OpenVDS::MetadataWriteAccess
  :members:
