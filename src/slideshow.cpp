#include "slideshow.h"
#include "settings.h"
#include "usermanagment.h"
#include <QDateTime>
#include <QDebug>
#include <QDirIterator>
#include <QImageReader>
#include <QRegularExpression>
#include <QSettings>
#include <algorithm>
#include <cmath>
#include <errornotifier.h>
#include <thread>

SlideShow::SlideShow() : randomNumberGenerator(static_cast<unsigned>(rand())) {
    QSettings s(QSettings::UserScope, Settings::OrganisationName, Settings::ApplicationName);
    showTimeInSeconds = std::max(3, s.value(QStringLiteral("showTimeInSeconds"), 3).toInt());
    order = static_cast<ShowOrder>(std::clamp<int>(s.value(QStringLiteral("showOrder"), 0).toInt(), Random, Last));
    path = s.value(QStringLiteral("path"), "").toString();
    scanForNewFiles();
    sort();
    emit hasImagesChanged();
    // if a new path gets added, add the new files to the images deque and sort
    QObject::connect(&watcher, &QFileSystemWatcher::directoryChanged, [this](const auto &path) {
        scanDirectory(path, false);
        sort();
    });
    // if the window gets visible and no image is loaded, load a image and show it
    QObject::connect(this, &SlideShow::windowVisibilityChanged, [this]() {
        if (windowVisibility == QWindow::Minimized || windowVisibility == QWindow::Hidden) {
            // if the window is not visible, we can stop the loading of new images
            killTimer(loadTimer);
            loadTimer = -1;
        } else {
            // the window gets visible or changed its state
            if (currentImage < 0) {
                // no image is loaded. Load one and show it
                loadNextImage();
                emit showNextImage();
            }

            if (loadTimer < 0) {
                // the load timer is stopped. start it
                loadTimer = startTimer(showTimeInSeconds * 1000);
            }
        }
    });
}

void SlideShow::setShowTimeInSeconds(int time) {
    if (time != showTimeInSeconds) {
        showTimeInSeconds = std::max(3, time);
        if (loadTimer >= 0) {
            killTimer(loadTimer);
            loadTimer = startTimer(showTimeInSeconds * 1000);
        }
        emit showTimeInSecondsChanged();
        if (UserManagment::get()->getCurrentUser()->havePermission(UserManagment::Permission::SAVE_SLIDE_SHOW_SETTINGS)) {
            QSettings(QSettings::UserScope, Settings::OrganisationName, Settings::ApplicationName).setValue(QStringLiteral("showTimeInSeconds"), showTimeInSeconds);
        }
    }
}

void SlideShow::setShowOrder(SlideShow::ShowOrder showOrder) {
    if (showOrder != order) {
        order = showOrder;
        emit showOrderChanged();
        sort();
        if (UserManagment::get()->getCurrentUser()->havePermission(UserManagment::Permission::SAVE_SLIDE_SHOW_SETTINGS)) {
            QSettings(QSettings::UserScope, Settings::OrganisationName, Settings::ApplicationName).setValue(QStringLiteral("showOrder"), showOrder);
        }
    }
}

void SlideShow::setPath(const QString &path) {
    if (path != this->path && QFileInfo(path).isDir()) {
        this->path = path;
        emit pathChanged();
        images.clear();
        alreadyScanned.clear();
        scanForNewFiles();
        sort();
        emit hasImagesChanged();
        if (UserManagment::get()->getCurrentUser()->havePermission(UserManagment::Permission::SAVE_SLIDE_SHOW_SETTINGS)) {
            QSettings(QSettings::UserScope, Settings::OrganisationName, Settings::ApplicationName).setValue(QStringLiteral("path"), path);
        }
    }
}

void SlideShow::reportInvalidImage() {
    removeImage(currentImage);
    --currentImage; // so that ++currentImage returns the next image
    loadNextImage();
    killTimer(offsetTimer);
    offsetTimer = startTimer(offsetWaitTime);
}

void SlideShow::loadNextImage() {
    // if it is locked, than by scanForNewFiles
    if (imageMutex.try_lock()) {
        if (order == Random) {
            while (!images.empty()) {
                auto i = dis(randomNumberGenerator);
                auto p = images[i];
                p.refresh();
                if (p.exists()) {
                    checkCustomShowTime(p.filePath());
                    emit loadNextImage(p.filePath());
                    break;
                }
                removeImage(static_cast<int>(i));
            }
        } else {
            // get index of next image
            ++currentImage;
            while (!images.empty()) {
                // the size of the deque can change
                currentImage %= images.size();
                auto &img = images[static_cast<size_t>(currentImage)];
                img.refresh();
                if (img.exists()) {
                    checkCustomShowTime(img.filePath());
                    emit loadNextImage(img.filePath());
                    break;
                }
                // if we remove an image, current index will point to the "next" image
                alreadyScanned.erase(img.filePath());
                images.erase(images.cbegin() + currentImage);
            }
        }
        imageMutex.unlock();
        if (images.empty()) {
            currentImage = -1;
            emit hasImagesChanged();
        }
    }
}
// regex to search for things like 10s in the file name for custom show times
const QRegularExpression regex(QStringLiteral("(\\d+)s"));

