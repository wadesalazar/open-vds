## OpenVDS Java bindings

This readme file is meant to help developers who want to build and use the OpenVDS java bindings

### Dependencies

This module depends on
- Java 1.8+
- TestNG (to compile and execute unit tests)

### Build

By default OpenVDS builds the Java bindings.  
To disable building the Java bindings use the -DBUILD_JAVA=OFF cmake argument.

### Supported platform and compiler

- Redhat/Centos 8+
- java >= 1.8 openjdk/oracle
