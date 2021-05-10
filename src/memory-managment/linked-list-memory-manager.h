#ifndef LINKED_LIST_MEMORY_MANAGER_HPP
#define LINKED_LIST_MEMORY_MANAGER_HPP

#include "memory-manager.hpp"

#include <deque>

enum class MemoryBlockType { ALLOCATED, HOLE };

struct MemoryBlock {
  MemoryBlockType type;
  int begin;
  int size;

  MemoryBlock(MemoryBlockType memory_block_type_, int begin_, int size_)
      : type{memory_block_type_}, begin{begin_}, size{size_} {}
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
  void _merge_hole_with_next_hole(std::deque<MemoryBlock>::iterator current_block_itr);

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

  /**
   * Deallocates memory, automatically merges with next and previous holes to make big blocks
   * @param begin Begin returned by the
   */
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
#endif // LINKED_LIST_MEMORY_MANAGER_HPP
