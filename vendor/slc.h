/*

Standard Lucas C (slc.h) Library! - v0.0.1

author: Lucas Nogueira Roberto
email:  lucasnogueira064@gmail.com
github: https://github.com/nPr0nn

A single-header standard library layer for C

Public domain or MIT-0. See license statements at the end of this file.

TODO:
*/

//=============================================================================
// System/Platform/Architecture Detection for SLC
//=============================================================================

#ifndef SLC_LIBRARY_H
#define SLC_LIBRARY_H

// OS Detection
#if defined(__linux__)
#define SLC_PLATFORM_LINUX
#define SLC_SYSTEM "Linux"
#elif defined(_WIN32) || defined(_WIN64)
#define SLC_PLATFORM_WINDOWS
#define SLC_SYSTEM "Windows"
#define _CRT_SECURE_NO_WARNINGS
#define NOGDI
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef near
#undef far
#elif defined(__EMSCRIPTEN__)
#define SLC_PLATFORM_EMSCRIPTEN
#define SLC_SYSTEM "Emscripten"
#include <emscripten.h>
#else
#error "slc.h: Unsupported platform!"
#endif

// Architeture Detection
#if defined(__x86_64__) || defined(_M_X64)
#define SLC_ARCH_X86_64
#define SLC_ARCHITECTURE "x86_64"
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
#define SLC_ARCH_X86_32
#define SLC_ARCHITECTURE "x86_32"
#elif defined(__aarch64__) || defined(_M_ARM64)
#define SLC_ARCH_ARM64
#define SLC_ARCHITECTURE "ARM64"
#else
#define SLC_ARCH_UNKNOWN
#define SLC_ARCHITECTURE "UNKNOWN"
#endif

// Compiler Detection
#if defined(__clang__)
#define SLC_COMPILER_CLANG
#define SLC_COMPILER "clang"
#elif defined(__GNUC__) && !defined(__clang__)
#define SLC_COMPILER_GCC
#define SLC_COMPILER "gcc"
#elif defined(__TINYC__)
#define SLC_COMPILER_TCC
#define SLC_COMPILER "tcc"
#elif defined(_MSC_VER)
#define SLC_COMPILER_MSVC
#define SLC_COMPILER "cl"
#elif defined(__MINGW32__)
#define SLC_COMPILER_MINGW32
#define SLC_COMPILER "mingw32"
#elif defined(__MINGW64__)
#define SLC_COMPILER_MINGW64
#define SLC_COMPILER "mingw64"
#else
#define SLC_COMPILER_UNKNOWN
#define SLC_COMPILER "unknown"
#endif

// CPU Bitness Detection
#if defined(__LP64__) || defined(_LP64)
#define SLC_CPU_64BIT
#else
#define SLC_CPU_32BIT
#endif

// Byte Order Detection
#if defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) &&                \
    defined(__ORDER_LITTLE_ENDIAN__)
#define SLC_ENDIAN_LITTLE __ORDER_LITTLE_ENDIAN__
#define SLC_ENDIAN_BIG __ORDER_BIG_ENDIAN__
#define SLC_BYTE_ORDER __BYTE_ORDER__
#elif defined(_MSC_VER)
#define SLC_ENDIAN_LITTLE 1234
#define SLC_ENDIAN_BIG 4321
#define SLC_BYTE_ORDER SLC_ENDIAN_LITTLE
#else
#error "SLC: Could not determine byte order!"
#endif

// File and Line Macro
#define SLC_FILE __FILE__
#define SLC_LINE __LINE__

// Functions Annotations
#if defined(SLC_COMPILER_GCC) || defined(SLC_COMPILER_CLANG)
#define SLC_EXPORT __attribute__((used))
#define SLC_NORETURN __attribute__((noreturn))
#define SLC_PURE_FUNC __attribute__((pure)) __attribute__((warn_unused_result))
#define SLC_LIKELY(exp) __builtin_expect(!!(exp), 1)
#define SLC_UNLIKELY(exp) __builtin_expect(!!(exp), 0)
#elif defined(SLC_COMPILER_MSVC)
#include <sal.h>
#define SLC_EXPORT __declspec(dllexport)
#define SLC_NORETURN __declspec(noreturn)
#define SLC_PURE_FUNC _Check_return_
#define SLC_LIKELY(exp) (exp)
#define SLC_UNLIKELY(exp) (exp)
#else
#define SLC_EXPORT
#define SLC_NORETURN
#define SLC_PURE_FUNC
#define SLC_CONST_FUNC
#define SLC_LIKELY(exp) (exp)
#define SLC_UNLIKELY(exp) (exp)
#endif

// Functions API Annotations (expose or not)
#define SLC_API_INTERNAL static
#define SLC_API_INLINE static inline
#if defined(SLC_LIB_STATIC)
#define SLC_API_PUBLIC SLC_API_INTERNAL
#elif defined(_WIN32)
#if defined(SLC_DLL_BUILD)
#define SLC_API_PUBLIC __declspec(dllexport)
#elif defined(SLC_DLL_LOAD)
#define SLC_API_PUBLIC __declspec(dllimport)
#else
#define SLC_API_PUBLIC
#endif
#else
#define SLC_API_PUBLIC __attribute__((visibility("default")))
#endif

// Lets mark functions that depend on libc for future re-work (?)
#define LIBC_DEP

