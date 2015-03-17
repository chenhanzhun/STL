/*	deque是双向队列，可以在头尾两端插入或删除元素；
*	deque与vector最大的差异就是：
*	一、deque允许于常数时间内对头端进行插入或删除元素；
*	二、deque是分段连续线性空间，随时可以增加一段新的空间；
*	deque不像vector那样，vector当内存不够时，需重新分配/复制数据/释放原始空间；
*	不过deque的迭代器设置比vector复杂，因为迭代器不能使用普通指针，因此尽量使用vector；
*	注：当对deque的元素进行排序时，为了提高效率，首先把deque数据复制到vector，
*	利用vector的排序算法(利用STL的sort算法)，排序之后再次复制回deque； 
*/

#include <concept_checks.h>

#ifndef __SGI_STL_INTERNAL_DEQUE_H
#define __SGI_STL_INTERNAL_DEQUE_H

/* Class invariants:
 *  For any nonsingular iterator i:
 *    i.node is the address of an element in the map array.  The
 *      contents of i.node is a pointer to the beginning of a node.
 *    i.first == *(i.node) 
 *    i.last  == i.first + node_size
 *    i.cur is a pointer in the range [i.first, i.last).  NOTE:
 *      the implication of this is that i.cur is always a dereferenceable
 *      pointer, even if i is a past-the-end iterator.
 *  Start and Finish are always nonsingular iterators.  NOTE: this means
 *    that an empty deque must have one node, and that a deque
 *    with N elements, where N is the buffer size, must have two nodes.
 *  For every node other than start.node and finish.node, every element
 *    in the node is an initialized object.  If start.node == finish.node,
 *    then [start.cur, finish.cur) are initialized objects, and
 *    the elements outside that range are uninitialized storage.  Otherwise,
 *    [start.cur, start.last) and [finish.first, finish.cur) are initialized
 *    objects, and [start.first, start.cur) and [finish.cur, finish.last)
 *    are uninitialized storage.
 *  [map, map + map_size) is a valid, non-empty range.  
 *  [start.node, finish.node] is a valid range contained within 
 *    [map, map + map_size).  
 *  A pointer in the range [map, map + map_size) points to an allocated node
 *    if and only if the pointer is in the range [start.node, finish.node].
 */


/*
 * In previous versions of deque, there was an extra template 
 * parameter so users could control the node size.  This extension
 * turns out to violate the C++ standard (it can be detected using
 * template template parameters), and it has been removed.
 */

__STL_BEGIN_NAMESPACE 

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1174
#pragma set woff 1375
#endif

//*	deque是分段连续线性空间，为了能够像连续线性空间那样访问数据，
//*	deque采用了一种中控方法map,map是块连续的空间，其每个元素都是一个指针，
//*	指向一块缓冲区，实质上map是T**；
//***************************************************************

// Note: this function is simply a kludge to work around several compilers'
//  bugs in handling constant expressions.
//这个函数是方便不同编译器处理常量表达式的bug
inline size_t __deque_buf_size(size_t __size) {
  return __size < 512 ? size_t(512 / __size) : size_t(1);
}

//deque迭代器的设计
//*deque是分段连续的线性空间，迭代器设计时必须能够进行operator++或operator--操作
//*	迭代器的功能：
//*	1、必须知道缓冲区的位置
//*	2、能够判断是否处于其所在缓冲区的边界
//*	3、能够知道其所在缓冲区当前所指位置的元素
//********************************************
template <class _Tp, class _Ref, class _Ptr>
struct _Deque_iterator {
  typedef _Deque_iterator<_Tp, _Tp&, _Tp*>             iterator;
  typedef _Deque_iterator<_Tp, const _Tp&, const _Tp*> const_iterator;
  static size_t _S_buffer_size() { return __deque_buf_size(sizeof(_Tp)); }

  typedef random_access_iterator_tag iterator_category;
  typedef _Tp value_type;
  typedef _Ptr pointer;
  typedef _Ref reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef _Tp** _Map_pointer;

  typedef _Deque_iterator _Self;

  //以下是迭代器设计的关键，访问容器的节点
  _Tp* _M_cur;//指向缓冲区当前的元素
  _Tp* _M_first;//指向缓冲区的头(起始地址)
  _Tp* _M_last;//指向缓冲区的尾(结束地址)
  _Map_pointer _M_node;//指向中控器的相应节点

  _Deque_iterator(_Tp* __x, _Map_pointer __y) 
    : _M_cur(__x), _M_first(*__y),
      _M_last(*__y + _S_buffer_size()), _M_node(__y) {}
  _Deque_iterator() : _M_cur(0), _M_first(0), _M_last(0), _M_node(0) {}
  _Deque_iterator(const iterator& __x)
    : _M_cur(__x._M_cur), _M_first(__x._M_first), 
      _M_last(__x._M_last), _M_node(__x._M_node) {}

  //****************************************************************************
  //*********************以下是迭代器_Deque_iterator的操作**********************
  //*	这些操作符的重载方便我们访问容器的内容
  //*	也是让deque从接口看来是维护连续线性空间的关键
  //****************************************************************************
  reference operator*() const { return *_M_cur; }//解除引用,返回当前元素
#ifndef __SGI_STL_NO_ARROW_OPERATOR
  pointer operator->() const { return _M_cur; }
#endif /* __SGI_STL_NO_ARROW_OPERATOR */

  //返回两个迭代器之间的距离
  difference_type operator-(const _Self& __x) const {
    return difference_type(_S_buffer_size()) * (_M_node - __x._M_node - 1) +
      (_M_cur - _M_first) + (__x._M_last - __x._M_cur);
  }

  //前缀自增++操作符重载
  _Self& operator++() {
    ++_M_cur;					//普通自增操作，移至下一个元素
    if (_M_cur == _M_last) {   //若已达到缓冲区的尾部
      _M_set_node(_M_node + 1);//切换至下一缓冲区(节点)
      _M_cur = _M_first;	  //的第一个元素
    }
    return *this;
  }
  //后缀自增++操作符重载
  //返回当前迭代器的一个副本
  _Self operator++(int)  {
    _Self __tmp = *this;//定义当前迭代器的一个副本
    ++*this;//这里前缀++不是普通的++操作，是上一步骤已经重载过的前缀++
    return __tmp;
  }

  //前缀自减--操作符重载
  //基本思想类似于前缀自增操作
  _Self& operator--() {
    if (_M_cur == _M_first) {  //若是当前缓冲区的第一个元素
      _M_set_node(_M_node - 1);//切换到上一个缓冲区
      _M_cur = _M_last;		  //的尾部(即最后一个元素的下一个位置)
    }
    --_M_cur;//普通的自减操作，移至前一个元素
    return *this;
  }
  //后缀自减--操作符重载
  //返回当前迭代器的副本
  _Self operator--(int) {
    _Self __tmp = *this;//定义一个副本
    --*this;			//迭代器自减操作
    return __tmp;
  }

  //以下实现随机存取，迭代器可以直接跳跃n个距离
  //将迭代器前移n个距离，当n负值时就为下面的operator-=操作
  _Self& operator+=(difference_type __n)
  {
    difference_type __offset = __n + (_M_cur - _M_first);//定义一个中间变量
    if (__offset >= 0 && __offset < difference_type(_S_buffer_size()))
		//若前移n个距离后，目标依然在同一个缓冲区
		//则直接前移n个距离
      _M_cur += __n;
    else {
		//若前移n个距离后，目标超出该缓冲区范围
		//__offset / difference_type(_S_buffer_size())计算向后移动多少个缓冲区
		//-difference_type((-__offset - 1) / _S_buffer_size()) - 1计算向前移动多少个缓冲区
      difference_type __node_offset =
        __offset > 0 ? __offset / difference_type(_S_buffer_size())
                   : -difference_type((-__offset - 1) / _S_buffer_size()) - 1;
	  //调整到正确的缓冲区
      _M_set_node(_M_node + __node_offset);
	  //切换至正确的元素
      _M_cur = _M_first + 
        (__offset - __node_offset * difference_type(_S_buffer_size()));
    }
    return *this;
  }

  //操作符+重载
  //返回操作之后的副本
  _Self operator+(difference_type __n) const
  {
    _Self __tmp = *this;
	//调用operator+=操作
    return __tmp += __n;
  }

  //利用operator+=操作实现
  _Self& operator-=(difference_type __n) { return *this += -__n; }
 
  _Self operator-(difference_type __n) const {
    _Self __tmp = *this;
    return __tmp -= __n;
  }

