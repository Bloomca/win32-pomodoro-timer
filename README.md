## Instructions

This app is supposed to be compiled by MSVC and built by MSBuild, so you'll need to install Visual Studio.

To build and run this app open `Developer Command Prompt for VS 2022` and execute:

```sh
msbuild app.proj /p:Platform=x64
.\x64\Debug\app.exe
```