//=============================================================================
// Primitive Types
//=============================================================================

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef SLC_NO_PRIMITIVES_TYPES
#include <stddef.h>
#include <stdint.h>
typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;
typedef size_t usize;
typedef ptrdiff_t isize;
typedef float f32;
typedef double f64;
typedef bool b8;
#endif

// =============================================================================
//  Implementations Summary
// =============================================================================

#ifdef SLC_IMPL
#define SLC_MEMORY_MANAGEMENT_IMPL
#define SLC_IO_IMPL

#define SLC_ARRAY_IMPL
#define SLC_LINKED_LIST_IMPL
#define SLC_STRING_IMPL
#define SLC_HASH_MAP_IMPL
#define SLC_STACK_IMPL
#define SLC_QUEUE_IMPL
#define SLC_SET_IMPL

#define SLC_OS_IMPL
#define SLC_ERROR_IMPL
#define SLC_CMD_IMPL
#define SLC_DLL_IMPL
#define SLC_FILE_SYSTEM_IMPL
#define SLC_TIME_IMPL
#define SLC_THREADS_IMPL

#endif // SLC_IMPL

// =============================================================================
//  Memory Definitions
// =============================================================================

#define slc_stack_array_size(T) sizeof(T) / sizeof(T[0])

#define SLC_KB(x) ((x) * 1024u)
#define SLC_MB(x) ((x) * 1024u * 1024u)
#define SLC_GB(x) ((x) * 1024u * 1024u * 1024u)

// @brief  Allocates a block of memory.
// @param  size    Size of the memory block in bytes.
// @return         Pointer to the allocated memory, or NULL on failure.
//
SLC_API_PUBLIC void *slc_malloc(usize size);

// @brief  Frees a previously allocated block of memory.
// @param  block   Pointer to the memory block to free.
//
SLC_API_PUBLIC void slc_free(void *block);

SLC_API_PUBLIC LIBC_DEP void *slc_memcpy(void *destination, const void *source,
                                         usize num);

SLC_API_PUBLIC LIBC_DEP i32 slc_memcmp(const void *ptr1, const void *ptr2,
                                       usize num);

SLC_API_PUBLIC LIBC_DEP void *slc_memset(void *ptr, i32 value, usize num);

// Chunk of Memory for Arenas
#define SLC_CHUNK_DEFAULT_SIZE SLC_KB(8)
typedef struct slc_MemChunk {
  struct slc_MemChunk *next, *prev;
  u32 capacity;
  u32 used;
  u8 data[];
} slc_MemChunk;

typedef struct slc_MemArena {
  slc_MemChunk *begin, *end;
} slc_MemArena;

/// @brief  Allocates a block of memory from the arena.
/// @param  arena   Pointer to the memory arena.
/// @param  size    Size of the memory block in bytes.
/// @return         Pointer to the allocated memory, or NULL on failure.
SLC_API_PUBLIC void *slc_mem_arena_alloc(slc_MemArena *arena, usize size);

/// @brief  Allocates and zero-initializes a block of memory from the arena.
/// @param  arena   Pointer to the memory arena.
/// @param  size    Size of the memory block in bytes.
/// @return         Pointer to the allocated memory, or NULL on failure.
SLC_API_PUBLIC void *slc_mem_arena_calloc(slc_MemArena *arena, usize size);

/// @brief  Frees all memory used by the arena.
/// @param  arena   Pointer to the memory arena.
SLC_API_PUBLIC void slc_mem_arena_free(slc_MemArena *arena);

/// @brief  Resets the arena without freeing any memory.
///         All memory is marked as unused, and reused in subsequent
///         allocations.
/// @param  arena   Pointer to the memory arena.
SLC_API_PUBLIC void slc_mem_arena_reset(slc_MemArena *arena);

/// @brief  Gets the total size of memory currently allocated from the arena.
/// @param  arena   Pointer to the memory arena.
/// @return         Used size in bytes.
SLC_API_PUBLIC usize slc_mem_arena_size(slc_MemArena *arena);

/// @brief  Gets the total capacity of all chunks in the arena.
/// @param  arena   Pointer to the memory arena.
/// @return         Total reserved size in bytes.
SLC_API_PUBLIC usize slc_mem_arena_real_size(slc_MemArena *arena);

/// @brief  Allocates a block of memory using a full dedicated chunk.
/// @param  arena   Pointer to the memory arena.
/// @param  size    Size of the memory block in bytes.
/// @return         Pointer to the chunk's memory, or NULL on failure.
SLC_API_PUBLIC void *slc_mem_arena_alloc_chunk(slc_MemArena *arena, usize size);

/// @brief  Allocates and zero-initializes a block of memory using a full
/// dedicated chunk.
/// @param  arena   Pointer to the memory arena.
/// @param  size    Size of the memory block in bytes.
/// @return         Pointer to the chunk's memory, or NULL on failure.
SLC_API_PUBLIC void *slc_mem_arena_calloc_chunk(slc_MemArena *arena,
                                                usize size);

/// @brief  Reallocates a previously allocated chunk to a new size.
/// @param  arena   Pointer to the memory arena.
/// @param  ptr     Old memory pointer.
/// @param  size    New size in bytes.
/// @return         New memory pointer, or NULL on failure.
SLC_API_PUBLIC void *slc_mem_arena_realloc_chunk(slc_MemArena *arena, void *ptr,
                                                 usize size);