  //返回指定位置的元素，即实现随机存取
  //该函数调用operator+，operator*
  reference operator[](difference_type __n) const { return *(*this + __n); }

  bool operator==(const _Self& __x) const { return _M_cur == __x._M_cur; }
  bool operator!=(const _Self& __x) const { return !(*this == __x); }
  bool operator<(const _Self& __x) const {
    return (_M_node == __x._M_node) ? 
      (_M_cur < __x._M_cur) : (_M_node < __x._M_node);
  }
  bool operator>(const _Self& __x) const  { return __x < *this; }
  bool operator<=(const _Self& __x) const { return !(__x < *this); }
  bool operator>=(const _Self& __x) const { return !(*this < __x); }

  //调整到正确的缓冲区
  //切换到正确的元素位置
  void _M_set_node(_Map_pointer __new_node) {
    _M_node = __new_node;//指向新的节点
    _M_first = *__new_node;//指向新节点的头部
    _M_last = _M_first + difference_type(_S_buffer_size());//指向新节点的尾部
  }
};

//operator+迭代器向前移动n个位置
template <class _Tp, class _Ref, class _Ptr>
inline _Deque_iterator<_Tp, _Ref, _Ptr>
operator+(ptrdiff_t __n, const _Deque_iterator<_Tp, _Ref, _Ptr>& __x)
{
  return __x + __n;
}

#ifndef __STL_CLASS_PARTIAL_SPECIALIZATION

template <class _Tp, class _Ref, class _Ptr>
inline random_access_iterator_tag
iterator_category(const _Deque_iterator<_Tp,_Ref,_Ptr>&)
{
  return random_access_iterator_tag();
}

template <class _Tp, class _Ref, class _Ptr>
inline _Tp* value_type(const _Deque_iterator<_Tp,_Ref,_Ptr>&) { return 0; }

template <class _Tp, class _Ref, class _Ptr>
inline ptrdiff_t* distance_type(const _Deque_iterator<_Tp,_Ref,_Ptr>&) {
  return 0;
}

#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

// Deque base class.  It has two purposes.  First, its constructor
//  and destructor allocate (but don't initialize) storage.  This makes
//  exception safety easier.  Second, the base class encapsulates all of
//  the differences between SGI-style allocators and standard-conforming
//  allocators.

#ifdef __STL_USE_STD_ALLOCATORS

// Base class for ordinary allocators.
//这里只负责中控器节点内存管理
template <class _Tp, class _Alloc, bool __is_static>
class _Deque_alloc_base {
public:
  typedef typename _Alloc_traits<_Tp,_Alloc>::allocator_type allocator_type;
  allocator_type get_allocator() const { return _M_node_allocator; }

  _Deque_alloc_base(const allocator_type& __a)
    : _M_node_allocator(__a), _M_map_allocator(__a),
      _M_map(0), _M_map_size(0)
  {}
  
protected:
  typedef typename _Alloc_traits<_Tp*, _Alloc>::allocator_type
          _Map_allocator_type;

  allocator_type      _M_node_allocator;
  _Map_allocator_type _M_map_allocator;

  _Tp* _M_allocate_node() {
    return _M_node_allocator.allocate(__deque_buf_size(sizeof(_Tp)));
  }
  void _M_deallocate_node(_Tp* __p) {
    _M_node_allocator.deallocate(__p, __deque_buf_size(sizeof(_Tp)));
  }
  _Tp** _M_allocate_map(size_t __n) 
    { return _M_map_allocator.allocate(__n); }
  void _M_deallocate_map(_Tp** __p, size_t __n) 
    { _M_map_allocator.deallocate(__p, __n); }

  _Tp** _M_map; //指向中控器map
  size_t _M_map_size;//中控器map可容纳指针的个数
};

// Specialization for instanceless allocators.
template <class _Tp, class _Alloc>
class _Deque_alloc_base<_Tp, _Alloc, true>
{
public:
  typedef typename _Alloc_traits<_Tp,_Alloc>::allocator_type allocator_type;
  allocator_type get_allocator() const { return allocator_type(); }

  _Deque_alloc_base(const allocator_type&) : _M_map(0), _M_map_size(0) {}
  
protected:
  typedef typename _Alloc_traits<_Tp, _Alloc>::_Alloc_type _Node_alloc_type;
  typedef typename _Alloc_traits<_Tp*, _Alloc>::_Alloc_type _Map_alloc_type;

  _Tp* _M_allocate_node() {
    return _Node_alloc_type::allocate(__deque_buf_size(sizeof(_Tp)));
  }
  void _M_deallocate_node(_Tp* __p) {
    _Node_alloc_type::deallocate(__p, __deque_buf_size(sizeof(_Tp)));
  }
  _Tp** _M_allocate_map(size_t __n) 
    { return _Map_alloc_type::allocate(__n); }
  void _M_deallocate_map(_Tp** __p, size_t __n) 
    { _Map_alloc_type::deallocate(__p, __n); }

  _Tp** _M_map;
  size_t _M_map_size;
};

template <class _Tp, class _Alloc>
class _Deque_base
  : public _Deque_alloc_base<_Tp,_Alloc,
                              _Alloc_traits<_Tp, _Alloc>::_S_instanceless>
{
public:
  typedef _Deque_alloc_base<_Tp,_Alloc,
                             _Alloc_traits<_Tp, _Alloc>::_S_instanceless>
          _Base;
  typedef typename _Base::allocator_type allocator_type;
  typedef _Deque_iterator<_Tp,_Tp&,_Tp*>             iterator;
  typedef _Deque_iterator<_Tp,const _Tp&,const _Tp*> const_iterator;

  //参数__num_elements表示缓冲区(节点)存储元素的个数
  _Deque_base(const allocator_type& __a, size_t __num_elements)
    : _Base(__a), _M_start(), _M_finish()
    { _M_initialize_map(__num_elements); }
  _Deque_base(const allocator_type& __a) 
    : _Base(__a), _M_start(), _M_finish() {}
  ~_Deque_base();    

protected:
  void _M_initialize_map(size_t);
  void _M_create_nodes(_Tp** __nstart, _Tp** __nfinish);
  void _M_destroy_nodes(_Tp** __nstart, _Tp** __nfinish);
  enum { _S_initial_map_size = 8 };//中控器map默认大小

protected:
  iterator _M_start;//指向第一个缓冲区的第一个元素
  iterator _M_finish;//指向最后一个缓冲区的最后一个元素的下一个位置
};

#else /* __STL_USE_STD_ALLOCATORS */

template <class _Tp, class _Alloc>
class _Deque_base {
public:
  typedef _Deque_iterator<_Tp,_Tp&,_Tp*>             iterator;
  typedef _Deque_iterator<_Tp,const _Tp&,const _Tp*> const_iterator;

  typedef _Alloc allocator_type;
  allocator_type get_allocator() const { return allocator_type(); }

  _Deque_base(const allocator_type&, size_t __num_elements)
    : _M_map(0), _M_map_size(0),  _M_start(), _M_finish() {
    _M_initialize_map(__num_elements);
  }
  _Deque_base(const allocator_type&)
    : _M_map(0), _M_map_size(0),  _M_start(), _M_finish() {}
  ~_Deque_base();    

protected:
  void _M_initialize_map(size_t);
  void _M_create_nodes(_Tp** __nstart, _Tp** __nfinish);
  void _M_destroy_nodes(_Tp** __nstart, _Tp** __nfinish);
  enum { _S_initial_map_size = 8 };

protected:
  _Tp** _M_map;
  size_t _M_map_size;  
  iterator _M_start;
  iterator _M_finish;

  typedef simple_alloc<_Tp, _Alloc>  _Node_alloc_type;
  typedef simple_alloc<_Tp*, _Alloc> _Map_alloc_type;

  _Tp* _M_allocate_node()
    { return _Node_alloc_type::allocate(__deque_buf_size(sizeof(_Tp))); }
  void _M_deallocate_node(_Tp* __p)
    { _Node_alloc_type::deallocate(__p, __deque_buf_size(sizeof(_Tp))); }
  _Tp** _M_allocate_map(size_t __n) 
    { return _Map_alloc_type::allocate(__n); }
  void _M_deallocate_map(_Tp** __p, size_t __n) 
    { _Map_alloc_type::deallocate(__p, __n); }
};

#endif /* __STL_USE_STD_ALLOCATORS */

// Non-inline member functions from _Deque_base.

template <class _Tp, class _Alloc>
_Deque_base<_Tp,_Alloc>::~_Deque_base() {
  if (_M_map) {
    _M_destroy_nodes(_M_start._M_node, _M_finish._M_node + 1);
    _M_deallocate_map(_M_map, _M_map_size);
  }
}

