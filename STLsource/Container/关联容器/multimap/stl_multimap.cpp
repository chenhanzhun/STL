#ifndef __SGI_STL_INTERNAL_MULTIMAP_H
#define __SGI_STL_INTERNAL_MULTIMAP_H

#include <concept_checks.h>

__STL_BEGIN_NAMESPACE

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1174
#pragma set woff 1375
#endif

// Forward declaration of operators < and ==, needed for friend declaration.

//multimap的特性及其用法和map完全相同，唯一的区别就是multimap允许键值key重复
//因此multimap的插入操作采用的是底层RB-Tree的insert_equal()而非insert_unique()
//有关map容器的剖析见前面博文
//map内部元素根据键值key默认使用递增排序less
//用户可自行制定比较类型
//内部维护的数据结构是红黑树, 具有非常优秀的最坏情况的时间复杂度
//注意:multimap允许元素重复,即键值和实值都可以重复,这点与map不同
template <class _Key, class _Tp, 
          class _Compare __STL_DEPENDENT_DEFAULT_TMPL(less<_Key>),
          class _Alloc = __STL_DEFAULT_ALLOCATOR(_Tp) >
class multimap;

template <class _Key, class _Tp, class _Compare, class _Alloc>
inline bool operator==(const multimap<_Key,_Tp,_Compare,_Alloc>& __x, 
                       const multimap<_Key,_Tp,_Compare,_Alloc>& __y);

template <class _Key, class _Tp, class _Compare, class _Alloc>
inline bool operator<(const multimap<_Key,_Tp,_Compare,_Alloc>& __x, 
                      const multimap<_Key,_Tp,_Compare,_Alloc>& __y);

template <class _Key, class _Tp, class _Compare, class _Alloc>
class multimap {
  // requirements:

  __STL_CLASS_REQUIRES(_Tp, _Assignable);
  __STL_CLASS_BINARY_FUNCTION_CHECK(_Compare, bool, _Key, _Key);

public:

// typedefs:

  //下面的定义与map相同
  typedef _Key                  key_type;
  typedef _Tp                   data_type;
  typedef _Tp                   mapped_type;
  typedef pair<const _Key, _Tp> value_type;
  typedef _Compare              key_compare;

   //嵌套类,提供键值key比较函数接口
  //继承自<stl_function.h>中的binary_function
  /*
	template <class _Arg1, class _Arg2, class _Result>
	struct binary_function {
		typedef _Arg1 first_argument_type;
		typedef _Arg2 second_argument_type;
		typedef _Result result_type;
	}; 
  */
  class value_compare : public binary_function<value_type, value_type, bool> {
  friend class multimap<_Key,_Tp,_Compare,_Alloc>;
  protected:
    _Compare comp;
    value_compare(_Compare __c) : comp(__c) {}
  public:
    bool operator()(const value_type& __x, const value_type& __y) const {
      return comp(__x.first, __y.first);
    }
  };

private:
	//底层机制是RB-Tree
  typedef _Rb_tree<key_type, value_type, 
                  _Select1st<value_type>, key_compare, _Alloc> _Rep_type;
  _Rep_type _M_t;  // red-black tree representing multimap
public:
  typedef typename _Rep_type::pointer pointer;
  typedef typename _Rep_type::const_pointer const_pointer;
  typedef typename _Rep_type::reference reference;
  typedef typename _Rep_type::const_reference const_reference;
  //map的迭代器不直接定义为const_iterator,而是分别定义iterator,const_iterator
  //是因为map的键值key不能被修改,因为必须遵守比较函数的排序规则,所以必须定义为const_iterator
  //而map的实值value可以被修改,则定义为iterator
  typedef typename _Rep_type::iterator iterator;
  typedef typename _Rep_type::const_iterator const_iterator; 
  typedef typename _Rep_type::reverse_iterator reverse_iterator;
  typedef typename _Rep_type::const_reverse_iterator const_reverse_iterator;
  typedef typename _Rep_type::size_type size_type;
  typedef typename _Rep_type::difference_type difference_type;
  typedef typename _Rep_type::allocator_type allocator_type;

// allocation/deallocation
  // 注意:multimap只能使用RB-tree的insert-equal(),不能使用insert-unique() 

  /*
  构造函数
		multimap();
		explicit multimap (const key_compare& comp = key_compare(),
					   const allocator_type& alloc = allocator_type());
		
		template <class InputIterator>
		multimap (InputIterator first, InputIterator last,
				const key_compare& comp = key_compare(),
				const allocator_type& alloc = allocator_type());
	
		multimap (const multimap& x);
  */

