from connection_defs import *
import openvds
if not TEST_URL:
  quit()
err = openvds.Error()
handle = openvds.open(TEST_URL, TEST_CONNECTION, err)

