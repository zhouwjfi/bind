echo off
rem
rem Copyright (C) 2004,2005  Internet Systems Consortium, Inc. ("ISC")
rem Copyright (C) 2001-2002  Internet Software Consortium.
rem 
rem Permission to use, copy, modify, and distribute this software for any
rem purpose with or without fee is hereby granted, provided that the above
rem copyright notice and this permission notice appear in all copies.
rem 
rem THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
rem REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
rem AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
rem INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
rem LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
rem OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
rem PERFORMANCE OF THIS SOFTWARE.

rem BuildSetup.bat
rem This script sets up the files necessary ready to build BIND 9.
rem This requires perl to be installed on the system.

rem Get and update for the latest build of the openssl library
perl updateopenssl.pl

rem Set up the configuration file
cd ..
copy config.h.win32 config.h
cd win32utils

rem Make sure all the files that need to be in DOS format really are
perl todos.pl

rem Generate the version information
perl makeversion.pl

rem Generate the SRCID information
perl makesrcid.pl

rem Generate header files for lib/dns
call dnsheadergen.bat

rem Make sure that the Build directories are there.

if NOT Exist ..\Build mkdir ..\Build
if NOT Exist ..\Build\Release mkdir ..\Build\Release
if NOT Exist ..\Build\Debug mkdir ..\Build\Debug

echo Copying the ARM and the Installation Notes.

copy ..\COPYRIGHT ..\Build\Release
copy ..\README ..\Build\Release
copy readme1st.txt ..\Build\Release
copy index.html ..\Build\Release
copy ..\doc\arm\*.html ..\Build\Release
copy ..\doc\arm\Bv9ARM.pdf ..\Build\Release
copy ..\CHANGES ..\Build\Release
if Exist ..\CHANGES.SE copy ..\CHANGES.SE ..\Build\Release
copy ..\FAQ ..\Build\Release

echo Copying the standalone manual pages.

copy ..\bin\named\named.html ..\Build\Release
copy ..\bin\rndc\*.html ..\Build\Release
copy ..\bin\dig\*.html ..\Build\Release
copy ..\bin\nsupdate\*.html ..\Build\Release
copy ..\bin\check\*.html ..\Build\Release
copy ..\bin\dnssec\dnssec-keygen.html ..\Build\Release
copy ..\bin\dnssec\dnssec-signzone.html ..\Build\Release
copy ..\bin\dnssec\dnssec-dsfromkey.html ..\Build\Release
copy ..\bin\dnssec\dnssec-keyfromlabel.html ..\Build\Release

echo Copying the migration notes.

copy ..\doc\misc\migration ..\Build\Release
copy ..\doc\misc\migration-4to9 ..\Build\Release

call BuildOpenSSL.bat

rem
rem try to find vcredist_x86.exe upper
rem
if Not Defined VCRedistPath (
if Exist ..\..\vcredist_x86.exe set VCRedistPath=..\..\vcredist_x86.exe
)

rem
rem get vcredist where someone said it should be
rem
if Defined VCRedistPath (

if Exist "%VCRedistPath%" (

echo Copying Visual C x86 Redistributable Installer

rem
rem Use /Y so we always have the current version of the installer.
rem

copy /Y "%VCRedistPath%" ..\Build\Release\
copy /Y	"%VCRedistPath%" ..\Build\Debug\

) else (

	echo "**** %VCRedistPath% not found ****"

)
) else (

if Defined FrameworkSDKDir (

rem
rem vcredist_x86.exe path relative to FrameworkSDKDir

rem
if Exist "%FrameworkSDKDir%\BootStrapper\Packages\vcredist_x86\vcredist_x86.exe" (

echo Copying Visual C x86 Redistributable Installer

rem
rem Use /Y so we allways have the current version of the installer.
rem

copy /Y "%FrameworkSDKDir%\BootStrapper\Packages\vcredist_x86\vcredist_x86.exe"  ..\Build\Release\
copy /Y "%FrameworkSDKDir%\BootStrapper\Packages\vcredist_x86\vcredist_x86.exe"  ..\Build\Debug\

) else (
	echo "**** %FrameworkSDKDir%\%vcredist% not found ****"
)
) else (
if NOT Defined FrameworkDir (
	echo "**** Warning FrameworkSDKDir not defined ****"
	echo "****         Run vsvars32.bat            ****"
) else (
	echo "**** vcredist_x86.exe not found ****"
	echo "**** please set VCRedistPath ****"
)
)
)

echo Running Message Compiler

cd ..\lib\win32\bindevt
mc bindevt.mc
cd ..\..\..\win32utils

rem Done