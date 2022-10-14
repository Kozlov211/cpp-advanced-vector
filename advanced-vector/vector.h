#pragma once
#include <cassert>
#include <cstdlib>
#include <new>
#include <utility>
#include <memory>


template <typename T>
class RawMemory {
public:
    RawMemory() = default;

    explicit RawMemory(size_t capacity)
        : buffer_(Allocate(capacity))
        , capacity_(capacity) {
    }

    RawMemory(const RawMemory&) = delete;

    RawMemory& operator=(const RawMemory& rhs) = delete;

    RawMemory(RawMemory&& other) noexcept {
        buffer_ = std::exchange(other.buffer_, nullptr);
        capacity_ = std::exchange(other.capacity_, 0);
    }

    RawMemory& operator=(RawMemory&& rhs) noexcept {
        if (this != &rhs) {
            buffer_ = std::exchange(rhs.buffer_, nullptr);
            capacity_ = std::exchange(rhs.capacity_, 0);
        }
        return *this;
    }

    ~RawMemory() {
        Deallocate(buffer_);
    }

    T* operator+(size_t offset) noexcept {
        assert(offset <= capacity_);
        return buffer_ + offset;
    }

    const T* operator+(size_t offset) const noexcept {
        return const_cast<RawMemory&>(*this) + offset;
    }

    const T& operator[](size_t index) const noexcept {
        return const_cast<RawMemory&>(*this)[index];
    }

    T& operator[](size_t index) noexcept {
        assert(index < capacity_);
        return buffer_[index];
    }

    void Swap(RawMemory& other) noexcept {
        std::swap(buffer_, other.buffer_);
        std::swap(capacity_, other.capacity_);
    }

    const T* GetAddress() const noexcept {
        return buffer_;
    }

    T* GetAddress() noexcept {
        return buffer_;
    }

    size_t Capacity() const {
        return capacity_;
    }

private:
    static T* Allocate(size_t n) {
        return n != 0 ? static_cast<T*>(operator new(n * sizeof(T))) : nullptr;
    }

    static void Deallocate(T* buf) noexcept {
        operator delete(buf);
    }

    T* buffer_ = nullptr;
    size_t capacity_ = 0;
};

template <typename T>
class Vector {
public:
    using iterator = T*;
    using const_iterator = const T*;

    iterator begin() noexcept {
        return data_.GetAddress();
    }

    iterator end() noexcept {
        return data_.GetAddress() + size_;
    }

    const_iterator begin() const noexcept {
        return data_.GetAddress();
    }

    const_iterator end() const noexcept {
        return data_.GetAddress() + size_;
    }

    const_iterator cbegin() const noexcept {
        return data_.GetAddress();
    }

    const_iterator cend() const noexcept {
        return data_.GetAddress() + size_;
    }

    Vector() = default;

    Vector(const size_t size) : data_(size), size_(size) {
        std::uninitialized_value_construct_n(data_.GetAddress(), size);
    }

    Vector(const Vector& other) : data_(other.size_), size_(other.size_) {
        std::uninitialized_copy_n(other.data_.GetAddress(), other.size_, data_.GetAddress());
    }

    Vector(Vector&& other) {
        data_ = std::move(other.data_);
        size_ = std::exchange(other.size_, 0);
    }

    Vector& operator=(const Vector& rhs) {
        if (this != &rhs) {
            if (rhs.size_ > Capacity()) {
                Vector new_vector(rhs);
                Swap(new_vector);
            } else {
                for(size_t i = 0; i < std::min(rhs.size_, size_) ; ++i) {
                    data_[i] = rhs.data_[i];
                }
                (rhs.size_ > size_) ? std::uninitialized_copy_n(rhs.data_.GetAddress() + size_, rhs.size_ - size_, data_.GetAddress() + size_) : std::destroy_n(data_.GetAddress() + rhs.size_, size_ - rhs.size_);
                size_ = rhs.size_;
            }
        }
        return *this;
    }

