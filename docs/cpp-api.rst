.. _cpp-api:

C++ API
*******

.. doxygenfunction:: OpenVDS::Open(const  OpenOptions  &, Error  &)

.. doxygenfunction:: OpenVDS::Open(IOManager *, Error  &)

.. doxygenfunction:: OpenVDS::Create(const  OpenOptions  &, VolumeDataLayoutDescriptor  const &, VectorWrapper <  VolumeDataAxisDescriptor  >, VectorWrapper <  VolumeDataChannelDescriptor  >, MetadataReadAccess  const &, Error  &)

.. doxygenfunction:: OpenVDS::Create(IOManager *, VolumeDataLayoutDescriptor  const &, VectorWrapper <  VolumeDataAxisDescriptor  >, VectorWrapper <  VolumeDataChannelDescriptor  >, MetadataReadAccess  const &, Error  &)

.. doxygenfunction:: OpenVDS::Close

.. doxygenstruct:: OpenVDS::AWSOpenOptions
  :members:

.. doxygenstruct:: OpenVDS::AzureOpenOptions
  :members:

.. doxygenstruct:: OpenVDS::GSOpenOptions
  :members:

.. doxygenfunction:: OpenVDS::GetAccessManager

.. doxygenfunction:: OpenVDS::GetLayout

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
