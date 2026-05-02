# GhostBinder
GhostBinder : A C - based Proof of Concept for PE resource manipulation.Demonstrates how to programmatically embed, extract, and execute multiple binaries within a single host executable using the Windows API.

How it Works
Code Generation : The builder dynamically generates a specialized C source file(GhostBinder.c) designed to handle resource extraction.

Compilation : The builder invokes gcc to compile the generated source into a standalone, static, and stripped "stub" executable.

Resource Injection : Using BeginUpdateResourceW and UpdateResourceW, the builder takes your two input files and injects them into the newly compiled stub.

Extraction& Execution : When the final GhostBinder.exe is run, it locates the embedded resources in memory, writes them to the TEMP directory, and executes them via ShellExecuteW.

Features
Static Linking : Uses - static to ensure the final binary runs on systems without MinGW DLLs.

Space Optimized : Automatically strips debug symbols(-s)

Native Win32 : Built entirely using the Windows API for resource handling.

Usage
Ensure you have GCC(MinGW - w64) in your system PATH.

Compile the builder :

Bash
gcc builder.c - o builder.exe
Run the builder with your target EXEs :

Powershell
.\Builder.exe "C:\Windows\System32\cmd.exe" "C:\Windows\System32\calc.exe"

Disclaimer
This project is for educational purposes only.It was created to demonstrate the mechanics of the Windows PE format and resource management.The author is not responsible for any misuse of this tool.Do not use this software for malicious purposes.
