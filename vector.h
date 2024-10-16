#include <cstddef>
#include <stdexcept>
#include <memory>
#include <initializer_list>

template <typename T>
class Vector {
public:
    class Iterator {
    public:
        Iterator(T* ptr) : ptr_(ptr) {
        }

        Iterator& operator++() {
            ++ptr_;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        T& operator*() const {
            return *ptr_;
        }

        T* operator->() {
            return ptr_;
        }

        friend bool operator==(const Iterator& lhs, const Iterator& rhs) {
            return lhs.ptr_ == rhs.ptr_;
        }

        friend bool operator!=(const Iterator& lhs, const Iterator& rhs) {
            return lhs.ptr_ != rhs.ptr_;
        }

    private:
        T* ptr_;
    };

    Vector() {
        data_ = allocator_.allocate(capacity_);
    }

    Vector(size_t capacity) : capacity_(capacity) {
        data_ = allocator_.allocate(capacity_);
    }

    Vector(std::initializer_list<T> init_list)
        : capacity_(init_list.size()), size_(init_list.size()) {
        data_ = allocator_.allocate(capacity_);
        size_t ind = 0;
        for (const T& value : init_list) {
            allocator_type::construct(allocator_, data_ + ind, value);
            ++ind;
        }
    }

    ~Vector() {
        Clear();
        allocator_.deallocate(data_, capacity_);
    }

    void PushBack(const T& value) {
        if (size_ == capacity_) {
            UpdateCapacity(capacity_ * 2);
        }
        allocator_type::construct(allocator_, data_ + size_, value);
        ++size_;
    }

    void PushBack(T&& value) {
        if (size_ == capacity_) {
            UpdateCapacity(capacity_ * 2);
        }
        allocator_type::construct(allocator_, data_ + size_, std::move(value));
        ++size_;
    }

    void PopBack() {
        if (size_ == 0) {
            throw std::out_of_range("Vector is empty");
        }
        allocator_type::destroy(allocator_, data_ + size_ - 1);
        --size_;
    }

    void Swap(Vector& other) {
        std::swap(data_, other.data_);
        std::swap(allocator_, other.allocator_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    void Clear() {
        for (size_t i = 0; i < size_; ++i) {
            allocator_type::destroy(allocator_, data_ + i);
        }
        size_ = 0;
    }

    size_t Size() const {
        return size_;
    }

    bool Empty() const {
        return size_ == 0;
    }

    size_t Capacity() const {
        return capacity_;
    }

    T& At(size_t ind) {
        if (ind >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[ind];
    }

    const T& At(size_t ind) const {
        if (ind >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[ind];
    }

    Vector& operator=(const Vector& other) {
        if (this != &other) {
            Vector tmp(other);
            Swap(tmp);
        }
        return *this;
    }

    Vector& operator=(Vector&& other) {
        if (this != &other) {
            Clear();
            allocator_.deallocate(data_, capacity_);
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            allocator_ = std::move(other.allocator_);
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    T& operator[](size_t ind) {
        return data_[ind];
    }

    T& operator[](size_t ind) const {
        return data_[ind];
    }

    Iterator begin() {
        return Iterator(data_);
    }

    Iterator end() {
        return Iterator(data_ + size_);
    }

private:
    using allocator_type = std::allocator_traits<std::allocator<T>>;

    T* data_ = nullptr;
    std::allocator<T> allocator_ = std::allocator<T>();
    size_t size_ = 0;
    size_t capacity_ = 1;

    void UpdateCapacity(size_t new_capacity) {
        T* data_new = allocator_.allocate(new_capacity);
        for (size_t i = 0; i < size_; ++i) {
            allocator_type::construct(allocator_, data_new + i, std::move(data_[i]));
            allocator_type::destroy(allocator_, data_ + i);
        }
        allocator_.deallocate(data_, capacity_);
        data_ = data_new;
        capacity_ = new_capacity;
    }
};
