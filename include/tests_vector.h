#pragma once

#include "vector.h"

#include <numeric>

namespace tests_vector {

namespace domain {

class DefaultClass {
public:
    DefaultClass()
        : DefaultClass(5) {
    }

    DefaultClass(size_t num)
        : x_(num) {
    }

    DefaultClass(const DefaultClass& other) = default;

    DefaultClass& operator=(const DefaultClass& other) = default;

    DefaultClass(DefaultClass&& other) = default;

    DefaultClass& operator=(DefaultClass&& other) = default;

    size_t GetX() const {
        return x_;
    }

private:
    size_t x_;
};

class NoDefaultConstor {
public:
    NoDefaultConstor() = delete;

    NoDefaultConstor(size_t num)
        : x_(num) {
    }

    size_t GetX() const {
        return x_;
    }

private:
    size_t x_ = 5;
};

inline bool operator<(const NoDefaultConstor& lhs, const NoDefaultConstor& rhs) {
    return lhs.GetX() < rhs.GetX();
}

class CopyClass {
public:
    CopyClass()
        : CopyClass(5) {
    }

    CopyClass(size_t num)
        : x_(num) {
    }

    CopyClass(const CopyClass& other) = default;

    CopyClass& operator=(const CopyClass& other) = default;

    CopyClass(CopyClass&& other) = delete;

    CopyClass& operator=(CopyClass&& other) = default;

    size_t GetX() const {
        return x_;
    }

private:
    size_t x_;
};

inline bool operator<(const CopyClass& lhs, const CopyClass& rhs) {
    return lhs.GetX() < rhs.GetX();
}

class MoveClass {
public:
    MoveClass()
        : MoveClass(5) {
    }

    MoveClass(size_t num)
        : x_(num) {
    }
    MoveClass(const MoveClass& other) = delete;

    MoveClass& operator=(const MoveClass& other) = delete;

    MoveClass(MoveClass&& other) {
        x_ = std::exchange(other.x_, 0);
    }

    MoveClass& operator=(MoveClass&& other) {
        x_ = std::exchange(other.x_, 0);
        return *this;
    }
    size_t GetX() const {
        return x_;
    }

private:
    size_t x_;
};

inline bool operator<(const MoveClass& lhs, const MoveClass& rhs) {
    return lhs.GetX() < rhs.GetX();
}

} // namespace domain

void AllTest();

void Constructor();

void ElementAccess();

void At();

void OperatorBracket();

void Front();

void Back();

void Data();

void Iterators();

void CapacityFunctions();

void Empty();

void Size();

void Reserve();

void Capacity();

void Modifiers();

void Clear();

void Insert();

void Emplace();

void Erase();

void PushBack();

void EmplaceBack();

void PopBack();

void Resize();

void Swap();

} // namespace tests_vector

