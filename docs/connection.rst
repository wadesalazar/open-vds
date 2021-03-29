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
  - sd
  - azureSAS
  - file

If the file protocol scheme is used the rest of the path will be decoded according to url encoding, however
if OpenVDS fails to recognise the protocol of a url it will treat the url as a local file.

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
  - LogFilenamePrefix
  - LogLevel (Note: this is to control the AWS specific logs. Possible values: Off, Fatal, Error, Warn, Info, Debug, Trace)
  - ConnectionTimoutMs
  - RequestTimeoutMs

``azure`` has two modes. The connection string mode or the bearer token mode.
If the connection string contains a key with name BearerToken it will parse the
string and extract following parameters:
  - AccountName
  - BearerToken
Both parameters are mandatory.

In the connection string mode the connection string is parsed according to the rules described here:
https://docs.microsoft.com/en-us/azure/storage/common/storage-configure-connection-string

``gs`` will accept the following keys

  - Token
  - CredentialsFile
  - JsonCredentials
  - SignedUrl (Possible values: True, Yes, On, False, No, Off. Default value: False, No, Off)
The Token is the OAuth2 token.

``sd`` will accept the following keys

  - sdauthorityurl
  - sdapikey
  - sdtoken
  - loglevel

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
  connection = "Token=some_token"

SD::

  url = "sd://tenent/subprojet/subfolder"
  connection = "sdauthorityurl=https://some_osdu.instance.com/seistore-svc/api/v3;sdapikey=ABC;sdtoken=some_token"

AzureSAS::

  url = "azureSAS://storageAccount.blob.core.windows.net/container/subpath"
  connection = "Suffix=?sv=2019-10-10&ss=bfqt&srt=sco&sp=rwdlacupx&se=2020-06-11T18:26:15Z&st=2020-06-11T10:26:15Z&spr=https&sig=V5glday54BztU8qtiIlRjEOnboiy4Y%2Fu%2FhbqRqWSN2E%3D"

