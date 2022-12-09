#pragma once
#include <cassert>
#include <new>
#include <memory>
#include <utility>

namespace raw_memory {

template <typename Type>
class RawMemory {
public:
    RawMemory() = default;

    explicit RawMemory(size_t capacity);

    RawMemory(const RawMemory&) = delete;

    RawMemory& operator=(const RawMemory& rhs) = delete;

    RawMemory(RawMemory&& other) noexcept;

    RawMemory& operator=(RawMemory&& rhs) noexcept;

    ~RawMemory();

    Type* operator+(size_t offset) noexcept;

    const Type* operator+(size_t offset) const noexcept;

    const Type& operator[](size_t index) const noexcept;

    Type& operator[](size_t index) noexcept;

    void Swap(RawMemory& other) noexcept;

    const Type* GetAddress() const noexcept;

    Type* GetAddress() noexcept;

    size_t Capacity() const;

private:
    static Type* Allocate(size_t n) {
        return n != 0 ? static_cast<Type*>(operator new(n * sizeof(Type))) : nullptr;
    }

    static void Deallocate(Type* buf) noexcept {
        operator delete(buf);
    }

private:
    Type* buffer_ = nullptr;
    size_t capacity_ = 0;
};

template <typename Type>
RawMemory<Type>::RawMemory(size_t capacity) : buffer_(Allocate(capacity)) , capacity_(capacity) {}

template <typename Type>
RawMemory<Type>::RawMemory(RawMemory&& other) noexcept {
    buffer_ = std::exchange(other.buffer_, nullptr);
    capacity_ = std::exchange(other.capacity_, 0);
}

template <typename Type>
RawMemory<Type>& RawMemory<Type>::operator=(RawMemory&& rhs) noexcept {
    if (this != &rhs) {
        buffer_ = std::exchange(rhs.buffer_, nullptr);
        capacity_ = std::exchange(rhs.capacity_, 0);
    }
    return *this;
}

template <typename Type>
Type* RawMemory<Type>::operator+(size_t offset) noexcept {
    assert(offset <= capacity_);
    return buffer_ + offset;
}

template <typename Type>
const Type* RawMemory<Type>::operator+(size_t offset) const noexcept {
    return const_cast<RawMemory&>(*this) + offset;
}

template <typename Type>
const Type& RawMemory<Type>::operator[](size_t index) const noexcept {
    return const_cast<RawMemory&>(*this)[index];
}

template <typename Type>
Type& RawMemory<Type>::operator[](size_t index) noexcept {
    assert(index < capacity_);
    return buffer_[index];
}

template <typename Type>
void RawMemory<Type>::Swap(RawMemory& other) noexcept {
    std::swap(buffer_, other.buffer_);
    std::swap(capacity_, other.capacity_);
}

template <typename Type>
const Type* RawMemory<Type>::GetAddress() const noexcept {
    return buffer_;
}

template <typename Type>
Type* RawMemory<Type>::GetAddress() noexcept {
    return buffer_;
}

template <typename Type>
size_t RawMemory<Type>::Capacity() const {
    return capacity_;
}

template <typename Type>
RawMemory<Type>::~RawMemory() {
    Deallocate(buffer_);
}

} // namesapce raw_memory

