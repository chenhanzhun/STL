#ifndef __SGI_STL_INTERNAL_MULTISET_H
#define __SGI_STL_INTERNAL_MULTISET_H

#include <concept_checks.h>

__STL_BEGIN_NAMESPACE

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1174
#pragma set woff 1375
#endif

//multiset的特性及其用法和set完全相同，唯一的区别就是multiset允许键值key重复
//因此multiset的插入操作采用的是底层RB-Tree的insert_equal()而非insert_unique()
//有关set容器的剖析见前面博文

// Forward declaration of operators < and ==, needed for friend declaration.

//multiset内部元素默认使用递增排序less
//用户可自行制定比较类型
//内部维护的数据结构是红黑树, 具有非常优秀的最坏情况的时间复杂度 
template <class _Key, class _Compare __STL_DEPENDENT_DEFAULT_TMPL(less<_Key>),
          class _Alloc = __STL_DEFAULT_ALLOCATOR(_Key) >
class multiset;

template <class _Key, class _Compare, class _Alloc>
inline bool operator==(const multiset<_Key,_Compare,_Alloc>& __x, 
                       const multiset<_Key,_Compare,_Alloc>& __y);

template <class _Key, class _Compare, class _Alloc>
inline bool operator<(const multiset<_Key,_Compare,_Alloc>& __x, 
                      const multiset<_Key,_Compare,_Alloc>& __y);

template <class _Key, class _Compare, class _Alloc>
class multiset {
  // requirements:
  
  __STL_CLASS_REQUIRES(_Key, _Assignable);
  __STL_CLASS_BINARY_FUNCTION_CHECK(_Compare, bool, _Key, _Key);

public:

  // typedefs:

  //以下的定义与set相同
  typedef _Key     key_type;
  typedef _Key     value_type;
  typedef _Compare key_compare;
  typedef _Compare value_compare;
private:
	//底层机制是采用RB-Tree
  typedef _Rb_tree<key_type, value_type, 
                  _Identity<value_type>, key_compare, _Alloc> _Rep_type;
  _Rep_type _M_t;  // red-black tree representing multiset
public:
  typedef typename _Rep_type::const_pointer pointer;
  typedef typename _Rep_type::const_pointer const_pointer;
  typedef typename _Rep_type::const_reference reference;
  typedef typename _Rep_type::const_reference const_reference;
  typedef typename _Rep_type::const_iterator iterator;
  typedef typename _Rep_type::const_iterator const_iterator;
  typedef typename _Rep_type::const_reverse_iterator reverse_iterator;
  typedef typename _Rep_type::const_reverse_iterator const_reverse_iterator;
  typedef typename _Rep_type::size_type size_type;
  typedef typename _Rep_type::difference_type difference_type;
  typedef typename _Rep_type::allocator_type allocator_type;

  // allocation/deallocation
  /*
  构造函数
		multiset();
		explicit multiset (const key_compare& comp = key_compare(),
					   const allocator_type& alloc = allocator_type());
		
		template <class InputIterator>
		multiset (InputIterator first, InputIterator last,
				const key_compare& comp = key_compare(),
				const allocator_type& alloc = allocator_type());
		
		multiset (const multiset& x);
  */

  multiset() : _M_t(_Compare(), allocator_type()) {}
  explicit multiset(const _Compare& __comp,
                    const allocator_type& __a = allocator_type())
    : _M_t(__comp, __a) {}

#ifdef __STL_MEMBER_TEMPLATES

  //multiset的插入操作采用的是底层RB-Tree的insert_equal()而非insert_unique()
  template <class _InputIterator>
  multiset(_InputIterator __first, _InputIterator __last)
    : _M_t(_Compare(), allocator_type())
    { _M_t.insert_equal(__first, __last); }

  template <class _InputIterator>
  multiset(_InputIterator __first, _InputIterator __last,
           const _Compare& __comp,
           const allocator_type& __a = allocator_type())
    : _M_t(__comp, __a) { _M_t.insert_equal(__first, __last); }

#else