/// @brief  Frees a chunk previously allocated by `slc_mem_arena_alloc_chunk`.
/// @param  arena   Pointer to the memory arena.
/// @param  ptr     Memory pointer to free.
SLC_API_PUBLIC void slc_mem_arena_free_chunk(slc_MemArena *arena, void *ptr);

#ifdef SLC_NO_LIB_PREFIX
#define KB(x) SLC_KB(x)
#define MB(x) SLC_MB(x)
#define GB(x) SLC_GB(x)
#define MemChunk slc_MemChunk
#define MemArena slc_MemArena
#define mem_arena_alloc slc_mem_arena_alloc
#define mem_arena_calloc slc_mem_arena_calloc
#define mem_arena_free slc_mem_arena_free
#define mem_arena_reset slc_mem_arena_reset
#define mem_arena_size slc_mem_arena_size
#define mem_arena_real_size slc_mem_arena_real_size
#define mem_arena_alloc_chunk slc_mem_arena_alloc_chunk
#define mem_arena_calloc_chunk slc_mem_arena_calloc_chunk
#define mem_arena_realloc_chunk slc_mem_arena_realloc_chunk
#define mem_arena_free_chunk slc_mem_arena_free_chunk
#define stack_array_size slc_stack_array_size
#endif

// =============================================================================
//  Memory Implementation
// =============================================================================

#ifdef SLC_MEMORY_MANAGEMENT_IMPL

SLC_API_PUBLIC LIBC_DEP void *slc_malloc(usize size) { return malloc(size); }

SLC_API_PUBLIC LIBC_DEP void slc_free(void *block) { free(block); }

SLC_API_PUBLIC LIBC_DEP void *slc_memcpy(void *destination, const void *source,
                                         usize num) {
  return memcpy(destination, source, num);
}

SLC_API_PUBLIC LIBC_DEP i32 slc_memcmp(const void *ptr1, const void *ptr2,
                                       usize num) {
  return memcmp(ptr1, ptr2, num);
}

SLC_API_PUBLIC LIBC_DEP void *slc_memset(void *ptr, i32 value, usize num) {
  return memset(ptr, value, num);
}

// Memory Arenas

// Internal helpers
SLC_API_INTERNAL slc_MemChunk *slc_mem_chunk_create(usize capacity) {
  slc_MemChunk *chunk =
      (slc_MemChunk *)slc_malloc(sizeof(slc_MemChunk) + capacity);
  if (!chunk)
    return NULL;
  chunk->next = chunk->prev = NULL;
  chunk->capacity = (u32)capacity;
  chunk->used = 0;
  return chunk;
}

SLC_API_INTERNAL void slc_mem_chunk_destroy(slc_MemChunk *chunk) {
  slc_free(chunk);
}

// Arena API
SLC_API_PUBLIC void *slc_mem_arena_alloc(slc_MemArena *arena, usize size) {
  if (!arena || size == 0)
    return NULL;
  size = (size + 7) & ~7ULL; // align to 8 bytes

  slc_MemChunk *chunk = arena->end;

  if (!chunk || chunk->used + size > chunk->capacity) {
    usize chunk_size =
        size > SLC_CHUNK_DEFAULT_SIZE ? size : SLC_CHUNK_DEFAULT_SIZE;
    slc_MemChunk *new_chunk = slc_mem_chunk_create(chunk_size);
    if (!new_chunk)
      return NULL;

    if (arena->end) {
      arena->end->next = new_chunk;
      new_chunk->prev = arena->end;
      arena->end = new_chunk;
    } else {
      arena->begin = arena->end = new_chunk;
    }
    chunk = new_chunk;
  }

  void *ptr = chunk->data + chunk->used;
  chunk->used += (u32)size;
  return ptr;
}

SLC_API_PUBLIC void *slc_mem_arena_calloc(slc_MemArena *arena, usize size) {
  void *ptr = slc_mem_arena_alloc(arena, size);
  if (ptr)
    slc_memset(ptr, 0, size);
  return ptr;
}

SLC_API_PUBLIC void slc_mem_arena_free(slc_MemArena *arena) {
  if (!arena)
    return;
  slc_MemChunk *chunk = arena->begin;
  while (chunk) {
    slc_MemChunk *next = chunk->next;
    slc_mem_chunk_destroy(chunk);
    chunk = next;
  }
  arena->begin = arena->end = NULL;
}

SLC_API_PUBLIC void slc_mem_arena_reset(slc_MemArena *arena) {
  if (!arena)
    return;
  for (slc_MemChunk *chunk = arena->begin; chunk; chunk = chunk->next) {
    chunk->used = 0;
  }
}

SLC_API_PUBLIC usize slc_mem_arena_size(slc_MemArena *arena) {
  if (!arena)
    return 0;
  usize total = 0;
  for (slc_MemChunk *chunk = arena->begin; chunk; chunk = chunk->next) {
    total += chunk->used;
  }
  return total;
}

SLC_API_PUBLIC usize slc_mem_arena_real_size(slc_MemArena *arena) {
  if (!arena)
    return 0;
  usize total = 0;
  for (slc_MemChunk *chunk = arena->begin; chunk; chunk = chunk->next) {
    total += chunk->capacity;
  }
  return total;
}

// Dedicated chunk API
SLC_API_PUBLIC void *slc_mem_arena_alloc_chunk(slc_MemArena *arena,
                                               usize size) {
  if (!arena || size == 0)
    return NULL;
  slc_MemChunk *chunk = slc_mem_chunk_create(size);
  if (!chunk)
    return NULL;

  if (arena->end) {
    arena->end->next = chunk;
    chunk->prev = arena->end;
    arena->end = chunk;
  } else {
    arena->begin = arena->end = chunk;
  }

  chunk->used = (u32)size;
  return chunk->data;
}

