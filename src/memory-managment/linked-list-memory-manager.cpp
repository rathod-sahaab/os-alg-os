#include "linked-list-memory-manager.h"

#include <algorithm>
#include <climits>

int LinkedListMemoryManager::_create_new_allocated_block(
    const std::deque<MemoryBlock>::iterator hole_block_original_itr, const int size) {

  const auto original_block = *hole_block_original_itr;

  // As we are allocating new block will be allocated, and will start at
  // begining of existing hole and just as big as the size required
  if (original_block.size == size) {
    // As the new block is perfectly equal to size of the existing hole and
    // data difference will only be in type of block begin and size will
    // remain same.
    hole_block_original_itr->type = MemoryBlockType::ALLOCATED;
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

void LinkedListMemoryManager::deallocate(const int begin) {
  // function to merge hole with next hole
  const auto current_block_itr =
      std::find_if(memory_list.begin(), memory_list.end(),
                   [begin](const MemoryBlock &block) { return block.begin == begin; });

  if (current_block_itr == memory_list.end()) {
    return;
  }

  current_block_itr->type = MemoryBlockType::HOLE;
  // merger with hole after it
  {
    // merge next block if it's a hole
    const auto next_block_itr = current_block_itr + 1;
    if (next_block_itr != memory_list.end() and next_block_itr->type == MemoryBlockType::HOLE) {
      current_block_itr->size += next_block_itr->size;
      memory_list.erase(next_block_itr);
    }
  }

  {
    // merge with previous block if it's a hole
    if (current_block_itr != memory_list.begin()) {
      // current_block is not the first element in list then only it can have a previous element
      const auto prev_block_itr = current_block_itr - 1;
      if (prev_block_itr->type == MemoryBlockType::HOLE) {
        prev_block_itr->size += current_block_itr->size;
        memory_list.erase(current_block_itr);
      }
    }
  }
}

int LinkedListMemoryManager::first_fit_allocate(const int size) {
  // Find first hole that is larger than or equal to required size
  const auto first_adequate_hole_itr =
      std::find_if(memory_list.begin(), memory_list.end(), [size](auto current_block) {
        return current_block.memory_block_type == MemoryBlockType::HOLE and
               current_block.size >= size;
      });

  if (first_adequate_hole_itr == memory_list.end()) {
    // No hole was big enough, memory not available
    return -1;
  }

  return _create_new_allocated_block(first_adequate_hole_itr, size);
}

int LinkedListMemoryManager::best_fit_allocate(const int size) {
  // invalid to start to determine if match was not found
  /*
   * TODO: use <algorithm>
   * auto best_fit_itr = std::min_element(memory_list.begin(), memory_list.end(),
   *               [size](const MemoryBlock &a, const MemoryBlock &b) -> bool {
   *                    const int diff_a = a.size - size;
   *                    const int diff_b = b.size - size;

   *                    return diff_a < diff_b;
   *               });
   */
  auto best_fit_itr = memory_list.end();
  int min_diff = INT_MAX;

  for (auto memory_list_itr = memory_list.begin(); memory_list_itr != memory_list.end();
       memory_list_itr++) {
    const auto &current_block = *memory_list_itr;

    // block is hole and its size is greater than or equal to required size
    if (current_block.type == MemoryBlockType::HOLE and size <= current_block.size) {
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
