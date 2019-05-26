#ifndef ZIP_H
#define UZIP_H

#include <QFileInfo>

namespace Zip {
    bool unzip(const QFileInfo& zip, const QFileInfo& unzip);
} // namespace Zip


#endif // UZIP_H
