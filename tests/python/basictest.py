from connection_defs import *
import openvds
if not TEST_URL:
  quit()
handle = openvds.open(TEST_URL, TEST_CONNECTION)
openvds.close(handle)

