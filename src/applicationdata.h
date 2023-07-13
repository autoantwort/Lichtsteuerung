#ifndef APPLICATIONDATA_H
#define APPLICATIONDATA_H

#include <QFile>
#include <functional>

namespace ApplicationData {

/**
 * @brief saveData saves all application data into the given file
 * @param file The file in which the data should be written
 * @return true on success, false on failure
 */
bool saveData(const QString &file);
/**
 * @brief saveData saves all application data into a QByteArray
 * @return The application data as data array
 */
QByteArray saveData();
/**
 * @brief loadData loads the application data from the file and returns a callback
 * that should be called, after the QML UI is loaded
 * @param file The file with the application data
 * @return a callback and a error message. An error occurred, when the error message is not empty
 */
std::pair<std::function<void()>, QString> loadData(const QString &file);
/**
 * @brief loadData loads the application data from the byte array and returns a callback
 * that should be called, after the QML UI is loaded
 * @param data the application data
 * @return a callback and a error message. An error occurred, when the error message is not empty
 */
std::pair<std::function<void()>, QString> loadData(const QByteArray &data);

} // namespace ApplicationData

#endif // APPLICATIONDATA_H
