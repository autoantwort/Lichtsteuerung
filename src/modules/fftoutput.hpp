#ifndef FFTOUTPUT_H
#define FFTOUTPUT_H

#include <iterator>
#include <type_traits>

namespace Modules {

template <typename T>
class FFTOutputView {
    // The view should never be empty to avoid issues with Bad code by users
    T emptyValue = 0;
    const T *data_ = &emptyValue;
    std::size_t size_ = 1;
    int sampleRate_ = 44100;

public:
    // from https://github.com/boostorg/beast/blob/5abac8b380ffcd914126c5019c980366c5a53f68/include/boost/beast/core/span.hpp
    /// The type of value, including cv qualifiers
    using element_type = T;

    /// The type of value of each span element
    using value_type = typename std::remove_const<T>::type;

    /// The type of integer used to index the span
    using index_type = std::ptrdiff_t;

    /// A pointer to a span element
    using pointer = T *;

    /// A reference to a span element
    using reference = T &;

    /// The iterator used by the container
    using iterator = pointer;

    /// The const pointer used by the container
    using const_pointer = T const *;

    /// The const reference used by the container
    using const_reference = T const &;

    /// The const iterator used by the container
    using const_iterator = const_pointer;

    /// Constructor
    FFTOutputView() = default;

    /// Constructor
    FFTOutputView(FFTOutputView const &) = default;

    /// Assignment
    FFTOutputView &operator=(FFTOutputView const &) = default;
    /** Constructor
        @param data A pointer to the beginning of the range of elements
        @param size The number of elements pointed to by `data`
    */
    FFTOutputView(T *data, std::size_t size, int sampleRate) : data_(data), size_(size), sampleRate_(sampleRate) {}

    /** Constructor
        @param container The container to construct from
    */
    template <class ContiguousContainer>
    explicit FFTOutputView(ContiguousContainer &&container, int sampleRate) : data_(container.data())
                                                                            , size_(container.size())
                                                                            , sampleRate_(sampleRate) {}

    /// Assignment
    template <class ContiguousContainer>
    FFTOutputView &operator=(ContiguousContainer &&container) {
        data_ = container.data();
        size_ = container.size();
        return *this;
    }
    /// Returns `true` if the span is empty
    bool empty() const { return size_ == 0; }

    /// Returns a pointer to the beginning of the span
    const T *data() const { return data_; }

    /// Returns the number of elements in the span
    std::size_t size() const { return size_; }

    /// Returns an iterator to the beginning of the span
    const_iterator begin() const { return data_; }

    /// Returns an iterator to the beginning of the span
    const_iterator cbegin() const { return data_; }

    /// Returns an iterator to one past the end of the span
    const_iterator end() const { return data_ + size_; }

    /// Returns an iterator to one past the end of the span
    const_iterator cend() const { return data_ + size_; }

    int getSampleRate() const { return sampleRate_; }

    void setSampleRate(int sampleRate) { sampleRate_ = sampleRate; }

    int getMaxFrequency() const { return sampleRate_ / 2; }

    /**
     * @brief getBlockSize Der Fft output ist in size() Blöcke eingeteilt und
     *        die Maximal registrierte Frequenz ist getMaxFrequency(). Jeder Block
     *        enthält somit eine Frequenzspanne von getBlockSize() Herz.
     * @return
     */
    double getBlockSize() const { return getMaxFrequency() / static_cast<double>(size_); }

    int getLowerBlockFrequency(const_iterator i) const { return getMaxFrequency() * std::distance(cbegin(), i) / size(); }
    int getLowerBlockFrequency(int index) const { return getMaxFrequency() * index / size(); }

    int getUpperBlockFrequency(const_iterator i) const { return getLowerBlockFrequency(++i); }

    int getUpperBlockFrequency(int index) const { return getLowerBlockFrequency(++index); }

    /// Returns an iterator to the beginning of the span
    const_iterator begin(int frequency) const { return data_ + frequency * size_ / getMaxFrequency(); }

    const_iterator end(int frequency) const { return begin(frequency) + 1; }

    T atFrequency(int frequency) const { return *begin(frequency); }

    T at(int index) {
        if (static_cast<unsigned int>(index) >= size()) {
            return -1;
        }
        return data_[index];
    }
};

} // namespace Modules

#endif // FFTOUTPUT_H
