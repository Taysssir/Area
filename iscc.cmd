@ECHO OFF
SET ISCC_PATH=C:\Program Files (x86)\Inno Setup 5\ISCC.exe
IF EXIST "%ISCC_PATH%" ("%ISCC_PATH%" "%1") ELSE (echo WARNING: Inno Setup is not installed!)