template <class _Tp, class _Alloc>
void
_Deque_base<_Tp,_Alloc>::_M_initialize_map(size_t __num_elements)
{
   //根据存储元素个数和缓冲区大小决定中控器map容量
	size_t __num_nodes = 
		__num_elements / __deque_buf_size(sizeof(_Tp)) + 1;

  //在默认值和计算值中，取其最大者
	//map管理的节点，最少是8个，最多是所需节点数加上2个
	//前后各预留一个，方便扩充
  _M_map_size = max((size_t) _S_initial_map_size, __num_nodes + 2);
  _M_map = _M_allocate_map(_M_map_size);//分配map内存空间

  //让start.node和finish.node指向map的内部，方便map的两端扩充
  _Tp** __nstart = _M_map + (_M_map_size - __num_nodes) / 2;
  _Tp** __nfinish = __nstart + __num_nodes;
    
  __STL_TRY {
    _M_create_nodes(__nstart, __nfinish);//分配map实际存储的节点数空间
  }
  __STL_UNWIND((_M_deallocate_map(_M_map, _M_map_size), 
                _M_map = 0, _M_map_size = 0));
  _M_start._M_set_node(__nstart);//使迭代器start指向正确的位置
  _M_finish._M_set_node(__nfinish - 1);//使迭代器start指向正确的位置
  _M_start._M_cur = _M_start._M_first;//初始化start.cur,使其指向第一个缓冲区的第一个元素
  //初始化finish.cur,使其指向最后一个缓冲区的最后一个元素
  _M_finish._M_cur = _M_finish._M_first +
               __num_elements % __deque_buf_size(sizeof(_Tp));
}

//分配map实际存储的节点数空间
template <class _Tp, class _Alloc>
void _Deque_base<_Tp,_Alloc>::_M_create_nodes(_Tp** __nstart, _Tp** __nfinish)
{
  _Tp** __cur;
  __STL_TRY {
    for (__cur = __nstart; __cur < __nfinish; ++__cur)
      *__cur = _M_allocate_node();
  }
  __STL_UNWIND(_M_destroy_nodes(__nstart, __cur));
}
//释放map的节点数空间
template <class _Tp, class _Alloc>
void
_Deque_base<_Tp,_Alloc>::_M_destroy_nodes(_Tp** __nstart, _Tp** __nfinish)
{
  for (_Tp** __n = __nstart; __n < __nfinish; ++__n)
    _M_deallocate_node(*__n);
}

//deque容器的定义
//配置器默认为第二级配置器
template <class _Tp, class _Alloc = __STL_DEFAULT_ALLOCATOR(_Tp) >
class deque : protected _Deque_base<_Tp, _Alloc> {

  // requirements:

  __STL_CLASS_REQUIRES(_Tp, _Assignable);

  typedef _Deque_base<_Tp, _Alloc> _Base;
public:                         // Basic types
  typedef _Tp value_type;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

  typedef typename _Base::allocator_type allocator_type;
  allocator_type get_allocator() const { return _Base::get_allocator(); }

public:                         // Iterators
  typedef typename _Base::iterator       iterator;
  typedef typename _Base::const_iterator const_iterator;

#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION
  typedef reverse_iterator<const_iterator> const_reverse_iterator;
  typedef reverse_iterator<iterator> reverse_iterator;
#else /* __STL_CLASS_PARTIAL_SPECIALIZATION */
  typedef reverse_iterator<const_iterator, value_type, const_reference, 
                           difference_type>  
          const_reverse_iterator;
  typedef reverse_iterator<iterator, value_type, reference, difference_type>
          reverse_iterator; 
#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

protected:                      // Internal typedefs
  typedef pointer* _Map_pointer;
  static size_t _S_buffer_size() { return __deque_buf_size(sizeof(_Tp)); }

protected:
	//继承基类的成员
#ifdef __STL_USE_NAMESPACES
  using _Base::_M_initialize_map;
  using _Base::_M_create_nodes;
  using _Base::_M_destroy_nodes;
  using _Base::_M_allocate_node;
  using _Base::_M_deallocate_node;
  using _Base::_M_allocate_map;
  using _Base::_M_deallocate_map;

  //下面是deque的数据结构
  using _Base::_M_map;
  using _Base::_M_map_size;
  using _Base::_M_start;
  using _Base::_M_finish;
#endif /* __STL_USE_NAMESPACES */

public:                         // Basic accessors
  iterator begin() { return _M_start; }
  iterator end() { return _M_finish; }
  const_iterator begin() const { return _M_start; }
  const_iterator end() const { return _M_finish; }

  reverse_iterator rbegin() { return reverse_iterator(_M_finish); }
  reverse_iterator rend() { return reverse_iterator(_M_start); }
  const_reverse_iterator rbegin() const 
    { return const_reverse_iterator(_M_finish); }
  const_reverse_iterator rend() const 
    { return const_reverse_iterator(_M_start); }

  //随机访问，这里调用迭代器的重载操作operator[]
  reference operator[](size_type __n)
    { return _M_start[difference_type(__n)]; }
  const_reference operator[](size_type __n) const 
    { return _M_start[difference_type(__n)]; }

//越界检查
#ifdef __STL_THROW_RANGE_ERRORS
  void _M_range_check(size_type __n) const {
    if (__n >= this->size())
      __stl_throw_range_error("deque");
  }

  //访问指定位置的元素
  reference at(size_type __n)
    { _M_range_check(__n); return (*this)[__n]; }
  const_reference at(size_type __n) const
    { _M_range_check(__n); return (*this)[__n]; }
#endif /* __STL_THROW_RANGE_ERRORS */

  //返回第一个缓冲区的第一个元素
  //调用迭代器的operator*
  reference front() { return *_M_start; }
  //返回最后一个缓冲区的最后一个元素
  reference back() {
    iterator __tmp = _M_finish;
    --__tmp;//调用迭代器的operator--
    return *__tmp;//调用迭代器的operator*
  }
  const_reference front() const { return *_M_start; }
  const_reference back() const {
    const_iterator __tmp = _M_finish;
    --__tmp;
    return *__tmp;
  }

  //返回deque容器大小，即容器元素个数
  //调用迭代器的operator-
  size_type size() const { return _M_finish - _M_start; }
  size_type max_size() const { return size_type(-1); }
  //判断容器是否为空
  /*Start and Finish are always nonsingular iterators.  NOTE: this means
   *    that an empty deque must have one node
   */
  bool empty() const { return _M_finish == _M_start; }

public:                         
	//**********************************************************
	//*	以下是构造函数和析构函数
	//*	默认构造函数
	//*	explicit deque( const Allocator& alloc = Allocator() );
	//*	容器大小为count个初始值为value的元素
	//*	explicit deque( size_type count,
    //*            const T& value = T(),
    //*            const Allocator& alloc = Allocator());
    //*     deque( size_type count,
    //*            const T& value,
    //*            const Allocator& alloc = Allocator());
	//*	容器大小为count个默认值的元素
	//*	explicit deque( size_type count );
	//*	拷贝构造函数
	//*	deque( const deque& other );
	//*	deque( const deque& other, const Allocator& alloc );
	//*	初始值为[first,last)内容的容器
	//*	template< class InputIt >
	//*	deque( InputIt first, InputIt last,
	//*			const Allocator& alloc = Allocator() );	
	//*	析构函数
	//*	~deque()
	//*
	//********************************************************
									// Constructor, destructor.
  explicit deque(const allocator_type& __a = allocator_type()) 
    : _Base(__a, 0) {}
  deque(const deque& __x) : _Base(__x.get_allocator(), __x.size()) 
    { uninitialized_copy(__x.begin(), __x.end(), _M_start); }
  deque(size_type __n, const value_type& __value,
        const allocator_type& __a = allocator_type()) : _Base(__a, __n)
    { _M_fill_initialize(__value); }
  explicit deque(size_type __n) : _Base(allocator_type(), __n)
    { _M_fill_initialize(value_type()); }

#ifdef __STL_MEMBER_TEMPLATES

  // Check whether it's an integral type.  If so, it's not an iterator.
  template <class _InputIterator>
  deque(_InputIterator __first, _InputIterator __last,
        const allocator_type& __a = allocator_type()) : _Base(__a) {
    typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
    _M_initialize_dispatch(__first, __last, _Integral());
  }

  template <class _Integer>
  void _M_initialize_dispatch(_Integer __n, _Integer __x, __true_type) {
    _M_initialize_map(__n);
    _M_fill_initialize(__x);
  }

