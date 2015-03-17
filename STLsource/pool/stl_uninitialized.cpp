/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996,1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

/* NOTE: This is an internal header file, included by other STL headers.
 *   You should not attempt to use it directly.
 */

#ifndef __SGI_STL_INTERNAL_UNINITIALIZED_H
#define __SGI_STL_INTERNAL_UNINITIALIZED_H

__STL_BEGIN_NAMESPACE

// uninitialized_copy

// Valid if copy construction is equivalent to assignment, and if the
//  destructor is trivial.
template <class _InputIter, class _ForwardIter>
/*该函数接受三个迭代器参数：迭代器first是输入的起始地址，
*迭代器last是输入的结束地址，迭代器result是输出的起始地址
*即把数据复制到[result,result+(last-first)]这个范围
*为了提高效率，首先用__VALUE_TYPE()萃取出迭代器result的型别value_type
*再利用__type_traits判断该型别是否为POD型别
*/
inline _ForwardIter
  uninitialized_copy(_InputIter __first, _InputIter __last,
                     _ForwardIter __result)
{
  return __uninitialized_copy(__first, __last, __result,
                              __VALUE_TYPE(__result));
}

template <class _InputIter, class _ForwardIter, class _Tp>
inline _ForwardIter
/*利用__type_traits判断该型别是否为POD型别*/
__uninitialized_copy(_InputIter __first, _InputIter __last,
                     _ForwardIter __result, _Tp*)
{
  typedef typename __type_traits<_Tp>::is_POD_type _Is_POD;
  return __uninitialized_copy_aux(__first, __last, __result, _Is_POD());
}

template <class _InputIter, class _ForwardIter>
_ForwardIter //若不是POD型别，就派送到这里
__uninitialized_copy_aux(_InputIter __first, _InputIter __last,
                         _ForwardIter __result,
                         __false_type)
{
  _ForwardIter __cur = __result;
  __STL_TRY {//这里加入了异常处理机制
    for ( ; __first != __last; ++__first, ++__cur)
      _Construct(&*__cur, *__first);//构造对象，必须是一个一个元素的构造，不能批量
    return __cur;
  }
  __STL_UNWIND(_Destroy(__result, __cur));//析构对象
}

template <class _InputIter, class _ForwardIter>
inline _ForwardIter //若是POD型别，就派送到这里
__uninitialized_copy_aux(_InputIter __first, _InputIter __last,
                         _ForwardIter __result,
                         __true_type)
{
	/*调用STL的算法copy()
	*函数原型：template< class InputIt, class OutputIt >
	* OutputIt copy( InputIt first, InputIt last, OutputIt d_first );
	*/
	return copy(__first, __last, __result);
}
//下面是针对char*，wchar_t* 的uninitialized_copy()特化版本
inline char* uninitialized_copy(const char* __first, const char* __last,
                                char* __result) {
/* void* memmove( void* dest, const void* src, std::size_t count );
* dest指向输出的起始地址
* src指向输入的其实地址
* count要复制的字节数
*/
  memmove(__result, __first, __last - __first);
  return __result + (__last - __first);
}

inline wchar_t* 
uninitialized_copy(const wchar_t* __first, const wchar_t* __last,
                   wchar_t* __result)
{
  memmove(__result, __first, sizeof(wchar_t) * (__last - __first));
  return __result + (__last - __first);
}


// Valid if copy construction is equivalent to assignment, and if the
// destructor is trivial.
template <class _ForwardIter, class _Tp>
/*若是POD型别，则调用此函数
	*/
inline void
__uninitialized_fill_aux(_ForwardIter __first, _ForwardIter __last, 
                         const _Tp& __x, __true_type)
{
/*函数原型：template< class ForwardIt, class T >
  * void fill( ForwardIt first, ForwardIt last, const T& value );  
  */
	fill(__first, __last, __x);
}

template <class _ForwardIter, class _Tp>
/*若不是POD型别，则调用此函数
	*/
