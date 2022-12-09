#pragma once

#include "raw_memory.h"

#include <cassert>
#include <cstdlib>
#include <initializer_list>
#include <new>
#include <memory>
#include <stdexcept>
#include <utility>

namespace vector {

class ReserveProxyObj{
public:
    ReserveProxyObj(size_t capacity);

    size_t Capacity() const noexcept;
private:
    size_t capacity_;
};

inline ReserveProxyObj::ReserveProxyObj(size_t capacity) : capacity_(capacity) {};

inline size_t ReserveProxyObj::Capacity() const noexcept{
    return capacity_;
}

inline ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class Vector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    Vector() noexcept = default;

    explicit Vector(size_t size);

    Vector(std::initializer_list<Type> initializer_list);

    Vector(size_t size, const Type& value);

    Vector(ReserveProxyObj obj_capacity);

    Vector(const Vector& other);

    Vector(Vector&& other);

    Vector& operator=(const Vector& rhs);

    Vector& operator=(Vector&& rhs);

    ~Vector();

    Type& At(size_t index);

    const Type& At(size_t index) const;

    Type& operator[](size_t index);

    const Type& operator[](size_t index) const;

    Type& Front();

    const Type& Front() const;

    Type& Back();

    const Type& Back() const;

    Type* Data() noexcept;

    Type* Data() const noexcept;

    Iterator begin() noexcept;

    Iterator end() noexcept;

    ConstIterator begin() const noexcept;

    ConstIterator end() const noexcept;

    ConstIterator cbegin() const noexcept;

    ConstIterator cend() const noexcept;

    bool IsEmpty() const noexcept;

    size_t Size() const noexcept;

    void Reserve(size_t new_capacity);

    size_t Capacity() const noexcept;

    void Clear() noexcept;

    Iterator Insert(ConstIterator pos, const Type& value);

    Iterator Insert(ConstIterator pos, Type&& value);

    template <typename... Args>
    Iterator Emplace(ConstIterator pos, Args&&... args);

    Iterator Erase(ConstIterator pos);

    void PushBack(const Type& value);

    void PushBack(Type&& value);

    template <typename... Args>
    Type& EmplaceBack(Args&&... args);

    void PopBack();

    void Resize(size_t new_size);

    void Swap(Vector& other) noexcept;

private:
    raw_memory::RawMemory<Type> data_ = {};
    size_t size_ = 0;
};


template <typename Type>
Vector<Type>::Vector(size_t size) : data_(size), size_(size) {
    std::uninitialized_value_construct_n(data_.GetAddress(), size);
}

template <typename Type>
Vector<Type>::Vector(std::initializer_list<Type> initializer_list) : data_(initializer_list.size()), size_(initializer_list.size()) {
    std::copy(initializer_list.begin(), initializer_list.end(), data_.GetAddress());
}

template <typename Type>
Vector<Type>::Vector(size_t size, const Type& value) : data_(size), size_(size) {
    std::uninitialized_fill(begin(), end(), value);
}

template <typename Type>
Vector<Type>::Vector(ReserveProxyObj obj_capacity) : data_(obj_capacity.Capacity()) {}

template <typename Type>
Vector<Type>::Vector(const Vector& other) : data_(other.size_), size_(other.size_) {
    std::uninitialized_copy_n(other.data_.GetAddress(), other.size_, data_.GetAddress());
}

template <typename Type>
Vector<Type>::Vector(Vector&& other) {
    data_ = std::move(other.data_);
    size_ = std::exchange(other.size_, 0);
}

template <typename Type>
Vector<Type>& Vector<Type>::operator=(const Vector& rhs) {
    if (this != &rhs) {
        if (rhs.size_ > Capacity()) {
            Vector new_vector(rhs);
            Swap(new_vector);
        } else {
            size_t size = std::min(rhs.size_, size_);
            for(size_t i = 0; i < size; ++i) {
                data_[i] = rhs.data_[i];
            }
            (rhs.size_ > size_) ? std::uninitialized_copy_n(rhs.data_.GetAddress() + size_, rhs.size_ - size_, data_.GetAddress() + size_) : std::destroy_n(data_.GetAddress() + rhs.size_, size_ - rhs.size_);
            size_ = rhs.size_;
        }
    }
    return *this;
}

