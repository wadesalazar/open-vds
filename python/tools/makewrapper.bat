@echo off
md generated 2>NUL
ECHO Generating...
python mkwrapper.py -I..\..\src ..\..\src\OpenVDS\*.h
IF ERRORLEVEL 1 GOTO end
del generated\Global.h
rename generated\OpenVDS.h Global.h
IF ERRORLEVEL 1 GOTO end
ECHO Merging...
python merge_generated.py
IF ERRORLEVEL 1 GOTO end
python makereport.py >WrapperReport.txt
IF ERRORLEVEL 1 GOTO end
type WrapperReport.txt
:end

