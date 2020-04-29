from aws_defs import *
import openvds
quit()
err = openvds.Error()
opt = openvds.AWSOpenOptions(TEST_AWS_BUCKET, TEST_AWS_OBJECTID, TEST_AWS_REGION)
handle = openvds.open(opt, err)