SLC_API_PUBLIC void *slc_mem_arena_calloc_chunk(slc_MemArena *arena,
                                                usize size) {
  void *ptr = slc_mem_arena_alloc_chunk(arena, size);
  if (ptr)
    slc_memset(ptr, 0, size);
  return ptr;
}

SLC_API_PUBLIC void *slc_mem_arena_realloc_chunk(slc_MemArena *arena, void *ptr,
                                                 usize size) {
  if (!arena || !ptr)
    return NULL;

  for (slc_MemChunk *chunk = arena->begin; chunk; chunk = chunk->next) {
    if (chunk->data == ptr) {
      void *new_ptr = slc_mem_arena_alloc_chunk(arena, size);
      if (!new_ptr)
        return NULL;
      usize copy_size = size < chunk->used ? size : chunk->used;
      slc_memcpy(new_ptr, ptr, copy_size);
      slc_mem_arena_free_chunk(arena, ptr);
      return new_ptr;
    }
  }
  return NULL;
}

SLC_API_PUBLIC void slc_mem_arena_free_chunk(slc_MemArena *arena, void *ptr) {
  if (!arena || !ptr)
    return;

  for (slc_MemChunk *chunk = arena->begin; chunk; chunk = chunk->next) {
    if (chunk->data == ptr) {
      if (chunk->prev)
        chunk->prev->next = chunk->next;
      else
        arena->begin = chunk->next;

      if (chunk->next)
        chunk->next->prev = chunk->prev;
      else
        arena->end = chunk->prev;

      slc_mem_chunk_destroy(chunk);
      return;
    }
  }
}

#endif // SLC_MEMORY_MANAGEMENT_IMPL

// =============================================================================
//  I/O Definitions
// =============================================================================

// @brief  Prints formatted output to the console (stdout).
// @param  format  Format string (printf-style).
// @param  ...     Additional arguments to format.
//
SLC_API_PUBLIC void slc_print(const char *format, ...);

// @brief  Reads a line of text safely from the console (stdin).
// @param  buffer  Destination buffer to store the line.
// @param  size    Maximum number of characters to read (including null
// terminator).
// @return         Number of characters read, or -1 on error.
//
SLC_API_PUBLIC i32 slc_read_line(char *buffer, i32 size);

#ifdef SLC_NO_LIB_PREFIX
#define slc_print print
#define slc_stream_print stream_print
#define slc_read_line read_line
#endif

// =============================================================================
//  I/O Implementation
// =============================================================================

#ifdef SLC_IO_IMPL

/// @brief Prints formatted text to the given FILE stream.
/// @param stream Target stream (e.g., stdout or stderr)
/// @param format printf-style format string
SLC_API_PUBLIC LIBC_DEP void slc_stream_print(FILE *stream, const char *format,
                                              ...) {
  if (!stream || !format)
    return; // safety guard

  va_list args;
  va_start(args, format);
  vfprintf(stream, format, args);
  va_end(args);
}

/// @brief Prints formatted text to stdout.
/// @param format printf-style format string
SLC_API_PUBLIC LIBC_DEP void slc_print(const char *format, ...) {
  if (!format)
    return;

  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
}

SLC_API_PUBLIC LIBC_DEP i32 slc_read_line(char *buffer, i32 size) {
  if (fgets(buffer, size, stdin) == NULL) {
    return -1; // Error or EOF
  }

  // Remove trailing newline, if it exists
  char *newline = strchr(buffer, '\n');
  if (newline) {
    *newline = '\0';
  }

  return (i32)strlen(buffer);
}

#endif // SLC_IMPL

// =============================================================================
//  Data Structures and Algorithms
// =============================================================================

// =============================================================================
//  Dynamic Array Definition
// =============================================================================

#define slc_DynamicArray(T)                                                    \
  struct {                                                                     \
    T *data;             /* Pointer to element storage */                      \
    usize size;          /* Number of elements currently in array */           \
    usize capacity;      /* Allocated capacity */                              \
    slc_MemArena *arena; /* Arena backing this array */                        \
  }

#ifdef SLC_NO_LIB_PREFIX
#define DynamicArray slc_DynamicArray
#define dynamic_array_create slc_dynamic_array_create
#define dynamic_array_push_back slc_dynamic_array_push_back
#define dynamic_array_pop_back slc_dynamic_array_pop_back
#define dynamic_array_pop_index slc_dynamic_array_pop_index
#define dynamic_array_get slc_dynamic_array_get
#define dynamic_array_set slc_dynamic_array_set
#define dynamic_array_size slc_dynamic_array_size
#define dynamic_array_clear slc_dynamic_array_clear
#define dynamic_array_destroy slc_dynamic_array_destroy
#define dynamic_array_sort slc_dynamic_array_sort
#endif

// =============================================================================
//  Dynamic Array Implementation
// =============================================================================

#ifdef SLC_ARRAY_IMPL

/// @brief Creates a dynamic array of type T in the given arena.
/// @param T Type of the elements
/// @param initial_capacity Initial capacity (number of elements)
/// @param arena Memory arena to allocate from
/// @return A dynamic array struct (by value)
#define slc_dynamic_array_create(T, initial_capacity, arena_ptr)               \
  {.data =                                                                     \
       (T *)slc_mem_arena_alloc((arena_ptr), sizeof(T) * (initial_capacity)),  \
   .size = 0,                                                                  \
   .capacity = (initial_capacity),                                             \
   .arena = (arena_ptr)}

