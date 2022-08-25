import QtQuick
import QtQuick.Dialogs
import "path_utils.js" as PathUtils

FileDialog{
    property var callback;
    function openAt(path, mode = FileDialog.OpenFile){
        currentFile = PathUtils.path_to_file_prot(path);
        fileMode = mode;
        open();
    }
    onAccepted: {
        if(callback){
            callback(PathUtils.file_prot_to_path(selectedFile));
        }else{
            console.error("Error in File Dialog in SystemFileDialog: No callback provided!")
        }
    }
}