  template <class _InputIter>
  void _M_initialize_dispatch(_InputIter __first, _InputIter __last,
                              __false_type) {
    _M_range_initialize(__first, __last, __ITERATOR_CATEGORY(__first));
  }

#else /* __STL_MEMBER_TEMPLATES */

  deque(const value_type* __first, const value_type* __last,
        const allocator_type& __a = allocator_type()) 
    : _Base(__a, __last - __first)
    { uninitialized_copy(__first, __last, _M_start); }
  deque(const_iterator __first, const_iterator __last,
        const allocator_type& __a = allocator_type()) 
    : _Base(__a, __last - __first)
    { uninitialized_copy(__first, __last, _M_start); }

#endif /* __STL_MEMBER_TEMPLATES */

  ~deque() { destroy(_M_start, _M_finish); }
  //构造函数和析构函数定义结束

 //容器对象赋值操作
  deque& operator= (const deque& __x) {
    const size_type __len = size();
    if (&__x != this) {
      if (__len >= __x.size())
        erase(copy(__x.begin(), __x.end(), _M_start), _M_finish);
      else {
        const_iterator __mid = __x.begin() + difference_type(__len);
        copy(__x.begin(), __mid, _M_start);
        insert(_M_finish, __mid, __x.end());
      }
    }
    return *this;
  }        

  //交换两个容器的内容
  //这里实际上并没有交换具体的元素，而是交换迭代器
  //和中控器信息
  void swap(deque& __x) {
    __STD::swap(_M_start, __x._M_start);
    __STD::swap(_M_finish, __x._M_finish);
    __STD::swap(_M_map, __x._M_map);
    __STD::swap(_M_map_size, __x._M_map_size);
  }

public: 
  // assign(), a generalized assignment member function.  Two
  // versions: one that takes a count, and one that takes a range.
  // The range version is a member template, so we dispatch on whether
  // or not the type is an integer.

  void _M_fill_assign(size_type __n, const _Tp& __val) {
    if (__n > size()) {//若新的大小比容器原来的大
      fill(begin(), end(), __val);//把原来容器填充为val
      insert(end(), __n - size(), __val);//容器追加填充值
    }
    else {//若新的大小比容器原来的小
      erase(begin() + __n, end());//则擦除多余的内容
      fill(begin(), end(), __val);//填充值为val覆盖原来的内容
    }
  }
  //对外接口的assign第一个版本
  void assign(size_type __n, const _Tp& __val) {
    _M_fill_assign(__n, __val);
  }

#ifdef __STL_MEMBER_TEMPLATES
  //对外接口的assign第二个版本
  //采用迭代器的traist技术
  template <class _InputIterator>
  void assign(_InputIterator __first, _InputIterator __last) {
    typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
    _M_assign_dispatch(__first, __last, _Integral());
  }

private:                        // helper functions for assign() 

  template <class _Integer>
  void _M_assign_dispatch(_Integer __n, _Integer __val, __true_type)
    { _M_fill_assign((size_type) __n, (_Tp) __val); }

  template <class _InputIterator>
  void _M_assign_dispatch(_InputIterator __first, _InputIterator __last,
                          __false_type) {
    _M_assign_aux(__first, __last, __ITERATOR_CATEGORY(__first));
  }

  template <class _InputIterator>
  void _M_assign_aux(_InputIterator __first, _InputIterator __last,
                     input_iterator_tag);

  template <class _ForwardIterator>
  void _M_assign_aux(_ForwardIterator __first, _ForwardIterator __last,
                     forward_iterator_tag) {
    size_type __len = 0;
    distance(__first, __last, __len);//计算两个输入迭代器的距离
    if (__len > size()) {//若输入迭代器距离比待赋值容器大小还大
      _ForwardIterator __mid = __first;
      advance(__mid, size());//将mid向前移动size()个距离
      copy(__first, __mid, begin());//先复制[first,mid)数据
      insert(end(), __mid, __last);//插入剩下的数据
    }
    else//否则擦除多余的数据
      erase(copy(__first, __last, begin()), end());
  }

#endif /* __STL_MEMBER_TEMPLATES */


public:                         // push_* and pop_*
  
  //在容器尾部加数据
	void push_back(const value_type& __t) {
    //若当前缓冲区存在可用空间
	if (_M_finish._M_cur != _M_finish._M_last - 1) {
      construct(_M_finish._M_cur, __t);//直接构造对象
      ++_M_finish._M_cur;//调整指针所指位置
    }
    else//若当前缓冲区不存在可用空间
		//需分配一段新的连续空间
      _M_push_back_aux(__t);
  }

  void push_back() {
    if (_M_finish._M_cur != _M_finish._M_last - 1) {
      construct(_M_finish._M_cur);
      ++_M_finish._M_cur;
    }
    else
      _M_push_back_aux();
  }

  void push_front(const value_type& __t) {
    if (_M_start._M_cur != _M_start._M_first) {
      construct(_M_start._M_cur - 1, __t);
      --_M_start._M_cur;
    }
    else
      _M_push_front_aux(__t);
  }

  void push_front() {
    if (_M_start._M_cur != _M_start._M_first) {
      construct(_M_start._M_cur - 1);
      --_M_start._M_cur;
    }
    else
      _M_push_front_aux();
  }


  void pop_back() {
    if (_M_finish._M_cur != _M_finish._M_first) {
      --_M_finish._M_cur;
      destroy(_M_finish._M_cur);
    }
    else
      _M_pop_back_aux();
  }

  void pop_front() {
    if (_M_start._M_cur != _M_start._M_last - 1) {
      destroy(_M_start._M_cur);
      ++_M_start._M_cur;
    }
    else 
      _M_pop_front_aux();
  }


public:             
	//***********************************************************
	/*	在指定位置之前插入数据
	*************************************************************
	*	iterator insert (iterator position, const value_type& val);
	*
    *	void insert (iterator position, size_type n, const value_type& val);
	*
	*	template <class InputIterator>
    *	void insert (iterator position, InputIterator first, InputIterator last);
	*/
	//***********************************************************
													// Insert

  iterator insert(iterator position, const value_type& __x) {
    if (position._M_cur == _M_start._M_cur) {//若当前位置为deque.begin()
      push_front(__x);//则在容器头部插入数据
      return _M_start;
    }
    else if (position._M_cur == _M_finish._M_cur) {//若当前位置为deque.end()
      push_back(__x);
      iterator __tmp = _M_finish;
      --__tmp;
      return __tmp;
    }
    else {//否则在容器直接插入数据
      return _M_insert_aux(position, __x);
    }
  }

  iterator insert(iterator __position)
    { return insert(__position, value_type()); }

  void insert(iterator __pos, size_type __n, const value_type& __x)
    { _M_fill_insert(__pos, __n, __x); }

  void _M_fill_insert(iterator __pos, size_type __n, const value_type& __x); 

#ifdef __STL_MEMBER_TEMPLATES  

  // Check whether it's an integral type.  If so, it's not an iterator.
  template <class _InputIterator>
  void insert(iterator __pos, _InputIterator __first, _InputIterator __last) {
    typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
    _M_insert_dispatch(__pos, __first, __last, _Integral());
  }

  template <class _Integer>
  void _M_insert_dispatch(iterator __pos, _Integer __n, _Integer __x,
                          __true_type) {
    _M_fill_insert(__pos, (size_type) __n, (value_type) __x);
  }

  template <class _InputIterator>
  void _M_insert_dispatch(iterator __pos,
                          _InputIterator __first, _InputIterator __last,
                          __false_type) {
    insert(__pos, __first, __last, __ITERATOR_CATEGORY(__first));
  }

#else /* __STL_MEMBER_TEMPLATES */

  void insert(iterator __pos,
              const value_type* __first, const value_type* __last);
  void insert(iterator __pos,
              const_iterator __first, const_iterator __last);

#endif /* __STL_MEMBER_TEMPLATES */

  //改变容器大小
  void resize(size_type __new_size, const value_type& __x) {
    const size_type __len = size();
    if (__new_size < __len)//新的大小较旧的小 
      erase(_M_start + __new_size, _M_finish);//擦除多余的元素
    else//否则，填充新增的节点
      insert(_M_finish, __new_size - __len, __x);
  }

