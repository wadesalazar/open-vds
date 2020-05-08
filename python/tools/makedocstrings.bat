@REM Generate docstrings for the OpenVDS Python bindings
@REM The first pass converts microsoft-style XML documentation to doxygen format. This pass is 
@REM necessary because mkdoc.py (actually clang) does not recognize the microsoft format.
python preprocess_header.py ..\..\src\OpenVDS\OpenVDS\*.h
python mkdoc.py -Itmpinclude tmpinclude\OpenVDS\*.h >..\openvds\generated_docstrings.h