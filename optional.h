#pragma once

#include <stdexcept>
#include <utility>

class BadOptionalAccess : public std::exception {
public:
    using exception::exception;

    virtual const char* what() const noexcept override {
        return "Bad optional access";
    }
};

template <typename T>
class Optional {
public:
    Optional() = default;

    Optional(const T& value) {
        buff_ = new (&data_[0]) T(value);
        is_initialized_ = true;
    }

    Optional(T&& value) {
        buff_ = new (&data_[0]) T(std::forward<T>(value));
        is_initialized_ = true;
    }

    Optional(const Optional& other) {
        if (HasValue() && !other.HasValue()) {
            Reset();
        }
        else {
            if (!this->HasValue() && other.HasValue()) {
                (void)(buff_ = new (&data_[0]) T(other.Value()), is_initialized_ = true);
            }
            else {
                if (HasValue() && other.HasValue()) {
                    (void)(*buff_ = other.Value(), is_initialized_ = true);
                }
                else {
                    void();
                }
            }
        }
    }

    Optional(Optional&& other) {
        if (this->HasValue() && !other.HasValue()) {
            Reset();
        }
        else {
            if (!HasValue() && other.HasValue()) {
                (void)(buff_ = new (&data_[0]) T(std::forward<T>(other.Value())), is_initialized_ = true);
            }
            else {
                if (HasValue() && other.HasValue()) {
                    (void)(*buff_ = std::forward<T>(other.Value()), is_initialized_ = true);
                }
                else {
                    void();
                }
            }
        }
    }

    Optional& operator=(const T& value) {
        if (HasValue()) {
            (void)(*buff_ = value);
        }
        else {
            (void)(buff_ = new (&data_[0]) T(value), is_initialized_ = true);
        }
        return *this;
    }

    Optional& operator=(T&& rhs) {
        if (HasValue()) {
            (void)(*buff_ = std::forward<T>(rhs));
        }
        else {
            (void)(buff_ = new (&data_[0]) T(std::forward<T>(rhs)), is_initialized_ = true);
        }
        return *this;
    }

    Optional& operator=(const Optional& rhs) {
        if (HasValue() && !rhs.HasValue()) {
            Reset();
        }
        else {
            if (!HasValue() && rhs.HasValue()) {
                (void)(buff_ = new (&data_[0]) T(rhs.Value()), is_initialized_ = true);
            }
            else {
                if (HasValue() && rhs.HasValue()) {
                    (void)(*buff_ = rhs.Value(), is_initialized_ = true);
                }
                else {
                    void();
                }
            }
        }
        return *this;
    }

    Optional& operator=(Optional&& rhs) {
        if (HasValue() && !rhs.HasValue()) {
            Reset();
        }
        else {
            if (!HasValue() && rhs.HasValue()) {
                (void)(buff_ = new (&data_[0]) T(std::forward<T>(rhs.Value())), is_initialized_ = true);
            }
            else {
                if (HasValue() && rhs.HasValue()) {
                    (void)(*buff_ = std::forward<T>(rhs.Value()), is_initialized_ = true);
                }
                else {
                    void();
                }
            }
        }
        return *this;
    }

    ~Optional() {
        is_initialized_ ? buff_->~T() : void();
    }

    bool HasValue() const {
        return is_initialized_;
    }

    T& operator*()& {
        return Value();
    }

    const T& operator*() const& {
        return Value();
    }

    T&& operator*()&& {
        return std::move(Value());
    }

    T* operator->() {
        return &Value();
    }

    const T* operator->() const {
        return &Value();
    }

    T& Value()& {
        if (!is_initialized_) {
            throw BadOptionalAccess();
        }
        else {
            return *buff_;
        }
    }

    const T& Value() const& {
        if (!is_initialized_) {
            throw BadOptionalAccess();
        }
        else {
            return *buff_;
        }
    }

    T&& Value()&& {
        if (!is_initialized_) {
            throw BadOptionalAccess();
        }
        else {
            return std::move(*buff_);
        }
    }

    void Reset() {
        is_initialized_ ? (is_initialized_ = false, buff_->~T()) : void();
    }

    template <typename... Args>
    void Emplace(Args&&... args) {
        if (is_initialized_) {
            buff_->~T();
        }
        buff_ = new (&data_[0]) T(std::forward<Args>(args)...);
        is_initialized_ = true;
    }

private:
    T* buff_ = nullptr;
    alignas(T) char data_[sizeof(T)];
    bool is_initialized_ = false;
};