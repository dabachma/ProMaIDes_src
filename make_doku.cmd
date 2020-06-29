echo off
REM cmd for create the dokumentation with doxygen
REM the output file of the creation is in ./doku/output.txt

echo Start doxygen

doxygen.exe ./doku/my_doxyfile > ./doku/output.txt

echo Finished creation of the dokumentation