/// @param array Pointer to the dynamic array
/// @param element Element value to add
#define slc_dynamic_array_push_back(array, element)                            \
  do {                                                                         \
    if ((array)->size >= (array)->capacity) {                                  \
      usize new_capacity =                                                     \
          ((array)->capacity > 0) ? ((array)->capacity * 2) : 1;               \
      __typeof__((array)->data[0]) *new_data =                                 \
          (__typeof__((array)->data[0]) *)slc_mem_arena_alloc(                 \
              (array)->arena, sizeof(*(array)->data) * new_capacity);          \
      if (!new_data)                                                           \
        break; /* allocation failed */                                         \
      slc_memcpy(new_data, (array)->data,                                      \
                 sizeof(*(array)->data) * (array)->size);                      \
      (array)->data = new_data;                                                \
      (array)->capacity = new_capacity;                                        \
    }                                                                          \
    (array)->data[(array)->size++] = (element);                                \
  } while (0)

/// @brief Pops the last element of the dynamic array. (No return value).
/// @param array Pointer to the dynamic array
/// @param out_val Pointer to store the popped value (should not be NULL)
#define slc_dynamic_array_pop_back(array, out_val)                             \
  do {                                                                         \
    if ((array)->size > 0) {                                                   \
      (array)->size--;                                                         \
      *((__typeof__((array)->data))out_val) = (array)->data[(array)->size];    \
    }                                                                          \
  } while (0)

/// @brief Pops the element at a specific index. (No return value).
/// @param array Pointer to the dynamic array
/// @param index Index to pop (0-based)
/// @param out_val Pointer to store the popped value (should not be NULL)
#define slc_dynamic_array_pop_index(array, index, out_val)                     \
  do {                                                                         \
    if ((index) < (array)->size) {                                             \
      *((__typeof__((array)->data))out_val) = (array)->data[(index)];          \
      for (usize _i = (index); _i < (array)->size - 1; _i++)                   \
        (array)->data[_i] = (array)->data[_i + 1];                             \
      (array)->size--;                                                         \
    }                                                                          \
  } while (0)

/// @brief Gets a pointer to the element at a given index.
/// @param array Pointer to the dynamic array
/// @param index Index
/// @return Pointer to element, or NULL if out of bounds
#define slc_dynamic_array_get(array, index)                                    \
  (((index) < (array)->size) ? &((array)->data[index]) : NULL)

/// @brief Sets the element at a given index.
/// @param array Pointer to the dynamic array
/// @param index Index to set
/// @param value Value to assign
/// @return 1 if successful, 0 if index out of bounds
#define slc_dynamic_array_set(array, index, value)                             \
  (((index) < (array)->size) ? ((array)->data[(index)] = (value), 1) : 0)

/// @brief Returns the number of elements in the array
#define slc_dynamic_array_size(array) ((array)->size)

/// @brief Clears the dynamic array (does not free arena memory)
#define slc_dynamic_array_clear(array) ((array)->size = 0)

/// @brief Destroys the dynamic array (clears metadata; memory remains in arena)
#define slc_dynamic_array_destroy(array)                                       \
  do {                                                                         \
    (array)->data = NULL;                                                      \
    (array)->size = 0;                                                         \
    (array)->capacity = 0;                                                     \
    (array)->arena = NULL;                                                     \
  } while (0)

/// @brief Sorts the dynamic array using the standard C qsort function.
/// @param array Pointer to the dynamic array.
/// @param compar_fn A qsort-compatible comparison function.
#define slc_dynamic_array_sort(array, compar_fn)                               \
  do {                                                                         \
    if ((array)->size >                                                        \
        1) { /* qsort is safe with 0/1, but this avoids work */                \
      qsort((array)->data, (array)->size, sizeof((array)->data[0]),            \
            compar_fn);                                                        \
    }                                                                          \
  } while (0)

#endif

// =============================================================================
//  String Declaration
// =============================================================================

/// @brief A non-owning, immutable view of a sequence of characters.
typedef struct slc_StringView {
  const char *data;
  usize size;
} slc_StringView;

/// @brief A dynamic, arena-allocated string, built upon the dynamic array.
/// The string is always null-terminated in its buffer for C interoperability,
/// though the `size` property does not include the null terminator.
typedef slc_DynamicArray(char) slc_String;

#ifdef SLC_NO_LIB_PREFIX
#define StringView slc_StringView
#define String slc_String
#define string_create slc_string_create
#define string_from_cstr slc_string_from_cstr
#define string_from_view slc_string_from_view
#define string_append_char slc_string_append_char
#define string_append_cstr slc_string_append_cstr
#define string_append_view slc_string_append_view
#define string_append slc_string_append
#define string_equals_view slc_string_equals_view
#define string_equals_cstr slc_string_equals_cstr
#define string_substring_view slc_string_substring_view
#define string_c_str slc_string_c_str
#define string_view slc_string_view
#define string_clear slc_string_clear
#endif

// Function Prototypes for slc_string
SLC_API_PUBLIC slc_String slc_string_create(slc_MemArena *arena_ptr);
SLC_API_PUBLIC slc_String slc_string_from_cstr(const char *cstr,
                                               slc_MemArena *arena_ptr);
