#ifndef SAMPLE_H
#define SAMPLE_H

#include <QtCore>
#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>

namespace Audio {

template <typename Type, std::size_t size_>
class Sample {
    std::array<Type, size_> array;

public:
    Sample() { std::memset(array.data(), 0, array.size() * sizeof(Type)); }
    /**
     * @brief addData adds data to the end of the sample and moves old data to the left
     * @param newData a pointer to the new Data
     * @param endPointer a pointer pointing to the byte after the data block, like the std::end by the
     * @param interleaved space between two relevant data values, eg. 1 by sterio data
     * @param startOffset the offset from the newData Pointer to the first relevant byte
     */
    void addData(Type *newData, Type *endPointer, int interleaved = 0, int startOffset = 0) {
        Q_ASSERT(newData < endPointer);
        newData += startOffset;
        int dis = std::distance(newData, endPointer);
        int length = (dis + interleaved) / (1 + interleaved);
        if (length >= static_cast<int>(size_)) {
            --endPointer;
            for (auto i = array.rbegin(); i != array.rend(); ++i, std::advance(endPointer, -(1 + interleaved))) {
                *i = *endPointer;
            }
            return;
        }
        std::copy(std::next(array.cbegin(), length), array.cend(), array.begin());
        for (auto i = std::next(array.begin(), array.size() - length); newData < endPointer; ++i, std::advance(newData, 1 + interleaved)) {
            *i = *newData;
            Q_ASSERT(i != array.cend());
        }
        // Q_ASSERT(newData == endPointer);
    }
    const std::array<Type, size_> &getArray() const { return array; }
    std::array<Type, size_> &getArray() { return array; }
    Type *data() { return array.data(); }
    const Type *data() const { return array.data(); }
    std::size_t size() { return array.size(); }
};
} // namespace Audio

#endif // SAMPLE_H