  void resize(size_type new_size) { resize(new_size, value_type()); }

public:          
		//********************************************************
		/*	擦除指定位置的元素
		*	iterator erase (iterator position);
		*	擦除指定区间的元素
		*	iterator erase (iterator first, iterator last);
		*/
		//********************************************************
										// Erase
  iterator erase(iterator __pos) {
    iterator __next = __pos;
    ++__next;
    difference_type __index = __pos - _M_start;//擦除点之前元素个数
    if (size_type(__index) < (this->size() >> 1)) {//若擦除点之前的元素个数较少
      copy_backward(_M_start, __pos, __next);//向后移动擦除点之前的元素
      pop_front();
    }
    else {
      copy(__next, _M_finish, __pos);//否则，向前移动擦除点之后的元素
      pop_back();
    }
    return _M_start + __index;
  }

  iterator erase(iterator __first, iterator __last);
  void clear(); 

protected:                        // Internal construction/destruction

  void _M_fill_initialize(const value_type& __value);

#ifdef __STL_MEMBER_TEMPLATES  

  template <class _InputIterator>
  void _M_range_initialize(_InputIterator __first, _InputIterator __last,
                        input_iterator_tag);

  template <class _ForwardIterator>
  void _M_range_initialize(_ForwardIterator __first, _ForwardIterator __last,
                        forward_iterator_tag);

#endif /* __STL_MEMBER_TEMPLATES */

protected:                        // Internal push_* and pop_*

  void _M_push_back_aux(const value_type&);
  void _M_push_back_aux();
  void _M_push_front_aux(const value_type&);
  void _M_push_front_aux();
  void _M_pop_back_aux();
  void _M_pop_front_aux();

protected:                        // Internal insert functions

#ifdef __STL_MEMBER_TEMPLATES  

  template <class _InputIterator>
  void insert(iterator __pos, _InputIterator __first, _InputIterator __last,
              input_iterator_tag);

  template <class _ForwardIterator>
  void insert(iterator __pos,
              _ForwardIterator __first, _ForwardIterator __last,
              forward_iterator_tag);

#endif /* __STL_MEMBER_TEMPLATES */

  iterator _M_insert_aux(iterator __pos, const value_type& __x);
  iterator _M_insert_aux(iterator __pos);
  void _M_insert_aux(iterator __pos, size_type __n, const value_type& __x);

#ifdef __STL_MEMBER_TEMPLATES  

  template <class _ForwardIterator>
  void _M_insert_aux(iterator __pos, 
                     _ForwardIterator __first, _ForwardIterator __last,
                     size_type __n);

#else /* __STL_MEMBER_TEMPLATES */
  
  void _M_insert_aux(iterator __pos,
                     const value_type* __first, const value_type* __last,
                     size_type __n);

  void _M_insert_aux(iterator __pos, 
                     const_iterator __first, const_iterator __last,
                     size_type __n);
 
#endif /* __STL_MEMBER_TEMPLATES */

  iterator _M_reserve_elements_at_front(size_type __n) {
    size_type __vacancies = _M_start._M_cur - _M_start._M_first;
    if (__n > __vacancies) 
      _M_new_elements_at_front(__n - __vacancies);
    return _M_start - difference_type(__n);
  }

  iterator _M_reserve_elements_at_back(size_type __n) {
    size_type __vacancies = (_M_finish._M_last - _M_finish._M_cur) - 1;
    if (__n > __vacancies)
      _M_new_elements_at_back(__n - __vacancies);
    return _M_finish + difference_type(__n);
  }

  void _M_new_elements_at_front(size_type __new_elements);
  void _M_new_elements_at_back(size_type __new_elements);

protected:                      // Allocation of _M_map and nodes

  // Makes sure the _M_map has space for new nodes.  Does not actually
  //  add the nodes.  Can invalidate _M_map pointers.  (And consequently, 
  //  deque iterators.)

  void _M_reserve_map_at_back (size_type __nodes_to_add = 1) {
	  //_M_map_size - (_M_finish._M_node - _M_map)表示map尾端节点的备用空间
    if (__nodes_to_add + 1 > _M_map_size - (_M_finish._M_node - _M_map))
      _M_reallocate_map(__nodes_to_add, false);
  }

  void _M_reserve_map_at_front (size_type __nodes_to_add = 1) {
	   //_M_start._M_node - _M_map表示map头部节点的备用空间
    if (__nodes_to_add > size_type(_M_start._M_node - _M_map))
      _M_reallocate_map(__nodes_to_add, true);
  }

  void _M_reallocate_map(size_type __nodes_to_add, bool __add_at_front);
};

// Non-inline member functions

#ifdef __STL_MEMBER_TEMPLATES

template <class _Tp, class _Alloc>
template <class _InputIter>
void deque<_Tp, _Alloc>
  ::_M_assign_aux(_InputIter __first, _InputIter __last, input_iterator_tag)
{
  iterator __cur = begin();
  for ( ; __first != __last && __cur != end(); ++__cur, ++__first)
    *__cur = *__first;
  if (__first == __last)//若当前容器比输入迭代器距离大
    erase(__cur, end());//擦除多余的数据
  else//否则插入剩下的数据
    insert(end(), __first, __last);
}

#endif /* __STL_MEMBER_TEMPLATES */

template <class _Tp, class _Alloc>
void deque<_Tp, _Alloc>::_M_fill_insert(iterator __pos,
                                        size_type __n, const value_type& __x)
{
  if (__pos._M_cur == _M_start._M_cur) {//若插入点在容器头部
    iterator __new_start = _M_reserve_elements_at_front(__n);
    __STL_TRY {
      uninitialized_fill(__new_start, _M_start, __x);
      _M_start = __new_start;
    }
    __STL_UNWIND(_M_destroy_nodes(__new_start._M_node, _M_start._M_node));
  }
  else if (__pos._M_cur == _M_finish._M_cur) {//若插入点在容器尾部
    iterator __new_finish = _M_reserve_elements_at_back(__n);
    __STL_TRY {
      uninitialized_fill(_M_finish, __new_finish, __x);
      _M_finish = __new_finish;
    }
    __STL_UNWIND(_M_destroy_nodes(_M_finish._M_node + 1, 
                                  __new_finish._M_node + 1));    
  }
  else 
    _M_insert_aux(__pos, __n, __x);
}

#ifndef __STL_MEMBER_TEMPLATES  

template <class _Tp, class _Alloc>
void deque<_Tp, _Alloc>::insert(iterator __pos,
                                const value_type* __first,
                                const value_type* __last) {
  size_type __n = __last - __first;
  if (__pos._M_cur == _M_start._M_cur) {
    iterator __new_start = _M_reserve_elements_at_front(__n);
    __STL_TRY {
      uninitialized_copy(__first, __last, __new_start);
      _M_start = __new_start;
    }
    __STL_UNWIND(_M_destroy_nodes(__new_start._M_node, _M_start._M_node));
  }
  else if (__pos._M_cur == _M_finish._M_cur) {
    iterator __new_finish = _M_reserve_elements_at_back(__n);
    __STL_TRY {
      uninitialized_copy(__first, __last, _M_finish);
      _M_finish = __new_finish;
    }
    __STL_UNWIND(_M_destroy_nodes(_M_finish._M_node + 1, 
                                  __new_finish._M_node + 1));
  }
  else
    _M_insert_aux(__pos, __first, __last, __n);
}

template <class _Tp, class _Alloc>
void deque<_Tp,_Alloc>::insert(iterator __pos,
                               const_iterator __first, const_iterator __last)
{
  size_type __n = __last - __first;
  if (__pos._M_cur == _M_start._M_cur) {
    iterator __new_start = _M_reserve_elements_at_front(__n);
    __STL_TRY {
      uninitialized_copy(__first, __last, __new_start);
      _M_start = __new_start;
    }
    __STL_UNWIND(_M_destroy_nodes(__new_start._M_node, _M_start._M_node));
  }
  else if (__pos._M_cur == _M_finish._M_cur) {
    iterator __new_finish = _M_reserve_elements_at_back(__n);
    __STL_TRY {
      uninitialized_copy(__first, __last, _M_finish);
      _M_finish = __new_finish;
    }
    __STL_UNWIND(_M_destroy_nodes(_M_finish._M_node + 1, 
                 __new_finish._M_node + 1));
  }
  else
    _M_insert_aux(__pos, __first, __last, __n);
}

#endif /* __STL_MEMBER_TEMPLATES */

