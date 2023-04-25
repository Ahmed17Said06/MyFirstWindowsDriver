# MyFirstWindowsDriver
Windows10 Driver prints the CPUID info and Process Monitor


A simple Windows kernel driver that prints the CPUID information from the kernel and a basic process monitor that logs processes being started. For example, after the driver being loaded any process starts (i.e. Chrome.exe) the driver will log the process info 

Path to the .sys: ".\MyFirstDriver\x64\Debug\MyFirstDriver.sys"

Project files number: 1 .cpp file

Complied using Visual Studio 2022 on windows 10 VM.

Run the CMD and the DebugView in elevated mode.  
