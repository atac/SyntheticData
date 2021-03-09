mkdir %1
cd %1
rem powershell -command "Expand-Archive -Force '%~dp0my_zip_file.zip' '%~dp0'"
"\Program Files\7-Zip\7z.exe" x ..\%1.zip
cd ..
del %1.zip
