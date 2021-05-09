#ifndef LINKED_LIST_MEMORY_MANAGER_HPP
#define LINKED_LIST_MEMORY_MANAGER_HPP

#include "memory-manager.hpp"

#include <climits>
#include <deque>

enum class MemoryBlockType { ALLOCATED, HOLE };

struct MemoryBlock {
  MemoryBlockType memory_block_type;
  int begin;
  int size;

  MemoryBlock(MemoryBlockType memory_block_type_, int begin_, int size_)
      : memory_block_type{memory_block_type_}, begin{begin_}, size{size_} {}
};

class LinkedListMemoryManager : public MemoryManager {
  std::deque<MemoryBlock> memory_list;

  /**
   * Helper function to allocate memory from a given hole
   * @param hole_block_original_itr iterator of the hole from which allocate.
   * @param size Amount of memory to allocate.
   * @return pointer to begining of allocated memory
   */
  int _create_new_allocated_block(const std::deque<MemoryBlock>::iterator hole_block_original_itr,
                                  const int size);

public:
  LinkedListMemoryManager(int total_memory) : MemoryManager(total_memory) {
    // Add a hole entry i.e. intially all of the memory is hole
    memory_list.push_back(MemoryBlock(MemoryBlockType::HOLE, 0, total_memory));
  }

  /**
   * Allocates memory, 'First fit' method by default
   * @param size Amount of memory to allocate.
   * @return pointer to begining of allocated memory, -1 if allocation failed
   */
  int allocate(const int size) override { return first_fit_allocate(size); }

  void deallocate(const int begin) override;

  /**
   * Allocates memory using 'First fit' method
   * @param size Amount of memory to allocate.
   * @return pointer to begining of allocated memory, -1 if allocation failed
   */
  int first_fit_allocate(const int size);

  /**
   * Allocates memory using 'Best fit' method
   * @param size Amount of memory to allocate.
   * @return pointer to begining of allocated memory, -1 if allocation failed
   */
  int best_fit_allocate(const int size);
};
void LinkedListMemoryManager::deallocate(const int begin) {
  // function to merge hole with next hole
  static const auto merge_hole_with_next_hole =
      [this](std::deque<MemoryBlock>::iterator current_block_itr) {
        // if next block is a hole then merge
        const auto next_block_itr = current_block_itr + 1;
        if (next_block_itr != memory_list.end() and
            next_block_itr->memory_block_type == MemoryBlockType::HOLE) {
          // next block is valid and is a hole
          current_block_itr->size += next_block_itr->size;
          memory_list.erase(next_block_itr);
        }
      };

  for (auto current_block_itr = memory_list.begin(); current_block_itr != memory_list.end();
       current_block_itr++) {
    if (current_block_itr->begin == begin) {
      current_block_itr->memory_block_type = MemoryBlockType::HOLE;
      // merger with hole after it
      merge_hole_with_next_hole(current_block_itr);

      if (current_block_itr != memory_list.begin()) {
        // current_block is not the first element in list
        const auto prev = current_block_itr - 1;
        if (prev->memory_block_type == MemoryBlockType::HOLE) {
          merge_hole_with_next_hole(prev);
        }
      }
    }
  }
}

int LinkedListMemoryManager::_create_new_allocated_block(
    const std::deque<MemoryBlock>::iterator hole_block_original_itr, const int size) {

  const auto original_block = *hole_block_original_itr;

  // As we are allocating new block will be allocated, and will start at
  // begining of existing hole and just as big as the size required
  if (original_block.size == size) {
    // As the new block is perfectly equal to size of the existing hole and
    // data difference will only be in type of block begin and size will
    // remain same.
    hole_block_original_itr->memory_block_type = MemoryBlockType::ALLOCATED;
  } else {
    const auto new_allocated_block =
        MemoryBlock(MemoryBlockType::ALLOCATED, original_block.begin, size);

    const auto new_block_itr = memory_list.insert(hole_block_original_itr, new_allocated_block) + 1;

    const auto hole_block_new_itr = new_block_itr + 1;

    // As orignal block is now partially allocated new begining is moved
    // forward by size. However, size is decreased we took from this hole.
    hole_block_new_itr->begin = original_block.begin + size;
    hole_block_new_itr->size = original_block.size - size;
  }

  return original_block.begin;
}

int LinkedListMemoryManager::first_fit_allocate(const int size) {
  // First fit
  for (auto memory_list_itr = memory_list.begin(); memory_list_itr != memory_list.end();
       memory_list_itr++) {
    const auto &current_block = *memory_list_itr;

    // block is hole and its size is greater than or equal to required size
    if (current_block.memory_block_type == MemoryBlockType::HOLE and size <= current_block.size) {
      return _create_new_allocated_block(memory_list_itr, size);
    }
  }

  return -1;
}

int LinkedListMemoryManager::best_fit_allocate(const int size) {
  // invalid to start to determine if match was not found
  auto best_fit_itr = memory_list.end();
  int min_diff = INT_MAX;

  for (auto memory_list_itr = memory_list.begin(); memory_list_itr != memory_list.end();
       memory_list_itr++) {
    const auto &current_block = *memory_list_itr;

    // block is hole and its size is greater than or equal to required size
    if (current_block.memory_block_type == MemoryBlockType::HOLE and size <= current_block.size) {
      // difference in hole block size and size required
      const int diff = current_block.size - size;
      // if it's less than minimum of diff's till now
      if (min_diff > diff) {
        best_fit_itr = memory_list_itr;
        if (diff == 0) {
          // can't be better than exact size
          break;
        }
        min_diff = diff;
      }
    }
  }

  if (best_fit_itr == memory_list.end()) {
    // no hole of required size was found
    return -1;
  }

  return _create_new_allocated_block(best_fit_itr, size);
}

#endif // LINKED_LIST_MEMORY_MANAGER_HPP
