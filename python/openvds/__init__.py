from .api import *

def getTestOpenOptions():
    return AWSOpenOptions(TEST_AWS_BUCKET, TEST_AWS_OBJECTID, TEST_AWS_REGION)
    