  multiset(const value_type* __first, const value_type* __last)
    : _M_t(_Compare(), allocator_type())
    { _M_t.insert_equal(__first, __last); }

  multiset(const value_type* __first, const value_type* __last,
           const _Compare& __comp,
           const allocator_type& __a = allocator_type())
    : _M_t(__comp, __a) { _M_t.insert_equal(__first, __last); }

  multiset(const_iterator __first, const_iterator __last)
    : _M_t(_Compare(), allocator_type())
    { _M_t.insert_equal(__first, __last); }

  multiset(const_iterator __first, const_iterator __last,
           const _Compare& __comp,
           const allocator_type& __a = allocator_type())
    : _M_t(__comp, __a) { _M_t.insert_equal(__first, __last); }
   
#endif /* __STL_MEMBER_TEMPLATES */

  multiset(const multiset<_Key,_Compare,_Alloc>& __x) : _M_t(__x._M_t) {}
  multiset<_Key,_Compare,_Alloc>&
  operator=(const multiset<_Key,_Compare,_Alloc>& __x) {
    _M_t = __x._M_t;//调用了底层红黑树的operator=操作函数 
    return *this;
  }

  //以下所有的multiset操作行为，RB-tree都已提供，所以multiset只要调用即可 
  // accessors:

  //返回用于key比较的函数,调用RB-Tree的key_comp()
  key_compare key_comp() const { return _M_t.key_comp(); }
   //由于multiset的性质, value和key使用同一个比较函数
  value_compare value_comp() const { return _M_t.key_comp(); }
  allocator_type get_allocator() const { return _M_t.get_allocator(); }

  iterator begin() const { return _M_t.begin(); }
  iterator end() const { return _M_t.end(); }
  reverse_iterator rbegin() const { return _M_t.rbegin(); } 
  reverse_iterator rend() const { return _M_t.rend(); }
  bool empty() const { return _M_t.empty(); }
  size_type size() const { return _M_t.size(); }
  size_type max_size() const { return _M_t.max_size(); }
  //这里调用的swap()函数是专属于RB-Tree的swap()，并不是STL的swap()算法
  void swap(multiset<_Key,_Compare,_Alloc>& __x) { _M_t.swap(__x._M_t); }

  // insert/erase
  /*
  插入节点
	iterator insert (const value_type& val);
	
	iterator insert (iterator position, const value_type& val);
		
	template <class InputIterator>
	void insert (InputIterator first, InputIterator last);
  */
  //插入数据节点
  iterator insert(const value_type& __x) { 
    return _M_t.insert_equal(__x);
  }
  //在指定位置插入节点
  iterator insert(iterator __position, const value_type& __x) {
    typedef typename _Rep_type::iterator _Rep_iterator;
    return _M_t.insert_equal((_Rep_iterator&)__position, __x);
  }

#ifdef __STL_MEMBER_TEMPLATES  
  template <class _InputIterator>
  void insert(_InputIterator __first, _InputIterator __last) {
    _M_t.insert_equal(__first, __last);
  }
#else
  void insert(const value_type* __first, const value_type* __last) {
    _M_t.insert_equal(__first, __last);
  }
  void insert(const_iterator __first, const_iterator __last) {
    _M_t.insert_equal(__first, __last);
  }
#endif /* __STL_MEMBER_TEMPLATES */
  //擦除指定位置的元素
  void erase(iterator __position) { 
    typedef typename _Rep_type::iterator _Rep_iterator;
    _M_t.erase((_Rep_iterator&)__position); 
  }
  //擦除元素值为x的节点
  size_type erase(const key_type& __x) { 
    return _M_t.erase(__x); 
  }
  //擦除指定区间的节点
  void erase(iterator __first, iterator __last) { 
    typedef typename _Rep_type::iterator _Rep_iterator;
    _M_t.erase((_Rep_iterator&)__first, (_Rep_iterator&)__last); 
  }
  //清除multiset
  void clear() { _M_t.clear(); }