template <class _Tp, class _Alloc>
typename deque<_Tp,_Alloc>::iterator 
deque<_Tp,_Alloc>::erase(iterator __first, iterator __last)
{
  if (__first == _M_start && __last == _M_finish) {//若擦除整个容器内容
    clear();
    return _M_finish;
  }
  else {
    difference_type __n = __last - __first;
    difference_type __elems_before = __first - _M_start;
    if (__elems_before < difference_type((this->size() - __n) / 2)) {
      copy_backward(_M_start, __first, __last);
      iterator __new_start = _M_start + __n;
      destroy(_M_start, __new_start);
      _M_destroy_nodes(__new_start._M_node, _M_start._M_node);
      _M_start = __new_start;
    }
    else {
      copy(__last, _M_finish, __first);
      iterator __new_finish = _M_finish - __n;
      destroy(__new_finish, _M_finish);
      _M_destroy_nodes(__new_finish._M_node + 1, _M_finish._M_node + 1);
      _M_finish = __new_finish;
    }
    return _M_start + __elems_before;
  }
}

//清空容器内容
template <class _Tp, class _Alloc> 
void deque<_Tp,_Alloc>::clear()
{
  //首先清空缓冲区是满数据的节点
	for (_Map_pointer __node = _M_start._M_node + 1;
       __node < _M_finish._M_node;
       ++__node) {
    destroy(*__node, *__node + _S_buffer_size());
    _M_deallocate_node(*__node);
  }

 //清除缓冲区还存在可用空间的节点，即只存储部分数据
	if (_M_start._M_node != _M_finish._M_node) {
    destroy(_M_start._M_cur, _M_start._M_last);
    destroy(_M_finish._M_first, _M_finish._M_cur);
    _M_deallocate_node(_M_finish._M_first);
  }
  else
    destroy(_M_start._M_cur, _M_finish._M_cur);

  _M_finish = _M_start;//表示容器为空
}

// Precondition: _M_start and _M_finish have already been initialized,
// but none of the deque's elements have yet been constructed.
template <class _Tp, class _Alloc>
void deque<_Tp,_Alloc>::_M_fill_initialize(const value_type& __value) {
  _Map_pointer __cur;
  __STL_TRY {//为每个节点的缓冲区设置初值
    for (__cur = _M_start._M_node; __cur < _M_finish._M_node; ++__cur)
      uninitialized_fill(*__cur, *__cur + _S_buffer_size(), __value);
	//最后一个缓冲区必须单独设置初值，因为可能存在可用空间
    uninitialized_fill(_M_finish._M_first, _M_finish._M_cur, __value);
  }
  __STL_UNWIND(destroy(_M_start, iterator(*__cur, __cur)));
}

#ifdef __STL_MEMBER_TEMPLATES  

template <class _Tp, class _Alloc> template <class _InputIterator>
void deque<_Tp,_Alloc>::_M_range_initialize(_InputIterator __first,
                                            _InputIterator __last,
                                            input_iterator_tag)
{
  _M_initialize_map(0);
  __STL_TRY {
    for ( ; __first != __last; ++__first)
      push_back(*__first);
  }
  __STL_UNWIND(clear());
}

template <class _Tp, class _Alloc> template <class _ForwardIterator>
void deque<_Tp,_Alloc>::_M_range_initialize(_ForwardIterator __first,
                                            _ForwardIterator __last,
                                            forward_iterator_tag)
{
  size_type __n = 0;
  distance(__first, __last, __n);
  _M_initialize_map(__n);

  _Map_pointer __cur_node;
  __STL_TRY {
    for (__cur_node = _M_start._M_node; 
         __cur_node < _M_finish._M_node; 
         ++__cur_node) {
      _ForwardIterator __mid = __first;
      advance(__mid, _S_buffer_size());
      uninitialized_copy(__first, __mid, *__cur_node);
      __first = __mid;
    }
    uninitialized_copy(__first, __last, _M_finish._M_first);
  }
  __STL_UNWIND(destroy(_M_start, iterator(*__cur_node, __cur_node)));
}

#endif /* __STL_MEMBER_TEMPLATES */

// Called only if _M_finish._M_cur == _M_finish._M_last - 1.
template <class _Tp, class _Alloc>
void deque<_Tp,_Alloc>::_M_push_back_aux(const value_type& __t)
{
  value_type __t_copy = __t;
  _M_reserve_map_at_back();//调整map
  *(_M_finish._M_node + 1) = _M_allocate_node();//分配一个新的节点
  __STL_TRY {
    construct(_M_finish._M_cur, __t_copy);
    _M_finish._M_set_node(_M_finish._M_node + 1);
    _M_finish._M_cur = _M_finish._M_first;
  }
  __STL_UNWIND(_M_deallocate_node(*(_M_finish._M_node + 1)));
}

// Called only if _M_finish._M_cur == _M_finish._M_last - 1.
template <class _Tp, class _Alloc>
void deque<_Tp,_Alloc>::_M_push_back_aux()
{
  _M_reserve_map_at_back();
  *(_M_finish._M_node + 1) = _M_allocate_node();
  __STL_TRY {
    construct(_M_finish._M_cur);
    _M_finish._M_set_node(_M_finish._M_node + 1);
    _M_finish._M_cur = _M_finish._M_first;
  }
  __STL_UNWIND(_M_deallocate_node(*(_M_finish._M_node + 1)));
}

// Called only if _M_start._M_cur == _M_start._M_first.
template <class _Tp, class _Alloc>
void  deque<_Tp,_Alloc>::_M_push_front_aux(const value_type& __t)
{
  value_type __t_copy = __t;
  _M_reserve_map_at_front();
  *(_M_start._M_node - 1) = _M_allocate_node();
  __STL_TRY {
    _M_start._M_set_node(_M_start._M_node - 1);
    _M_start._M_cur = _M_start._M_last - 1;
    construct(_M_start._M_cur, __t_copy);
  }
  __STL_UNWIND((++_M_start, _M_deallocate_node(*(_M_start._M_node - 1))));
} 

// Called only if _M_start._M_cur == _M_start._M_first.
template <class _Tp, class _Alloc>
void deque<_Tp,_Alloc>::_M_push_front_aux()
{
  _M_reserve_map_at_front();
  *(_M_start._M_node - 1) = _M_allocate_node();
  __STL_TRY {
    _M_start._M_set_node(_M_start._M_node - 1);
    _M_start._M_cur = _M_start._M_last - 1;
    construct(_M_start._M_cur);
  }
  __STL_UNWIND((++_M_start, _M_deallocate_node(*(_M_start._M_node - 1))));
} 

// Called only if _M_finish._M_cur == _M_finish._M_first.
template <class _Tp, class _Alloc>
void deque<_Tp,_Alloc>::_M_pop_back_aux()
{
  _M_deallocate_node(_M_finish._M_first);
  _M_finish._M_set_node(_M_finish._M_node - 1);
  _M_finish._M_cur = _M_finish._M_last - 1;
  destroy(_M_finish._M_cur);
}

// Called only if _M_start._M_cur == _M_start._M_last - 1.  Note that 
// if the deque has at least one element (a precondition for this member 
// function), and if _M_start._M_cur == _M_start._M_last, then the deque 
// must have at least two nodes.
template <class _Tp, class _Alloc>
void deque<_Tp,_Alloc>::_M_pop_front_aux()
{
  destroy(_M_start._M_cur);
  _M_deallocate_node(_M_start._M_first);
  _M_start._M_set_node(_M_start._M_node + 1);
  _M_start._M_cur = _M_start._M_first;
}      

#ifdef __STL_MEMBER_TEMPLATES  

template <class _Tp, class _Alloc> template <class _InputIterator>
void deque<_Tp,_Alloc>::insert(iterator __pos,
                               _InputIterator __first, _InputIterator __last,
                               input_iterator_tag)
{
  copy(__first, __last, inserter(*this, __pos));
}

template <class _Tp, class _Alloc> template <class _ForwardIterator>
void
deque<_Tp,_Alloc>::insert(iterator __pos,
                          _ForwardIterator __first, _ForwardIterator __last,
                          forward_iterator_tag) {
  size_type __n = 0;
  distance(__first, __last, __n);
  if (__pos._M_cur == _M_start._M_cur) {
    iterator __new_start = _M_reserve_elements_at_front(__n);
    __STL_TRY {
      uninitialized_copy(__first, __last, __new_start);
      _M_start = __new_start;
    }
    __STL_UNWIND(_M_destroy_nodes(__new_start._M_node, _M_start._M_node));
  }
  else if (__pos._M_cur == _M_finish._M_cur) {
    iterator __new_finish = _M_reserve_elements_at_back(__n);
    __STL_TRY {
      uninitialized_copy(__first, __last, _M_finish);
      _M_finish = __new_finish;
    }
    __STL_UNWIND(_M_destroy_nodes(_M_finish._M_node + 1, 
                                  __new_finish._M_node + 1));
  }
  else
    _M_insert_aux(__pos, __first, __last, __n);
}

