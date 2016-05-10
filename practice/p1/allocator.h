#include <stdexcept>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <stdio.h>

const size_t max_ptrs = 2048;

enum  AllocErrorType {
    InvalidFree,
    NoMemory,
};

struct ptr_with_len{
  void *ptr;
  size_t len;
};

class AllocError: std::runtime_error {
private:
    AllocErrorType type;

public:
    AllocError(AllocErrorType _type, std::string message):
            runtime_error(message),
            type(_type)
    {}

    AllocErrorType getType() const { return type; }
};

class Allocator;

class Pointer {
public:
    void *get() const {
        if (mptr==NULL)
          return mptr;
        return mptr->ptr;
         }
    Pointer()  {mptr=NULL;}
    Pointer(ptr_with_len *_mptr) : mptr(_mptr) {}
    void set_ptr(void *new_ptr);
    void set_size(size_t size);

    size_t get_size() {
      if (mptr!=NULL)
        return mptr->len;
      return -1;
    }
private:
    ptr_with_len *mptr;
};

class Allocator {
public:
    Allocator(void *base, size_t size);
    ~Allocator();
    Pointer alloc(size_t N);
    void realloc(Pointer &p, size_t N);
    void free(Pointer &p);
    // void print_memory_status();
    void defrag();
    std::string dump() { return ""; }
private:
    ptr_with_len array_of_pointers[max_ptrs];
    void *start_point;
    size_t memory_len;
    bool *memory_status;

};
