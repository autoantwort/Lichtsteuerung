.pragma library

function path_to_file_prot(file) {
    if (Qt.platform.os === "windows") {
        return "file:///" + file;
    }
    return "file://" + file
}

function file_prot_to_path(file_prot) {
    if (Qt.platform.os === "windows") {
        return file_prot.toString().replace("file:///", "");
    }
    return file_prot.toString().replace("file://", "");
}