#endif /* __STL_MEMBER_TEMPLATES */

template <class _Tp, class _Alloc>
typename deque<_Tp, _Alloc>::iterator
deque<_Tp,_Alloc>::_M_insert_aux(iterator __pos, const value_type& __x)
{
  difference_type __index = __pos - _M_start;//计算插入点之前元素个数
  value_type __x_copy = __x;
  if (size_type(__index) < this->size() / 2) {//若插入点之前的元素较少
    push_front(front());//先在容器头部插入与第一个元素相同的元素
    iterator __front1 = _M_start;
    ++__front1;
    iterator __front2 = __front1;
    ++__front2;
    __pos = _M_start + __index;//这里的pos相较与接受参数的pos已经前移一位了
    iterator __pos1 = __pos;
    ++__pos1;
	//原型
	/*
	*	template<class InputIterator, class OutputIterator>
	*	OutputIterator copy (InputIterator first, InputIterator last, OutputIterator result)
	*	{
	*	  while (first!=last) {
	*		*result = *first;
	*		++result; ++first;
	*	  }
	*	  return result;
	*	}
	*/
    copy(__front2, __pos1, __front1);//移动元素
  }
  else {//若插入点之后的元素较少
    push_back(back());//先在容器尾部插入与最后一个元素相同的元素
    iterator __back1 = _M_finish;
    --__back1;
    iterator __back2 = __back1;
    --__back2;
    __pos = _M_start + __index;//这里的pos与接受参数的pos相同
	//**原型
	//*	template<class BidirectionalIterator1, class BidirectionalIterator2>
	//*	BidirectionalIterator2 copy_backward ( BidirectionalIterator1 first,
    //*                                     BidirectionalIterator1 last,
    //*                                     BidirectionalIterator2 result )
	//*	{
	//*	  while (last!=first) *(--result) = *(--last);
	//*	  return result;
	//*	}
    copy_backward(__pos, __back2, __back1);//移动元素
  }
  *__pos = __x_copy;
  return __pos;
}

template <class _Tp, class _Alloc>
typename deque<_Tp,_Alloc>::iterator 
deque<_Tp,_Alloc>::_M_insert_aux(iterator __pos)
{
  difference_type __index = __pos - _M_start;
  if (__index < size() / 2) {
    push_front(front());
    iterator __front1 = _M_start;
    ++__front1;
    iterator __front2 = __front1;
    ++__front2;
    __pos = _M_start + __index;
    iterator __pos1 = __pos;
    ++__pos1;
    copy(__front2, __pos1, __front1);
  }
  else {
    push_back(back());
    iterator __back1 = _M_finish;
    --__back1;
    iterator __back2 = __back1;
    --__back2;
    __pos = _M_start + __index;
    copy_backward(__pos, __back2, __back1);
  }
  *__pos = value_type();
  return __pos;
}

template <class _Tp, class _Alloc>
void deque<_Tp,_Alloc>::_M_insert_aux(iterator __pos,
                                      size_type __n,
                                      const value_type& __x)
{
  const difference_type __elems_before = __pos - _M_start;
  size_type __length = this->size();
  value_type __x_copy = __x;
  if (__elems_before < difference_type(__length / 2)) {
    iterator __new_start = _M_reserve_elements_at_front(__n);
    iterator __old_start = _M_start;
    __pos = _M_start + __elems_before;
    __STL_TRY {
      if (__elems_before >= difference_type(__n)) {
        iterator __start_n = _M_start + difference_type(__n);
        uninitialized_copy(_M_start, __start_n, __new_start);
        _M_start = __new_start;
        copy(__start_n, __pos, __old_start);
        fill(__pos - difference_type(__n), __pos, __x_copy);
      }
      else {
        __uninitialized_copy_fill(_M_start, __pos, __new_start, 
                                  _M_start, __x_copy);
        _M_start = __new_start;
        fill(__old_start, __pos, __x_copy);
      }
    }
    __STL_UNWIND(_M_destroy_nodes(__new_start._M_node, _M_start._M_node));
  }
  else {
    iterator __new_finish = _M_reserve_elements_at_back(__n);
    iterator __old_finish = _M_finish;
    const difference_type __elems_after = 
      difference_type(__length) - __elems_before;
    __pos = _M_finish - __elems_after;
    __STL_TRY {
      if (__elems_after > difference_type(__n)) {
        iterator __finish_n = _M_finish - difference_type(__n);
        uninitialized_copy(__finish_n, _M_finish, _M_finish);
        _M_finish = __new_finish;
        copy_backward(__pos, __finish_n, __old_finish);
        fill(__pos, __pos + difference_type(__n), __x_copy);
      }
      else {
        __uninitialized_fill_copy(_M_finish, __pos + difference_type(__n),
                                  __x_copy, __pos, _M_finish);
        _M_finish = __new_finish;
        fill(__pos, __old_finish, __x_copy);
      }
    }
    __STL_UNWIND(_M_destroy_nodes(_M_finish._M_node + 1, 
                                  __new_finish._M_node + 1));
  }
}

#ifdef __STL_MEMBER_TEMPLATES  

template <class _Tp, class _Alloc> template <class _ForwardIterator>
void deque<_Tp,_Alloc>::_M_insert_aux(iterator __pos,
                                      _ForwardIterator __first,
                                      _ForwardIterator __last,
                                      size_type __n)
{
  const difference_type __elemsbefore = __pos - _M_start;
  size_type __length = size();
  if (__elemsbefore < __length / 2) {
    iterator __new_start = _M_reserve_elements_at_front(__n);
    iterator __old_start = _M_start;
    __pos = _M_start + __elemsbefore;
    __STL_TRY {
      if (__elemsbefore >= difference_type(__n)) {
        iterator __start_n = _M_start + difference_type(__n); 
        uninitialized_copy(_M_start, __start_n, __new_start);
        _M_start = __new_start;
        copy(__start_n, __pos, __old_start);
        copy(__first, __last, __pos - difference_type(__n));
      }
      else {
        _ForwardIterator __mid = __first;
        advance(__mid, difference_type(__n) - __elemsbefore);
        __uninitialized_copy_copy(_M_start, __pos, __first, __mid,
                                  __new_start);
        _M_start = __new_start;
        copy(__mid, __last, __old_start);
      }
    }
    __STL_UNWIND(_M_destroy_nodes(__new_start._M_node, _M_start._M_node));
  }
  else {
    iterator __new_finish = _M_reserve_elements_at_back(__n);
    iterator __old_finish = _M_finish;
    const difference_type __elemsafter = 
      difference_type(__length) - __elemsbefore;
    __pos = _M_finish - __elemsafter;
    __STL_TRY {
      if (__elemsafter > difference_type(__n)) {
        iterator __finish_n = _M_finish - difference_type(__n);
        uninitialized_copy(__finish_n, _M_finish, _M_finish);
        _M_finish = __new_finish;
        copy_backward(__pos, __finish_n, __old_finish);
        copy(__first, __last, __pos);
      }
      else {
        _ForwardIterator __mid = __first;
        advance(__mid, __elemsafter);
        __uninitialized_copy_copy(__mid, __last, __pos, _M_finish, _M_finish);
        _M_finish = __new_finish;
        copy(__first, __mid, __pos);
      }
    }
    __STL_UNWIND(_M_destroy_nodes(_M_finish._M_node + 1, 
                                  __new_finish._M_node + 1));
  }
}

#else /* __STL_MEMBER_TEMPLATES */

template <class _Tp, class _Alloc>
void deque<_Tp,_Alloc>::_M_insert_aux(iterator __pos,
                                      const value_type* __first,
                                      const value_type* __last,
                                      size_type __n)
{
  const difference_type __elemsbefore = __pos - _M_start;
  size_type __length = size();
  if (__elemsbefore < __length / 2) {
    iterator __new_start = _M_reserve_elements_at_front(__n);
    iterator __old_start = _M_start;
    __pos = _M_start + __elemsbefore;
    __STL_TRY {
      if (__elemsbefore >= difference_type(__n)) {
        iterator __start_n = _M_start + difference_type(__n);
        uninitialized_copy(_M_start, __start_n, __new_start);
        _M_start = __new_start;
        copy(__start_n, __pos, __old_start);
        copy(__first, __last, __pos - difference_type(__n));
      }
      else {
        const value_type* __mid = 
          __first + (difference_type(__n) - __elemsbefore);
        __uninitialized_copy_copy(_M_start, __pos, __first, __mid,
                                  __new_start);
        _M_start = __new_start;
        copy(__mid, __last, __old_start);
      }
    }
    __STL_UNWIND(_M_destroy_nodes(__new_start._M_node, _M_start._M_node));
  }
  else {
    iterator __new_finish = _M_reserve_elements_at_back(__n);
    iterator __old_finish = _M_finish;
    const difference_type __elemsafter = 
      difference_type(__length) - __elemsbefore;
    __pos = _M_finish - __elemsafter;
    __STL_TRY {
      if (__elemsafter > difference_type(__n)) {
        iterator __finish_n = _M_finish - difference_type(__n);
        uninitialized_copy(__finish_n, _M_finish, _M_finish);
        _M_finish = __new_finish;
        copy_backward(__pos, __finish_n, __old_finish);
        copy(__first, __last, __pos);
      }
      else {
        const value_type* __mid = __first + __elemsafter;
        __uninitialized_copy_copy(__mid, __last, __pos, _M_finish, _M_finish);
        _M_finish = __new_finish;
        copy(__first, __mid, __pos);
      }
    }
    __STL_UNWIND(_M_destroy_nodes(_M_finish._M_node + 1, 
                                  __new_finish._M_node + 1));
  }
}