  // multiset operations:

  //查找元素值为x的节点
  iterator find(const key_type& __x) const { return _M_t.find(__x); }
  //返回指定元素的个数
  size_type count(const key_type& __x) const { return _M_t.count(__x); }
  //Returns an iterator pointing to the first element in the container 
  //whose key is not considered to go before k (i.e., either it is equivalent or goes after).
  iterator lower_bound(const key_type& __x) const {
    return _M_t.lower_bound(__x);
  }
  //Returns an iterator pointing to the first element that is greater than key.
  iterator upper_bound(const key_type& __x) const {
    return _M_t.upper_bound(__x); 
  }
  //Returns the bounds of a range that includes all the elements in the container 
  //which have a key equivalent to k
  //Because the elements in a map container have unique keys, 
  //the range returned will contain a single element at most.
  pair<iterator,iterator> equal_range(const key_type& __x) const {
    return _M_t.equal_range(__x);
  }

  //以下是操作符的重载

#ifdef __STL_TEMPLATE_FRIENDS
  template <class _K1, class _C1, class _A1>
  friend bool operator== (const multiset<_K1,_C1,_A1>&,
                          const multiset<_K1,_C1,_A1>&);
  template <class _K1, class _C1, class _A1>
  friend bool operator< (const multiset<_K1,_C1,_A1>&,
                         const multiset<_K1,_C1,_A1>&);
#else /* __STL_TEMPLATE_FRIENDS */
  friend bool __STD_QUALIFIER
  operator== __STL_NULL_TMPL_ARGS (const multiset&, const multiset&);
  friend bool __STD_QUALIFIER
  operator< __STL_NULL_TMPL_ARGS (const multiset&, const multiset&);
#endif /* __STL_TEMPLATE_FRIENDS */
};

template <class _Key, class _Compare, class _Alloc>
inline bool operator==(const multiset<_Key,_Compare,_Alloc>& __x, 
                       const multiset<_Key,_Compare,_Alloc>& __y) {
  return __x._M_t == __y._M_t;
}

template <class _Key, class _Compare, class _Alloc>
inline bool operator<(const multiset<_Key,_Compare,_Alloc>& __x, 
                      const multiset<_Key,_Compare,_Alloc>& __y) {
  return __x._M_t < __y._M_t;
}

#ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER

template <class _Key, class _Compare, class _Alloc>
inline bool operator!=(const multiset<_Key,_Compare,_Alloc>& __x, 
                       const multiset<_Key,_Compare,_Alloc>& __y) {
  return !(__x == __y);
}

template <class _Key, class _Compare, class _Alloc>
inline bool operator>(const multiset<_Key,_Compare,_Alloc>& __x, 
                      const multiset<_Key,_Compare,_Alloc>& __y) {
  return __y < __x;
}

template <class _Key, class _Compare, class _Alloc>
inline bool operator<=(const multiset<_Key,_Compare,_Alloc>& __x, 
                       const multiset<_Key,_Compare,_Alloc>& __y) {
  return !(__y < __x);
}

template <class _Key, class _Compare, class _Alloc>
inline bool operator>=(const multiset<_Key,_Compare,_Alloc>& __x, 
                       const multiset<_Key,_Compare,_Alloc>& __y) {
  return !(__x < __y);
}

template <class _Key, class _Compare, class _Alloc>
inline void swap(multiset<_Key,_Compare,_Alloc>& __x, 
                 multiset<_Key,_Compare,_Alloc>& __y) {
  __x.swap(__y);
}

#endif /* __STL_FUNCTION_TMPL_PARTIAL_ORDER */

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1174
#pragma reset woff 1375
#endif

__STL_END_NAMESPACE

#endif /* __SGI_STL_INTERNAL_MULTISET_H */

// Local Variables:
// mode:C++
// End:
