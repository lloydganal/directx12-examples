#ifndef LAG_MEMORY_HPP
#define LAG_MEMORY_HPP

#include "../third_party/fluid_studios_memory_manager/mmgr.h"

#ifndef mem_alloc
#define mem_alloc(size) m_allocator(__FILE__, __LINE__, __FUNCTION__, m_alloc_malloc, size)
#endif

#ifndef mem_calloc
#define mem_calloc(size) m_allocator(__FILE__, __LINE__, __FUNCTION__, m_alloc_calloc, size)
#endif

#ifndef mem_realloc
#define mem_realloc(size) m_allocator(__FILE__, __LINE__, __FUNCTION__, m_alloc_realloc, size)
#endif

#ifndef mem_new
#define mem_new(size) m_allocator(__FILE__, __LINE__, __FUNCTION__, m_alloc_new, size)
#endif

#ifndef mem_new_array
#define mem_new_array(size) m_allocator(__FILE__, __LINE__, __FUNCTION__, m_alloc_new_array, size)
#endif

#ifndef mem_free
#define mem_free(ptr) m_deallocator(__FILE__, __LINE__, __FUNCTION__,m_alloc_free, ptr)
#endif

#ifndef mem_delete
#define mem_delete(ptr) m_deallocator(__FILE__, __LINE__, __FUNCTION__,m_alloc_delete, ptr)
#endif

#ifndef mem_delete_array
#define mem_delete_array(ptr) m_deallocator(__FILE__, __LINE__, __FUNCTION__,m_alloc_delete_array, ptr)
#endif


#endif // LAG_MEMORY_HPP