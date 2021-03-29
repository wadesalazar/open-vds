from __future__ import print_function

import argparse
import re
import io
import numpy
import openvds
import sys

from http import HTTPStatus
from flask import Flask, make_response, request, abort
from PIL import Image

VDSs = {}

parser = argparse.ArgumentParser(description="Starts a service that creates png images of the slices of a VDS.")
parser.add_argument("--url", help="The url of the VDSs to start serving")
parser.add_argument("--connection", help="The connection string for the url.")
parser.add_argument("--key", help="The key of the VDSs to start serving")
parser.add_argument("--allow-remote", action='store_true', help="Allow remote connections to server.")

if len(sys.argv) == 1:
    parser.print_usage()
    sys.exit(0)

args = parser.parse_args()

persistentID = args.key if args.key else ""

VDSs[persistentID] = openvds.open(args.url)

app = Flask(__name__)

def handle_request(vds, sliceType, sliceIndex):
    accessManager = openvds.getAccessManager(vds)
    layout = openvds.getLayout(vds)
    axisDescriptors = [layout.getAxisDescriptor(dim) for dim in range(layout.getDimensionality())]
    sliceDimension = 2 if sliceType == 'inline' else 1 if sliceType == 'crossline' else 0 if sliceType == 'timeslice' else 0 if sliceType == 'depthslice' else -1
    min = tuple(sliceIndex + 0 if dim == sliceDimension else 0 for dim in range(6))
    max = tuple(sliceIndex + 1 if dim == sliceDimension else layout.getDimensionNumSamples(dim) for dim in range(6))
    req = accessManager.requestVolumeSubset(min, max, format = openvds.VolumeDataChannelDescriptor.Format.Format_U8)
    height = max[0] if sliceDimension != 0 else max[1]
    width  = max[2] if sliceDimension != 2 else max[1]
    data = req.data.reshape(width, height).transpose()
    image = Image.fromarray(data, mode='L')
    with io.BytesIO() as output:
        image.save(output, format='PNG')
        responseData = output.getvalue()
    response = make_response((responseData, HTTPStatus.OK))
    response.headers['Content-Type'] = 'image/png'
    response.headers['Content-Disposition'] = 'inline; filename={0}{1}.png'.format(sliceType, sliceIndex)
    return response


@app.route('/<sliceType>/<int:sliceIndex>')
def vds_data(sliceType, sliceIndex):
    if len(VDSs) == 0:
        abort(HTTPStatus.NOT_FOUND)
    return handle_request(next(iter(VDSs.values())), sliceType, sliceIndex)

@app.route('/<persistentID>/<sliceType>/<int:sliceIndex>')
def vds_data_persistentID(persistentID, sliceType, sliceIndex):
    if persistentID in VDSs:
        return handle_request(VDSs[persistentID], slizeType, sliceIndex)
    abort(HTTPStatus.NOT_FOUND)

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0' if args.allow_remote else '127.0.0.1')
