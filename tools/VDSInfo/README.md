## VDSInfo

VDSInfo is a simple tool for getting info about a VDS. It prints out the result
of the query in json, and it will try and give the shortes json for the
specific query by eliminating redundant json parent structures.

Some examples:
```
$ VDSInfo.exe --bucket <some_bucket> --region <a_region> --persistentID <some_vds_id> --channels --axis --layout
```
This will print out a json object with the following children
 * axisDescriptors (an array)
 * channelDescriptors (an array)
 * layoutDescriptor (an object)

while
```
$ VDSInfo.exe --bucket <some_bucket> --region <a_region> --persistentID <some_vds_id> --channels
```
will print an array with channel descriptors

```
$ VDSInfo.exe --bucket <some_bucket> --region <a_region> --persistentID <some_vds_id> --metadatakeys
```
will print an array with the metadata keys. Together with other queries it
will be in the metaKeysInfo member of the root object;

Metadata can be queried with the `--metadata-category` and `--metadata-name`
predicates. All metadata matching category and name are put in a json structure.
BLOBs are not added to this json structure. Missing category or name predicate
is acting as a wildcard.

If `--metadata-firstblob` is specified or that all parameters end up only matching
BLOB meta properties, then the first matched property will print.

```
$ VDSInfo.exe --bucket <some_bucket> --region <a_region> --persistentID <some_vds_id> --metadata-name TextHeader
```
will write the first TextHeader metadata property in the VDS. If this is a
blob property it will be written directly to stdout.

The text header property can sometimes be encoded in EBCDIC enconding. Add the
`-e` parameter to autodetect EBCDIC and decode to ASCII.

To force a width for BLOB printing use the `-w` parameter.
```
$ VDSInfo.exe --bucket <some_bucket> --region <a_region> --persistentID <some_vds_id> --metadata-name TextHeader -e -w 80
```

