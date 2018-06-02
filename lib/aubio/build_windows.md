# Build Aubio on Windows


1. Download Aubio from [here](https://aubio.org/download) or version [0.4.6](https://aubio.org/pub/aubio-0.4.6.tar.bz2).
2. `cd` to the decompressed aubio folder.
3. install python and make sure python.exe is in your PATH variable (you can set PATH with `set PATH=<your_path>;%PATH%`) 
4. `cd scripts` -> `get_waf.sh` -> `cd ..`
5. `curl -fsS -o waf https://waf.io/waf-1.8.22`
6. `curl -fsS -o waf.bat https://raw.githubusercontent.com/waf-project/waf/master/utils/waf.bat`
7. open the waf file with an editor and change VERSION="1.8.22" to VERSION="2.0.1" ans save
8. when you want to compile with mingw, add the mingw bin folder to the first position in the PATH variable
9. `waf configure --msvc_version='mingw'` or with msvc `waf configure`
10. `waf build` -> `waf install`
11. In the folder build/src are now the lib files