template <typename Type>
Vector<Type>& Vector<Type>::operator=(Vector&& rhs) {
    if (this != &rhs) {
        data_ = std::move(rhs.data_);
        size_ = std::exchange(rhs.size_, 0);
    }
    return *this;
}

template <typename Type>
Vector<Type>::~Vector() {
    std::destroy_n(data_.GetAddress(), size_);
}


template <typename Type>
Type& Vector<Type>::At(size_t index) {
    if (index >= size_) {
        throw std::out_of_range("out_of_range");
    }
    return data_[index];
}


template <typename Type>
const Type& Vector<Type>::At(size_t index) const {
    if (index >= size_) {
        throw std::out_of_range("out_of_range");
    }
    return data_[index];
}

template <typename Type>
const Type& Vector<Type>::operator[](size_t index) const {
    return const_cast<Vector&>(*this)[index];
}

template <typename Type>
Type& Vector<Type>::operator[](size_t index) {
    assert(index < size_);
    return data_[index];
}


template <typename Type>
Type& Vector<Type>::Front() {
    return data_[0];
}

template <typename Type>
const Type& Vector<Type>::Front() const {
    return data_[0];
}

template <typename Type>
Type& Vector<Type>::Back() {
    return data_[size_ - 1];
}

template <typename Type>
const Type& Vector<Type>::Back() const {
    return const_cast<Vector&>(*this)[size_ - 1];
}

template <typename Type>
Type* Vector<Type>::Data() noexcept {
    return data_.GetAddress();
}

template <typename Type>
Type* Vector<Type>::Data() const noexcept {
    return data_.GetAddress();
}


template <typename Type>
typename Vector<Type>::Iterator Vector<Type>::begin() noexcept {
    return data_.GetAddress();
}

template <typename Type>
typename Vector<Type>::Iterator Vector<Type>::end() noexcept {
    return data_.GetAddress() + size_;
}

template <typename Type>
typename Vector<Type>::ConstIterator Vector<Type>::begin() const noexcept {
    return data_.GetAddress();
}

template <typename Type>
typename Vector<Type>::ConstIterator Vector<Type>::end() const noexcept {
    return data_.GetAddress() + size_;
}

template <typename Type>
typename Vector<Type>::ConstIterator Vector<Type>::cbegin() const noexcept {
    return data_.GetAddress();
}

template <typename Type>
typename Vector<Type>::ConstIterator Vector<Type>::cend() const noexcept {
    return data_.GetAddress() + size_;
}

template <typename Type>
bool Vector<Type>::IsEmpty() const noexcept {
    return (size_ == 0 ? true : false);
}

template <typename Type>
size_t Vector<Type>::Size() const noexcept {
    return size_;
}

template <typename Type>
void Vector<Type>::Reserve(size_t new_capacity) {
    if (new_capacity > Capacity()) {
        raw_memory::RawMemory<Type> new_data_(new_capacity);
        if constexpr (std::is_nothrow_move_constructible_v<Type> || !std::is_copy_constructible_v<Type>) {
            std::uninitialized_move_n(data_.GetAddress(), size_, new_data_.GetAddress());
        } else {
            std::uninitialized_copy_n(data_.GetAddress(), size_, new_data_.GetAddress());
        }
        std::destroy_n(data_.GetAddress(), size_);
        data_.Swap(new_data_);
    }
}

template <typename Type>
size_t Vector<Type>::Capacity() const noexcept {
    return data_.Capacity();
}

template <typename Type>
void Vector<Type>::Clear() noexcept {
    std::destroy_n(data_.GetAddress(), size_);
    size_ = 0;
}

template <typename Type>
typename Vector<Type>::Iterator Vector<Type>::Insert(ConstIterator pos, const Type& value) {
    return Emplace(pos, value);
}

template <typename Type>
typename Vector<Type>::Iterator Vector<Type>::Insert(ConstIterator pos, Type&& value) {
    return Emplace(pos, std::forward<Type>(value));
}

