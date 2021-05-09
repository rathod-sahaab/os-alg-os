#ifndef MEMORY_MANAGER_HPP
#define MEMORY_MANAGER_HPP

class MemoryManager {
protected:
  int total_memory;

public:
  MemoryManager(int total_memory_) : total_memory{total_memory_} {}
  /**
   * Allocates memory
   * @param size bytes of memory to allocate
   * @return begin address of memory return -1 if not enough memory available
   */
  virtual int allocate(const int size);
  /**
   * Deallocates memory
   * @param begin Begining of an allocated block
   */
  virtual void deallocate(const int begin);
};

#endif // MEMORY_MANAGER_HPP
