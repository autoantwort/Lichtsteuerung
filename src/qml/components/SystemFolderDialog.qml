import QtQuick
import QtQuick.Dialogs
import "path_utils.js" as PathUtils

FolderDialog{
    property var callback;
    function openAt(path){
        currentFolder = PathUtils.path_to_file_prot(path);
        open();
    }
    onAccepted: {
        if(callback){
            callback(PathUtils.file_prot_to_path(selectedFolder));
        }else{
            console.error("Error in File Dialog in SystemFileDialog: No callback provided!")
        }
    }
}