template <typename Type>
template <typename... Args>
typename Vector<Type>::Iterator Vector<Type>::Emplace(ConstIterator pos, Args&&... args) {
    assert(pos >= begin() && pos <= end());
    size_t index_pos = pos - cbegin();
    if (size_ == Capacity()) {
        raw_memory::RawMemory<Type> new_data_(size_ == 0 ? 1 : size_ * 2);
        new (new_data_ + index_pos) Type(std::forward<Args>(args)...);
        if constexpr (std::is_nothrow_move_constructible_v<Type> || !std::is_copy_constructible_v<Type>) {
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
            new (data_ + size_) Type(std::forward<Args>(args)...);
        } else {
            Type tmp(std::forward<Args>(args)...);
            new (data_ + size_) Type(std::move(Back()));
            std::move_backward(data_ + index_pos, end() - 1, end());
            data_[index_pos] = std::move(tmp);
        }
    }
    ++size_;
    return data_.GetAddress() + index_pos;
}

template <typename Type>
typename Vector<Type>::Iterator Vector<Type>::Erase(ConstIterator pos) {
    assert(pos >= begin() && pos <= end());
    size_t index_pos = pos - cbegin();
    Iterator new_pos = begin() + index_pos;
    if constexpr (std::is_nothrow_move_constructible_v<Type> || !std::is_copy_constructible_v<Type>) {
        std::move(new_pos + 1, end(), new_pos);
    } else {
        std::copy(new_pos + 1, end(), new_pos);
    }
    std::destroy_n(end() - 1, 1);
    --size_;
    return new_pos;
}

template <typename Type>
void Vector<Type>::PushBack(const Type& value) {
    EmplaceBack(value);
}

template <typename Type>
void Vector<Type>::PushBack(Type&& value) {
    EmplaceBack(std::forward<Type>(value));
}

template <typename Type>
template <typename... Args>
Type& Vector<Type>::EmplaceBack(Args&&... args) {
    if (size_ == Capacity()) {
        raw_memory::RawMemory<Type> new_data_(size_ == 0 ? 1 : size_ * 2);
        new (new_data_ + size_) Type(std::forward<Args>(args)...);
        if constexpr (std::is_nothrow_move_constructible_v<Type> || !std::is_copy_constructible_v<Type>) {
            std::uninitialized_move_n(data_.GetAddress(), size_, new_data_.GetAddress());
        } else {
            std::uninitialized_copy_n(data_.GetAddress(), size_, new_data_.GetAddress());
        }
        std::destroy_n(data_.GetAddress(), size_);
        data_.Swap(new_data_);
    } else {
        new (end()) Type(std::forward<Args>(args)...);
    }
    ++size_;
    return Back();
}

template <typename Type>
void Vector<Type>::PopBack() {
    std::destroy_n(data_.GetAddress(), 1);
    --size_;
}

template <typename Type>
void Vector<Type>::Resize(size_t new_size) {
    if (new_size < size_) {
        std::destroy_n(data_.GetAddress() + new_size, size_ - new_size);
    }
    if (new_size > size_) {
        Reserve(new_size);
        std::uninitialized_value_construct_n(data_.GetAddress() + size_, new_size - size_);
    }
    size_ = new_size;
}

template <typename Type>
void Vector<Type>::Swap(Vector& other) noexcept {
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
}

template <typename Type>
inline bool operator<(const Vector<Type>& lhs, const Vector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator>(const Vector<Type>& lhs, const Vector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator==(const Vector<Type>& lhs, const Vector<Type>& rhs) {
    return !(lhs < rhs || rhs < lhs);
}

template <typename Type>
inline bool operator!=(const Vector<Type>& lhs, const Vector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<=(const Vector<Type>& lhs, const Vector<Type>& rhs) {
    return !(lhs > rhs);
}

template <typename Type>
inline bool operator>=(const Vector<Type>& lhs, const Vector<Type>& rhs) {
    return !(lhs < rhs);
}



} //namespace vector

