vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO Plaristote/segvcatch
    REF afe79c49f7d996e2b3143199e6cef69f406247bb
    SHA512 2229671f62bf5a9f7177c3ecbda2f5172adc4d8a529b66cc4f5a7d1eb426c0b08d94fad58576d7a86d48b0ca5826ed5dd30f1838fdbef4416902ca799a6423aa
    HEAD_REF master
    PATCHES
        newer-cmake.patch
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
)

vcpkg_cmake_install()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

file(INSTALL "${SOURCE_PATH}/README.md" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