template <class _Tp, class _Alloc>
void deque<_Tp,_Alloc>::_M_insert_aux(iterator __pos,
                                      const_iterator __first,
                                      const_iterator __last,
                                      size_type __n)
{
  const difference_type __elemsbefore = __pos - _M_start;
  size_type __length = size();
  if (__elemsbefore < __length / 2) {
    iterator __new_start = _M_reserve_elements_at_front(__n);
    iterator __old_start = _M_start;
    __pos = _M_start + __elemsbefore;
    __STL_TRY {
      if (__elemsbefore >= __n) {
        iterator __start_n = _M_start + __n;
        uninitialized_copy(_M_start, __start_n, __new_start);
        _M_start = __new_start;
        copy(__start_n, __pos, __old_start);
        copy(__first, __last, __pos - difference_type(__n));
      }
      else {
        const_iterator __mid = __first + (__n - __elemsbefore);
        __uninitialized_copy_copy(_M_start, __pos, __first, __mid,
                                  __new_start);
        _M_start = __new_start;
        copy(__mid, __last, __old_start);
      }
    }
    __STL_UNWIND(_M_destroy_nodes(__new_start._M_node, _M_start._M_node));
  }
  else {
    iterator __new_finish = _M_reserve_elements_at_back(__n);
    iterator __old_finish = _M_finish;
    const difference_type __elemsafter = __length - __elemsbefore;
    __pos = _M_finish - __elemsafter;
    __STL_TRY {
      if (__elemsafter > __n) {
        iterator __finish_n = _M_finish - difference_type(__n);
        uninitialized_copy(__finish_n, _M_finish, _M_finish);
        _M_finish = __new_finish;
        copy_backward(__pos, __finish_n, __old_finish);
        copy(__first, __last, __pos);
      }
      else {
        const_iterator __mid = __first + __elemsafter;
        __uninitialized_copy_copy(__mid, __last, __pos, _M_finish, _M_finish);
        _M_finish = __new_finish;
        copy(__first, __mid, __pos);
      }
    }
    __STL_UNWIND(_M_destroy_nodes(_M_finish._M_node + 1, 
                 __new_finish._M_node + 1));
  }
}

#endif /* __STL_MEMBER_TEMPLATES */

template <class _Tp, class _Alloc>
void deque<_Tp,_Alloc>::_M_new_elements_at_front(size_type __new_elems)
{
  size_type __new_nodes
      = (__new_elems + _S_buffer_size() - 1) / _S_buffer_size();
  _M_reserve_map_at_front(__new_nodes);
  size_type __i;
  __STL_TRY {
    for (__i = 1; __i <= __new_nodes; ++__i)
      *(_M_start._M_node - __i) = _M_allocate_node();
  }
#       ifdef __STL_USE_EXCEPTIONS
  catch(...) {
    for (size_type __j = 1; __j < __i; ++__j)
      _M_deallocate_node(*(_M_start._M_node - __j));      
    throw;
  }
#       endif /* __STL_USE_EXCEPTIONS */
}

template <class _Tp, class _Alloc>
void deque<_Tp,_Alloc>::_M_new_elements_at_back(size_type __new_elems)
{
  size_type __new_nodes
      = (__new_elems + _S_buffer_size() - 1) / _S_buffer_size();
  _M_reserve_map_at_back(__new_nodes);
  size_type __i;
  __STL_TRY {
    for (__i = 1; __i <= __new_nodes; ++__i)
      *(_M_finish._M_node + __i) = _M_allocate_node();
  }
#       ifdef __STL_USE_EXCEPTIONS
  catch(...) {
    for (size_type __j = 1; __j < __i; ++__j)
      _M_deallocate_node(*(_M_finish._M_node + __j));      
    throw;
  }
#       endif /* __STL_USE_EXCEPTIONS */
}

template <class _Tp, class _Alloc>
void deque<_Tp,_Alloc>::_M_reallocate_map(size_type __nodes_to_add,
                                          bool __add_at_front)
{
  //map原始节点数
  size_type __old_num_nodes = _M_finish._M_node - _M_start._M_node + 1;
  //map新的节点数
  size_type __new_num_nodes = __old_num_nodes + __nodes_to_add;

  _Map_pointer __new_nstart;
  if (_M_map_size > 2 * __new_num_nodes) {//若map的大小比新节点数的两倍还大，则只需调整迭代器start和finish
    __new_nstart = _M_map + (_M_map_size - __new_num_nodes) / 2 
                     + (__add_at_front ? __nodes_to_add : 0);
    if (__new_nstart < _M_start._M_node)
      copy(_M_start._M_node, _M_finish._M_node + 1, __new_nstart);
    else
      copy_backward(_M_start._M_node, _M_finish._M_node + 1, 
                    __new_nstart + __old_num_nodes);
  }
  else {//否则需重新分配一个新的map空间
    size_type __new_map_size = 
      _M_map_size + max(_M_map_size, __nodes_to_add) + 2;

    _Map_pointer __new_map = _M_allocate_map(__new_map_size);
    __new_nstart = __new_map + (__new_map_size - __new_num_nodes) / 2
                         + (__add_at_front ? __nodes_to_add : 0);
    //拷贝原始map的内容
	copy(_M_start._M_node, _M_finish._M_node + 1, __new_nstart);
	//释放原始map空间
    _M_deallocate_map(_M_map, _M_map_size);
	//更新map的起始地址和大小
    _M_map = __new_map;
    _M_map_size = __new_map_size;
  }

  //更新迭代器start和finish
  _M_start._M_set_node(__new_nstart);
  _M_finish._M_set_node(__new_nstart + __old_num_nodes - 1);
}


// Nonmember functions.

template <class _Tp, class _Alloc>
inline bool operator==(const deque<_Tp, _Alloc>& __x,
                       const deque<_Tp, _Alloc>& __y) {
  return __x.size() == __y.size() &&
         equal(__x.begin(), __x.end(), __y.begin());
}

template <class _Tp, class _Alloc>
inline bool operator<(const deque<_Tp, _Alloc>& __x,
                      const deque<_Tp, _Alloc>& __y) {
  return lexicographical_compare(__x.begin(), __x.end(), 
                                 __y.begin(), __y.end());
}

#ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER

template <class _Tp, class _Alloc>
inline bool operator!=(const deque<_Tp, _Alloc>& __x,
                       const deque<_Tp, _Alloc>& __y) {
  return !(__x == __y);
}

template <class _Tp, class _Alloc>
inline bool operator>(const deque<_Tp, _Alloc>& __x,
                      const deque<_Tp, _Alloc>& __y) {
  return __y < __x;
}

template <class _Tp, class _Alloc>
inline bool operator<=(const deque<_Tp, _Alloc>& __x,
                       const deque<_Tp, _Alloc>& __y) {
  return !(__y < __x);
}
template <class _Tp, class _Alloc>
inline bool operator>=(const deque<_Tp, _Alloc>& __x,
                       const deque<_Tp, _Alloc>& __y) {
  return !(__x < __y);
}

template <class _Tp, class _Alloc>
inline void swap(deque<_Tp,_Alloc>& __x, deque<_Tp,_Alloc>& __y) {
  __x.swap(__y);
}

#endif /* __STL_FUNCTION_TMPL_PARTIAL_ORDER */

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1174
#pragma reset woff 1375
#endif
          
__STL_END_NAMESPACE 
  
#endif /* __SGI_STL_INTERNAL_DEQUE_H */

// Local Variables:
// mode:C++
// End:
