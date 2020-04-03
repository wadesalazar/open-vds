## SEGYImport

SEGYImport is a tool that can transform a single SEG-Y file to a VDS.

To create a valid VDS from a SEG-Y file SEGYImport needs two files. The SEG-Y
file and a file describing the content of the SEG-Y file. This is because of
the loose definition of the contents of a SEG-Y file. SEGY-Y calls this meta
file the "file-info" file and can be passed to SEG-Y info with the `-i` or the
`--file-info` flags.

SEGYImport can also generate a "file-info" file. This is done when either the
`--scan` option is specified or no `file-info` flag has been specified.

If `--scan` is specified then `--file-info` argument will specify the output
file. If no output file is given, the file will be printed to stdout.

If neither `--scan` or `--file-info` is specified, SEGYImport will perform a
scan and generate a "file-info" that will be used in the subsequent converting
of SEG-Y file to a VDS. The generated file-info will not be written to disk.

The way SEGYImport scans the SEG-Y file can be controlled by specifying the
`--primary-key` (`-p`) and `--secondary-key` (`-s`) options. The value values
of these arguments should be the name of the trace header field.

When SEGYImport is either done generating a "file-info" or it is supplied with
a file, it will start generating VDS chunks that will be uploaded to the given
connection parameters.

During the "file-info" generation stage SEGYImport will read the binary header of
the SEG-Y file and extract some keys at certain predefined positions.

| Name                                      | Offset | Width |
|-------------------------------------------|--------|-------|
| TracesPerEnsembleHeaderField              | 13     | 2     |
| AuxiliaryTracesPerEnsembleHeaderField     | 15     | 2     |
| SampleIntervalHeaderField                 | 17     | 2     |
| NumSamplesHeaderField                     | 21     | 2     |
| DataSampleFormatCodeHeaderField           | 25     | 2     |
| EnsembleFoldHeaderField                   | 27     | 2     |
| TraceSortingCodeHeaderField               | 29     | 2     |
| MeasurementSystemHeaderField              | 55     | 2     |
| SEGYFormatRevisionNumberHeaderField       | 301    | 2     |
| FixedLengthTraceFlagHeaderField           | 303    | 2     |
| ExtendedTextualFileHeaderCountHeaderField | 305    | 2     |

Also some headers from the trace headers are read. The width and position of
the trace header fields are configurable. They can be configured by specifying
a JSON file containing a root object, where the fields contain an array of size
two containing byte position (offset) and field width. ie:

```
{
  "CoordinateScale" : [69, "TwoByte"],
  "StartTimeHeaderField" : [107, "FourByte"]
}
```

Configurable trace headers fields names are:
 * TraceSequenceNumber
 * TraceSequenceNumberWithinFile
 * EnergySourcePointNumber
 * EnsembleNumber
 * TraceNumberWithinEnsemble
 * TraceIdentificationCode
 * CoordinateScale
 * SourceXCoordinate
 * SourceYCoordinate
 * GroupXCoordinate
 * GroupYCoordinate
 * CoordinateUnits
 * StartTime
 * NumSamples
 * SampleInterval
 * EnsembleXCoordinate
 * EnsembleYCoordinate
 * InlineNumber
 * CrosslineNumber

Trace headers field names have also some aliases.

| SEGYImport Name     | Alias               |
|---------------------|---------------------|
| InlineNumber        | Inline              |
| CrosslineNumber     | Crossline           |
| EnsembleXCoordinate | Easting             |
| EnsembleYCoordinate | Northing            |
| EnsembleXCoordinate | CDPXCoordinate      |
| EnsembleYCoordinate | CDPYCoordinate      |
| EnsembleXCoordinate | CDP-X               |
| EnsembleYCoordinate | CDP-Y               |
| SourceXCoordinate   | Source-X            |
| SourceYCoordinate   | Source-Y            |
| GroupXCoordinate    | Group-X             |
| GroupYCoordinate    | Group-Y             |
| GroupXCoordinate    | ReceiverXCoordinate |
| GroupYCoordinate    | ReceiverYCoordinate |
| GroupXCoordinate    | Receiver-X          |
| GroupYCoordinate    | Receiver-Y          |
| CoordinateScale     | Scalar              |

