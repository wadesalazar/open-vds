#!/bin/bash
set -e

# CWD should be /data, default mount point, if used
cd /data

# only expose SEGYImport, SEGYExport and VDSInfo,
# with import, export, and info being synonymous
case "$1" in
SEGYImport|import)
    shift
    exec SEGYImport "$@"
    ;;
SEGYExport|export)
    shift
    exec SEGYExport "$@"
    ;;
VDSInfo|info)
    shift
    exec VDSInfo "$@"
    ;;
esac

echo OpenVDS Import/Export image
echo Please use import/export/info commands
