#ifndef SLIDESHOW_H
#define SLIDESHOW_H

#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QObject>
#include <QWindow>
#include <atomic>
#include <deque>
#include <mutex>
#include <random>
#include <set>

class SlideShow : public QObject {
    Q_OBJECT
public:
    enum ShowOrder { Random, OldestFirst, NewestFirst, NameAZ, NameZA, Last = NameZA };
    Q_ENUM(ShowOrder)
private:
    /**
     * @brief path the path to the folder, in which the images are
     */
    QString path;
    /**
     * @brief showTimeInSeconds how long a image is visible
     */
    int showTimeInSeconds;
    /**
     * @brief order the oder of the images
     */
    ShowOrder order;
    /**
     * if the image deque is locked by the thread that loads the images asnyc, you can
     * set this bool to true. If this bool is true, the async loading thread will sort
     * the image deque when the loading is finished
     */
    std::atomic_bool shouldSort;
    // the mutex to protect the image deque
    std::recursive_mutex imageMutex;
    // all loaded images, sorted if order != Random
    std::deque<QFileInfo> images;
    // all paths that were already scanned
    std::set<QString> alreadyScanned;
    // a watcher to check if new files/folders gets added into the folder <path>
    QFileSystemWatcher watcher;
    // the current displayed image (index in the images deque)
    int currentImage = -1;
    // the id of the load timer
    int loadTimer = -1;
    /**
     * @brief offsetTimer the id of the offset timer. The offset timer should wait offsetWaitTime after the loadNextImage
     * is emitted to emit emit the showNextImage signal. So the Slideshow has some time to load the image
     */
    int offsetTimer = -1;
    // if a file contains [0-9]+s, the wait time will be this custom time
    bool wasCustomShowTime = false;
    /**
     * @brief windowVisibility the window visibility of the slideshow window
     */
    QWindow::Visibility windowVisibility = QWindow::Hidden;
    // the used random number generator for <order> = Random
    std::mt19937 randomNumberGenerator;
    // the distribution to get a random number from the random number generator
    std::uniform_int_distribution<size_t> dis;
    Q_PROPERTY(int showTimeInSeconds READ getShowTimeInSeconds WRITE setShowTimeInSeconds NOTIFY showTimeInSecondsChanged)
    Q_PROPERTY(ShowOrder showOrder READ getShowOrder WRITE setShowOrder NOTIFY showOrderChanged)
    Q_PROPERTY(QString path READ getPath WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QWindow::Visibility windowVisibility MEMBER windowVisibility NOTIFY windowVisibilityChanged)
    Q_PROPERTY(bool hasImages READ hasImages NOTIFY hasImagesChanged)
    // the time that should be waitet by the offset timer
    static constexpr int offsetWaitTime = 100;

public:
    static SlideShow &get() {
        static SlideShow s;
        return s;
    }

public:
    SlideShow();
    void setShowTimeInSeconds(int time);
    [[nodiscard]] int getShowTimeInSeconds() const { return showTimeInSeconds; }
    void setShowOrder(ShowOrder showOrder);
    [[nodiscard]] ShowOrder getShowOrder() const { return order; }
    void setPath(const QString &path);
    [[nodiscard]] QString getPath() const { return path; }
    [[nodiscard]] bool hasImages() const { return images.size(); }
    /**
     * @brief reportInvalidImage you can report the current image as invalid.
     * The image gets removed from a images deque and a new image gets selected
     */
    Q_INVOKABLE void reportInvalidImage();

protected:
    /**
     * @brief scanForNewFiles scans in the path for new files and updates the <watcher>. The scanning is done in a seperate thread.
     */
    void scanForNewFiles();
    /**
     * @brief sort Sorts the images deque. If the imageMutex can not be locked, shouldSort is set to true.
     */
    void sort();
    void timerEvent(QTimerEvent *event) override;
    /**
     * @brief loadNextImage determins the next image and emit the signal loadNextImage, if the imageMutex can be locked
     */
    void loadNextImage();

private:
    /**
     * @brief checkCustomShowTime checks if the path contains a custom show time ([0-9]+s) and manages the load timers
     * @param path The path that should be checked
     */
    void checkCustomShowTime(const QString &path);
    /**
     * @brief removeImage removes a image from the images deque and from the alreadyScanned set and updates the distribution
     * @param index index in the deque, can be invalid
     */
    void removeImage(int index);
    /**
     * @brief scanDirectory scans the directory path for new files. If a new directory is found, scan it recursive, even if recursive is false
     * @param path the path of the directory that should be scanned
     * @param recursive false, if only the directory should be scanned. If true, subdirectories are also scanned.
     */
    void scanDirectory(const QString &path, bool recursive);

signals:
    void loadNextImage(QString path);
    void showNextImage();
    void showTimeInSecondsChanged();
    void showOrderChanged();
    void pathChanged();
    void windowVisibilityChanged();
    void hasImagesChanged();
};

#endif // SLIDESHOW_H
