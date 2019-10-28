#include "settingsfilewrapper.h"
#include "applicationdata.h"
#include "errornotifier.h"
#include "usermanagment.h"
#include <QDesktopServices>
#include <QUrl>

void SettingsFileWrapper::setStatus(SettingsFileWrapper::Status status) {
    if (this->status != status) {
        this->status = status;
        emit statusChanged();
    }
}

void SettingsFileWrapper::setErrorMessage(const QString &errorMessage) {
    if (this->errorMessage != errorMessage) {
        this->errorMessage = errorMessage;
        emit errorMessageChanged();
    }
}

void SettingsFileWrapper::loadFile(const QString &filepath) {
    Q_ASSERT(status != Loaded);
    settings.setJsonSettingsFilePath(filepath);

    auto [after, error] = ApplicationData::loadData(filepath);
    if (error.isEmpty()) {
        after();
        UserManagment::get()->autoLoginUser();
        setStatus(Loaded);
    } else {
        setErrorMessage(error);
        setStatus(LoadingFailed);
    }
}

bool SettingsFileWrapper::openFileInDefaultEditor() {
    return QDesktopServices::openUrl(QUrl::fromLocalFile(settings.getJsonSettingsFilePath()));
}

void SettingsFileWrapper::openFileExplorerAtFile() {
    QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(settings.getJsonSettingsFilePath()).path()));
}
