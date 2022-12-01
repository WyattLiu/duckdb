#include "duckdb/common/allocator.hpp"
#include <iostream>
#include <mutex>
namespace duckdb {

std::mutex mtx;

AllocatedData::AllocatedData(Allocator &allocator, data_ptr_t pointer, idx_t allocated_size)
    : allocator(allocator), pointer(pointer), allocated_size(allocated_size) {
	std::cout << "AllocatedData: " << allocated_size << std::endl;
}
AllocatedData::~AllocatedData() {
	std::cout << "AllocatedData~: " << allocated_size << std::endl;
	Reset();
}

void AllocatedData::Reset() {
	if (!pointer) {
		return;
	}
	allocator.FreeData(pointer, allocated_size);
	pointer = nullptr;
}

Allocator::Allocator()
    : allocate_function(Allocator::DefaultAllocate), free_function(Allocator::DefaultFree),
      reallocate_function(Allocator::DefaultReallocate) {
	std::cout << "Allocator()" << std::endl;
}

Allocator::Allocator(allocate_function_ptr_t allocate_function_p, free_function_ptr_t free_function_p,
                     reallocate_function_ptr_t reallocate_function_p, unique_ptr<PrivateAllocatorData> private_data)
    : allocate_function(allocate_function_p), free_function(free_function_p),
      reallocate_function(reallocate_function_p), private_data(move(private_data)) {
	std::cout << "Allocator(something)" << std::endl;
}

data_ptr_t Allocator::AllocateData(idx_t size) {
	mtx.lock();
	std::cout << "malloc: " << size << std::endl;
	mtx.unlock();
	return allocate_function(private_data.get(), size);
}

void Allocator::FreeData(data_ptr_t pointer, idx_t size) {
	if (!pointer) {
		return;
	}
	return free_function(private_data.get(), pointer, size);
}

data_ptr_t Allocator::ReallocateData(data_ptr_t pointer, idx_t size) {
	if (!pointer) {
		return pointer;
	}
	return reallocate_function(private_data.get(), pointer, size);
}

} // namespace duckdb
