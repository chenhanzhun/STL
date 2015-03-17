/*tihs program is in the file of stl_alloc.h from line 588 to 628 */

template <class _Tp>
class allocator {
  typedef alloc _Alloc;          // The underlying allocator.
public:							//数据类型的成员变量在后续章节(traits编程技巧)介绍
  typedef size_t     size_type;
  typedef ptrdiff_t  difference_type;
  typedef _Tp*       pointer;
  typedef const _Tp* const_pointer;
  typedef _Tp&       reference;
  typedef const _Tp& const_reference;
  typedef _Tp        value_type;

  template <class _Tp1> struct rebind {//嵌套一个template,且仅包含唯一成员other,是一个typedef;
    typedef allocator<_Tp1> other;
  };
  //下面是成员函数
  allocator() __STL_NOTHROW {}	//默认构造函数,__STL_NOTHROW在 stl_config.h中定义,要么为空,要么为 throw()异常机制
  allocator(const allocator&) __STL_NOTHROW {}	//复制构造函数
  template <class _Tp1> allocator(const allocator<_Tp1>&) __STL_NOTHROW {}//泛化的复制构造函数
  ~allocator() __STL_NOTHROW {}//析构函数

  pointer address(reference __x) const { return &__x; }//返回对象的地址
  const_pointer address(const_reference __x) const { return &__x; }//返回const对象的地址

  // __n is permitted to be 0.  The C++ standard says nothing about what
  // the return value is when __n == 0.
  _Tp* allocate(size_type __n, const void* = 0) {// 配置空间，如果申请的空间块数不为0，那么调用 _Alloc 也即 alloc 的 allocate 函数来分配内存，
 //这里的 alloc 在 SGI STL 中默认使用的是__default_alloc_template<__NODE_ALLOCATOR_THREADS, 0>这个实现（见第402行）
    return __n != 0 ? static_cast<_Tp*>(_Alloc::allocate(__n * sizeof(_Tp))) 
                    : 0;
  }

  // __p is not permitted to be a null pointer.
  void deallocate(pointer __p, size_type __n)//释放已配置的空间
    { _Alloc::deallocate(__p, __n * sizeof(_Tp)); }

  size_type max_size() const __STL_NOTHROW //返回可成功配置的最大值
    { return size_t(-1) / sizeof(_Tp); }

  void construct(pointer __p, const _Tp& __val) { new(__p) _Tp(__val); }//构造，等同于new ((void*)p) T(x)
  void destroy(pointer __p) { __p->~_Tp(); }//析构，等同于p->~T()
};

//异常处理
/*tihs program is in the file of stl_alloc.h*/
//line 109 to 118
class __malloc_alloc_template {

private:
//内存不足异常处理
  static void* _S_oom_malloc(size_t);
  static void* _S_oom_realloc(void*, size_t);

#ifndef __STL_STATIC_TEMPLATE_MEMBER_BUG
  static void (* __malloc_alloc_oom_handler)();
#endif
  //line 141 to 146
  //指定自己的异常处理
  static void (* __set_malloc_handler(void (*__f)()))()
  {
    void (* __old)() = __malloc_alloc_oom_handler;
    __malloc_alloc_oom_handler = __f;
    return(__old);
  }
//line 152 to 155
#ifndef __STL_STATIC_TEMPLATE_MEMBER_BUG
template <int __inst>
void (* __malloc_alloc_template<__inst>::__malloc_alloc_oom_handler)() = 0;
#endif
//line 41 to 50
#ifndef __THROW_BAD_ALLOC
#  if defined(__STL_NO_BAD_ALLOC) || !defined(__STL_USE_EXCEPTIONS)
#    include <stdio.h>
#    include <stdlib.h>
//默认的强制退出
#    define __THROW_BAD_ALLOC fprintf(stderr, "out of memory\n"); exit(1)
#  else /* Standard conforming out-of-memory handling */
#    include <new>
//抛出用户设计异常处理例程
#    define __THROW_BAD_ALLOC throw std::bad_alloc()
#  endif
#endif