SLC_API_PUBLIC slc_String slc_string_from_view(slc_StringView view,
                                               slc_MemArena *arena_ptr);
SLC_API_PUBLIC void slc_string_append_char(slc_String *str, char c);
SLC_API_PUBLIC void slc_string_append_cstr(slc_String *str, const char *cstr);
SLC_API_PUBLIC void slc_string_append_view(slc_String *str,
                                           slc_StringView view);
SLC_API_PUBLIC void slc_string_append(slc_String *str1, slc_String *str2);
SLC_API_PUBLIC void slc_string_clear(slc_String *str);

/// @brief Retutn lenght of c string
SLC_API_INLINE i32 slc_strlen(const char *cstr) {
  if (cstr == NULL)
    return -1; // signal invalid input

  i32 len = 0;
  while (cstr[len] != '\0') {
    len++;
    if (len > SLC_MB(1))
      return -2; // signal untrusted input
  }
  return len;
}

/// @brief Compares a string and a StringView for equality.
/// @param str Pointer to the slc_String.
/// @param view The StringView to compare against.
SLC_API_INLINE bool slc_string_equals_view(const slc_String *str,
                                           slc_StringView view) {
  if (!str)
    return false; // A null string is not equal to any view.
  if (str->size != view.size)
    return false;
  if (str->size == 0)
    return true; // Both are empty
  if (!view.data)
    return false; // Non-empty string vs null view
  return slc_memcmp(str->data, view.data, str->size) == 0;
}

/// @brief Compares two strings for equality.
/// @param str1 Pointer to the first slc_String.
SLC_API_INLINE bool slc_string_equals(const slc_String *str1,
                                      const slc_String *str2) {
  if (str1 == str2)
    return true; // Same pointer
  if (!str1 || !str2)
    return false; // One is null
  if (str1->size != str2->size)
    return false;
  if (str1->size == 0)
    return true; // Both are empty
  return slc_memcmp(str1->data, str2->data, str1->size) == 0;
}

/// @brief Compares a slc_String with a null-terminated C string for equality.
/// @param str Pointer to the slc_String.
/// @param cstr Null-terminated C string to compare against.
/// @return true if they are equal, false otherwise.
SLC_API_INLINE bool slc_string_equals_cstr(const slc_String *str,
                                           const char *cstr) {
  if (!str || !cstr)
    return false;

  i32 cstr_len = slc_strlen(cstr);

  if (str->size != cstr_len)
    return false;
  if (str->size == 0)
    return true; // Both are empty strings

  return slc_memcmp(str->data, cstr, str->size) == 0;
}

/// @brief Extracts a non-owning StringView from a string.
/// @param str Pointer to the slc_String.
/// @param start The starting index of the substring.
/// @param len The length of the substring.
/// @return An slc_StringView. If bounds are invalid, returns a zero-sized view.
SLC_API_INLINE slc_StringView slc_string_substring_view(const slc_String *str,
                                                        usize start,
                                                        usize len) {
  slc_StringView view = {NULL, 0};
  if (str && start + len <= str->size) {
    view.data = str->data + start;
    view.size = len;
  }
  return view;
}

/// @brief Returns a const, null-terminated C-style string.
SLC_API_INLINE const char *slc_string_c_str(const slc_String *str) {
  return str ? (const char *)str->data : "";
}

/// @brief Creates a non-owning StringView spanning the entire string.
SLC_API_INLINE slc_StringView slc_string_view(const slc_String *str) {
  if (!str)
    return (slc_StringView){NULL, 0};
  return (slc_StringView){.data = str->data, .size = str->size};
}

// =============================================================================
//  String Implementation
// =============================================================================

#ifdef SLC_STRING_IMPL
SLC_API_INTERNAL void slc_string_ensure_capacity(slc_String *str,
                                                 usize add_len) {
  usize needed_cap = str->size + add_len + 1; // +1 for '\0'
  if (needed_cap > str->capacity) {
    usize new_capacity = str->capacity > 0 ? str->capacity * 2 : 8;
    if (new_capacity < needed_cap) {
      new_capacity = needed_cap;
    }
    char *new_data = (char *)slc_mem_arena_alloc(str->arena, new_capacity);
    if (!new_data) {
      // Allocation failed. The string remains unchanged. This is a silent
      // failure, consistent with the original macro's 'break' behavior.
      return;
    }
    slc_memcpy(new_data, str->data, str->size);
    str->data = new_data;
    str->capacity = new_capacity;
  }
}

/// @brief Creates an empty string with a small initial capacity.
SLC_API_PUBLIC slc_String slc_string_create(slc_MemArena *arena_ptr) {
  slc_String str = slc_dynamic_array_create(char, 8, arena_ptr);
  if (str.data) {
    str.data[0] = '\0';
  }
  return str;
}

/// @brief Creates a string from a null-terminated C-style string.
SLC_API_PUBLIC slc_String slc_string_from_cstr(const char *cstr,
                                               slc_MemArena *arena_ptr) {
  usize len = cstr ? slc_strlen(cstr) : 0;
  slc_String str = slc_dynamic_array_create(char, len + 1, arena_ptr);
  if (str.data) {
    if (len > 0) {
      slc_memcpy(str.data, cstr, len);
    }
    str.data[len] = '\0';
    str.size = len;
  }
  return str;
}

