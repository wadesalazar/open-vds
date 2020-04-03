## SEGYExport

SEGYExport is a simple tool for generate a SEG-Y file from a VDS. VDS stores
the SEGY-Y binary and text headers from the original SEGY-Y, and these will be
used when exporting the VDS.

It is possible to generate a byte for byte equal SEGY-Y file as the original
file, but only if the VDS is generated using lossless compression and the VDS
contain the original SEG-Y headers. If the VDS does not contain SEG-Y headers,
SEGYExport will generate generic headers.