void
__uninitialized_fill_aux(_ForwardIter __first, _ForwardIter __last, 
                         const _Tp& __x, __false_type)
{
  _ForwardIter __cur = __first;
  __STL_TRY {
    for ( ; __cur != __last; ++__cur)
      _Construct(&*__cur, __x);
  }
  __STL_UNWIND(_Destroy(__first, __cur));
}

template <class _ForwardIter, class _Tp, class _Tp1>
//用__type_traits技术判断该型别是否
为POD型别
inline void __uninitialized_fill(_ForwardIter __first, 
                                 _ForwardIter __last, const _Tp& __x, _Tp1*)
{
  typedef typename __type_traits<_Tp1>::is_POD_type _Is_POD;
  __uninitialized_fill_aux(__first, __last, __x, _Is_POD());
                   
}

template <class _ForwardIter, class _Tp>
/*该函数接受三个参数：
*迭代器first指向欲初始化的空间起始地址
*迭代器last指向欲初始化的空间结束地址
*x表示初值
*首先利用__VALUE_TYPE()萃取出迭代器first的型别value_type
*然后用__type_traits技术判断该型别是否为POD型别
*/
inline void uninitialized_fill(_ForwardIter __first,
                               _ForwardIter __last, 
                               const _Tp& __x)
{
  __uninitialized_fill(__first, __last, __x, __VALUE_TYPE(__first));
}

// Valid if copy construction is equivalent to assignment, and if the
//  destructor is trivial.
template <class _ForwardIter, class _Size, class _Tp>
inline _ForwardIter
	/*若是POD型别，则调用此函数
	*/
__uninitialized_fill_n_aux(_ForwardIter __first, _Size __n,
                           const _Tp& __x, __true_type)
{
  /*调用STL算法
  *原型：template< class OutputIt, class Size, class T >
  * void fill_n( OutputIt first, Size count, const T& value );
  * template< class OutputIt, class Size, class T >
  * OutputIt fill_n( OutputIt first, Size count, const T& value );
  */
	return fill_n(__first, __n, __x);
}

template <class _ForwardIter, class _Size, class _Tp>
_ForwardIter
	/*若不是POD型别，则调用此函数
	*/
__uninitialized_fill_n_aux(_ForwardIter __first, _Size __n,
                           const _Tp& __x, __false_type)
{
  _ForwardIter __cur = __first;
  __STL_TRY {
    for ( ; __n > 0; --__n, ++__cur)
      _Construct(&*__cur, __x);
    return __cur;
  }
  __STL_UNWIND(_Destroy(__first, __cur));
}

template <class _ForwardIter, class _Size, class _Tp, class _Tp1>
inline _ForwardIter 
	//用__type_traits技术判断该型别是否为POD型别
__uninitialized_fill_n(_ForwardIter __first, _Size __n, const _Tp& __x, _Tp1*)
{
  typedef typename __type_traits<_Tp1>::is_POD_type _Is_POD;
  //_Is_POD()判断value_type是否为POD型别
  return __uninitialized_fill_n_aux(__first, __n, __x, _Is_POD());
}

template <class _ForwardIter, class _Size, class _Tp>
inline _ForwardIter 
/*该函数接受三个参数：
*迭代器first指向欲初始化的空间起始地址
*n表示欲初始化空间大小
*x表示初值
*首先利用__VALUE_TYPE()萃取出迭代器first的型别value_type
*然后用__type_traits技术判断该型别是否为POD型别
*/
uninitialized_fill_n(_ForwardIter __first, _Size __n, const _Tp& __x)
{
  //__VALUE_TYPE(__first)萃取出first的型别value_type
	return __uninitialized_fill_n(__first, __n, __x, __VALUE_TYPE(__first));
}



__STL_END_NAMESPACE

#endif /* __SGI_STL_INTERNAL_UNINITIALIZED_H */

// Local Variables:
// mode:C++
// End:
