## VDSInfo

A tool for extracting info from a VDS.

Usage:
```
VDSInfo [OPTION...]
```

| Option                        | Decription |
|-------------------------------|------------|
| --url \<string>               | Url for the VDS
| --connection \<string>        | Connection string for the VDS
| --persistentID \<ID>          | A globally unique ID for the VDS, usually an 8-digit hexadecimal number.
| --axis                        | Print axis descriptors.
| --channels                    | Print channel descriptors.
| --layout                      | Print layout.
| --metadatakeys                | Print metadata keys.
| --metadata-name \<string>     | Print metadata matching name.
| --metadata-category \<string> | Print metadata matching category.
| --metadata-firstblob          | Print first blob found.
| --metadata-autodecode         | Autodetect EBCDIC and decode to ASCII for blobs.
| --metadata-force-width \<arg> | Force output width.

VDSInfo prints out the result of the query in json, and it will try and give
the shortest json for the specific query by eliminating redundant json parent
structures.

For more information about the ``--url`` and ``--connection`` parameter please see:
http://osdu.pages.community.opengroup.org/platform/domain-data-mgmt-services/seismic/open-vds/connection.html
Some examples:

```
$ VDSInfo.exe --url s3://openvds-test --persistentID 7068247E9CA6EA05 --channels --axis --layout
```
This will print out a json object with the following children
 * axisDescriptors (an array)
 * channelDescriptors (an array)
 * layoutDescriptor (an object)

while
```
$ VDSInfo.exe --url s3://openvds-test --persistentID 7068247E9CA6EA05 --channels
```
will print an array with channel descriptors

```
$ VDSInfo.exe --url s3://openvds-test --persistentID 7068247E9CA6EA05 --metadatakeys
```
will print an array with the metadata keys. Together with other queries it will
be in the metaKeysInfo member of the root object;

Metadata can be queried with the `--metadata-category` and `--metadata-name`
predicates. All metadata matching category and name are put in a json
structure.  BLOBs are not added to this json structure. Missing category or
name predicate is acting as a wildcard.

If `--metadata-firstblob` is specified or that all parameters end up only
matching BLOB meta properties, then the first matched property will print.

```
$ VDSInfo.exe --url s3://openvds-test --persistentID 7068247E9CA6EA05 --metadata-name TextHeader
```
will write the first TextHeader metadata property in the VDS. If this is a blob
property it will be written directly to stdout.

The text header property can sometimes be encoded in EBCDIC enconding. Add the
`-e` parameter to autodetect EBCDIC and decode to ASCII.

To force a width for BLOB printing use the `-w` parameter.
```
$ VDSInfo.exe --url s3://openvds-test --persistentID 7068247E9CA6EA05 --metadata-name TextHeader -e -w 80
```