    Vector& operator=(Vector&& rhs) {
        if (this != &rhs) {
            data_ = std::move(rhs.data_);
            size_ = std::exchange(rhs.size_, 0);
        }
        return *this;
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > Capacity()) {
            RawMemory<T> new_data_(new_capacity);
            if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
                std::uninitialized_move_n(data_.GetAddress(), size_, new_data_.GetAddress());
            } else {
                std::uninitialized_copy_n(data_.GetAddress(), size_, new_data_.GetAddress());
            }
            std::destroy_n(data_.GetAddress(), size_);
            data_.Swap(new_data_);
        }
    }

    void Resize(size_t new_size) {
        if (new_size < size_) {
            std::destroy_n(data_.GetAddress() + new_size, size_ - new_size);
        }
        if (new_size > size_) {
            Reserve(new_size);
            std::uninitialized_value_construct_n(data_.GetAddress() + size_, new_size - size_);
        }
        size_ = new_size;
    }

    void PushBack(const T& value) {
        EmplaceBack(value);
    }

    void PushBack(T&& value) {
        EmplaceBack(std::forward<T>(value));
    }

    iterator Insert(const_iterator pos, const T& value) {
        return Emplace(pos, value);
    }

    iterator Insert(const_iterator pos, T&& value) {
        return Emplace(pos, std::forward<T>(value));
    }

    template <typename... Type>
    iterator Emplace(const_iterator pos, Type&&... args) {
        assert(pos >= begin() && pos <= end());
        size_t index_pos = pos - cbegin();
        if (size_ == Capacity()) {
            RawMemory<T> new_data_(size_ == 0 ? 1 : size_ * 2);
            new (new_data_ + index_pos) T(std::forward<Type>(args)...);
            if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
                std::uninitialized_move_n(data_ + 0, index_pos, new_data_ + 0);
                std::uninitialized_move_n(data_ + index_pos, size_ - index_pos, new_data_ + index_pos + 1);
            } else {
                std::uninitialized_copy_n(data_ + 0, index_pos, new_data_ + 0);
                std::uninitialized_copy_n(data_ + index_pos, size_ - index_pos, new_data_ + index_pos + 1);
            }
            std::destroy_n(data_.GetAddress(), size_);
            data_.Swap(new_data_);
        } else {
            if (pos == end()) {
                new (data_ + size_) T(std::forward<Type>(args)...);
            } else {
                T tmp(std::forward<Type>(args)...);
                new (data_ + size_) T(std::move(Back()));
                std::move_backward(data_ + index_pos, end() - 1, end());
                data_[index_pos] = std::move(tmp);
            }
        }
        ++size_;
        return data_.GetAddress() + index_pos;
    }

    template <typename... Type>
    T& EmplaceBack(Type&&... args) {
        if (size_ == Capacity()) {
            RawMemory<T> new_data_(size_ == 0 ? 1 : size_ * 2);
            new (new_data_ + size_) T(std::forward<Type>(args)...);
            if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
                std::uninitialized_move_n(data_.GetAddress(), size_, new_data_.GetAddress());
            } else {
                std::uninitialized_copy_n(data_.GetAddress(), size_, new_data_.GetAddress());
            }
            std::destroy_n(data_.GetAddress(), size_);
            data_.Swap(new_data_);
        } else {
            new (end()) T(std::forward<Type>(args)...);
        }
        ++size_;
        return Back();
    }

    iterator Erase(const_iterator pos) {
        assert(pos >= begin() && pos <= end());
        size_t index_pos = pos - cbegin();
        iterator new_pos = begin() + index_pos;
        if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
            std::move(new_pos + 1, end(), new_pos);
        } else {
            std::copy(new_pos + 1, end(), new_pos);
        }
        std::destroy_n(end() - 1, 1);
        --size_;
        return new_pos;
    }

    void PopBack() noexcept {
        std::destroy_n(data_.GetAddress(), 1);
        --size_;
    }

    const T& Back() const noexcept {
        return const_cast<Vector&>(*this)[size_ - 1];
    }

    T& Back() {
        return data_[size_ - 1];
    }

    void Swap(Vector& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
    }

    size_t Size() const noexcept {
        return size_;
    }

    size_t Capacity() const noexcept {
        return data_.Capacity();
    }

    const T& operator[](size_t index) const noexcept {
        return const_cast<Vector&>(*this)[index];
    }

    T& operator[](size_t index) noexcept {
        assert(index < size_);
        return data_[index];
    }

    ~Vector() {
        std::destroy_n(data_.GetAddress(), size_);
    }

private:
    RawMemory<T> data_ = {};
    size_t size_ = 0;

};