  multimap() : _M_t(_Compare(), allocator_type()) { }
  explicit multimap(const _Compare& __comp,
                    const allocator_type& __a = allocator_type())
    : _M_t(__comp, __a) { }

#ifdef __STL_MEMBER_TEMPLATES  
  template <class _InputIterator>
  multimap(_InputIterator __first, _InputIterator __last)
    : _M_t(_Compare(), allocator_type())
    { _M_t.insert_equal(__first, __last); }

  template <class _InputIterator>
  multimap(_InputIterator __first, _InputIterator __last,
           const _Compare& __comp,
           const allocator_type& __a = allocator_type())
    : _M_t(__comp, __a) { _M_t.insert_equal(__first, __last); }
#else
  multimap(const value_type* __first, const value_type* __last)
    : _M_t(_Compare(), allocator_type())
    { _M_t.insert_equal(__first, __last); }
  multimap(const value_type* __first, const value_type* __last,
           const _Compare& __comp,
           const allocator_type& __a = allocator_type())
    : _M_t(__comp, __a) { _M_t.insert_equal(__first, __last); }

  multimap(const_iterator __first, const_iterator __last)
    : _M_t(_Compare(), allocator_type())
    { _M_t.insert_equal(__first, __last); }
  multimap(const_iterator __first, const_iterator __last,
           const _Compare& __comp,
           const allocator_type& __a = allocator_type())
    : _M_t(__comp, __a) { _M_t.insert_equal(__first, __last); }
#endif /* __STL_MEMBER_TEMPLATES */
  //拷贝构造函数
  multimap(const multimap<_Key,_Tp,_Compare,_Alloc>& __x) : _M_t(__x._M_t) { }
  //这里提供了operator=,即可以通过=初始化对象
  multimap<_Key,_Tp,_Compare,_Alloc>&
  operator=(const multimap<_Key,_Tp,_Compare,_Alloc>& __x) {
    _M_t = __x._M_t;
    return *this; 
  }

  // accessors:
   //返回键值的比较函数,这里是调用RB-Tree的key_comp()
  key_compare key_comp() const { return _M_t.key_comp(); }
   //返回实值的比较函数
  //这里调用的是map嵌套类中定义的比较函数
  /*
	  class value_compare
		: public binary_function<value_type, value_type, bool> {
	  friend class map<_Key,_Tp,_Compare,_Alloc>;
	  protected :
		_Compare comp;
		value_compare(_Compare __c) : comp(__c) {}
	  public:
		bool operator()(const value_type& __x, const value_type& __y) const {
		  return comp(__x.first, __y.first);//以键值调用比较函数
		}
  */
  //实际上最终还是调用键值key的比较函数，即他们是调用同一个比较函数
  value_compare value_comp() const { return value_compare(_M_t.key_comp()); }
  allocator_type get_allocator() const { return _M_t.get_allocator(); }

  iterator begin() { return _M_t.begin(); }
  const_iterator begin() const { return _M_t.begin(); }
  iterator end() { return _M_t.end(); }
  const_iterator end() const { return _M_t.end(); }
  reverse_iterator rbegin() { return _M_t.rbegin(); }
  const_reverse_iterator rbegin() const { return _M_t.rbegin(); }
  reverse_iterator rend() { return _M_t.rend(); }
  const_reverse_iterator rend() const { return _M_t.rend(); }
  //判断容器multimap是否为空
  bool empty() const { return _M_t.empty(); }
  //返回容器multimap的大小
  size_type size() const { return _M_t.size(); }
  size_type max_size() const { return _M_t.max_size(); }
  //交换multimap对象的内容
  void swap(multimap<_Key,_Tp,_Compare,_Alloc>& __x) { _M_t.swap(__x._M_t); }

  // insert/erase
  /*
	multimap只能使用RB-tree的insert-equal()
	插入元素
	iterator insert (const value_type& val);
	
	iterator insert (iterator position, const value_type& val);
	
	template <class InputIterator>
	void insert (InputIterator first, InputIterator last);
  */
  //插入元素节点,调用RB-Tree的insert-equal();
  //插入元素的键值key允许重复
  iterator insert(const value_type& __x) { return _M_t.insert_equal(__x); }
  //在指定位置插入元素
  iterator insert(iterator __position, const value_type& __x) {
    return _M_t.insert_equal(__position, __x);
  }
#ifdef __STL_MEMBER_TEMPLATES  
  //插入[first,last)元素
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
  /*
	擦除元素
	void erase (iterator position);

	size_type erase (const key_type& k);
	
    void erase (iterator first, iterator last);
  */
  //在指定位置擦除元素
  void erase(iterator __position) { _M_t.erase(__position); }
  //擦除指定键值的节点
  size_type erase(const key_type& __x) { return _M_t.erase(__x); }
  //擦除指定区间的节点
  void erase(iterator __first, iterator __last)
    { _M_t.erase(__first, __last); }
  //清空容器
  void clear() { _M_t.clear(); }

