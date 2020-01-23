from __future__ import print_function

import argparse
import re
import io
import numpy
import openvds

from http import HTTPStatus
from flask import Flask, make_response, request, abort
from PIL import Image

VDSs = {}

parser = argparse.ArgumentParser(description="Starts a service that creates png images of the slices of a VDS.")
parser.add_argument("--bucket", help="The AWS bucket of the VDSs to start serving")
parser.add_argument("--region", help="The AWS region of the VDSs to start serving")
parser.add_argument("--key",    help="The AWS key of the VDSs to start serving")
parser.add_argument("--allow-remote", action='store_true', help="Allow remote connections to server.") 
args = parser.parse_args()

opt = openvds.AWSOpenOptions(bucket=args.bucket, region=args.region, key=args.key)
opt.bucket = args.bucket
opt.region = args.region
opt.key = args.key
err = openvds.Error()
VDSs[opt.key] = openvds.open(opt, err)

app = Flask(__name__)

@app.route('/<persistentID>/<sliceType>/<int:sliceIndex>')
def vds_data(persistentID, sliceType, sliceIndex):
    if persistentID in VDSs:
        accessManager = openvds.getAccessManager(VDSs[persistentID])
        layout = openvds.getLayout(VDSs[persistentID])
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
    abort(HTTPStatus.NOT_FOUND)

if __name__ == '__main__':
     app.run(debug=True, host='0.0.0.0' if args.allow_remote else '127.0.0.1')
