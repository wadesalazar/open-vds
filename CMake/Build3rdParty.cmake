include(CMake/BuildAWSSDK.cmake)
include(CMake/BuildJsonCpp.cmake)

macro(Build3rdParty)
  BuildAWSSDK()
  BuildJsonCpp()
endmacro()