void SlideShow::checkCustomShowTime(const QString &path) {
    auto i = regex.globalMatch(path);
    int customTime = -1;
    while (i.hasNext()) {
        auto match = i.next();
        customTime = match.captured(1).toInt();
    }
    if (customTime > 2) {
        wasCustomShowTime = true;
        killTimer(loadTimer);
        loadTimer = startTimer(customTime * 1000);
    } else if (wasCustomShowTime) {
        wasCustomShowTime = false;
        killTimer(loadTimer);
        loadTimer = startTimer(showTimeInSeconds * 1000);
    }
}

void SlideShow::removeImage(int index) {
    if (index >= 0 && index < static_cast<int>(images.size()) && imageMutex.try_lock()) {
        alreadyScanned.erase(images[index].filePath());
        images.erase(images.cbegin() + index);
        dis = std::uniform_int_distribution<size_t>(0, images.size() - 1);
        imageMutex.unlock();
    }
}

void SlideShow::scanDirectory(const QString &path, bool recursive) {
    std::unique_lock lock(imageMutex);
    QDirIterator it(path, QStringList() << QStringLiteral("*.jpg") << QStringLiteral("*.jpeg") << QStringLiteral("*.png") << QStringLiteral("*.gif") << QStringLiteral("*.bmp") << QStringLiteral("*.webp"), QDir::Files | QDir::Readable | QDir::AllDirs | QDir::NoDotAndDotDot,
                    recursive ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags);
    while (it.hasNext()) {
        auto p = it.next();
        auto [_, newInserted] = alreadyScanned.insert(p);
        if (newInserted) {
            QFileInfo f(p);
            if (f.isDir()) {
                // scan new dirs
                if (!recursive) {
                    scanDirectory(p, true);
                }
                watcher.addPath(p);
            }
            // invalid files from macOS ../._abc.png
            else if (!p.contains(QLatin1String("._"))) {
                images.emplace_back(std::move(p));
            }
        }
    }
    dis = std::uniform_int_distribution<size_t>(0, images.size() - 1);
}

void SlideShow::scanForNewFiles() {
    const auto dirs = watcher.directories();
    if (!dirs.empty()) {
        watcher.removePaths(dirs);
    }
    if (path.isEmpty()) {
        return;
    }
    QFileInfo dir(path);
    if (dir.isFile()) {
        std::unique_lock lock(imageMutex);
        images.emplace_back(path);
        dis = std::uniform_int_distribution<size_t>(0);
    } else {
        std::thread t([this]() {
            scanDirectory(path, true);
            emit hasImagesChanged();
            if (images.empty()) {
                ErrorNotifier::showError(QStringLiteral("The selected folder for the slideshow contains no images!"));
            }
            watcher.addPath(path);
            if (shouldSort) {
                shouldSort = false;
                sort();
            }
        });
        t.detach();
    }
}

void SlideShow::sort() {
    struct newest {
        bool operator()(const QFileInfo &l, const QFileInfo &r) const { return l.lastModified() > r.lastModified(); }
    };
    struct oldest {
        bool operator()(const QFileInfo &l, const QFileInfo &r) const { return l.lastModified() < r.lastModified(); }
    };
    struct nameAZ {
        bool operator()(const QFileInfo &l, const QFileInfo &r) const { return l.filePath() < r.filePath(); }
    };
    struct nameZA {
        bool operator()(const QFileInfo &l, const QFileInfo &r) const { return l.filePath() > r.filePath(); }
    };
    if (order == Random) {
        return;
    }
    // if it is locked, than by scanForNewFiles
    if (imageMutex.try_lock()) {
        switch (order) {
        case Random: /*No warning*/ break;
        case NewestFirst: std::sort(images.begin(), images.end(), newest{}); break;
        case OldestFirst: std::sort(images.begin(), images.end(), oldest{}); break;
        case NameAZ: std::sort(images.begin(), images.end(), nameAZ{}); break;
        case NameZA: std::sort(images.begin(), images.end(), nameZA{}); break;
        }
        imageMutex.unlock();
    } else {
        shouldSort = true;
    }
}

void SlideShow::timerEvent(QTimerEvent *event) {
    if (event->timerId() == loadTimer) {
        event->accept();
        offsetTimer = startTimer(offsetWaitTime);
        loadNextImage();
    } else if (event->timerId() == offsetTimer) {
        event->accept();
        killTimer(offsetTimer);
        offsetTimer = -1;
        emit showNextImage();
    }
}
