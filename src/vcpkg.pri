
isEmpty(VCPKG_TARGET_TRIPLET){
    isEmpty(VCPKG_SYSTEM){
        macx{
            VCPKG_SYSTEM = osx
        }
        linux{
            VCPKG_SYSTEM = linux
        }
        win32-g++{
            VCPKG_SYSTEM = mingw
        }
        win32-msvc{
            VCPKG_SYSTEM = windows
        }
    }
    isEmpty(VCPKG_SYSTEM){
        error("Could not detect the system. Set the system manually before including this file with 'VCPKG_SYSTEM = your-system'. Eg 'VCPKG_SYSTEM = mingw'")
    }

    isEmpty(VCPKG_ARCH){
        isEqual(QMAKE_HOST.arch, x86_64):{
            VCPKG_ARCH = x64
        } else {
            VCPKG_ARCH = $${QMAKE_HOST.arch}
        }
    }
    isEmpty(VCPKG_ARCH){
        error("Could not detect the target architecture. Set the architecture manually before including this file with 'VCPKG_ARCH = x64|x86|arm|arm64'. Eg 'VCPKG_ARCH = x64'")
    }

    VCPKG_TARGET_TRIPLET = "$${VCPKG_ARCH}-$${VCPKG_SYSTEM}"
}
message("The triplet $${VCPKG_TARGET_TRIPLET} was selected. You can manually set the triplet by inserting 'VCPKG_TARGET_TRIPLET = your-triplet' before including this file. E.g. 'VCPKG_TARGET_TRIPLET = x64-windows-static'")

system(../vcpkg/vcpkg install --triplet=$${VCPKG_TARGET_TRIPLET} --feature-flags=manifests --x-install-root="$${OUT_PWD}/vcpkg_installed"){
    message("Successfully installed vcpkg dependencies!")
}else{
    message("If you see the message 'Failed to take the filesystem lock on ...' is it likely that qmake is already running (it is called automatically when you change a .pro/.pri file)")
    error("Failed to install the dependencies with vcpkg. See the 'Compile Output' Tab for more information.")
}

INCLUDEPATH += "$${OUT_PWD}/vcpkg_installed/$${VCPKG_TARGET_TRIPLET}/include"

CONFIG(debug, debug|release) {
    LIBS += $${files($${OUT_PWD}/vcpkg_installed/$${VCPKG_TARGET_TRIPLET}/debug/lib/*.a)}
    LIBS += $${files($${OUT_PWD}/vcpkg_installed/$${VCPKG_TARGET_TRIPLET}/debug/lib/*.so)}
    LIBS += $${files($${OUT_PWD}/vcpkg_installed/$${VCPKG_TARGET_TRIPLET}/debug/lib/*.dylib)}
    LIBS += $${files($${OUT_PWD}/vcpkg_installed/$${VCPKG_TARGET_TRIPLET}/debug/lib/*.dll)}
}
CONFIG(release, debug|release) {
    LIBS += $${files($${OUT_PWD}/vcpkg_installed/$${VCPKG_TARGET_TRIPLET}/lib/*.a)}
    LIBS += $${files($${OUT_PWD}/vcpkg_installed/$${VCPKG_TARGET_TRIPLET}/lib/*.so)}
    LIBS += $${files($${OUT_PWD}/vcpkg_installed/$${VCPKG_TARGET_TRIPLET}/lib/*.dylib)}
    LIBS += $${files($${OUT_PWD}/vcpkg_installed/$${VCPKG_TARGET_TRIPLET}/lib/*.dll)}
}


