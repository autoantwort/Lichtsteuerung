## build boost for Windows
1. Download boost from [here](https://www.boost.org/users/download/)
2. Unzip the file
3. open powershell and cd to the unziped folder
4. exec: `.\bootstrap.bat`
5. Set the PATH Variable to the compiler
    - eg: `$env:path="C:\Qt\Tools\mingw730_64\bin";` for mingw-64 installed with QT
<!-- 6. Execute `.\b2.exe toolset=gcc --build-dir=dist context coroutine` (run `.\b2.exe --show-libraries` to see which libs can be build) -->
6. Execute `.\b2.exe toolset=gcc --build-dir=build --libdir=lib --stagedir=dist link=shared` this will need some time(40 min)