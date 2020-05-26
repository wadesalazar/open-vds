## SEGYExport

A tool to export a volume data store (VDS) to a SEG-Y file.

Usage:
```
SEGYExport [OPTION...] <output file>
```

| Option                       | Decription |
|------------------------------|------------|
|--bucket \<string>             | AWS S3 bucket to export from.
|--region \<string>             | AWS region of bucket to export from.
|--connection-string \<string>  | Azure Blob Storage connection string.
|--container \<string>          | Azure Blob Storage container to export from .
|--parallelism-factor \<value>  | Azure parallelism factor.
|--prefix \<string>             | Top-level prefix to prepend to all object-keys.
|--persistentID \<ID>          | A globally unique ID for the VDS, usually an 8-digit hexadecimal number.
|--output \<arg>               | The output SEG-Y file.

SEGYExport is used to export from a VDS to a SEG-Y file. A VDS that was
imported from a SEG-Y file stores the SEGY-Y binary and text headers from the
original SEG-Y (in the VDS metadata) and the original trace headers and live
trace flags (in separate data channels) and these will be used to re-create the
original SEG-Y file. The output file will only be identical to the original if
the VDS was compressed with a lossless algorithm (or uncompressed) and all
traces fit in the array defined by the VDS (i.e. no duplicate traces).

Either a `--container` (for Azure) or a `--bucket` (for AWS) argument, a
`--persistentID` argument and an output SEG-Y file must be specified.

Example usage:
```
SEGYExport --bucket openvds-test --persistentID 7068247E9CA6EA05 D:\\Datasets\\Australia\\shakespeare3d_pstm_Time_export.segy
```

NOTE:
If the VDS does not contain SEG-Y headers, SEGYExport will not currently be
able to export to a SEG-Y file, but this will be fixed in a later release.
