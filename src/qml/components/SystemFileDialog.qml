import QtQuick 2.12
import QtQuick.Dialogs 1.3

FileDialog{
    property var callback;
    selectExisting: false
    function openAt(path, isFolder, selectExisting_ = true){
        selectFolder = isFolder;
        folder = pathToUrl(path);
        selectExisting = selectExisting_;
        open();
    }
    id: fileDialog
    title: "Please choose a " + (selectFolder ? "folder" : "file")
    onAccepted: {
        if(callback){
            callback(urlToPath(fileDialog.fileUrl));
        }else{
            console.error("Error in File Dialog in SystemFileDialog: No callback provided!")
        }
    }
}