/// @brief Creates a string by copying from a StringView.
SLC_API_PUBLIC slc_String slc_string_from_view(slc_StringView view,
                                               slc_MemArena *arena_ptr) {
  slc_String str = slc_dynamic_array_create(char, view.size + 1, arena_ptr);
  if (str.data) {
    if (view.size > 0) {
      slc_memcpy(str.data, view.data, view.size);
    }
    str.data[view.size] = '\0';
    str.size = view.size;
  }
  return str;
}

/// @brief Appends a single character to the string.
SLC_API_PUBLIC void slc_string_append_char(slc_String *str, char c) {
  slc_string_ensure_capacity(str, 1);
  if (str->data) {
    str->data[str->size++] = c;
    str->data[str->size] = '\0';
  }
}

/// @brief Appends a null-terminated C-string to the string.
SLC_API_PUBLIC void slc_string_append_cstr(slc_String *str, const char *cstr) {
  usize len = cstr ? slc_strlen(cstr) : 0;
  if (len == 0)
    return;
  slc_string_ensure_capacity(str, len);
  if (str->data) {
    slc_memcpy(str->data + str->size, cstr, len);
    str->size += len;
    str->data[str->size] = '\0';
  }
}

/// @brief Appends another StringView to the string.
SLC_API_PUBLIC void slc_string_append_view(slc_String *str,
                                           slc_StringView view) {
  if (view.size == 0)
    return;
  slc_string_ensure_capacity(str, view.size);
  if (str->data) {
    slc_memcpy(str->data + str->size, view.data, view.size);
    str->size += view.size;
    str->data[str->size] = '\0';
  }
}

/// @brief Appends another StringView to the string.
SLC_API_PUBLIC void slc_string_append(slc_String *str1, slc_String *str2) {
  slc_StringView str2_view = slc_string_view(str2);
  slc_string_append_view(str1, str2_view);
}

/// @brief Clears the string, setting its size to 0.
SLC_API_PUBLIC void slc_string_clear(slc_String *str) {
  str->size = 0;
  if (str->data) {
    str->data[0] = '\0';
  }
}

#endif

#ifdef SLC_LINKED_LIST_IMPL
#endif

#ifdef SLC_HASH_MAP_IMPL
#endif

#ifdef SLC_STACK_IMPL
#endif

#ifdef SLC_QUEUE_IMPL
#endif

#ifdef SLC_SET_IMPL
#endif

// =============================================================================
//  CMD Interface Definitions
// =============================================================================

SLC_API_PUBLIC LIBC_DEP void slc_cmd_exec(i32 argc, slc_String *cmd_args);
SLC_API_PUBLIC LIBC_DEP slc_String *slc_list_files(const char *folder_path,
                                                   size_t *out_count,
                                                   slc_MemArena *arena_ptr);
#ifdef SLC_NO_LIB_PREFIX
#define cmd_exec slc_cmd_exec
#endif

// =============================================================================
//  CMD Interface Implementation
// =============================================================================

#ifdef SLC_CMD_IMPL
// TODO: Implement this more carefully, use string view and a proper string
// and CMD struct, its fine for now tho

#ifdef SLC_PLATFORM_LINUX
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
/// @brief Executes a command in a new process and waits for it to complete.
/// This function is a reimplementation that uses the slc_String library
/// to simplify argument handling.
/// @param argc Number of arguments.
/// @param argv Array of argument strings.
SLC_API_PUBLIC LIBC_DEP void slc_cmd_exec(i32 argc, slc_String *cmd_args) {
  if (!cmd_args || argc == 0) {
    slc_stream_print(stderr,
                     "SLC_CMD Error: No command provided to execute.\n");
    return;
  }

  pid_t pid = fork();
  if (pid == -1) {
    slc_stream_print(stderr, "SLC_CMD Error: fork() failed: %s\n",
                     strerror(errno));
    return;
  }

  if (pid == 0) {
    // --- Child Process ---
    slc_MemArena arena = {0};
    slc_MemArena *arena_ptr = &arena;

    // A small arena is sufficient, as we are only allocating the array of
    // pointers, not the strings themselves.
    slc_mem_arena_alloc_chunk(arena_ptr, 1024);

    // Build the argument list for execvp. execvp requires a `char*` array,
    // so we create a temporary dynamic array to hold pointers to the data
    // of the input slc_String array.
    slc_DynamicArray(char *) exec_argv =
        slc_dynamic_array_create(char *, (usize)argc + 1, arena_ptr);

    for (size_t i = 0; i < argc; i++) {
      // Push the raw, null-terminated char* pointer from each slc_String.
      slc_dynamic_array_push_back(&exec_argv, cmd_args[i].data);
    }
    // execvp expects the argument array to be NULL-terminated.
    slc_dynamic_array_push_back(&exec_argv, NULL);

    execvp(exec_argv.data[0], exec_argv.data);

    // If execvp returns, an error occurred.
    slc_stream_print(stderr, "SLC_CMD Error: execvp failed for '%s': %s\n",
                     exec_argv.data[0], strerror(errno));
    slc_mem_arena_free(arena_ptr);
    exit(127);
  }

  // --- Parent Process ---
  int status = 0;
  waitpid(pid, &status, 0);

  if (WIFEXITED(status)) {
    int exit_code = WEXITSTATUS(status);
    if (exit_code != 0) {
      slc_stream_print(stderr,
                       "SLC_CMD Info: Command exited with non-zero code: %d\n",
                       exit_code);
    }
  } else if (WIFSIGNALED(status)) {
    int signal_num = WTERMSIG(status);
    slc_stream_print(stderr,
                     "SLC_CMD Error: Command terminated by signal: %d\n",
                     signal_num);
  }
}