  // multimap operations:

  //查找指定键值的节点
  iterator find(const key_type& __x) { return _M_t.find(__x); }
  const_iterator find(const key_type& __x) const { return _M_t.find(__x); }
  //计算指定键值元素的个数
  size_type count(const key_type& __x) const { return _M_t.count(__x); }
   //Returns an iterator pointing to the first element in the container 
  //whose key is not considered to go before k (i.e., either it is equivalent or goes after).
  //this->first is greater than or equivalent to __x.
  iterator lower_bound(const key_type& __x) {return _M_t.lower_bound(__x); }
  const_iterator lower_bound(const key_type& __x) const {
    return _M_t.lower_bound(__x); 
  }
  //Returns an iterator pointing to the first element that is greater than key.
  iterator upper_bound(const key_type& __x) {return _M_t.upper_bound(__x); }
  const_iterator upper_bound(const key_type& __x) const {
    return _M_t.upper_bound(__x); 
  }
  //Returns the bounds of a range that includes all the elements in the container 
  //which have a key equivalent to k
  //Because the elements in a map container have unique keys, 
  //the range returned will contain a single element at most.
   pair<iterator,iterator> equal_range(const key_type& __x) {
    return _M_t.equal_range(__x);
  }
  pair<const_iterator,const_iterator> equal_range(const key_type& __x) const {
    return _M_t.equal_range(__x);
  }


  //以下是操作符重载
#ifdef __STL_TEMPLATE_FRIENDS 
  template <class _K1, class _T1, class _C1, class _A1>
  friend bool operator== (const multimap<_K1, _T1, _C1, _A1>&,
                          const multimap<_K1, _T1, _C1, _A1>&);
  template <class _K1, class _T1, class _C1, class _A1>
  friend bool operator< (const multimap<_K1, _T1, _C1, _A1>&,
                         const multimap<_K1, _T1, _C1, _A1>&);
#else /* __STL_TEMPLATE_FRIENDS */
  friend bool __STD_QUALIFIER
  operator== __STL_NULL_TMPL_ARGS (const multimap&, const multimap&);
  friend bool __STD_QUALIFIER
  operator< __STL_NULL_TMPL_ARGS (const multimap&, const multimap&);
#endif /* __STL_TEMPLATE_FRIENDS */
};

template <class _Key, class _Tp, class _Compare, class _Alloc>
inline bool operator==(const multimap<_Key,_Tp,_Compare,_Alloc>& __x, 
                       const multimap<_Key,_Tp,_Compare,_Alloc>& __y) {
  return __x._M_t == __y._M_t;
}

template <class _Key, class _Tp, class _Compare, class _Alloc>
inline bool operator<(const multimap<_Key,_Tp,_Compare,_Alloc>& __x, 
                      const multimap<_Key,_Tp,_Compare,_Alloc>& __y) {
  return __x._M_t < __y._M_t;
}

#ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER

template <class _Key, class _Tp, class _Compare, class _Alloc>
inline bool operator!=(const multimap<_Key,_Tp,_Compare,_Alloc>& __x, 
                       const multimap<_Key,_Tp,_Compare,_Alloc>& __y) {
  return !(__x == __y);
}

template <class _Key, class _Tp, class _Compare, class _Alloc>
inline bool operator>(const multimap<_Key,_Tp,_Compare,_Alloc>& __x, 
                      const multimap<_Key,_Tp,_Compare,_Alloc>& __y) {
  return __y < __x;
}

template <class _Key, class _Tp, class _Compare, class _Alloc>
inline bool operator<=(const multimap<_Key,_Tp,_Compare,_Alloc>& __x, 
                       const multimap<_Key,_Tp,_Compare,_Alloc>& __y) {
  return !(__y < __x);
}

template <class _Key, class _Tp, class _Compare, class _Alloc>
inline bool operator>=(const multimap<_Key,_Tp,_Compare,_Alloc>& __x, 
                       const multimap<_Key,_Tp,_Compare,_Alloc>& __y) {
  return !(__x < __y);
}

template <class _Key, class _Tp, class _Compare, class _Alloc>
inline void swap(multimap<_Key,_Tp,_Compare,_Alloc>& __x, 
                 multimap<_Key,_Tp,_Compare,_Alloc>& __y) {
  __x.swap(__y);
}

#endif /* __STL_FUNCTION_TMPL_PARTIAL_ORDER */

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1174
#pragma reset woff 1375
#endif

__STL_END_NAMESPACE

#endif /* __SGI_STL_INTERNAL_MULTIMAP_H */

// Local Variables:
// mode:C++
// End:
