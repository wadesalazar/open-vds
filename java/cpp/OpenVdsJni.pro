TEMPLATE = lib
CONFIG *= debug_and_release shared thread rtti warn_on static_runtime
CONFIG -= qt
QT -= core gui
TARGET = OpenVdsJni

VDS = C:/INT/OpenVDS/open-vds-master
INCLUDEPATH += include $$(VDS)/src/OpenVDS $$(VDS)/3rdparty/fmt-6.0.0/include
LIBS *= \
    $$(VDS)\src\OpenVDS\Debug\openvds.lib \
    $$(VDS)\aws-cpp-sdk_1.7.282_install\Debug\bin\aws-cpp-sdk-core.lib \
    $$(VDS)\aws-cpp-sdk_1.7.282_install\Debug\bin\aws-cpp-sdk-s3.lib \
    $$(VDS)\aws-cpp-sdk_1.7.282_install\Debug\bin\aws-cpp-sdk-sts.lib \
    $$(VDS)\azure-storage_7.0.0_install\Debug\lib\wastorage.lib \
    $$(VDS)\cpp-rest-api_2.10.14_install\Debug\lib\cpprest140_2_10d.lib \
    $$(VDS)\fmt_6.0.0\Debug\fmtd.lib \
    $$(VDS)\zlib_1.2.11_install\Debug\lib\zlibd.lib \
    winhttp.lib

win32*-g++* {
    # Uncomment flags below (function names decoration) if dll is loaded from Java for Windows
    #QMAKE_LFLAGS *= -Wl,--subsystem,windows,--kill-at
}

# compile with OpenMP support
win32*-msvc* {
    QMAKE_CXXFLAGS *= /openmp
}
win32*-g++* | linux-g++* {
    QMAKE_CXXFLAGS *= -fopenmp
    LIBS *= -lgomp -lpthread
    QMAKE_LFLAGS += -Wl,-z,defs
}
linux-icc* {
    QMAKE_CXXFLAGS *= -qopenmp
    LIBS  *= -liomp5
    QMAKE_LFLAGS += -Wl,-z,defs
}

ENV_JAVA_HOME = $$(JAVA_HOME)
isEmpty(ENV_JAVA_HOME) {
    error("You should define $JAVA_HOME environment variable. Specifically, the <javah> Java utility expected to be found in $JAVA_HOME/bin")
}
INCLUDEPATH += "$$(JAVA_HOME)/include"

win32 {
    INCLUDEPATH += "$$(JAVA_HOME)/include/win32"
}
unix {
    INCLUDEPATH += "$$(JAVA_HOME)/include/linux"
}
macx {
    INCLUDEPATH += "$$(JAVA_HOME)/include/darwin"
}

HEADERS += \
    include/Common.h \
    include/CommonJni.h \
    include/Portability.h \
    include/org_opengroup_openvds_JniPointer.h \
    include/org_opengroup_openvds_MemoryVdsGenerator.h \
    include/org_opengroup_openvds_MetadataReadAccess.h \
    include/org_opengroup_openvds_NativeBuffer.h \
    include/org_opengroup_openvds_QuantizingValueConverter_FloatToByte.h \
    include/org_opengroup_openvds_VdsHandle.h \
    include/org_opengroup_openvds_VolumeDataAccessManager.h \
    include/org_opengroup_openvds_VolumeDataAccessor.h \
    include/org_opengroup_openvds_VolumeDataLayout.h \
    include/org_opengroup_openvds_VolumeDataPage.h \
    include/org_opengroup_openvds_VolumeDataPageAccessor.h \

SOURCES += \
    src/Common.cpp \
    src/CommonJni.cpp \
    src/JniPointer.cpp \
    src/MemoryVdsGenerator.cpp \
    src/MetadataReadAccess.cpp \
    src/NativeBuffer.cpp \
    src/QuantizingValueConverter_FloatToByte.cpp \
    src/VdsHandle.cpp \
    src/VolumeDataAccessManager.cpp \
    src/VolumeDataAccessor.cpp \
    src/VolumeDataLayout.cpp \
    src/VolumeDataPage.cpp \
    src/VolumeDataPageAccessor.cpp \