#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

/// @brief Returns an array of all file paths inside a directory.
/// @param folder_path Path to the folder.
/// @param out_count Optional pointer to store the number of files found.
/// @return Dynamic array of slc_String representing file paths.
SLC_API_PUBLIC LIBC_DEP slc_String *slc_list_files(const char *folder_path,
                                                   size_t *out_count,
                                                   slc_MemArena *arena_ptr) {
  if (!folder_path || !arena_ptr)
    return NULL;

  slc_DynamicArray(slc_String) files =
      slc_dynamic_array_create(slc_String, 16, arena_ptr);

  DIR *dir = opendir(folder_path);
  if (!dir) {
    slc_stream_print(stderr,
                     "SLC_FS Error: Failed to open directory '%s': %s\n",
                     folder_path, strerror(errno));
    if (out_count)
      *out_count = 0;
    return NULL;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    // Skip "." and ".."
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;

    // Build the full path: folder_path + "/" + entry->d_name
    usize len = strlen(folder_path) + 1 + strlen(entry->d_name) + 1;
    char *full_path = slc_mem_arena_alloc(arena_ptr, len);
    snprintf(full_path, len, "%s/%s", folder_path, entry->d_name);

    slc_String path_str = slc_string_from_cstr(full_path, arena_ptr);
    slc_dynamic_array_push_back(&files, path_str);
  }

  closedir(dir);

  if (out_count)
    *out_count = files.size;

  // Return the internal array from the dynamic array
  return files.data;
}

#endif

#ifdef SLC_PLATFORM_WINDOWS
#include <stdio.h>
#include <string.h>
#include <windows.h>

/// @brief Executes a command in a new process and waits for it to complete.
/// This function is a reimplementation that uses the slc_String library
/// to simplify argument handling.
/// @param argc Number of arguments.
/// @param argv Array of slc_String arguments.
SLC_API_PUBLIC LIBC_DEP void slc_cmd_exec(i32 argc, slc_String *cmd_args) {
  if (!cmd_args || argc == 0) {
    slc_stream_print(stderr,
                     "SLC_CMD Error: No command provided to execute.\n");
    return;
  }

  slc_MemArena arena = {0};
  slc_MemArena *arena_ptr = &arena;
  slc_mem_arena_alloc_chunk(arena_ptr, SLC_CHUNK_DEFAULT_SIZE);

  // --- Windows requires a single command-line string ---
  // We must build one from the cmd_args array, quoting any arguments that
  // contain spaces to ensure they are parsed correctly by the new process.

  // 1. Calculate the total length needed for the final string.
  size_t total_len = 0;
  for (i32 i = 0; i < argc; i++) {
    total_len += cmd_args[i].size;
    if (strchr(cmd_args[i].data, ' ')) {
      total_len += 2; // For quotes ""
    }
  }
  if (argc > 1) {
    total_len += (size_t)argc - 1; // For spaces between arguments
  }
  total_len += 1; // For the null terminator

  // 2. Allocate and build the string. CreateProcessA requires a mutable buffer.
  char *cmdline = slc_mem_arena_alloc(arena_ptr, total_len);
  if (!cmdline) {
    slc_stream_print(
        stderr, "SLC_CMD Error: Failed to allocate memory for command line.\n");
    slc_mem_arena_free(arena_ptr);
    return;
  }
  cmdline[0] = '\0';

  for (i32 i = 0; i < argc; i++) {
    const char *arg_data = cmd_args[i].data;
    if (strchr(arg_data, ' ')) {
      strcat(cmdline, "\"");
      strcat(cmdline, arg_data);
      strcat(cmdline, "\"");
    } else {
      strcat(cmdline, arg_data);
    }
    if (i < argc - 1) {
      strcat(cmdline, " ");
    }
  }

  // Create and wait for the process
  STARTUPINFOA si;
  PROCESS_INFORMATION pi;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));

  if (!CreateProcessA(NULL, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &si,
                      &pi)) {
    DWORD ec = GetLastError();
    slc_stream_print(
        stderr, "SLC_CMD Error: CreateProcessA failed (code %lu) for '%s'\n",
        ec, cmdline);
    slc_mem_arena_free(arena_ptr);
    return;
  }

  // Wait for the child process to complete.
  WaitForSingleObject(pi.hProcess, INFINITE);

  DWORD exit_code = 0;
  if (!GetExitCodeProcess(pi.hProcess, &exit_code)) {
    slc_stream_print(
        stderr, "SLC_CMD Error: Could not get process exit code (err %lu)\n",
        GetLastError());
  } else if (exit_code != 0) {
    slc_stream_print(stderr,
                     "SLC_CMD Info: Command exited with non-zero code: %lu\n",
                     exit_code);
  }

  // Clean up process handles and memory.
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
  slc_mem_arena_free(arena_ptr);
}
#endif

#endif // SLC_CMD_IMPL
#endif // SLC_LIBRARY_H

/*

This software is available as a choice of the following licenses. Choose
whichever you prefer.

===============================================================================
ALTERNATIVE 1 - Public Domain (www.unlicense.org)
===============================================================================
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or distribute
this software, either in source code form or as a compiled binary, for any
purpose, commercial or non-commercial, and by any means.

In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>

===============================================================================
ALTERNATIVE 2 - MIT No Attribution
===============================================================================

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
