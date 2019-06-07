#ifndef ZIP_H
#define UZIP_H

#include <QFileInfo>
#include <functional>

namespace Zip {
    void unzip(const QFileInfo& zip, const QFileInfo& unzip, const std::function<void(bool)>& callback);
} // namespace Zip


#endif // UZIP_H
