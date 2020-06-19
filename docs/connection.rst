.. _connection:

Connection URL and Connection String
************************************

When opening a VDS its possible to use a URL and a connection string. The
purpose of this is to create a unified connection API that can be used for
multiple IO backends.

The URL consist ``protocol://resource/sub_path``. The protocol will define the
backend to be used. Currently this can be:
  - s3
  - azure
  - gs
  - azureSAS

For ``s3`` and ``gs`` the ``resource`` will be the bucket while for ``azure`` the ``resource``
will be the container. ``azureSAS`` will treat ``resource/sub_path`` as the baseURL
of the SAS url and prepend ``https``.

The ``connection`` string will provide ``protocol`` or backend specific data for
the connection to the VDS. The string is a key/value string where the key and
value is separated with a ``=``. Each key/value pair is separated with a ``;``. It
is allowed for the value to contain extra ``=``, but the key can not.

The recognised keys for ``s3`` connection string are

  - Region
  - EndpointOverride
  - AccessKeyId
  - SecretKey
  - SessionToken
  - Expiration (Note: AWS SDK only accepts 'Z' timezone for ISO 8601 time)

``azure`` will treat the connection string as the connection string you get with your access keys.

``azureSAS`` will accept the following keys

  - Suffix
The suffix is the SAS token.

Some examples:

S3::

  url = "s3://my_bucket/somepath"
  connection = "Region=eu-north-1"
  #note that if Region is empty then the backend will try and find the region, but this only works if the credentials are the bucket owner

Azure::

  url = "azure://my_container/somepath"
  connection = "DefaultEndpointsProtocol=https;AccountName=developer;AccountKey=somekey;EndpointSuffix=core.windows.net"

GS::
  url = "gs://my_bucket/somepath"
  # there are no good paramteres to connection at current time.

AzureSAS::

  url = "azureSAS://storageAccount.blob.core.windows.net/container/subpath"
  connection = "Suffix=?sv=2019-10-10&ss=bfqt&srt=sco&sp=rwdlacupx&se=2020-06-11T18:26:15Z&st=2020-06-11T10:26:15Z&spr=https&sig=V5glday54BztU8qtiIlRjEOnboiy4Y%2Fu%2FhbqRqWSN2E%3D"

