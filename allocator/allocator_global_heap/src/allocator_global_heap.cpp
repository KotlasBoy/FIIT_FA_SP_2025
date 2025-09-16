#include <not_implemented.h>
#include "../include/allocator_global_heap.h"

allocator_global_heap::allocator_global_heap(logger *logger)
    : _logger(logger)
{
    debug_with_guard("Created allocator_global_heap");
}

[[nodiscard]] void *allocator_global_heap::do_allocate_sm(size_t size)
{
    trace_with_guard("start do_allocate_sm");
    debug_with_guard("called do_allocate_sm(size_t)");

    if (size == 0) {
        error_with_guard("Zero size allocation");
        return nullptr;
    }

    void *ptr = nullptr;
    try {
        ptr = ::operator new(size);
    } catch (const std::bad_alloc &) {
        error_with_guard("Failed to allocate " + std::to_string(size));
        throw;
    }

    trace_with_guard("End do_allocate_sm");
    return ptr;
}

void allocator_global_heap::do_deallocate_sm(void *at)
{
    if (at == nullptr) {
        error_with_guard("Attempt to release nullptr");
        return;
    }
    debug_with_guard("Deallocating memory at " + std::to_string(reinterpret_cast<uintptr_t>(at)));

    ::operator delete(at);

    debug_with_guard("Successfully deallocated memory");
}

inline logger *allocator_global_heap::get_logger() const
{
    return _logger;
}

inline std::string allocator_global_heap::get_typename() const
{
    return "allocator_global_heap";
}

allocator_global_heap::~allocator_global_heap()
{
    debug_with_guard("Allocator destructor called");
}

allocator_global_heap::allocator_global_heap(const allocator_global_heap &other)
    : _logger(other._logger)
{
    debug_with_guard("Allocator copy constructor called");
}

allocator_global_heap &allocator_global_heap::operator=(const allocator_global_heap &other)
{
    if (this != &other) {
        _logger = other._logger;
        debug_with_guard("Allocator copy assignment called");
    }
    return *this;
}

bool allocator_global_heap::do_is_equal(const std::pmr::memory_resource &other) const noexcept
{
    return dynamic_cast<const allocator_global_heap *>(&other) != nullptr;
}

allocator_global_heap::allocator_global_heap(allocator_global_heap &&other) noexcept
    : _logger(other._logger)
{
    other._logger = nullptr;
    debug_with_guard("Allocator move constructor called");
}

allocator_global_heap &allocator_global_heap::operator=(allocator_global_heap &&other) noexcept
{
    if (this != &other) {
        _logger = other._logger;
        other._logger = nullptr;
        debug_with_guard("Allocator move assignment called");
    }
    return *this;
}
