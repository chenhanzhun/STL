#ifndef __SGI_STL_INTERNAL_ALGO_H
#define __SGI_STL_INTERNAL_ALGO_H

#include <stl_heap.h>//这里包含堆的相关算法
//最大堆的相关算法已经介绍过了，有兴趣详见
//http://blog.csdn.net/chenhanzhun/article/details/39434491

// See concept_checks.h for the concept-checking macros 
// __STL_REQUIRES, __STL_CONVERTIBLE, etc.


__STL_BEGIN_NAMESPACE

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1209
#endif

// __median (an extension, not present in the C++ standard).
//函数功能：取三个元素a，b，c的中间值
//版本一采用默认的operator<操作
template <class _Tp>
inline const _Tp& __median(const _Tp& __a, const _Tp& __b, const _Tp& __c) {
  __STL_REQUIRES(_Tp, _LessThanComparable);
  if (__a < __b)
    if (__b < __c)
      return __b;//若a<b<c，则返回b
    else if (__a < __c)
      return __c;//若a<c<b，则返回c
    else
      return __a;//若c<a<b，则返回a
  else if (__a < __c)
    return __a;//若b<a<c，则返回a
  else if (__b < __c)
    return __c;//若b<c<a，则返回c
  else
    return __b;//否则返回b
}
//版本二采用仿函数comp比较
template <class _Tp, class _Compare>
inline const _Tp&
__median(const _Tp& __a, const _Tp& __b, const _Tp& __c, _Compare __comp) {
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool, _Tp, _Tp);
  if (__comp(__a, __b))
    if (__comp(__b, __c))
      return __b;
    else if (__comp(__a, __c))
      return __c;
    else
      return __a;
  else if (__comp(__a, __c))
    return __a;
  else if (__comp(__b, __c))
    return __c;
  else
    return __b;
}

// for_each.  Apply a function to every element of a range.
//功能：Applies function fn to each of the elements in the range [first,last).
//将仿函数f应用于[first,last)区间内的每一个元素上
//注：不能改变[first,last)内元素值
template <class _InputIter, class _Function>
_Function for_each(_InputIter __first, _InputIter __last, _Function __f) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  for ( ; __first != __last; ++__first)
    __f(*__first);//调用仿函数f
  return __f;
}
//for_each函数举例：
/*
	#include <iostream>     // std::cout
	#include <algorithm>    // std::for_each
	#include <vector>       // std::vector

	void myfunction (int i) {  // function:
	  std::cout << ' ' << i;
	}

	struct myclass {           // function object type:
	  void operator() (int i) {std::cout << ' ' << i;}
	} myobject;

	int main () {
	  std::vector<int> myvector;
	  myvector.push_back(10);
	  myvector.push_back(20);
	  myvector.push_back(30);

	  std::cout << "myvector contains:";
	  for_each (myvector.begin(), myvector.end(), myfunction);
	  std::cout << '\n';

	  // or:
	  std::cout << "myvector contains:";
	  for_each (myvector.begin(), myvector.end(), myobject);
	  std::cout << '\n';

	  return 0;
	}
	Output:
	myvector contains: 10 20 30
	myvector contains: 10 20 30
*/

// find and find_if.
//查找区间[first,last)内元素第一个与value值相等的元素，并返回其位置
//其中find函数是采用默认的equality操作operator==
//find_if是采用用户自行指定的操作pred

//若find函数萃取出来的迭代器类型为输入迭代器input_iterator_tag，则调用此函数
template <class _InputIter, class _Tp>
inline _InputIter find(_InputIter __first, _InputIter __last,
                       const _Tp& __val,
                       input_iterator_tag)
{//若尚未到达区间的尾端，且未找到匹配的值，则继续查找
  while (__first != __last && !(*__first == __val))
    ++__first;
  //若找到匹配的值，则返回该位置
  //若找不到，即到达区间尾端，此时first=last，则返回first
  return __first;
}
//若find_if函数萃取出来的迭代器类型为输入迭代器input_iterator_tag，则调用此函数
template <class _InputIter, class _Predicate>
inline _InputIter find_if(_InputIter __first, _InputIter __last,
                          _Predicate __pred,
                          input_iterator_tag)
{//若尚未到达区间的尾端，且未找到匹配的值，则继续查找
  while (__first != __last && !__pred(*__first))
    ++__first;
  //若找到匹配的值，则返回该位置
  //若找不到，即到达区间尾端，此时first=last，则返回first
  return __first;
}

#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION
//若find函数萃取出来的迭代器类型为随机访问迭代器random_access_iterator_tag，则调用此函数
template <class _RandomAccessIter, class _Tp>
_RandomAccessIter find(_RandomAccessIter __first, _RandomAccessIter __last,
                       const _Tp& __val,
                       random_access_iterator_tag)
{
  typename iterator_traits<_RandomAccessIter>::difference_type __trip_count
    = (__last - __first) >> 2;

  for ( ; __trip_count > 0 ; --__trip_count) {
    if (*__first == __val) return __first;
    ++__first;

    if (*__first == __val) return __first;
    ++__first;

    if (*__first == __val) return __first;
    ++__first;

    if (*__first == __val) return __first;
    ++__first;
  }

  switch(__last - __first) {
  case 3:
    if (*__first == __val) return __first;
    ++__first;
  case 2:
    if (*__first == __val) return __first;
    ++__first;
  case 1:
    if (*__first == __val) return __first;
    ++__first;
  case 0:
  default:
    return __last;
  }
}
//若find_if函数萃取出来的迭代器类型为随机访问迭代器random_access_iterator_tag，则调用此函数
template <class _RandomAccessIter, class _Predicate>
_RandomAccessIter find_if(_RandomAccessIter __first, _RandomAccessIter __last,
                          _Predicate __pred,
                          random_access_iterator_tag)
{
  typename iterator_traits<_RandomAccessIter>::difference_type __trip_count
    = (__last - __first) >> 2;

  for ( ; __trip_count > 0 ; --__trip_count) {
    if (__pred(*__first)) return __first;
    ++__first;

    if (__pred(*__first)) return __first;
    ++__first;

    if (__pred(*__first)) return __first;
    ++__first;

    if (__pred(*__first)) return __first;
    ++__first;
  }

  switch(__last - __first) {
  case 3:
    if (__pred(*__first)) return __first;
    ++__first;
  case 2:
    if (__pred(*__first)) return __first;
    ++__first;
  case 1:
    if (__pred(*__first)) return __first;
    ++__first;
  case 0:
  default:
    return __last;
  }
}

#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */
/*find函数功能：Returns an iterator to the first element in the range [first,last) that compares equal to val. 
If no such element is found, the function returns last.
find函数原型：
	template <class InputIterator, class T>
	InputIterator find (InputIterator first, InputIterator last, const T& val);
*/
//find函数对外接口
template <class _InputIter, class _Tp>
inline _InputIter find(_InputIter __first, _InputIter __last,
                       const _Tp& __val)
{
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool, 
            typename iterator_traits<_InputIter>::value_type, _Tp);
  //首先萃取出first迭代器的类型，根据迭代器的类型调用不同的函数
  return find(__first, __last, __val, __ITERATOR_CATEGORY(__first));
}
/*find_if函数功能：Returns an iterator to the first element in the range [first,last) for which pred returns true. 
If no such element is found, the function returns last.
find_if函数原型：
	template <class InputIterator, class UnaryPredicate>
	InputIterator find_if (InputIterator first, InputIterator last, UnaryPredicate pred);
*/
//find_if 函数对外接口
template <class _InputIter, class _Predicate>
inline _InputIter find_if(_InputIter __first, _InputIter __last,
                          _Predicate __pred) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool,
          typename iterator_traits<_InputIter>::value_type);
  //首先萃取出first迭代器的类型，根据迭代器的类型调用不同的函数
  return find_if(__first, __last, __pred, __ITERATOR_CATEGORY(__first));
}
//find和find_if函数举例：
/*
	#include <iostream>     // std::cout
	#include <algorithm>    // std::find_if
	#include <vector>       // std::vector

	bool IsOdd (int i) {
	  return ((i%2)==1);
	}

	int main () {
	  std::vector<int> myvector;

	  myvector.push_back(10);
	  myvector.push_back(25);
	  myvector.push_back(40);
	  myvector.push_back(55);

	  std::vector<int>::iterator it = std::find_if (myvector.begin(), myvector.end(), IsOdd);
	  std::cout << "The first odd value is " << *it << '\n';

	  // using std::find with vector and iterator:
	  it = find (myvector.begin(), myvector.end(), 40);
	  if (it != myvector.end())
		std::cout << "Element found in myvector: " << *it << '\n';
	  else
		std::cout << "Element not found in myints\n";

	  return 0;
	}
	Output:
	The first odd value is 25
	Element found in myvector: 40
 
*/

// adjacent_find.

//查找区间[first,last)内第一次重复的相邻元素
//若存在返回相邻元素的第一个元素位置
//若不存在返回last位置
/*该函数有两个版本：第一版本是默认操作operator==；第二版本是用户指定的二元操作pred
函数对外接口的原型：
equality (1)：默认操作是operator==
	template <class ForwardIterator>
	ForwardIterator adjacent_find (ForwardIterator first, ForwardIterator last);
predicate (2)：用户指定的二元操作pred	
	template <class ForwardIterator, class BinaryPredicate>
	ForwardIterator adjacent_find (ForwardIterator first, ForwardIterator last,
                                  BinaryPredicate pred);

*/
//版本一：默认操作是operator==
template <class _ForwardIter>
_ForwardIter adjacent_find(_ForwardIter __first, _ForwardIter __last) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES(typename iterator_traits<_ForwardIter>::value_type,
                 _EqualityComparable);
  /*
  情况1：若输入区间为空，则直接返回尾端last；
  情况2：若输入区间不为空，且存在相邻重复元素，则返回相邻元素的第一个元素的位置；
  情况3：若输入区间不为空，但是不存在相邻重复元素，则直接返回尾端last；
  */
  //情况1：
  if (__first == __last)//若输入区间为空
    return __last;//直接返回last
  //情况2：
  _ForwardIter __next = __first;//定义当前位置的下一个位置(即当前元素的相邻元素)
  while(++__next != __last) {//若还没到达尾端，执行while循环
    if (*__first == *__next)//相邻元素值相等，则找到相邻重复元素
      return __first;//返回第一个元素的位置
    __first = __next;//若暂时找不到，则继续找，直到到达区间尾端
  }
  //情况3：
  return __last;//直接返回尾端last
}

//版本二：用户指定的二元操作pred	
//实现过程和版本一一样，只是判断规则不同
template <class _ForwardIter, class _BinaryPredicate>
_ForwardIter adjacent_find(_ForwardIter __first, _ForwardIter __last,
                           _BinaryPredicate __binary_pred) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_BinaryPredicate, bool,
          typename iterator_traits<_ForwardIter>::value_type,
          typename iterator_traits<_ForwardIter>::value_type);
  if (__first == __last)
    return __last;
  _ForwardIter __next = __first;
  while(++__next != __last) {
	  //如果找到相邻元素符合用户指定条件，就返回第一元素位置
    if (__binary_pred(*__first, *__next))
      return __first;
    __first = __next;
  }
  return __last;
}
//adjacent_find函数举例：
/*
	#include <iostream>     // std::cout
	#include <algorithm>    // std::adjacent_find
	#include <vector>       // std::vector

	bool myfunction (int i, int j) {
	  return (i==j);
	}

	int main () {
	  int myints[] = {5,20,5,30,30,20,10,10,20};
	  std::vector<int> myvector (myints,myints+8);
	  std::vector<int>::iterator it;

	  // using default comparison:
	  it = std::adjacent_find (myvector.begin(), myvector.end());

	  if (it!=myvector.end())
		std::cout << "the first pair of repeated elements are: " << *it << '\n';

	  //using predicate comparison:
	  it = std::adjacent_find (++it, myvector.end(), myfunction);

	  if (it!=myvector.end())
		std::cout << "the second pair of repeated elements are: " << *it << '\n';

	  return 0;
	}
	Output:
	the first pair of repeated elements are: 30
	the second pair of repeated elements are: 10

*/

// count and count_if.  There are two version of each, one whose return type
// type is void and one (present only if we have partial specialization)
// whose return type is iterator_traits<_InputIter>::difference_type.  The
// C++ standard only has the latter version, but the former, which was present
// in the HP STL, is retained for backward compatibility.

//计算指定元素的个数
//SGI STL中提供两个版本，但是C++标准只提供一种版本
/*功能：Returns the number of elements in the range [first,last) that compare equal to val.
C++标准只提供一种count原型：
	template <class InputIterator, class T>
		typename iterator_traits<InputIterator>::difference_type
	count (InputIterator first, InputIterator last, const T& val);
*/
//SGI STL提供的版本一count，不是C++标准：默认使用operator==
template <class _InputIter, class _Tp, class _Size>
void count(_InputIter __first, _InputIter __last, const _Tp& __value,
           _Size& __n) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(typename iterator_traits<_InputIter>::value_type,
                 _EqualityComparable);
  __STL_REQUIRES(_Tp, _EqualityComparable);
  //将区间[first,last)内的元素和指定值value比较
  //若没到达区间尾端，继续查找
  for ( ; __first != __last; ++__first)
    if (*__first == __value)//若存在相等的值
      ++__n;//计数器累加1
}

/*功能：Returns the number of elements in the range [first,last) for which pred is true.
C++标准只提供一种count_if原型：
	template <class InputIterator, class Predicate>
		typename iterator_traits<InputIterator>::difference_type
    count_if (InputIterator first, InputIterator last, UnaryPredicate pred);
*/
//SGI STL提供的版本一count_if，不是C++标准：默认使用operator==
template <class _InputIter, class _Predicate, class _Size>
void count_if(_InputIter __first, _InputIter __last, _Predicate __pred,
              _Size& __n) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool, 
                  typename iterator_traits<_InputIter>::value_type);
  //将区间[first,last)内的元素和指定值value比较
  //若没到达区间尾端，继续查找
  for ( ; __first != __last; ++__first)
    if (__pred(*__first))//存在符合规则的元素
      ++__n;//计数器累加1
}

#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION
//SGI STL提供的版本二count，也C++标准提供的版本
template <class _InputIter, class _Tp>
typename iterator_traits<_InputIter>::difference_type
count(_InputIter __first, _InputIter __last, const _Tp& __value) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(typename iterator_traits<_InputIter>::value_type,
                 _EqualityComparable);
  __STL_REQUIRES(_Tp, _EqualityComparable);
  typename iterator_traits<_InputIter>::difference_type __n = 0;
  //将区间[first,last)内的元素和指定值value比较
  //若没到达区间尾端，继续查找
  for ( ; __first != __last; ++__first)
    if (*__first == __value)//存在相等的元素
      ++__n;//计数器累加1
  return __n;
}

//SGI STL提供的版本二count_if，也C++标准提供的版本
template <class _InputIter, class _Predicate>
typename iterator_traits<_InputIter>::difference_type
count_if(_InputIter __first, _InputIter __last, _Predicate __pred) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool, 
                  typename iterator_traits<_InputIter>::value_type);
  typename iterator_traits<_InputIter>::difference_type __n = 0;
  //将区间[first,last)内的元素和指定值value比较
  //若没到达区间尾端，继续查找
  for ( ; __first != __last; ++__first)
    if (__pred(*__first))//存在符合规则的元素
      ++__n;//计数器累加1
  return __n;
}

//下面针对count和count_if函数举例：
/*
	#include <iostream>     // std::cout
	#include <algorithm>    // std::count
	#include <vector>       // std::vector

	bool IsOdd (int i) { return ((i%2)==1); }

	int main () {
	  // counting elements in array:
	  int myints[] = {10,20,31,30,21,10,11,20};   // 8 elements
	  int mycount = std::count (myints, myints+8, 10);
	  std::cout << "10 appears " << mycount << " times.\n";

	  // counting elements in container:
	  std::vector<int> myvector (myints, myints+8);
	  mycount = std::count (myvector.begin(), myvector.end(), 20);
	  std::cout << "20 appears " << mycount  << " times.\n";
  
	  mycount = count_if (myvector.begin(), myvector.end(), IsOdd);
	  std::cout << "myvector contains " << mycount  << " odd values.\n";

	  return 0;
	}
	Output:
	10 appears 2 times.
	20 appears 2 times.
	myvector contains 3 odd values.
*/
#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

// search.
//在序列一[first1,last1)所涵盖的区间中，查找序列二[first2,last2)的首次出现点
//该查找函数有两个版本：
//版本一：使用默认的equality操作operator==
//版本二：用户根据需要自行指定操作规则
/*search函数功能：Searches the range [first1,last1) for the first occurrence of the sequence defined by [first2,last2), 
and returns an iterator to its first element, or last1 if no occurrences are found.

search函数的原型：
equality (1)：版本一
	template <class ForwardIterator1, class ForwardIterator2>
	ForwardIterator1 search (ForwardIterator1 first1, ForwardIterator1 last1,
                            ForwardIterator2 first2, ForwardIterator2 last2);
predicate (2)：版本二	
	template <class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
	ForwardIterator1 search (ForwardIterator1 first1, ForwardIterator1 last1,
                            ForwardIterator2 first2, ForwardIterator2 last2,
                            BinaryPredicate pred);
*/
//版本一：使用默认的equality操作operator==
template <class _ForwardIter1, class _ForwardIter2>
_ForwardIter1 search(_ForwardIter1 __first1, _ForwardIter1 __last1,
                     _ForwardIter2 __first2, _ForwardIter2 __last2) 
{
  __STL_REQUIRES(_ForwardIter1, _ForwardIterator);
  __STL_REQUIRES(_ForwardIter2, _ForwardIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool,
   typename iterator_traits<_ForwardIter1>::value_type,
   typename iterator_traits<_ForwardIter2>::value_type);

  // Test for empty ranges
  if (__first1 == __last1 || __first2 == __last2)
    return __first1;

  // Test for a pattern of length 1.
  _ForwardIter2 __tmp(__first2);
  ++__tmp;
  if (__tmp == __last2)
    return find(__first1, __last1, *__first2);

  // General case.

  _ForwardIter2 __p1, __p;

  __p1 = __first2; ++__p1;

  _ForwardIter1 __current = __first1;

  while (__first1 != __last1) {//若还没到达区间尾端
    __first1 = find(__first1, __last1, *__first2);//查找*first2在区间[first1,last1)首次出现的位置
    if (__first1 == __last1)//若在[first1,last1)中不存在*first2，即在[first1,last1)不存在子序列[first2,last2)
      return __last1;//则直接返回区间尾端

    __p = __p1;
    __current = __first1; 
    if (++__current == __last1)//若[first1,last1)只有一个元素，即序列[first1,last1)小于序列[first2,last2)
      return __last1;//不可能成为其子序列，返回last1

    while (*__current == *__p) {//若两个序列相对应的值相同
      if (++__p == __last2)//若序列[first2,last2)只有两个元素，且与序列一匹配
        return __first1;//则返回匹配的首次位置
      if (++__current == __last1)//若第一个序列小于第二个序列
        return __last1;//返回last1
    }

    ++__first1;
  }
  return __first1;
}

//版本二：用户根据需要自行指定操作规则
template <class _ForwardIter1, class _ForwardIter2, class _BinaryPred>
_ForwardIter1 search(_ForwardIter1 __first1, _ForwardIter1 __last1,
                     _ForwardIter2 __first2, _ForwardIter2 __last2,
                     _BinaryPred  __predicate) 
{
  __STL_REQUIRES(_ForwardIter1, _ForwardIterator);
  __STL_REQUIRES(_ForwardIter2, _ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_BinaryPred, bool,
   typename iterator_traits<_ForwardIter1>::value_type,
   typename iterator_traits<_ForwardIter2>::value_type);

  // Test for empty ranges
  if (__first1 == __last1 || __first2 == __last2)
    return __first1;

  // Test for a pattern of length 1.
  _ForwardIter2 __tmp(__first2);
  ++__tmp;
  if (__tmp == __last2) {
    while (__first1 != __last1 && !__predicate(*__first1, *__first2))
      ++__first1;
    return __first1;    
  }

  // General case.

  _ForwardIter2 __p1, __p;

  __p1 = __first2; ++__p1;

  _ForwardIter1 __current = __first1;

  while (__first1 != __last1) {
    while (__first1 != __last1) {
      if (__predicate(*__first1, *__first2))
        break;
      ++__first1;
    }
    while (__first1 != __last1 && !__predicate(*__first1, *__first2))
      ++__first1;
    if (__first1 == __last1)
      return __last1;

    __p = __p1;
    __current = __first1; 
    if (++__current == __last1) return __last1;

    while (__predicate(*__current, *__p)) {
      if (++__p == __last2)
        return __first1;
      if (++__current == __last1)
        return __last1;
    }

    ++__first1;
  }
  return __first1;
}

// search_n.  Search for __count consecutive copies of __val.
//在序列[first,last)查找连续count个符合条件值value元素的位置
//该查找函数有两个版本：
//版本一：使用默认的equality操作operator==
//版本二：用户根据需要自行指定操作规则
/*search_n函数功能：Searches the range [first,last) for a sequence of count elements, 
each comparing equal to val (or for which pred returns true).


search_n函数的原型：
equality (1)：版本一	
	template <class ForwardIterator, class Size, class T>
	ForwardIterator search_n (ForwardIterator first, ForwardIterator last,
                             Size count, const T& val);
predicate (2)：版本二	
	template <class ForwardIterator, class Size, class T, class BinaryPredicate>
	ForwardIterator search_n ( ForwardIterator first, ForwardIterator last,
                              Size count, const T& val, BinaryPredicate pred );
*/
//版本一：使用默认的equality操作operator==
template <class _ForwardIter, class _Integer, class _Tp>
_ForwardIter search_n(_ForwardIter __first, _ForwardIter __last,
                      _Integer __count, const _Tp& __val) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES(typename iterator_traits<_ForwardIter>::value_type,
                 _EqualityComparable);
  __STL_REQUIRES(_Tp, _EqualityComparable);

  if (__count <= 0)
    return __first;
  else {//首先查找value第一次出现的位置
    __first = find(__first, __last, __val);
    while (__first != __last) {//若出现的位置不是区间尾端
      _Integer __n = __count - 1;//更新个数，下面只需查找n=count-1个连续相同value即可
      _ForwardIter __i = __first;
      ++__i;//从当前位置的下一个位置开始查找
	  //若没有到达区间尾端，且个数n大于0，且区间元素与value值相等
      while (__i != __last && __n != 0 && *__i == __val) {
        ++__i;//继续查找
        --__n;//减少查找的次数，因为已经找到value再次出现
      }
      if (__n == 0)//若区间尚未到达尾端，但是count个value已经查找到
        return __first;//则输出查找到的首次出现value的位置
      else
        __first = find(__i, __last, __val);//若尚未找到连续count个value值的位置，则找出value下次出现的位置，并准备下一次while循环
    }
    return __last;
  }
}
//版本二：用户根据需要自行指定操作规则
template <class _ForwardIter, class _Integer, class _Tp, class _BinaryPred>
_ForwardIter search_n(_ForwardIter __first, _ForwardIter __last,
                      _Integer __count, const _Tp& __val,
                      _BinaryPred __binary_pred) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_BinaryPred, bool, 
             typename iterator_traits<_ForwardIter>::value_type, _Tp);
  if (__count <= 0)
    return __first;
  else {
    while (__first != __last) {
      if (__binary_pred(*__first, __val))
        break;
      ++__first;
    }
    while (__first != __last) {
      _Integer __n = __count - 1;
      _ForwardIter __i = __first;
      ++__i;
      while (__i != __last && __n != 0 && __binary_pred(*__i, __val)) {
        ++__i;
        --__n;
      }
      if (__n == 0)
        return __first;
      else {
        while (__i != __last) {
          if (__binary_pred(*__i, __val))
            break;
          ++__i;
        }
        __first = __i;
      }
    }
    return __last;
  }
} 
//search和search_n函数举例：
/*
	#include <iostream>     // std::cout
	#include <algorithm>    // std::search_n
	#include <vector>       // std::vector

	bool mypredicate (int i, int j) {
	  return (i==j);
	}

	int main () {
	  int myints[]={10,20,30,30,20,10,10,20};
	  std::vector<int> myvector (myints,myints+8);
	  std::vector<int>::iterator it;

	  // using default comparison:
	  it = std::search_n (myvector.begin(), myvector.end(), 2, 30);
	  if (it!=myvector.end())
		std::cout << "two 30s found at position " << (it-myvector.begin()) << '\n';
	  else
		std::cout << "match not found\n";

	  // using predicate comparison:
	  it = std::search_n (myvector.begin(), myvector.end(), 2, 10, mypredicate);
	  if (it!=myvector.end())
		std::cout << "two 10s found at position " << int(it-myvector.begin()) << '\n';
	  else
		std::cout << "match not found\n";
    
	  int needle1[] = {10,20};
  
	  // using default comparison:
	  it = std::search (myvector.begin(), myvector.end(), needle1, needle1+2);  
	   if (it!=myvector.end())
		std::cout << "needle1 found at position " << (it-myvector.begin()) << '\n';
	  else
		std::cout << "needle1 not found\n";
    
	  // using predicate comparison:
	  int needle2[] = {30,20,10};
	  it = std::search (myvector.begin(), myvector.end(), needle2, needle2+3, mypredicate);
	  if (it!=myvector.end())
		std::cout << "needle2 found at position " << (it-myvector.begin()) << '\n';
	  else
		std::cout << "needle2 not found\n";

	  return 0;
	}
	Output:
	two 30s found at position 2
	two 10s found at position 5
	needle1 found at position 0
	needle2 found at position 3
*/

// swap_ranges
//将区间[first1,last1)内的元素与“从first2开始，个数相同”的元素相互交换
//这两个序列可位于同一容器，或不同容器
//如果第二序列小于第一序列长度，或者两序列在同一容器且重叠，则结果未可预期
//Exchanges the values of each of the elements in the range [first1,last1) 
//with those of their respective elements in the range beginning at first2.
template <class _ForwardIter1, class _ForwardIter2>
_ForwardIter2 swap_ranges(_ForwardIter1 __first1, _ForwardIter1 __last1,
                          _ForwardIter2 __first2) {
  __STL_REQUIRES(_ForwardIter1, _Mutable_ForwardIterator);
  __STL_REQUIRES(_ForwardIter2, _Mutable_ForwardIterator);
  __STL_CONVERTIBLE(typename iterator_traits<_ForwardIter1>::value_type,
                    typename iterator_traits<_ForwardIter2>::value_type);
  __STL_CONVERTIBLE(typename iterator_traits<_ForwardIter2>::value_type,
                    typename iterator_traits<_ForwardIter1>::value_type);
  for ( ; __first1 != __last1; ++__first1, ++__first2)//遍历第一个序列
    iter_swap(__first1, __first2);//交换迭代器所指的元素
  return __first2;
}
//swap_ranges函数举例：
/*
	#include <iostream>     // std::cout
	#include <algorithm>    // std::swap_ranges
	#include <vector>       // std::vector

	int main () {
	  std::vector<int> foo (5,10);        // foo: 10 10 10 10 10
	  std::vector<int> bar (5,33);        // bar: 33 33 33 33 33

	  std::swap_ranges(foo.begin()+1, foo.end()-1, bar.begin());

	  // print out results of swap:
	  std::cout << "foo contains:";
	  for (std::vector<int>::iterator it=foo.begin(); it!=foo.end(); ++it)
		std::cout << ' ' << *it;
	  std::cout << '\n';

	  std::cout << "bar contains:";
	  for (std::vector<int>::iterator it=bar.begin(); it!=bar.end(); ++it)
		std::cout << ' ' << *it;
	  std::cout << '\n';

	  return 0;
	}
	Output:
	foo contains: 10 33 33 33 10
	bar contains: 10 10 10 33 33
*/

// transform
//两个版本
/*
函数原型：
unary operation(1)：版本一	
	template <class InputIterator, class OutputIterator, class UnaryOperation>
	OutputIterator transform (InputIterator first1, InputIterator last1,
                            OutputIterator result, UnaryOperation op);
binary operation(2)：版本二	
	template <class InputIterator1, class InputIterator2,
          class OutputIterator, class BinaryOperation>
	OutputIterator transform (InputIterator1 first1, InputIterator1 last1,
                            InputIterator2 first2, OutputIterator result,
                            BinaryOperation binary_op);
函数功能：
(1) unary operation
	Applies op to each of the elements in the range [first1,last1) and stores the value 
	returned by each operation in the range that begins at result.
(2) binary operation
	Calls binary_op using each of the elements in the range [first1,last1) as first argument, 
	and the respective argument in the range that begins at first2 as second argument. 
	The value returned by each call is stored in the range that begins at result.
*/
//第一个版本：以仿函数opr作用于[first,last)中的每一个元素，并以其结果产生出一个新的序列
template <class _InputIter, class _OutputIter, class _UnaryOperation>
_OutputIter transform(_InputIter __first, _InputIter __last,
                      _OutputIter __result, _UnaryOperation __opr) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);

  for ( ; __first != __last; ++__first, ++__result)
    *__result = __opr(*__first);
  return __result;
}
//第二个版本：以仿函数binary_op作用于一双元素上（其中一个元素来自[first1,last1)，另一个元素来自“从first2开始的序列”）
//并以其结果产生出一个新的序列
template <class _InputIter1, class _InputIter2, class _OutputIter,
          class _BinaryOperation>
_OutputIter transform(_InputIter1 __first1, _InputIter1 __last1,
                      _InputIter2 __first2, _OutputIter __result,
                      _BinaryOperation __binary_op) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  for ( ; __first1 != __last1; ++__first1, ++__first2, ++__result)
    *__result = __binary_op(*__first1, *__first2);
  return __result;
}
//transform函数举例：
/*
	#include <vector>       // std::vector
	#include <functional>   // std::plus

	int op_increase (int i) { return ++i; }

	int main () {
	  std::vector<int> foo;
	  std::vector<int> bar;

	  // set some values:
	  for (int i=1; i<6; i++)
		foo.push_back (i*10);                         // foo: 10 20 30 40 50

	  bar.resize(foo.size());                         // allocate space

	  std::transform (foo.begin(), foo.end(), bar.begin(), op_increase);
													  // bar: 11 21 31 41 51
	  std::cout << "bar contains:";
	  for (std::vector<int>::iterator it=bar.begin(); it!=bar.end(); ++it)
		std::cout << ' ' << *it;
		std::cout << '\n';

	  // std::plus adds together its two arguments:
	  std::transform (foo.begin(), foo.end(), bar.begin(), foo.begin(), std::plus<int>());
													  // foo: 21 41 61 81 101

	  std::cout << "foo contains:";
	  for (std::vector<int>::iterator it=foo.begin(); it!=foo.end(); ++it)
		std::cout << ' ' << *it;
	  std::cout << '\n';

	  return 0;
	}
	Output:
	bar contains: 11 21 31 41 51
	foo contains: 21 41 61 81 101
*/

// replace, replace_if, replace_copy, replace_copy_if
//将区间[first,last)内的所有old_value都以new_value替代.
template <class _ForwardIter, class _Tp>
void replace(_ForwardIter __first, _ForwardIter __last,
             const _Tp& __old_value, const _Tp& __new_value) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool,
         typename iterator_traits<_ForwardIter>::value_type, _Tp);
  __STL_CONVERTIBLE(_Tp, typename iterator_traits<_ForwardIter>::value_type);
  for ( ; __first != __last; ++__first)
	  //将区间内所有old_value都以new_value替代.
    if (*__first == __old_value)
      *__first = __new_value;
}
//将区间[first,last)内的所有被pred判断为true的元素都以new_value替代.
template <class _ForwardIter, class _Predicate, class _Tp>
void replace_if(_ForwardIter __first, _ForwardIter __last,
                _Predicate __pred, const _Tp& __new_value) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  __STL_CONVERTIBLE(_Tp, typename iterator_traits<_ForwardIter>::value_type);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool,
             typename iterator_traits<_ForwardIter>::value_type);
  for ( ; __first != __last; ++__first)
    if (__pred(*__first))//pred判断为true
      *__first = __new_value;//修改其值
}
//将区间[first,last)内的所有old_value都以new_value替代.将新序列复制到result所指的容器中
//原始容器的内容并不会改变
template <class _InputIter, class _OutputIter, class _Tp>
_OutputIter replace_copy(_InputIter __first, _InputIter __last,
                         _OutputIter __result,
                         const _Tp& __old_value, const _Tp& __new_value) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool,
         typename iterator_traits<_InputIter>::value_type, _Tp);
  for ( ; __first != __last; ++__first, ++__result)
    *__result = *__first == __old_value ? __new_value : *__first;
  return __result;
}
//将区间[first,last)内的所有被pred判断为true的元素都以new_value替代.将新序列复制到result所指的容器中
//原始容器的内容并不会改变
template <class _InputIter, class _OutputIter, class _Predicate, class _Tp>
_OutputIter replace_copy_if(_InputIter __first, _InputIter __last,
                            _OutputIter __result,
                            _Predicate __pred, const _Tp& __new_value) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool,
                typename iterator_traits<_InputIter>::value_type);
  for ( ; __first != __last; ++__first, ++__result)
    *__result = __pred(*__first) ? __new_value : *__first;
  return __result;
}

// generate and generate_n
//将仿函数gen的处理结果填充在[first, last)区间内所有元素上，所谓填写
//就是用迭代器所指元素的assignment操作
//注意：对于用户自定义类型要提供operator =() 

template <class _ForwardIter, class _Generator>
void generate(_ForwardIter __first, _ForwardIter __last, _Generator __gen) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_GENERATOR_CHECK(_Generator, 
          typename iterator_traits<_ForwardIter>::value_type);
  for ( ; __first != __last; ++__first)//遍历整个序列
    *__first = __gen();
}
//将仿函数gen的处理结果填充在first开始的n个元素上，所谓填写
//就是用迭代器所指元素的assignment操作
template <class _OutputIter, class _Size, class _Generator>
_OutputIter generate_n(_OutputIter __first, _Size __n, _Generator __gen) {
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  for ( ; __n > 0; --__n, ++__first)//只限于n个元素
    *__first = __gen();
  return __first;
}
//generate和generate_n函数举例：
/*
	#include <iostream>     // std::cout
	#include <algorithm>    // std::generate_n

	int current = 0;
	int UniqueNumber () { return ++current; }

	int main () {
	  int myarray[9];

	  std::generate_n (myarray, 9, UniqueNumber);

	  std::cout << "myarray contains:";
	  for (int i=0; i<9; ++i)
		std::cout << ' ' << myarray[i];
	  std::cout << '\n';
	  std::cout <<"the value of current: "<<current;
	  std::cout << '\n';

	  std::vector<int> myvector (6);
	  std::generate (myvector.begin(), myvector.end(), UniqueNumber);

	  std::cout << "myvector contains:";
	  for (std::vector<int>::iterator it=myvector.begin(); it!=myvector.end(); ++it)
		std::cout << ' ' << *it;
	  std::cout << '\n';

	  return 0;
	}
	Output:
	myarray contains: 1 2 3 4 5 6 7 8 9
	the value of current: 9
	myvector contains: 10 11 12 13 14 15
*/

// remove, remove_if, remove_copy, remove_copy_if

//移除[first,last)区间内所有与value值相等的元素，并不是真正的从容器中删除这些元素(原容器的内容不会改变)
//而是将结果复制到一个以result为起始位置的容器中。新容器可以与原容器重叠
template <class _InputIter, class _OutputIter, class _Tp>
_OutputIter remove_copy(_InputIter __first, _InputIter __last,
                        _OutputIter __result, const _Tp& __value) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool,
       typename iterator_traits<_InputIter>::value_type, _Tp);
  for ( ; __first != __last; ++__first)//遍历容器
    if (!(*__first == __value)) {//如果不相等
      *__result = *__first;//赋值给新容器
      ++__result;//新容器前进一个位置
    }
  return __result;
}
//移除[first,last)区间内被仿函数pred判断为true的元素,并不是真正的从容器中删除这些元素(原容器的内容不会改变)
//而是将结果复制到一个以result为起始位置的容器中。新容器可以与原容器重叠
template <class _InputIter, class _OutputIter, class _Predicate>
_OutputIter remove_copy_if(_InputIter __first, _InputIter __last,
                           _OutputIter __result, _Predicate __pred) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool,
             typename iterator_traits<_InputIter>::value_type);
  for ( ; __first != __last; ++__first)//遍历容器
    if (!__pred(*__first)) {//若pred判断为false
      *__result = *__first;//赋值给新容器
      ++__result;//新容器前进一个位置
    }
  return __result;
}
//移除[first,last)区间内所有与value值相等的元素,该操作不会改变容器大小，只是容器中元素值改变
//即移除之后，重新整理容器的内容
template <class _ForwardIter, class _Tp>
_ForwardIter remove(_ForwardIter __first, _ForwardIter __last,
                    const _Tp& __value) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool,
       typename iterator_traits<_ForwardIter>::value_type, _Tp);
  __STL_CONVERTIBLE(_Tp, typename iterator_traits<_ForwardIter>::value_type);
  __first = find(__first, __last, __value);//利用顺序查找找出第一个与value相等的元素
  _ForwardIter __i = __first;
  //下面调用remove_copy
  return __first == __last ? __first 
                           : remove_copy(++__i, __last, __first, __value);
}
//移除[first,last)区间内所有被pred判断为true的元素,该操作不会改变容器大小，只是容器中元素值改变
//即移除之后，重新整理容器的内容
template <class _ForwardIter, class _Predicate>
_ForwardIter remove_if(_ForwardIter __first, _ForwardIter __last,
                       _Predicate __pred) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool,
               typename iterator_traits<_ForwardIter>::value_type);
  __first = find_if(__first, __last, __pred);//利用顺序查找找出第一个与value相等的元素
  _ForwardIter __i = __first;
  //下面调用remove_copy_if
  return __first == __last ? __first 
                           : remove_copy_if(++__i, __last, __first, __pred);
}
//上面四个移除函数举例：
/*
	#include <iostream>     // std::cout
	#include <algorithm>    // std::remove

	bool IsOdd (int i) { return ((i%2)==1); }

	int main () {
	  int myints[] = {10,20,31,30,20,11,10,20};      // 10 20 31 30 20 11 10 20

	  std::vector<int> myvector (8);
	  std::remove_copy (myints,myints+8,myvector.begin(),20); // 10 31 30 11 10 0 0 0
	  std::cout << "myvector contains:";
	  for (std::vector<int>::iterator it=myvector.begin(); it!=myvector.end(); ++it)
		std::cout << ' ' << *it;
	  std::cout << '\n';
  
	  // bounds of range:
	  int* pbegin = myints;                          // ^
	  int* pend = myints+sizeof(myints)/sizeof(int); // ^                       ^
	  pend = std::remove (pbegin, pend, 20);         // 10 31 30 11 10 ?  ?  ?
													 // ^              ^
	  std::cout << "range contains:";
	  for (int* p=pbegin; p!=pend; ++p)
		std::cout << ' ' << *p;
	  std::cout << '\n';
  
	  std::vector<int> myvector2 (7);
	  std::remove_copy_if (myints,myints+7,myvector2.begin(),IsOdd);
	  std::cout << "myvector2 contains:";
	  for (std::vector<int>::iterator it=myvector2.begin(); it!=myvector2.end(); ++it)
		std::cout << ' ' << *it;
	  std::cout << '\n';
  
	  pend = std::remove_if (pbegin, pend, IsOdd);   // 10 30 10 ? ? ? ? ?
													 // ^       ^
	  std::cout << "the range contains:";
	  for (int* p=pbegin; p!=pend; ++p)
		std::cout << ' ' << *p;
	  std::cout << '\n';  

	  return 0;
	}
	Output:
	myvector contains: 10 31 30 11 10 0 0 0
	range contains: 10 31 30 11 10
	myvector2 contains: 10 30 10 10 0 0 0
	the range contains: 10 30 10
*/

// unique and unique_copy

template <class _InputIter, class _OutputIter, class _Tp>
_OutputIter __unique_copy(_InputIter __first, _InputIter __last,
                          _OutputIter __result, _Tp*) {
  _Tp __value = *__first;
  *__result = __value;
  while (++__first != __last)
    if (!(__value == *__first)) {
      __value = *__first;
      *++__result = __value;
    }
  return ++__result;
}
//若result类型为output_iterator_tag，则调用该函数
template <class _InputIter, class _OutputIter>
inline _OutputIter __unique_copy(_InputIter __first, _InputIter __last,
                                 _OutputIter __result, 
                                 output_iterator_tag) {
		//判断first的value_type类型，根据不同类型调用不同函数
  return __unique_copy(__first, __last, __result, __VALUE_TYPE(__first));
}
//若result类型为forward_iterator_tag，则调用该函数
template <class _InputIter, class _ForwardIter>
_ForwardIter __unique_copy(_InputIter __first, _InputIter __last,
                           _ForwardIter __result, forward_iterator_tag) {
  *__result = *__first;//记录第一个元素
  while (++__first != __last)//遍历区间
	  //若不存在相邻重复元素，则继续记录到目标区result
    if (!(*__result == *__first))
      *++__result = *__first;//记录元素到目标区
  return ++__result;
}
////unique_copy将区间[first,last)内元素复制到以result开头的区间上，但是如果存在相邻重复元素时，只复制其中第一个元素
//和unique一样，这里也有两个版本
/*
函数原型：
equality (1)	
	template <class InputIterator, class OutputIterator>
	OutputIterator unique_copy (InputIterator first, InputIterator last,
                              OutputIterator result);
predicate (2)	
	template <class InputIterator, class OutputIterator, class BinaryPredicate>
	OutputIterator unique_copy (InputIterator first, InputIterator last,
                              OutputIterator result, BinaryPredicate pred);
*/
//版本一
template <class _InputIter, class _OutputIter>
inline _OutputIter unique_copy(_InputIter __first, _InputIter __last,
                               _OutputIter __result) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES(typename iterator_traits<_InputIter>::value_type,
                 _EqualityComparable);
  if (__first == __last) return __result;
  //根据result迭代器的类型，调用不同的函数
  return __unique_copy(__first, __last, __result,
                       __ITERATOR_CATEGORY(__result));
}

template <class _InputIter, class _OutputIter, class _BinaryPredicate,
          class _Tp>
_OutputIter __unique_copy(_InputIter __first, _InputIter __last,
                          _OutputIter __result,
                          _BinaryPredicate __binary_pred, _Tp*) {
  __STL_BINARY_FUNCTION_CHECK(_BinaryPredicate, bool, _Tp, _Tp);
  _Tp __value = *__first;
  *__result = __value;
  while (++__first != __last)
    if (!__binary_pred(__value, *__first)) {
      __value = *__first;
      *++__result = __value;
    }
  return ++__result;
}

template <class _InputIter, class _OutputIter, class _BinaryPredicate>
inline _OutputIter __unique_copy(_InputIter __first, _InputIter __last,
                                 _OutputIter __result,
                                 _BinaryPredicate __binary_pred,
                                 output_iterator_tag) {
  return __unique_copy(__first, __last, __result, __binary_pred,
                       __VALUE_TYPE(__first));
}

template <class _InputIter, class _ForwardIter, class _BinaryPredicate>
_ForwardIter __unique_copy(_InputIter __first, _InputIter __last,
                           _ForwardIter __result, 
                           _BinaryPredicate __binary_pred,
                           forward_iterator_tag) {
  __STL_BINARY_FUNCTION_CHECK(_BinaryPredicate, bool,
     typename iterator_traits<_ForwardIter>::value_type,
     typename iterator_traits<_InputIter>::value_type);
  *__result = *__first;
  while (++__first != __last)
    if (!__binary_pred(*__result, *__first)) *++__result = *__first;
  return ++__result;
}
//版本二
template <class _InputIter, class _OutputIter, class _BinaryPredicate>
inline _OutputIter unique_copy(_InputIter __first, _InputIter __last,
                               _OutputIter __result,
                               _BinaryPredicate __binary_pred) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  if (__first == __last) return __result;
  //根据result迭代器的类型，调用不同的函数
  return __unique_copy(__first, __last, __result, __binary_pred,
                       __ITERATOR_CATEGORY(__result));
}
//移除区间[first,last)相邻连续重复的元素
//unique有两个版本
//功能：Removes all but the first element from every consecutive group of equivalent elements in the range [first,last).
/*
函数原型：
equality (1)：版本一采用operator==	
	template <class ForwardIterator>
	ForwardIterator unique (ForwardIterator first, ForwardIterator last);
predicate (2)：版本二采用pred操作	
	template <class ForwardIterator, class BinaryPredicate>
	ForwardIterator unique (ForwardIterator first, ForwardIterator last,
                          BinaryPredicate pred);
*/
//版本一
template <class _ForwardIter>
_ForwardIter unique(_ForwardIter __first, _ForwardIter __last) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  __STL_REQUIRES(typename iterator_traits<_ForwardIter>::value_type,
                 _EqualityComparable);
  __first = adjacent_find(__first, __last);//找出第一个相邻元素的起始位置
  return unique_copy(__first, __last, __first);//调用unique_copy完成操作
}
//版本二
template <class _ForwardIter, class _BinaryPredicate>
_ForwardIter unique(_ForwardIter __first, _ForwardIter __last,
                    _BinaryPredicate __binary_pred) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_BinaryPredicate, bool, 
      typename iterator_traits<_ForwardIter>::value_type,
      typename iterator_traits<_ForwardIter>::value_type);
  __first = adjacent_find(__first, __last, __binary_pred);//找出第一个相邻元素的起始位置
  return unique_copy(__first, __last, __first, __binary_pred);//调用unique_copy完成操作
}

// reverse and reverse_copy, and their auxiliary functions

//若迭代器类型为bidirectional_iterator_tag，则调用此函数
template <class _BidirectionalIter>
void __reverse(_BidirectionalIter __first, _BidirectionalIter __last, 
               bidirectional_iterator_tag) {
  while (true)
    if (__first == __last || __first == --__last)//这里需注意，每次判断last迭代器都会后退一位
      return;
    else
      iter_swap(__first++, __last);//单向交换迭代器所指的元素
}
//若迭代器类型为random_access_iterator_tag，则调用此函数
template <class _RandomAccessIter>
void __reverse(_RandomAccessIter __first, _RandomAccessIter __last,
               random_access_iterator_tag) {
  while (__first < __last)//遍历容器
    iter_swap(__first++, --__last);//交换两端迭代器所指的元素
}
//将序列[first,last)的所有元素在原容器中颠倒重排
template <class _BidirectionalIter>
inline void reverse(_BidirectionalIter __first, _BidirectionalIter __last) {
  __STL_REQUIRES(_BidirectionalIter, _Mutable_BidirectionalIterator);
  //首先萃取出迭代器的类型
  __reverse(__first, __last, __ITERATOR_CATEGORY(__first));
}
//行为类似reverse，但产生的新序列会被置于以result指出的容器中
template <class _BidirectionalIter, class _OutputIter>
_OutputIter reverse_copy(_BidirectionalIter __first,
                         _BidirectionalIter __last,
                         _OutputIter __result) {
  __STL_REQUIRES(_BidirectionalIter, _BidirectionalIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  while (__first != __last) {//遍历容器
    --__last;//尾端前移一个位置
    *__result = *__last;//result容器的起始位置元素值为原始容器尾端元素值
    ++__result;//更新result，使其前进一个位置
  }
  return __result;
}

// rotate and rotate_copy, and their auxiliary functions

template <class _EuclideanRingElement>
_EuclideanRingElement __gcd(_EuclideanRingElement __m,
                            _EuclideanRingElement __n)
{
  while (__n != 0) {
    _EuclideanRingElement __t = __m % __n;
    __m = __n;
    __n = __t;
  }
  return __m;
}
//迭代器类型为forward_iterator_tag，调用此函数
template <class _ForwardIter, class _Distance>
_ForwardIter __rotate(_ForwardIter __first,
                      _ForwardIter __middle,
                      _ForwardIter __last,
                      _Distance*,
                      forward_iterator_tag) {
  if (__first == __middle)
    return __last;
  if (__last  == __middle)
    return __first;

  _ForwardIter __first2 = __middle;
  do {
    swap(*__first++, *__first2++);
    if (__first == __middle)
      __middle = __first2;
  } while (__first2 != __last);

  _ForwardIter __new_middle = __first;

  __first2 = __middle;

  while (__first2 != __last) {
    swap (*__first++, *__first2++);
    if (__first == __middle)
      __middle = __first2;
    else if (__first2 == __last)
      __first2 = __middle;
  }

  return __new_middle;
}

//迭代器类型为bidirectional_iterator_tag，调用此函数
template <class _BidirectionalIter, class _Distance>
_BidirectionalIter __rotate(_BidirectionalIter __first,
                            _BidirectionalIter __middle,
                            _BidirectionalIter __last,
                            _Distance*,
                            bidirectional_iterator_tag) {
  __STL_REQUIRES(_BidirectionalIter, _Mutable_BidirectionalIterator);
  if (__first == __middle)
    return __last;
  if (__last  == __middle)
    return __first;

  __reverse(__first,  __middle, bidirectional_iterator_tag());
  __reverse(__middle, __last,   bidirectional_iterator_tag());

  while (__first != __middle && __middle != __last)
    swap (*__first++, *--__last);

  if (__first == __middle) {
    __reverse(__middle, __last,   bidirectional_iterator_tag());
    return __last;
  }
  else {
    __reverse(__first,  __middle, bidirectional_iterator_tag());
    return __first;
  }
}
//迭代器类型为Random_iterator_tag，调用此函数
template <class _RandomAccessIter, class _Distance, class _Tp>
_RandomAccessIter __rotate(_RandomAccessIter __first,
                           _RandomAccessIter __middle,
                           _RandomAccessIter __last,
                           _Distance *, _Tp *) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  _Distance __n = __last   - __first;
  _Distance __k = __middle - __first;
  _Distance __l = __n - __k;
  _RandomAccessIter __result = __first + (__last - __middle);

  if (__k == 0)
    return __last;

  else if (__k == __l) {
    swap_ranges(__first, __middle, __middle);
    return __result;
  }

  _Distance __d = __gcd(__n, __k);

  for (_Distance __i = 0; __i < __d; __i++) {
    _Tp __tmp = *__first;
    _RandomAccessIter __p = __first;

    if (__k < __l) {
      for (_Distance __j = 0; __j < __l/__d; __j++) {
        if (__p > __first + __l) {
          *__p = *(__p - __l);
          __p -= __l;
        }

        *__p = *(__p + __k);
        __p += __k;
      }
    }

    else {
      for (_Distance __j = 0; __j < __k/__d - 1; __j ++) {
        if (__p < __last - __k) {
          *__p = *(__p + __k);
          __p += __k;
        }

        *__p = * (__p - __l);
        __p -= __l;
      }
    }

    *__p = __tmp;
    ++__first;
  }

  return __result;
}
//将区间[first,middle)内的元素和[middle,last)内的元素互换。minddle所指的元素会成为容器的第一个元素
//例如对序列{1,2,3,4,5,6,7}，对元素3进行旋转操作,则结果为{3,4,5,6,7,1,2}
template <class _ForwardIter>
inline _ForwardIter rotate(_ForwardIter __first, _ForwardIter __middle,
                           _ForwardIter __last) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  //萃取出迭代器的类型，根据迭代器的类型调用不同的函数
  return __rotate(__first, __middle, __last,
                  __DISTANCE_TYPE(__first),
                  __ITERATOR_CATEGORY(__first));
}
//将区间[first,middle)内的元素和[middle,last)内的元素互换。minddle所指的元素会成为新容器result的第一个元素
template <class _ForwardIter, class _OutputIter>
_OutputIter rotate_copy(_ForwardIter __first, _ForwardIter __middle,
                        _ForwardIter __last, _OutputIter __result) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  //这里直接采用复制操作，先把[middle,last)复制到result容器中，
  //再把[first,middle)内容复制到result容器中
  return copy(__first, __middle, copy(__middle, __last, __result));
}

// Return a random number in the range [0, __n).  This function encapsulates
// whether we're using rand (part of the standard C library) or lrand48
// (not standard, but a much better choice whenever it's available).

template <class _Distance>
inline _Distance __random_number(_Distance __n) {
#ifdef __STL_NO_DRAND48
  return rand() % __n;
#else
  return lrand48() % __n;
#endif
}

// random_shuffle
//将区间[first,last)内的元素随机重排
//两个版本的不同是随机数的取得
//版本一是使用内部随机数产生器
//版本二是使用一个会产生随机数的仿函数

/*
函数功能：Rearranges the elements in the range [first,last) randomly.
函数原型：
generator by default (1)	
	template <class RandomAccessIterator>
	void random_shuffle (RandomAccessIterator first, RandomAccessIterator last);
specific generator (2)	
	template <class RandomAccessIterator, class RandomNumberGenerator>
	void random_shuffle (RandomAccessIterator first, RandomAccessIterator last,
                       RandomNumberGenerator& gen);
*/
//版本一
template <class _RandomAccessIter>
inline void random_shuffle(_RandomAccessIter __first,
                           _RandomAccessIter __last) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  if (__first == __last) return;
  for (_RandomAccessIter __i = __first + 1; __i != __last; ++__i)
    iter_swap(__i, __first + __random_number((__i - __first) + 1));
}
//版本二
template <class _RandomAccessIter, class _RandomNumberGenerator>
void random_shuffle(_RandomAccessIter __first, _RandomAccessIter __last,
                    _RandomNumberGenerator& __rand) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  if (__first == __last) return;
  for (_RandomAccessIter __i = __first + 1; __i != __last; ++__i)
    iter_swap(__i, __first + __rand((__i - __first) + 1));
}

// random_sample and random_sample_n (extensions, not part of the standard).

template <class _ForwardIter, class _OutputIter, class _Distance>
_OutputIter random_sample_n(_ForwardIter __first, _ForwardIter __last,
                            _OutputIter __out, const _Distance __n)
{
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  _Distance __remaining = 0;
  distance(__first, __last, __remaining);
  _Distance __m = min(__n, __remaining);

  while (__m > 0) {
    if (__random_number(__remaining) < __m) {
      *__out = *__first;
      ++__out;
      --__m;
    }

    --__remaining;
    ++__first;
  }
  return __out;
}

template <class _ForwardIter, class _OutputIter, class _Distance,
          class _RandomNumberGenerator>
_OutputIter random_sample_n(_ForwardIter __first, _ForwardIter __last,
                            _OutputIter __out, const _Distance __n,
                            _RandomNumberGenerator& __rand)
{
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_UNARY_FUNCTION_CHECK(_RandomNumberGenerator, _Distance, _Distance);
  _Distance __remaining = 0;
  distance(__first, __last, __remaining);
  _Distance __m = min(__n, __remaining);

  while (__m > 0) {
    if (__rand(__remaining) < __m) {
      *__out = *__first;
      ++__out;
      --__m;
    }

    --__remaining;
    ++__first;
  }
  return __out;
}

template <class _InputIter, class _RandomAccessIter, class _Distance>
_RandomAccessIter __random_sample(_InputIter __first, _InputIter __last,
                                  _RandomAccessIter __out,
                                  const _Distance __n)
{
  _Distance __m = 0;
  _Distance __t = __n;
  for ( ; __first != __last && __m < __n; ++__m, ++__first) 
    __out[__m] = *__first;

  while (__first != __last) {
    ++__t;
    _Distance __M = __random_number(__t);
    if (__M < __n)
      __out[__M] = *__first;
    ++__first;
  }

  return __out + __m;
}

template <class _InputIter, class _RandomAccessIter,
          class _RandomNumberGenerator, class _Distance>
_RandomAccessIter __random_sample(_InputIter __first, _InputIter __last,
                                  _RandomAccessIter __out,
                                  _RandomNumberGenerator& __rand,
                                  const _Distance __n)
{
  __STL_UNARY_FUNCTION_CHECK(_RandomNumberGenerator, _Distance, _Distance);
  _Distance __m = 0;
  _Distance __t = __n;
  for ( ; __first != __last && __m < __n; ++__m, ++__first)
    __out[__m] = *__first;

  while (__first != __last) {
    ++__t;
    _Distance __M = __rand(__t);
    if (__M < __n)
      __out[__M] = *__first;
    ++__first;
  }

  return __out + __m;
}

template <class _InputIter, class _RandomAccessIter>
inline _RandomAccessIter
random_sample(_InputIter __first, _InputIter __last,
              _RandomAccessIter __out_first, _RandomAccessIter __out_last) 
{
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  return __random_sample(__first, __last,
                         __out_first, __out_last - __out_first);
}


template <class _InputIter, class _RandomAccessIter, 
          class _RandomNumberGenerator>
inline _RandomAccessIter
random_sample(_InputIter __first, _InputIter __last,
              _RandomAccessIter __out_first, _RandomAccessIter __out_last,
              _RandomNumberGenerator& __rand) 
{
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  return __random_sample(__first, __last,
                         __out_first, __rand,
                         __out_last - __out_first);
}

// partition, stable_partition, and their auxiliary functions
//若迭代器的类型为forward_iterator_tag，则调用此函数
template <class _ForwardIter, class _Predicate>
_ForwardIter __partition(_ForwardIter __first,
		         _ForwardIter __last,
			 _Predicate   __pred,
			 forward_iterator_tag) {
  if (__first == __last) return __first;//若为空，直接退出

  while (__pred(*__first))//若pred出first的值为true
    if (++__first == __last) return __first;//先移动迭代器first，在判断是否到达尾端last

  _ForwardIter __next = __first;//继续判断

  while (++__next != __last)//若下一个位置依然不是尾端
    if (__pred(*__next)) {//继续pred出next的值，若为true
      swap(*__first, *__next);//交换值
      ++__first;//继续下一位置
    }

  return __first;
}
//若迭代器的类型为bidirectional_iterator_tag，则调用此函数
template <class _BidirectionalIter, class _Predicate>
_BidirectionalIter __partition(_BidirectionalIter __first,
                               _BidirectionalIter __last,
			       _Predicate __pred,
			       bidirectional_iterator_tag) {
  while (true) {
    while (true)
      if (__first == __last)//若为空
        return __first;//直接退出
      else if (__pred(*__first))//first的值符合不移动条件，则不移动该值
        ++__first;//只移动迭代器
      else//若头指针符合移动
        break;//跳出循环
    --__last;//尾指针回溯
    while (true)
      if (__first == __last)//头指针等于尾指针
        return __first;//操作结束
      else if (!__pred(*__last))//尾指针的元素符合不移动操作
        --__last;//至移动迭代器，并不移动具体元素
      else//尾指针的元素符合移动操作
        break;//跳出循环
    iter_swap(__first, __last);//头尾指针交换元素
    ++__first;//准备下一次循环
  }
}
//将区间[first,last)的元素进行排序，被pred判断为true的放在区间的前段，判定为false的放在区间后段
//该算算可能会使元素的元素位置放生改变.
/*
算法功能：Rearranges the elements from the range [first,last), in such a way that all the elements
for which pred returns true precede all those for which it returns false. 
The iterator returned points to the first element of the second group.

算法原型：
	template <class BidirectionalIterator, class UnaryPredicate>
	BidirectionalIterator partition (BidirectionalIterator first,
                                   BidirectionalIterator last, UnaryPredicate pred);
*/
template <class _ForwardIter, class _Predicate>
inline _ForwardIter partition(_ForwardIter __first,
   			      _ForwardIter __last,
			      _Predicate   __pred) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool, 
        typename iterator_traits<_ForwardIter>::value_type);
  //首先萃取出迭代器first的类型，根据迭代器的类型调用不同的函数
  return __partition(__first, __last, __pred, __ITERATOR_CATEGORY(__first));
}
//partition函数举例：
/*
	#include <iostream>     // std::cout
	#include <algorithm>    // std::partition
	#include <vector>       // std::vector

	bool IsOdd (int i) { return (i%2)==1; }

	int main () {
	  std::vector<int> myvector;

	  // set some values:
	  for (int i=1; i<10; ++i) myvector.push_back(i); // 1 2 3 4 5 6 7 8 9

	  std::vector<int>::iterator bound;
	  bound = std::partition (myvector.begin(), myvector.end(), IsOdd);

	  // print out content:
	  std::cout << "odd elements:";
	  for (std::vector<int>::iterator it=myvector.begin(); it!=bound; ++it)
		std::cout << ' ' << *it;
	  std::cout << '\n';

	  std::cout << "even elements:";
	  for (std::vector<int>::iterator it=bound; it!=myvector.end(); ++it)
		std::cout << ' ' << *it;
	  std::cout << '\n';

	  return 0;
	}
	Output:
	odd elements: 1 9 3 7 5
	even elements: 6 4 8 2
 
*/

template <class _ForwardIter, class _Predicate, class _Distance>
_ForwardIter __inplace_stable_partition(_ForwardIter __first,
                                        _ForwardIter __last,
                                        _Predicate __pred, _Distance __len) {
  if (__len == 1)
    return __pred(*__first) ? __last : __first;
  _ForwardIter __middle = __first;
  advance(__middle, __len / 2);
  return rotate(__inplace_stable_partition(__first, __middle, __pred, 
                                           __len / 2),
                __middle,
                __inplace_stable_partition(__middle, __last, __pred,
                                           __len - __len / 2));
}

template <class _ForwardIter, class _Pointer, class _Predicate, 
          class _Distance>
_ForwardIter __stable_partition_adaptive(_ForwardIter __first,
                                         _ForwardIter __last,
                                         _Predicate __pred, _Distance __len,
                                         _Pointer __buffer,
                                         _Distance __buffer_size) 
{
  if (__len <= __buffer_size) {
    _ForwardIter __result1 = __first;
    _Pointer __result2 = __buffer;
    for ( ; __first != __last ; ++__first)
      if (__pred(*__first)) {
        *__result1 = *__first;
        ++__result1;
      }
      else {
        *__result2 = *__first;
        ++__result2;
      }
    copy(__buffer, __result2, __result1);
    return __result1;
  }
  else {
    _ForwardIter __middle = __first;
    advance(__middle, __len / 2);
    return rotate(__stable_partition_adaptive(
                          __first, __middle, __pred,
                          __len / 2, __buffer, __buffer_size),
                    __middle,
                    __stable_partition_adaptive(
                          __middle, __last, __pred,
                          __len - __len / 2, __buffer, __buffer_size));
  }
}

template <class _ForwardIter, class _Predicate, class _Tp, class _Distance>
inline _ForwardIter
__stable_partition_aux(_ForwardIter __first, _ForwardIter __last, 
                       _Predicate __pred, _Tp*, _Distance*)
{
  _Temporary_buffer<_ForwardIter, _Tp> __buf(__first, __last);
  if (__buf.size() > 0)
    return __stable_partition_adaptive(__first, __last, __pred,
                                       _Distance(__buf.requested_size()),
                                       __buf.begin(), __buf.size());
  else
    return __inplace_stable_partition(__first, __last, __pred, 
                                      _Distance(__buf.requested_size()));
}

template <class _ForwardIter, class _Predicate>
inline _ForwardIter stable_partition(_ForwardIter __first,
                                     _ForwardIter __last, 
                                     _Predicate __pred) {
  __STL_REQUIRES(_ForwardIter, _Mutable_ForwardIterator);
  __STL_UNARY_FUNCTION_CHECK(_Predicate, bool,
      typename iterator_traits<_ForwardIter>::value_type);
  if (__first == __last)
    return __first;
  else
    return __stable_partition_aux(__first, __last, __pred,
                                  __VALUE_TYPE(__first),
                                  __DISTANCE_TYPE(__first));
}
//找出快速排序的枢纽位置
//版本一采用operator<
template <class _RandomAccessIter, class _Tp>
_RandomAccessIter __unguarded_partition(_RandomAccessIter __first, 
                                        _RandomAccessIter __last, 
                                        _Tp __pivot) 
{
	//找出枢纽轴的位置
	//令头端迭代器向尾端方向移动，尾端迭代器向头端移动。
	//当*first不小于枢纽值时，就停下来，当*last不大于枢纽值时也停下来，然后检测两个迭代器是否交错
	//如果first仍然在左侧而last仍然在右侧，就交换两个元素，然后各自调整位置，向中央逼近，再继续执行相同的行为.
	//直到first和last两个迭代器交错，此时表示已找到枢纽轴位置即first所在的位置
  while (true) {
    while (*__first < __pivot)
      ++__first;//first向尾端移动，直到遇到不小于枢纽值时，停止
    --__last;
    while (__pivot < *__last)
      --__last;//last向头端移动，直到遇到不大于枢纽值时，停止
    if (!(__first < __last))//检测两个迭代器是否交错
      return __first;//交错，则此时已找到，即为first迭代器所指位置
    iter_swap(__first, __last);//否则交换迭代器所指的元素
    ++__first;//继续执行相同行为
  }
}    
//版本一采用__comp
template <class _RandomAccessIter, class _Tp, class _Compare>
_RandomAccessIter __unguarded_partition(_RandomAccessIter __first, 
                                        _RandomAccessIter __last, 
                                        _Tp __pivot, _Compare __comp) 
{
  while (true) {
    while (__comp(*__first, __pivot))
      ++__first;
    --__last;
    while (__comp(__pivot, *__last))
      --__last;
    if (!(__first < __last))
      return __first;
    iter_swap(__first, __last);
    ++__first;
  }
}

const int __stl_threshold = 16;

// sort() and its auxiliary functions. 
//__insertion_sort版本一的辅助函数
template <class _RandomAccessIter, class _Tp>
void __unguarded_linear_insert(_RandomAccessIter __last, _Tp __val) {
  _RandomAccessIter __next = __last;
  --__next;
  //__insertion_sort的内循环
  //注意：一旦不再出现逆转对，循环就结束
  while (__val < *__next) {//存在逆转对
    *__last = *__next;//调整元素
    __last = __next;//调整迭代器
    --__next;//左移一个位置
  }
  *__last = __val;//value的正确插入位置
}
//__insertion_sort版本二的辅助函数
template <class _RandomAccessIter, class _Tp, class _Compare>
void __unguarded_linear_insert(_RandomAccessIter __last, _Tp __val, 
                               _Compare __comp) {
  _RandomAccessIter __next = __last;
  --__next;  
  while (__comp(__val, *__next)) {
    *__last = *__next;
    __last = __next;
    --__next;
  }
  *__last = __val;
}
//__insertion_sort版本一的辅助函数
template <class _RandomAccessIter, class _Tp>
inline void __linear_insert(_RandomAccessIter __first, 
                            _RandomAccessIter __last, _Tp*) {
  _Tp __val = *__last;//记录尾元素
  if (__val < *__first) {//尾元素比头元素还小
	  //将整个区间向右移一个位置
    copy_backward(__first, __last, __last + 1);
    *__first = __val;//令头元素等于原先的尾元素
	//以上两行命令的功能相等于交换两个元素
  }
  else//尾元素不小于头元素
    __unguarded_linear_insert(__last, __val);
}
//__insertion_sort版本二的辅助函数
template <class _RandomAccessIter, class _Tp, class _Compare>
inline void __linear_insert(_RandomAccessIter __first, 
                            _RandomAccessIter __last, _Tp*, _Compare __comp) {
  _Tp __val = *__last;
  if (__comp(__val, *__first)) {
    copy_backward(__first, __last, __last + 1);
    *__first = __val;
  }
  else
    __unguarded_linear_insert(__last, __val, __comp);
}
//__insertion_sort以双层循环形式进行。外循环遍历整个序列，每次迭代决定出一个子区间；
//内循环遍历子区间，将子区间内的每一个“逆转对”倒转过来，如果一旦不存在“逆转对”，表示排序完毕。
//“逆转对”概念：指任何两个迭代器i和j，i<j,而*i>*j.
//版本一
template <class _RandomAccessIter>
void __insertion_sort(_RandomAccessIter __first, _RandomAccessIter __last) {
  if (__first == __last) return; //若区间为空，则退出
  for (_RandomAccessIter __i = __first + 1; __i != __last; ++__i)//外循环，遍历整个区间
	  //[first,i)形成的子空间
    __linear_insert(__first, __i, __VALUE_TYPE(__first));
}
//版本二
template <class _RandomAccessIter, class _Compare>
void __insertion_sort(_RandomAccessIter __first,
                      _RandomAccessIter __last, _Compare __comp) {
  if (__first == __last) return;
  for (_RandomAccessIter __i = __first + 1; __i != __last; ++__i)
    __linear_insert(__first, __i, __VALUE_TYPE(__first), __comp);
}

template <class _RandomAccessIter, class _Tp>
void __unguarded_insertion_sort_aux(_RandomAccessIter __first, 
                                    _RandomAccessIter __last, _Tp*) {
  for (_RandomAccessIter __i = __first; __i != __last; ++__i)
    __unguarded_linear_insert(__i, _Tp(*__i));
}
//sort版本一的辅助函数
template <class _RandomAccessIter>
inline void __unguarded_insertion_sort(_RandomAccessIter __first, 
                                _RandomAccessIter __last) {
  __unguarded_insertion_sort_aux(__first, __last, __VALUE_TYPE(__first));
}

template <class _RandomAccessIter, class _Tp, class _Compare>
void __unguarded_insertion_sort_aux(_RandomAccessIter __first, 
                                    _RandomAccessIter __last,
                                    _Tp*, _Compare __comp) {
  for (_RandomAccessIter __i = __first; __i != __last; ++__i)
    __unguarded_linear_insert(__i, _Tp(*__i), __comp);
}

template <class _RandomAccessIter, class _Compare>
inline void __unguarded_insertion_sort(_RandomAccessIter __first, 
                                       _RandomAccessIter __last,
                                       _Compare __comp) {
  __unguarded_insertion_sort_aux(__first, __last, __VALUE_TYPE(__first),
                                 __comp);
}
//sort版本一的辅助函数
template <class _RandomAccessIter>
void __final_insertion_sort(_RandomAccessIter __first, 
                            _RandomAccessIter __last) {
  if (__last - __first > __stl_threshold) {//判断元素个数是否大于16
	  //则把区间分割成两段，一端长度为16，另一端为剩余的长度
    __insertion_sort(__first, __first + __stl_threshold);
    __unguarded_insertion_sort(__first + __stl_threshold, __last);
  }
  else//若不大于16，直接调用插入排序
    __insertion_sort(__first, __last);
}

template <class _RandomAccessIter, class _Compare>
void __final_insertion_sort(_RandomAccessIter __first, 
                            _RandomAccessIter __last, _Compare __comp) {
  if (__last - __first > __stl_threshold) {
    __insertion_sort(__first, __first + __stl_threshold, __comp);
    __unguarded_insertion_sort(__first + __stl_threshold, __last, __comp);
  }
  else
    __insertion_sort(__first, __last, __comp);
}
//_lg()函数是用来控制分割恶化的情况
//该函数找出2^k <= n 的最大值k;
//例如：n=7,得k=2; n=20,得k=4; n=8,得k=3; 
template <class _Size>
inline _Size __lg(_Size __n) {
  _Size __k;
  for (__k = 0; __n != 1; __n >>= 1) ++__k;
  return __k;
}
//sort版本一的辅助函数
//参数__depth_limit表示最大的分割层数
template <class _RandomAccessIter, class _Tp, class _Size>
void __introsort_loop(_RandomAccessIter __first,
                      _RandomAccessIter __last, _Tp*,
                      _Size __depth_limit)
{
	//__stl_threshold为全局常量，其值为16
  while (__last - __first > __stl_threshold) {//若区间长度大于16
    if (__depth_limit == 0) {//表示分割恶化
      partial_sort(__first, __last, __last);//转而调用堆排序heap_sort()
      return;
    }
    --__depth_limit;
	//计算分割点cut，枢纽值是采用首、尾、中央三个的中间值
    _RandomAccessIter __cut =
      __unguarded_partition(__first, __last,
                            _Tp(__median(*__first,
                                         *(__first + (__last - __first)/2),
                                         *(__last - 1))));
	//对右半部分递归地进行排序
    __introsort_loop(__cut, __last, (_Tp*) 0, __depth_limit);
    __last = __cut;//接下来对左半部分递归地进行排序
  }
}

template <class _RandomAccessIter, class _Tp, class _Size, class _Compare>
void __introsort_loop(_RandomAccessIter __first,
                      _RandomAccessIter __last, _Tp*,
                      _Size __depth_limit, _Compare __comp)
{
  while (__last - __first > __stl_threshold) {
    if (__depth_limit == 0) {
      partial_sort(__first, __last, __last, __comp);
      return;
    }
    --__depth_limit;
    _RandomAccessIter __cut =
      __unguarded_partition(__first, __last,
                            _Tp(__median(*__first,
                                         *(__first + (__last - __first)/2),
                                         *(__last - 1), __comp)),
       __comp);
    __introsort_loop(__cut, __last, (_Tp*) 0, __depth_limit, __comp);
    __last = __cut;
  }
}
//SGI STL的排序算法，迭代器参数的类型必须是随机访问迭代器_RandomAccessIter
/*
函数功能：Sorts the elements in the range [first,last) into ascending order.
函数原型：
default (1)	：版本一采用默认的operator<
	template <class RandomAccessIterator>
	void sort (RandomAccessIterator first, RandomAccessIterator last);
custom (2)	：版本二采用仿函数comp
	template <class RandomAccessIterator, class Compare>
	void sort (RandomAccessIterator first, RandomAccessIterator last, Compare comp);
 */
//版本一
template <class _RandomAccessIter>
inline void sort(_RandomAccessIter __first, _RandomAccessIter __last) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_REQUIRES(typename iterator_traits<_RandomAccessIter>::value_type,
                 _LessThanComparable);
  //_lg()函数是用来控制分割恶化的情况
  if (__first != __last) {
    __introsort_loop(__first, __last,
                     __VALUE_TYPE(__first),
                     __lg(__last - __first) * 2);
	//进行插入排序
    __final_insertion_sort(__first, __last);
  }
}
//版本二
template <class _RandomAccessIter, class _Compare>
inline void sort(_RandomAccessIter __first, _RandomAccessIter __last,
                 _Compare __comp) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
       typename iterator_traits<_RandomAccessIter>::value_type,
       typename iterator_traits<_RandomAccessIter>::value_type);
  if (__first != __last) {
    __introsort_loop(__first, __last,
                     __VALUE_TYPE(__first),
                     __lg(__last - __first) * 2,
                     __comp);
    __final_insertion_sort(__first, __last, __comp);
  }
}

// stable_sort() and its auxiliary functions.

template <class _RandomAccessIter>
void __inplace_stable_sort(_RandomAccessIter __first,
                           _RandomAccessIter __last) {
  if (__last - __first < 15) {
    __insertion_sort(__first, __last);
    return;
  }
  _RandomAccessIter __middle = __first + (__last - __first) / 2;
  __inplace_stable_sort(__first, __middle);
  __inplace_stable_sort(__middle, __last);
  __merge_without_buffer(__first, __middle, __last,
                         __middle - __first,
                         __last - __middle);
}

template <class _RandomAccessIter, class _Compare>
void __inplace_stable_sort(_RandomAccessIter __first,
                           _RandomAccessIter __last, _Compare __comp) {
  if (__last - __first < 15) {
    __insertion_sort(__first, __last, __comp);
    return;
  }
  _RandomAccessIter __middle = __first + (__last - __first) / 2;
  __inplace_stable_sort(__first, __middle, __comp);
  __inplace_stable_sort(__middle, __last, __comp);
  __merge_without_buffer(__first, __middle, __last,
                         __middle - __first,
                         __last - __middle,
                         __comp);
}

template <class _RandomAccessIter1, class _RandomAccessIter2,
          class _Distance>
void __merge_sort_loop(_RandomAccessIter1 __first,
                       _RandomAccessIter1 __last, 
                       _RandomAccessIter2 __result, _Distance __step_size) {
  _Distance __two_step = 2 * __step_size;

  while (__last - __first >= __two_step) {
    __result = merge(__first, __first + __step_size,
                     __first + __step_size, __first + __two_step,
                     __result);
    __first += __two_step;
  }

  __step_size = min(_Distance(__last - __first), __step_size);
  merge(__first, __first + __step_size, __first + __step_size, __last,
        __result);
}

template <class _RandomAccessIter1, class _RandomAccessIter2,
          class _Distance, class _Compare>
void __merge_sort_loop(_RandomAccessIter1 __first,
                       _RandomAccessIter1 __last, 
                       _RandomAccessIter2 __result, _Distance __step_size,
                       _Compare __comp) {
  _Distance __two_step = 2 * __step_size;

  while (__last - __first >= __two_step) {
    __result = merge(__first, __first + __step_size,
                     __first + __step_size, __first + __two_step,
                     __result,
                     __comp);
    __first += __two_step;
  }
  __step_size = min(_Distance(__last - __first), __step_size);

  merge(__first, __first + __step_size,
        __first + __step_size, __last,
        __result,
        __comp);
}

const int __stl_chunk_size = 7;
        
template <class _RandomAccessIter, class _Distance>
void __chunk_insertion_sort(_RandomAccessIter __first, 
                            _RandomAccessIter __last, _Distance __chunk_size)
{
  while (__last - __first >= __chunk_size) {
    __insertion_sort(__first, __first + __chunk_size);
    __first += __chunk_size;
  }
  __insertion_sort(__first, __last);
}

template <class _RandomAccessIter, class _Distance, class _Compare>
void __chunk_insertion_sort(_RandomAccessIter __first, 
                            _RandomAccessIter __last,
                            _Distance __chunk_size, _Compare __comp)
{
  while (__last - __first >= __chunk_size) {
    __insertion_sort(__first, __first + __chunk_size, __comp);
    __first += __chunk_size;
  }
  __insertion_sort(__first, __last, __comp);
}

template <class _RandomAccessIter, class _Pointer, class _Distance>
void __merge_sort_with_buffer(_RandomAccessIter __first, 
                              _RandomAccessIter __last,
                              _Pointer __buffer, _Distance*) {
  _Distance __len = __last - __first;
  _Pointer __buffer_last = __buffer + __len;

  _Distance __step_size = __stl_chunk_size;
  __chunk_insertion_sort(__first, __last, __step_size);

  while (__step_size < __len) {
    __merge_sort_loop(__first, __last, __buffer, __step_size);
    __step_size *= 2;
    __merge_sort_loop(__buffer, __buffer_last, __first, __step_size);
    __step_size *= 2;
  }
}

template <class _RandomAccessIter, class _Pointer, class _Distance,
          class _Compare>
void __merge_sort_with_buffer(_RandomAccessIter __first, 
                              _RandomAccessIter __last, _Pointer __buffer,
                              _Distance*, _Compare __comp) {
  _Distance __len = __last - __first;
  _Pointer __buffer_last = __buffer + __len;

  _Distance __step_size = __stl_chunk_size;
  __chunk_insertion_sort(__first, __last, __step_size, __comp);

  while (__step_size < __len) {
    __merge_sort_loop(__first, __last, __buffer, __step_size, __comp);
    __step_size *= 2;
    __merge_sort_loop(__buffer, __buffer_last, __first, __step_size, __comp);
    __step_size *= 2;
  }
}

template <class _RandomAccessIter, class _Pointer, class _Distance>
void __stable_sort_adaptive(_RandomAccessIter __first, 
                            _RandomAccessIter __last, _Pointer __buffer,
                            _Distance __buffer_size) {
  _Distance __len = (__last - __first + 1) / 2;
  _RandomAccessIter __middle = __first + __len;
  if (__len > __buffer_size) {
    __stable_sort_adaptive(__first, __middle, __buffer, __buffer_size);
    __stable_sort_adaptive(__middle, __last, __buffer, __buffer_size);
  }
  else {
    __merge_sort_with_buffer(__first, __middle, __buffer, (_Distance*)0);
    __merge_sort_with_buffer(__middle, __last, __buffer, (_Distance*)0);
  }
  __merge_adaptive(__first, __middle, __last, _Distance(__middle - __first), 
                   _Distance(__last - __middle), __buffer, __buffer_size);
}

template <class _RandomAccessIter, class _Pointer, class _Distance, 
          class _Compare>
void __stable_sort_adaptive(_RandomAccessIter __first, 
                            _RandomAccessIter __last, _Pointer __buffer,
                            _Distance __buffer_size, _Compare __comp) {
  _Distance __len = (__last - __first + 1) / 2;
  _RandomAccessIter __middle = __first + __len;
  if (__len > __buffer_size) {
    __stable_sort_adaptive(__first, __middle, __buffer, __buffer_size, 
                           __comp);
    __stable_sort_adaptive(__middle, __last, __buffer, __buffer_size, 
                           __comp);
  }
  else {
    __merge_sort_with_buffer(__first, __middle, __buffer, (_Distance*)0,
                               __comp);
    __merge_sort_with_buffer(__middle, __last, __buffer, (_Distance*)0,
                               __comp);
  }
  __merge_adaptive(__first, __middle, __last, _Distance(__middle - __first), 
                   _Distance(__last - __middle), __buffer, __buffer_size,
                   __comp);
}

template <class _RandomAccessIter, class _Tp, class _Distance>
inline void __stable_sort_aux(_RandomAccessIter __first,
                              _RandomAccessIter __last, _Tp*, _Distance*) {
  _Temporary_buffer<_RandomAccessIter, _Tp> buf(__first, __last);
  if (buf.begin() == 0)
    __inplace_stable_sort(__first, __last);
  else 
    __stable_sort_adaptive(__first, __last, buf.begin(),
                           _Distance(buf.size()));
}

template <class _RandomAccessIter, class _Tp, class _Distance, class _Compare>
inline void __stable_sort_aux(_RandomAccessIter __first,
                              _RandomAccessIter __last, _Tp*, _Distance*,
                              _Compare __comp) {
  _Temporary_buffer<_RandomAccessIter, _Tp> buf(__first, __last);
  if (buf.begin() == 0)
    __inplace_stable_sort(__first, __last, __comp);
  else 
    __stable_sort_adaptive(__first, __last, buf.begin(),
                           _Distance(buf.size()),
                           __comp);
}

template <class _RandomAccessIter>
inline void stable_sort(_RandomAccessIter __first,
                        _RandomAccessIter __last) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_REQUIRES(typename iterator_traits<_RandomAccessIter>::value_type,
                 _LessThanComparable);
  __stable_sort_aux(__first, __last,
                    __VALUE_TYPE(__first),
                    __DISTANCE_TYPE(__first));
}

template <class _RandomAccessIter, class _Compare>
inline void stable_sort(_RandomAccessIter __first,
                        _RandomAccessIter __last, _Compare __comp) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
       typename iterator_traits<_RandomAccessIter>::value_type,
       typename iterator_traits<_RandomAccessIter>::value_type);
  __stable_sort_aux(__first, __last,
                    __VALUE_TYPE(__first),
                    __DISTANCE_TYPE(__first), 
                    __comp);
}

// partial_sort, partial_sort_copy, and auxiliary functions.
//重新安排序列[first,last)，使序列前半部分middle-first个最小元素以递增顺序排序，并将其置于[first,middle)
//其余last-middle个元素不指定任何排序，并将其置于[middle,last)
//注意：迭代器middle是在[first,last)范围之内

/*
函数功能：Rearranges the elements in the range [first,last), 
in such a way that the elements before middle are the smallest elements in the entire range 
and are sorted in ascending order, while the remaining elements are left without any specific order.

函数原型：
default (1)	版本一 operator< 
	template <class RandomAccessIterator>
	void partial_sort (RandomAccessIterator first, RandomAccessIterator middle,
                     RandomAccessIterator last);
custom (2) 版本二 comp	
	template <class RandomAccessIterator, class Compare>
	void partial_sort (RandomAccessIterator first, RandomAccessIterator middle,
                     RandomAccessIterator last, Compare comp);
*/

template <class _RandomAccessIter, class _Tp>
void __partial_sort(_RandomAccessIter __first, _RandomAccessIter __middle,
                    _RandomAccessIter __last, _Tp*) {
		//利用heap的知识，在SGI STL中，是采用最大堆
		//将[first,middle)区间的元素创建成最大堆
		//再根据最大堆的性质，一个一个弹出堆，并将其保存，即堆排序
  make_heap(__first, __middle);//创建最大堆，定义与<stl_heap.h>文件
  //以下是在区间中[first,last)找出middle-first个最小元素
  //这里的是将后半部分[middle,last)的元素依次与最大堆的根节点元素(即堆的最大元素)比较
  //若小于堆的最大元素，则与堆的最大元素交换，并调整堆，使其依次成为最大堆
  //若不小于堆的最大元素，则不作任何操作
  for (_RandomAccessIter __i = __middle; __i < __last; ++__i)
    if (*__i < *__first) 
      __pop_heap(__first, __middle, __i, _Tp(*__i),
                 __DISTANCE_TYPE(__first));
  sort_heap(__first, __middle);//对最大堆进行堆排序
}
//版本一
template <class _RandomAccessIter>
inline void partial_sort(_RandomAccessIter __first,
                         _RandomAccessIter __middle,
                         _RandomAccessIter __last) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_REQUIRES(typename iterator_traits<_RandomAccessIter>::value_type,
                 _LessThanComparable);
  __partial_sort(__first, __middle, __last, __VALUE_TYPE(__first));
}

template <class _RandomAccessIter, class _Tp, class _Compare>
void __partial_sort(_RandomAccessIter __first, _RandomAccessIter __middle,
                    _RandomAccessIter __last, _Tp*, _Compare __comp) {
  make_heap(__first, __middle, __comp);
  for (_RandomAccessIter __i = __middle; __i < __last; ++__i)
    if (__comp(*__i, *__first))
      __pop_heap(__first, __middle, __i, _Tp(*__i), __comp,
                 __DISTANCE_TYPE(__first));
  sort_heap(__first, __middle, __comp);
}
//版本二
template <class _RandomAccessIter, class _Compare>
inline void partial_sort(_RandomAccessIter __first,
                         _RandomAccessIter __middle,
                         _RandomAccessIter __last, _Compare __comp) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool, 
      typename iterator_traits<_RandomAccessIter>::value_type,
      typename iterator_traits<_RandomAccessIter>::value_type);
  __partial_sort(__first, __middle, __last, __VALUE_TYPE(__first), __comp);
}
//partial_sort_copy与partial_sort的实现机制是相同，只是partial_sort_copy将元素排序后放在以result起始的容器中
template <class _InputIter, class _RandomAccessIter, class _Distance,
          class _Tp>
_RandomAccessIter __partial_sort_copy(_InputIter __first,
                                      _InputIter __last,
                                      _RandomAccessIter __result_first,
                                      _RandomAccessIter __result_last, 
                                      _Distance*, _Tp*) {
  if (__result_first == __result_last) return __result_last;
  _RandomAccessIter __result_real_last = __result_first;
  while(__first != __last && __result_real_last != __result_last) {
    *__result_real_last = *__first;
    ++__result_real_last;
    ++__first;
  }
  make_heap(__result_first, __result_real_last);
  while (__first != __last) {
    if (*__first < *__result_first) 
      __adjust_heap(__result_first, _Distance(0),
                    _Distance(__result_real_last - __result_first),
                    _Tp(*__first));
    ++__first;
  }
  sort_heap(__result_first, __result_real_last);
  return __result_real_last;
}

template <class _InputIter, class _RandomAccessIter>
inline _RandomAccessIter
partial_sort_copy(_InputIter __first, _InputIter __last,
                  _RandomAccessIter __result_first,
                  _RandomAccessIter __result_last) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_CONVERTIBLE(typename iterator_traits<_InputIter>::value_type,
                    typename iterator_traits<_RandomAccessIter>::value_type);
  __STL_REQUIRES(typename iterator_traits<_RandomAccessIter>::value_type,
                 _LessThanComparable);
  __STL_REQUIRES(typename iterator_traits<_InputIter>::value_type,
                 _LessThanComparable);
  return __partial_sort_copy(__first, __last, __result_first, __result_last, 
                             __DISTANCE_TYPE(__result_first),
                             __VALUE_TYPE(__first));
}

template <class _InputIter, class _RandomAccessIter, class _Compare,
          class _Distance, class _Tp>
_RandomAccessIter __partial_sort_copy(_InputIter __first,
                                         _InputIter __last,
                                         _RandomAccessIter __result_first,
                                         _RandomAccessIter __result_last,
                                         _Compare __comp, _Distance*, _Tp*) {
  if (__result_first == __result_last) return __result_last;
  _RandomAccessIter __result_real_last = __result_first;
  while(__first != __last && __result_real_last != __result_last) {
    *__result_real_last = *__first;
    ++__result_real_last;
    ++__first;
  }
  make_heap(__result_first, __result_real_last, __comp);
  while (__first != __last) {
    if (__comp(*__first, *__result_first))
      __adjust_heap(__result_first, _Distance(0),
                    _Distance(__result_real_last - __result_first),
                    _Tp(*__first),
                    __comp);
    ++__first;
  }
  sort_heap(__result_first, __result_real_last, __comp);
  return __result_real_last;
}

template <class _InputIter, class _RandomAccessIter, class _Compare>
inline _RandomAccessIter
partial_sort_copy(_InputIter __first, _InputIter __last,
                  _RandomAccessIter __result_first,
                  _RandomAccessIter __result_last, _Compare __comp) {
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_CONVERTIBLE(typename iterator_traits<_InputIter>::value_type,
                    typename iterator_traits<_RandomAccessIter>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
     typename iterator_traits<_RandomAccessIter>::value_type,
     typename iterator_traits<_RandomAccessIter>::value_type);  
  return __partial_sort_copy(__first, __last, __result_first, __result_last,
                             __comp,
                             __DISTANCE_TYPE(__result_first),
                             __VALUE_TYPE(__first));
}

// nth_element() and its auxiliary functions.  
//nth_element版本一辅助函数
template <class _RandomAccessIter, class _Tp>
void __nth_element(_RandomAccessIter __first, _RandomAccessIter __nth,
                   _RandomAccessIter __last, _Tp*) {
  while (__last - __first > 3) {//区间长度大于3
	  //获取分割点cut
    _RandomAccessIter __cut =
      __unguarded_partition(__first, __last,
                            _Tp(__median(*__first,
                                         *(__first + (__last - __first)/2),
                                         *(__last - 1))));
    if (__cut <= __nth)//若分割点小于指定位置，则nth位置在右半段
      __first = __cut;//再对右半段进行分割
    else //否则，对左半段进行分割
      __last = __cut;
  }
  __insertion_sort(__first, __last);
}
//重新排序序列[first,last)，使迭代器nth所指的元素，与“整个[first,last)序列完整排序后，同一位置的元素”同值.
//此外，必须保证[nth,last)内的所有元素不小于[first,nth)内的元素，但是对于序列[first,nth)和序列[nth,last)内的元素的排序顺序不能确定.

/*
函数功能：Rearranges the elements in the range [first,last), 
in such a way that the element at the nth position is the element that would be in that position in a sorted sequence.
函数原型：
default (1)	
	template <class RandomAccessIterator>
	void nth_element (RandomAccessIterator first, RandomAccessIterator nth,
                    RandomAccessIterator last);
custom (2)	
	template <class RandomAccessIterator, class Compare>
	void nth_element (RandomAccessIterator first, RandomAccessIterator nth,
                    RandomAccessIterator last, Compare comp);
*/
//nth_element版本一
template <class _RandomAccessIter>
inline void nth_element(_RandomAccessIter __first, _RandomAccessIter __nth,
                        _RandomAccessIter __last) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_REQUIRES(typename iterator_traits<_RandomAccessIter>::value_type,
                 _LessThanComparable);
  __nth_element(__first, __nth, __last, __VALUE_TYPE(__first));
}

template <class _RandomAccessIter, class _Tp, class _Compare>
void __nth_element(_RandomAccessIter __first, _RandomAccessIter __nth,
                   _RandomAccessIter __last, _Tp*, _Compare __comp) {
  while (__last - __first > 3) {
    _RandomAccessIter __cut =
      __unguarded_partition(__first, __last,
                            _Tp(__median(*__first,
                                         *(__first + (__last - __first)/2), 
                                         *(__last - 1),
                                         __comp)),
                            __comp);
    if (__cut <= __nth)
      __first = __cut;
    else 
      __last = __cut;
  }
  __insertion_sort(__first, __last, __comp);
}

template <class _RandomAccessIter, class _Compare>
inline void nth_element(_RandomAccessIter __first, _RandomAccessIter __nth,
                        _RandomAccessIter __last, _Compare __comp) {
  __STL_REQUIRES(_RandomAccessIter, _Mutable_RandomAccessIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
     typename iterator_traits<_RandomAccessIter>::value_type,
     typename iterator_traits<_RandomAccessIter>::value_type);
  __nth_element(__first, __nth, __last, __VALUE_TYPE(__first), __comp);
}


// Binary search (lower_bound, upper_bound, equal_range, binary_search).

template <class _ForwardIter, class _Tp, class _Distance>
_ForwardIter __lower_bound(_ForwardIter __first, _ForwardIter __last,
                           const _Tp& __val, _Distance*) 
{
  _Distance __len = 0;
  distance(__first, __last, __len);//求取整个区间的长度len
  _Distance __half;
  _ForwardIter __middle;//定义区间的中间迭代器

  while (__len > 0) {//若区间不为空，则在区间[first,last)开始查找value值
    __half = __len >> 1;//向右移一位，相当于除以2，即取区间的中间值
    __middle = __first;//middle初始化为区间的起始位置
    advance(__middle, __half);//middle向后移half位，此时middle为区间的中间值
    if (*__middle < __val) {//将value值与中间值比较，即是二分查找,若中间值小于value，则继续查找右半部分
		//下面两行令first指向middle的下一个位置
      __first = __middle;
      ++__first;
      __len = __len - __half - 1;//调整查找区间的长度
    }
    else
      __len = __half;//否则查找左半部分
  }
  return __first;
}
//在已排序区间[first,last)查找value值
//若该区间存在与value相等的元素，则返回指向第一个与value相等的迭代器
//若该区间不存在与value相等的元素，则返回指向第一个不小于value值的迭代器
//若该区间的任何元素都比value值小，则返回last
/*
函数功能：Returns an iterator pointing to the first element in the range [first,last) which does not compare less than val.
函数原型：
default (1)	：版本一采用operator<比较
	template <class ForwardIterator, class T>
	ForwardIterator lower_bound (ForwardIterator first, ForwardIterator last,
                               const T& val);
custom (2)	：版本二采用仿函数comp比较规则
	template <class ForwardIterator, class T, class Compare>
	ForwardIterator lower_bound (ForwardIterator first, ForwardIterator last,
                               const T& val, Compare comp);
*/
//版本一
template <class _ForwardIter, class _Tp>
inline _ForwardIter lower_bound(_ForwardIter __first, _ForwardIter __last,
				const _Tp& __val) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp,
      typename iterator_traits<_ForwardIter>::value_type);
  __STL_REQUIRES(_Tp, _LessThanComparable);
  return __lower_bound(__first, __last, __val,
                       __DISTANCE_TYPE(__first));
}

template <class _ForwardIter, class _Tp, class _Compare, class _Distance>
_ForwardIter __lower_bound(_ForwardIter __first, _ForwardIter __last,
                              const _Tp& __val, _Compare __comp, _Distance*)
{
  _Distance __len = 0;
  distance(__first, __last, __len);//求取整个区间的长度len
  _Distance __half;
  _ForwardIter __middle;//定义区间的中间迭代器

  while (__len > 0) {//若区间不为空，则在区间[first,last)开始查找value值
    __half = __len >> 1;//向右移一位，相当于除以2，即取区间的中间值
    __middle = __first;//middle初始化为区间的起始位置
    advance(__middle, __half);//middle向后移half位，此时middle为区间的中间值
    if (__comp(*__middle, __val)) {//若comp判断为true，则继续在右半部分查找
		//下面两行令first指向middle的下一个位置
      __first = __middle;
      ++__first;
      __len = __len - __half - 1;//调整查找区间的长度
    }
    else
      __len = __half;//否则查找左半部分
  }
  return __first;
}
//版本二：
template <class _ForwardIter, class _Tp, class _Compare>
inline _ForwardIter lower_bound(_ForwardIter __first, _ForwardIter __last,
                                const _Tp& __val, _Compare __comp) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp,
      typename iterator_traits<_ForwardIter>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool, _Tp, _Tp);
  return __lower_bound(__first, __last, __val, __comp,
                       __DISTANCE_TYPE(__first));
}

template <class _ForwardIter, class _Tp, class _Distance>
_ForwardIter __upper_bound(_ForwardIter __first, _ForwardIter __last,
                           const _Tp& __val, _Distance*)
{
  _Distance __len = 0;
  distance(__first, __last, __len);//求取整个区间的长度len
  _Distance __half;
  _ForwardIter __middle;//定义区间的中间迭代器

  while (__len > 0) {//若区间不为空，则在区间[first,last)开始查找value值
    __half = __len >> 1;//向右移一位，相当于除以2，即取区间的中间值
    __middle = __first;//middle初始化为区间的起始位置
    advance(__middle, __half);//middle向后移half位，此时middle为区间的中间值
    if (__val < *__middle)//若value小于中间元素值
      __len = __half;//查找左半部分
    else {
		//下面两行令first指向middle的下一个位置
      __first = __middle;
      ++__first;
      __len = __len - __half - 1;//更新len的值
    }
  }
  return __first;
}
//在已排序区间[first,last)查找value值
//返回大于value值的第一个元素的迭代器
/*
函数功能：Returns an iterator pointing to the first element in the range [first,last) which compares greater than val.
函数原型：
default (1)	：版本一采用operator<比较
	template <class ForwardIterator, class T>
	ForwardIterator upper_bound (ForwardIterator first, ForwardIterator last,
                               const T& val);
custom (2)	：版本二采用仿函数comp比较规则
	template <class ForwardIterator, class T, class Compare>
	ForwardIterator upper_bound (ForwardIterator first, ForwardIterator last,
                               const T& val, Compare comp);
*/
//版本一
template <class _ForwardIter, class _Tp>
inline _ForwardIter upper_bound(_ForwardIter __first, _ForwardIter __last,
                                const _Tp& __val) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp,
      typename iterator_traits<_ForwardIter>::value_type);
  __STL_REQUIRES(_Tp, _LessThanComparable);
  return __upper_bound(__first, __last, __val,
                       __DISTANCE_TYPE(__first));
}

template <class _ForwardIter, class _Tp, class _Compare, class _Distance>
_ForwardIter __upper_bound(_ForwardIter __first, _ForwardIter __last,
                           const _Tp& __val, _Compare __comp, _Distance*)
{
  _Distance __len = 0;
  distance(__first, __last, __len);
  _Distance __half;
  _ForwardIter __middle;

  while (__len > 0) {
    __half = __len >> 1;
    __middle = __first;
    advance(__middle, __half);
    if (__comp(__val, *__middle))
      __len = __half;
    else {
      __first = __middle;
      ++__first;
      __len = __len - __half - 1;
    }
  }
  return __first;
}
//版本二
template <class _ForwardIter, class _Tp, class _Compare>
inline _ForwardIter upper_bound(_ForwardIter __first, _ForwardIter __last,
                                const _Tp& __val, _Compare __comp) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp,
      typename iterator_traits<_ForwardIter>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool, _Tp, _Tp);
  return __upper_bound(__first, __last, __val, __comp,
                       __DISTANCE_TYPE(__first));
}
//函数举例
/*
	#include <iostream>     // std::cout
	#include <algorithm>    // std::lower_bound, std::upper_bound, std::sort
	#include <vector>       // std::vector

	int main () {
	  int myints[] = {10,20,30,30,20,10,10,20};
	  std::vector<int> v(myints,myints+8);           // 10 20 30 30 20 10 10 20

	  std::sort (v.begin(), v.end());                // 10 10 10 20 20 20 30 30

	  std::vector<int>::iterator low,up;
	  low=std::lower_bound (v.begin(), v.end(), 20); //          ^
	  up= std::upper_bound (v.begin(), v.end(), 20); //                   ^

	  std::cout << "lower_bound at position " << (low- v.begin()) << '\n';
	  std::cout << "upper_bound at position " << (up - v.begin()) << '\n';

	  return 0;
	}
	Output:
	lower_bound at position 3
	upper_bound at position 6
*/
template <class _ForwardIter, class _Tp, class _Distance>
pair<_ForwardIter, _ForwardIter>
__equal_range(_ForwardIter __first, _ForwardIter __last, const _Tp& __val,
              _Distance*)
{
  _Distance __len = 0;
  distance(__first, __last, __len);//计算区间的长度len
  _Distance __half;
  _ForwardIter __middle, __left, __right;

  while (__len > 0) {//若区间非空
    __half = __len >> 1;//len右移一位，相等于除以2，即half为区间的长度的一半
    __middle = __first;//初始化middle的值
    advance(__middle, __half);//前进middle位置，使其指向区间中间位置
    if (*__middle < __val) {//若指定元素value大于中间元素值，则在右半部分继续查找
		//下面两行使first指向middle的下一个位置，即右半区间的起始位置
      __first = __middle;
      ++__first;
      __len = __len - __half - 1;//更新待查找区间的长度
    }
    else if (__val < *__middle)//若指定元素value小于中间元素值，则在左半部分继续查找
      __len = __half;//更新待查找区间的长度
    else {//若指定元素value等于中间元素值
		//在前半部分找lower_bound位置
      __left = lower_bound(__first, __middle, __val);
      advance(__first, __len);
	  //在后半部分找upper_bound
      __right = upper_bound(++__middle, __first, __val);
      return pair<_ForwardIter, _ForwardIter>(__left, __right);//返回pair对象，第一个迭代器为left，第二个迭代器为right
    }
  }
  return pair<_ForwardIter, _ForwardIter>(__first, __first);
}
//查找区间与value相等的相邻重复元素的起始位置和结束位置
//注意：[first,last)是已排序，思想还是采用二分查找法
//同样也有两个版本
/*
函数功能：Returns the bounds of the subrange that includes all the elements of the range [first,last) with values equivalent to val.
函数原型：
default (1)	：版本一默认operator<
	template <class ForwardIterator, class T>
		pair<ForwardIterator,ForwardIterator>
    equal_range (ForwardIterator first, ForwardIterator last, const T& val);
custom (2)	：版本二采用仿函数comp
	template <class ForwardIterator, class T, class Compare>
		pair<ForwardIterator,ForwardIterator>
    equal_range (ForwardIterator first, ForwardIterator last, const T& val,
                  Compare comp);
*/
//版本一
template <class _ForwardIter, class _Tp>
inline pair<_ForwardIter, _ForwardIter>
equal_range(_ForwardIter __first, _ForwardIter __last, const _Tp& __val) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp, 
       typename iterator_traits<_ForwardIter>::value_type);
  __STL_REQUIRES(_Tp, _LessThanComparable);
  return __equal_range(__first, __last, __val,
                       __DISTANCE_TYPE(__first));
}

template <class _ForwardIter, class _Tp, class _Compare, class _Distance>
pair<_ForwardIter, _ForwardIter>
__equal_range(_ForwardIter __first, _ForwardIter __last, const _Tp& __val,
              _Compare __comp, _Distance*)
{
  _Distance __len = 0;
  distance(__first, __last, __len);
  _Distance __half;
  _ForwardIter __middle, __left, __right;

  while (__len > 0) {
    __half = __len >> 1;
    __middle = __first;
    advance(__middle, __half);
    if (__comp(*__middle, __val)) {
      __first = __middle;
      ++__first;
      __len = __len - __half - 1;
    }
    else if (__comp(__val, *__middle))
      __len = __half;
    else {
      __left = lower_bound(__first, __middle, __val, __comp);
      advance(__first, __len);
      __right = upper_bound(++__middle, __first, __val, __comp);
      return pair<_ForwardIter, _ForwardIter>(__left, __right);
    }
  }
  return pair<_ForwardIter, _ForwardIter>(__first, __first);
}           
//版本二
template <class _ForwardIter, class _Tp, class _Compare>
inline pair<_ForwardIter, _ForwardIter>
equal_range(_ForwardIter __first, _ForwardIter __last, const _Tp& __val,
            _Compare __comp) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp, 
       typename iterator_traits<_ForwardIter>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool, _Tp, _Tp);
  return __equal_range(__first, __last, __val, __comp,
                       __DISTANCE_TYPE(__first));
} 
//equal_range函数举例：
/*
	#include <iostream>     // std::cout
	#include <algorithm>    // std::equal_range, std::sort
	#include <vector>       // std::vector

	bool mygreater (int i,int j) { return (i>j); }

	int main () {
	  int myints[] = {10,20,30,30,20,10,10,20};
	  std::vector<int> v(myints,myints+8);                         // 10 20 30 30 20 10 10 20
	  std::pair<std::vector<int>::iterator,std::vector<int>::iterator> bounds;

	  // using default comparison:
	  std::sort (v.begin(), v.end());                              // 10 10 10 20 20 20 30 30
	  bounds=std::equal_range (v.begin(), v.end(), 20);            //          ^        ^
  
	  std::cout << "bounds at positions " << (bounds.first - v.begin());
	  std::cout << " and " << (bounds.second - v.begin()) << '\n';
  
	  // using "mygreater" as comp:
	  std::sort (v.begin(), v.end(), mygreater);                   // 30 30 20 20 20 10 10 10
	  bounds=std::equal_range (v.begin(), v.end(), 20, mygreater); //       ^        ^

	  std::cout << "bounds at positions " << (bounds.first - v.begin());
	  std::cout << " and " << (bounds.second - v.begin()) << '\n';

	  return 0;
	}
	Output：
	bounds at positions 3 and 6
	bounds at positions 2 and 5 
*/

//二分查找法
//注意：[first,last)是已排序
//同样也有两个版本
/*
函数功能：Returns true if any element in the range [first,last) is equivalent to val, and false otherwise.
函数原型：
default (1)	：版本一默认operator<
	template <class ForwardIterator, class T>
	bool binary_search (ForwardIterator first, ForwardIterator last,
                      const T& val);
custom (2)	：版本二采用仿函数comp
	template <class ForwardIterator, class T, class Compare>
	bool binary_search (ForwardIterator first, ForwardIterator last,
                      const T& val, Compare comp);
*/
template <class _ForwardIter, class _Tp>
bool binary_search(_ForwardIter __first, _ForwardIter __last,
                   const _Tp& __val) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp,
        typename iterator_traits<_ForwardIter>::value_type);
  __STL_REQUIRES(_Tp, _LessThanComparable);
  _ForwardIter __i = lower_bound(__first, __last, __val);//调用二分查找函数，并返回不小于value值的第一个迭代器位置i
  return __i != __last && !(__val < *__i);
}


template <class _ForwardIter, class _Tp, class _Compare>
bool binary_search(_ForwardIter __first, _ForwardIter __last,
                   const _Tp& __val,
                   _Compare __comp) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_SAME_TYPE(_Tp,
        typename iterator_traits<_ForwardIter>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool, _Tp, _Tp);
  _ForwardIter __i = lower_bound(__first, __last, __val, __comp);//调用二分查找函数，并返回不小于value值的第一个迭代器位置i
  return __i != __last && !__comp(__val, *__i);
}

// merge, with and without an explicitly supplied comparison function.
//将两个已排序的区间[first1,last1)和区间[first2,last2)合并
/*
函数功能：Combines the elements in the sorted ranges [first1,last1) and [first2,last2), 
into a new range beginning at result with all its elements sorted.

函数原型：
default (1)	：版本一
	template <class InputIterator1, class InputIterator2, class OutputIterator>
	OutputIterator merge (InputIterator1 first1, InputIterator1 last1,
                        InputIterator2 first2, InputIterator2 last2,
                        OutputIterator result);
custom (2)	：版本二
	template <class InputIterator1, class InputIterator2,
          class OutputIterator, class Compare>
	OutputIterator merge (InputIterator1 first1, InputIterator1 last1,
                        InputIterator2 first2, InputIterator2 last2,
                        OutputIterator result, Compare comp);
*/
//版本一：
template <class _InputIter1, class _InputIter2, class _OutputIter>
_OutputIter merge(_InputIter1 __first1, _InputIter1 __last1,
                  _InputIter2 __first2, _InputIter2 __last2,
                  _OutputIter __result) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_SAME_TYPE(
          typename iterator_traits<_InputIter1>::value_type,
          typename iterator_traits<_InputIter2>::value_type);
  __STL_REQUIRES(typename iterator_traits<_InputIter1>::value_type,
                 _LessThanComparable);
  //两个序列都尚未到达尾端，则执行while循环
  /*
  情况1：若序列二元素较小,则记录到目标区，且移动序列二的迭代器，但是序列一的迭代器不变.
  情况2：若序列一元素较小或相等,则记录到目标区，且移动序列一的迭代器，但是序列二的迭代器不变.
  最后：把剩余元素的序列复制到目标区
  */
  while (__first1 != __last1 && __first2 != __last2) {
	  //情况1
    if (*__first2 < *__first1) {//若序列二元素较小
      *__result = *__first2;//将元素记录到目标区
      ++__first2;//移动迭代器
    }
	//情况2
    else {//若序列一元素较小或相等
      *__result = *__first1;//将元素记录到目标区
      ++__first1;//移动迭代器
    }
    ++__result;//更新目标区位置，以便下次记录数据
  }
  //若有序列到达尾端，则把没到达尾端的序列剩余元素复制到目标区
  //此时，区间[first1,last1)和区间[first2,last2)至少一个必定为空
  return copy(__first2, __last2, copy(__first1, __last1, __result));
}
//版本二
template <class _InputIter1, class _InputIter2, class _OutputIter,
          class _Compare>
_OutputIter merge(_InputIter1 __first1, _InputIter1 __last1,
                  _InputIter2 __first2, _InputIter2 __last2,
                  _OutputIter __result, _Compare __comp) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES_SAME_TYPE(
          typename iterator_traits<_InputIter1>::value_type,
          typename iterator_traits<_InputIter2>::value_type);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
          typename iterator_traits<_InputIter1>::value_type,
          typename iterator_traits<_InputIter1>::value_type);
  while (__first1 != __last1 && __first2 != __last2) {
    if (__comp(*__first2, *__first1)) {
      *__result = *__first2;
      ++__first2;
    }
    else {
      *__result = *__first1;
      ++__first1;
    }
    ++__result;
  }
  return copy(__first2, __last2, copy(__first1, __last1, __result));
}
//merge函数举例：
/*
	#include <iostream>     // std::cout
	#include <algorithm>    // std::merge, std::sort
	#include <vector>       // std::vector

	int main () {
	  int first[] = {5,10,15,20,25};
	  int second[] = {50,40,30,20,10};
	  std::vector<int> v(10);

	  std::sort (first,first+5);
	  std::sort (second,second+5);
	  std::merge (first,first+5,second,second+5,v.begin());

	  std::cout << "The resulting vector contains:";
	  for (std::vector<int>::iterator it=v.begin(); it!=v.end(); ++it)
		std::cout << ' ' << *it;
	  std::cout << '\n';

	  return 0;
	}
	Output:
	The resulting vector contains: 5 10 10 15 20 20 25 30 40 50
*/

// inplace_merge and its auxiliary functions. 
//版本一的辅助函数，无缓冲区的操作
template <class _BidirectionalIter, class _Distance>
void __merge_without_buffer(_BidirectionalIter __first,
                            _BidirectionalIter __middle,
                            _BidirectionalIter __last,
                            _Distance __len1, _Distance __len2) {
  if (__len1 == 0 || __len2 == 0)
    return;
  if (__len1 + __len2 == 2) {
    if (*__middle < *__first)
      iter_swap(__first, __middle);
    return;
  }
  _BidirectionalIter __first_cut = __first;
  _BidirectionalIter __second_cut = __middle;
  _Distance __len11 = 0;
  _Distance __len22 = 0;
  if (__len1 > __len2) {
    __len11 = __len1 / 2;
    advance(__first_cut, __len11);
    __second_cut = lower_bound(__middle, __last, *__first_cut);
    distance(__middle, __second_cut, __len22);
  }
  else {
    __len22 = __len2 / 2;
    advance(__second_cut, __len22);
    __first_cut = upper_bound(__first, __middle, *__second_cut);
    distance(__first, __first_cut, __len11);
  }
  _BidirectionalIter __new_middle
    = rotate(__first_cut, __middle, __second_cut);
  __merge_without_buffer(__first, __first_cut, __new_middle,
                         __len11, __len22);
  __merge_without_buffer(__new_middle, __second_cut, __last, __len1 - __len11,
                         __len2 - __len22);
}

template <class _BidirectionalIter, class _Distance, class _Compare>
void __merge_without_buffer(_BidirectionalIter __first,
                            _BidirectionalIter __middle,
                            _BidirectionalIter __last,
                            _Distance __len1, _Distance __len2,
                            _Compare __comp) {
  if (__len1 == 0 || __len2 == 0)
    return;
  if (__len1 + __len2 == 2) {
    if (__comp(*__middle, *__first))
      iter_swap(__first, __middle);
    return;
  }
  _BidirectionalIter __first_cut = __first;
  _BidirectionalIter __second_cut = __middle;
  _Distance __len11 = 0;
  _Distance __len22 = 0;
  if (__len1 > __len2) {
    __len11 = __len1 / 2;
    advance(__first_cut, __len11);
    __second_cut = lower_bound(__middle, __last, *__first_cut, __comp);
    distance(__middle, __second_cut, __len22);
  }
  else {
    __len22 = __len2 / 2;
    advance(__second_cut, __len22);
    __first_cut = upper_bound(__first, __middle, *__second_cut, __comp);
    distance(__first, __first_cut, __len11);
  }
  _BidirectionalIter __new_middle
    = rotate(__first_cut, __middle, __second_cut);
  __merge_without_buffer(__first, __first_cut, __new_middle, __len11, __len22,
                         __comp);
  __merge_without_buffer(__new_middle, __second_cut, __last, __len1 - __len11,
                         __len2 - __len22, __comp);
}
//版本一的辅助函数，有缓冲区的操作
template <class _BidirectionalIter1, class _BidirectionalIter2,
          class _Distance>
_BidirectionalIter1 __rotate_adaptive(_BidirectionalIter1 __first,
                                      _BidirectionalIter1 __middle,
                                      _BidirectionalIter1 __last,
                                      _Distance __len1, _Distance __len2,
                                      _BidirectionalIter2 __buffer,
                                      _Distance __buffer_size) {
  _BidirectionalIter2 __buffer_end;
  if (__len1 > __len2 && __len2 <= __buffer_size) {//缓冲区足够放置序列二
    __buffer_end = copy(__middle, __last, __buffer);
    copy_backward(__first, __middle, __last);
    return copy(__buffer, __buffer_end, __first);
  }
  else if (__len1 <= __buffer_size) {//缓冲区足够放置序列一
    __buffer_end = copy(__first, __middle, __buffer);
    copy(__middle, __last, __first);
    return copy_backward(__buffer, __buffer_end, __last);
  }
  else//若缓冲区仍然不够，则调用STL算法rotate，不使用缓冲区
    return rotate(__first, __middle, __last);
}

template <class _BidirectionalIter1, class _BidirectionalIter2,
          class _BidirectionalIter3>
_BidirectionalIter3 __merge_backward(_BidirectionalIter1 __first1,
                                     _BidirectionalIter1 __last1,
                                     _BidirectionalIter2 __first2,
                                     _BidirectionalIter2 __last2,
                                     _BidirectionalIter3 __result) {
  if (__first1 == __last1)
    return copy_backward(__first2, __last2, __result);
  if (__first2 == __last2)
    return copy_backward(__first1, __last1, __result);
  --__last1;
  --__last2;
  while (true) {
    if (*__last2 < *__last1) {
      *--__result = *__last1;
      if (__first1 == __last1)
        return copy_backward(__first2, ++__last2, __result);
      --__last1;
    }
    else {
      *--__result = *__last2;
      if (__first2 == __last2)
        return copy_backward(__first1, ++__last1, __result);
      --__last2;
    }
  }
}

template <class _BidirectionalIter1, class _BidirectionalIter2,
          class _BidirectionalIter3, class _Compare>
_BidirectionalIter3 __merge_backward(_BidirectionalIter1 __first1,
                                     _BidirectionalIter1 __last1,
                                     _BidirectionalIter2 __first2,
                                     _BidirectionalIter2 __last2,
                                     _BidirectionalIter3 __result,
                                     _Compare __comp) {
  if (__first1 == __last1)
    return copy_backward(__first2, __last2, __result);
  if (__first2 == __last2)
    return copy_backward(__first1, __last1, __result);
  --__last1;
  --__last2;
  while (true) {
    if (__comp(*__last2, *__last1)) {
      *--__result = *__last1;
      if (__first1 == __last1)
        return copy_backward(__first2, ++__last2, __result);
      --__last1;
    }
    else {
      *--__result = *__last2;
      if (__first2 == __last2)
        return copy_backward(__first1, ++__last1, __result);
      --__last2;
    }
  }
}
//版本一的辅助函数，有缓冲区的操作
template <class _BidirectionalIter, class _Distance, class _Pointer>
void __merge_adaptive(_BidirectionalIter __first,
                      _BidirectionalIter __middle, 
                      _BidirectionalIter __last,
                      _Distance __len1, _Distance __len2,
                      _Pointer __buffer, _Distance __buffer_size) {
  if (__len1 <= __len2 && __len1 <= __buffer_size) {
	  //case1：把序列一放在缓冲区
    _Pointer __buffer_end = copy(__first, __middle, __buffer);
	//直接调用归并函数merge
    merge(__buffer, __buffer_end, __middle, __last, __first);
  }
  else if (__len2 <= __buffer_size) {
	  //case2：把序列二放在缓冲区
    _Pointer __buffer_end = copy(__middle, __last, __buffer);
    __merge_backward(__first, __middle, __buffer, __buffer_end, __last);
  }
  else {//case3：缓冲区不足放置任何一个序列
    _BidirectionalIter __first_cut = __first;
    _BidirectionalIter __second_cut = __middle;
    _Distance __len11 = 0;
    _Distance __len22 = 0;
    if (__len1 > __len2) {//若序列一比较长
      __len11 = __len1 / 2;//计算序列一的一半
      advance(__first_cut, __len11);//让first_cut指向序列一的中间位置
	  //找出*__first_cut在[middle,last)区间中的第一个不小于*__first_cut的元素位置
      __second_cut = lower_bound(__middle, __last, *__first_cut);
	  //计算middle到__second_cut之间的距离，保存在__len22
      distance(__middle, __second_cut, __len22); 
    }
    else {//若序列二比较长
      __len22 = __len2 / 2;//计算序列二的一半
      advance(__second_cut, __len22);//让__second_cut指向序列二的中间位置
	  //找出*__second_cut在[first,middle)区间中的第一个大于*__second_cut的元素位置
      __first_cut = upper_bound(__first, __middle, *__second_cut);
	  //计算__first到__first_cut之间的距离，保存在__len11
      distance(__first, __first_cut, __len11);
    }
    _BidirectionalIter __new_middle =
      __rotate_adaptive(__first_cut, __middle, __second_cut, __len1 - __len11,
                        __len22, __buffer, __buffer_size);
	//对左半段递归调用
    __merge_adaptive(__first, __first_cut, __new_middle, __len11,
                     __len22, __buffer, __buffer_size);
	//对右半段递归调用
    __merge_adaptive(__new_middle, __second_cut, __last, __len1 - __len11,
                     __len2 - __len22, __buffer, __buffer_size);
  }
}

template <class _BidirectionalIter, class _Distance, class _Pointer,
          class _Compare>
void __merge_adaptive(_BidirectionalIter __first, 
                      _BidirectionalIter __middle, 
                      _BidirectionalIter __last,
                      _Distance __len1, _Distance __len2,
                      _Pointer __buffer, _Distance __buffer_size,
                      _Compare __comp) {
  if (__len1 <= __len2 && __len1 <= __buffer_size) {
    _Pointer __buffer_end = copy(__first, __middle, __buffer);
    merge(__buffer, __buffer_end, __middle, __last, __first, __comp);
  }
  else if (__len2 <= __buffer_size) {
    _Pointer __buffer_end = copy(__middle, __last, __buffer);
    __merge_backward(__first, __middle, __buffer, __buffer_end, __last,
                     __comp);
  }
  else {
    _BidirectionalIter __first_cut = __first;
    _BidirectionalIter __second_cut = __middle;
    _Distance __len11 = 0;
    _Distance __len22 = 0;
    if (__len1 > __len2) {
      __len11 = __len1 / 2;
      advance(__first_cut, __len11);
      __second_cut = lower_bound(__middle, __last, *__first_cut, __comp);
      distance(__middle, __second_cut, __len22);   
    }
    else {
      __len22 = __len2 / 2;
      advance(__second_cut, __len22);
      __first_cut = upper_bound(__first, __middle, *__second_cut, __comp);
      distance(__first, __first_cut, __len11);
    }
    _BidirectionalIter __new_middle =
      __rotate_adaptive(__first_cut, __middle, __second_cut, __len1 - __len11,
                        __len22, __buffer, __buffer_size);
    __merge_adaptive(__first, __first_cut, __new_middle, __len11,
                     __len22, __buffer, __buffer_size, __comp);
    __merge_adaptive(__new_middle, __second_cut, __last, __len1 - __len11,
                     __len2 - __len22, __buffer, __buffer_size, __comp);
  }
}
//版本一的辅助函数
template <class _BidirectionalIter, class _Tp, class _Distance>
inline void __inplace_merge_aux(_BidirectionalIter __first,
                                _BidirectionalIter __middle,
                                _BidirectionalIter __last, _Tp*, _Distance*) {
  _Distance __len1 = 0;
  distance(__first, __middle, __len1);//计算序列一的长度
  _Distance __len2 = 0;
  distance(__middle, __last, __len2);//计算序列二的长度

  //使用暂时缓冲区
  _Temporary_buffer<_BidirectionalIter, _Tp> __buf(__first, __last);
  if (__buf.begin() == 0)//若缓冲区配置失败
	  //则调用不使用缓冲区的合并操作
    __merge_without_buffer(__first, __middle, __last, __len1, __len2);
  else//若分配成功
	  //则调用具有缓冲区的合并操作
    __merge_adaptive(__first, __middle, __last, __len1, __len2,
                     __buf.begin(), _Distance(__buf.size()));
}

template <class _BidirectionalIter, class _Tp, 
          class _Distance, class _Compare>
inline void __inplace_merge_aux(_BidirectionalIter __first,
                                _BidirectionalIter __middle,
                                _BidirectionalIter __last, _Tp*, _Distance*,
                                _Compare __comp) {
  _Distance __len1 = 0;
  distance(__first, __middle, __len1);
  _Distance __len2 = 0;
  distance(__middle, __last, __len2);

  _Temporary_buffer<_BidirectionalIter, _Tp> __buf(__first, __last);
  if (__buf.begin() == 0)
    __merge_without_buffer(__first, __middle, __last, __len1, __len2, __comp);
  else
    __merge_adaptive(__first, __middle, __last, __len1, __len2,
                     __buf.begin(), _Distance(__buf.size()),
                     __comp);
}
//将两个已排序的序列[first,middle)和[middle,last)合并成单一有序序列.
//若原来是增序，现在也是递增排序，若原来是递减排序，现在也是递减排序
/*
函数功能：Merges two consecutive sorted ranges: [first,middle) and [middle,last), 
putting the result into the combined sorted range [first,last).
函数原型：
default (1)	：版本一
	template <class BidirectionalIterator>
	void inplace_merge (BidirectionalIterator first, BidirectionalIterator middle,
                      BidirectionalIterator last);
custom (2)	：版本二
	template <class BidirectionalIterator, class Compare>
	void inplace_merge (BidirectionalIterator first, BidirectionalIterator middle,
                      BidirectionalIterator last, Compare comp);
*/
//版本一
template <class _BidirectionalIter>
inline void inplace_merge(_BidirectionalIter __first,
                          _BidirectionalIter __middle,
                          _BidirectionalIter __last) {
  __STL_REQUIRES(_BidirectionalIter, _Mutable_BidirectionalIterator);
  __STL_REQUIRES(typename iterator_traits<_BidirectionalIter>::value_type,
                 _LessThanComparable);
  if (__first == __middle || __middle == __last)//若有空序列，则之间返回
    return;
  __inplace_merge_aux(__first, __middle, __last,
                      __VALUE_TYPE(__first), __DISTANCE_TYPE(__first));
}
//版本二
template <class _BidirectionalIter, class _Compare>
inline void inplace_merge(_BidirectionalIter __first,
                          _BidirectionalIter __middle,
                          _BidirectionalIter __last, _Compare __comp) {
  __STL_REQUIRES(_BidirectionalIter, _Mutable_BidirectionalIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
           typename iterator_traits<_BidirectionalIter>::value_type,
           typename iterator_traits<_BidirectionalIter>::value_type);
  if (__first == __middle || __middle == __last)
    return;
  __inplace_merge_aux(__first, __middle, __last,
                      __VALUE_TYPE(__first), __DISTANCE_TYPE(__first),
                      __comp);
}
//inplace_merge函数举例：
/*
	#include <iostream>     // std::cout
	#include <algorithm>    // std::inplace_merge, std::sort, std::copy
	#include <vector>       // std::vector

	int main () {
	  int first[] = {5,10,15,20,25};
	  int second[] = {50,40,30,20,10};
	  std::vector<int> v(10);
	  std::vector<int>::iterator it;

	  std::sort (first,first+5);
	  std::sort (second,second+5);

	  it=std::copy (first, first+5, v.begin());
		 std::copy (second,second+5,it);

	  std::inplace_merge (v.begin(),v.begin()+5,v.end());

	  std::cout << "The resulting vector contains:";
	  for (it=v.begin(); it!=v.end(); ++it)
		std::cout << ' ' << *it;
	  std::cout << '\n';

	  return 0;
	}
	Output：
	The resulting vector contains: 5 10 10 15 20 20 25 30 40 50
*/

// Set algorithms: includes, set_union, set_intersection, set_difference,
// set_symmetric_difference.  All of these algorithms have the precondition
// that their input ranges are sorted and the postcondition that their output
// ranges are sorted.

/*
下面是计算set集合的相关算法，分别是并集set_union，差集set_difference，交集set_intersection
和对称差集set_symmetric_difference，这是个函数都提供了两个版本的函数原型
第一个版本是采用默认的排序比较方式 operator<
第二个版本是用户通过comp自行指定排序方式
注意：这四个算法接受的输入区间都是有序的，输出也是有序的
*/

// Set algorithms: includes, set_union, set_intersection, set_difference,
// set_symmetric_difference.  All of these algorithms have the precondition
// that their input ranges are sorted and the postcondition that their output
// ranges are sorted.

// 判断[first1, last1)是否包含[first2, last2),  
// 注意: 两个区间要保证有序,默认排序方式是operator<，若要自行定义排序方式，则调用第二版本;
template <class _InputIter1, class _InputIter2>
bool includes(_InputIter1 __first1, _InputIter1 __last1,
              _InputIter2 __first2, _InputIter2 __last2) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_REQUIRES(typename iterator_traits<_InputIter1>::value_type,
                 _LessThanComparable);
  while (__first1 != __last1 && __first2 != __last2)//遍历两个区间
    if (*__first2 < *__first1)//first2小于first1表示不包含
      return false;//返回FALSE
    else if(*__first1 < *__first2)//若first1小于first2 
      ++__first1;//寻找第一个区间下一个位置
    else
      ++__first1, ++__first2;//若first2等于first1,遍历两区间的下一位置

  return __first2 == __last2;//若第二个区间先到达尾端，则返回TRUE
}

//版本二：用户通过comp自行指定排序方式
template <class _InputIter1, class _InputIter2, class _Compare>
bool includes(_InputIter1 __first1, _InputIter1 __last1,
              _InputIter2 __first2, _InputIter2 __last2, _Compare __comp) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  while (__first1 != __last1 && __first2 != __last2)
    if (__comp(*__first2, *__first1))
      return false;
    else if(__comp(*__first1, *__first2)) 
      ++__first1;
    else
      ++__first1, ++__first2;

  return __first2 == __last2;
}

//两个集合区间的并集，同样也有两个版本
//求存在于[first1, last1)或存在于[first2, last2)内的所有元素
//注意：输入区间必须是已排序
/*
default (1)	:默认是operator<操作的排序方式
	template <class InputIterator1, class InputIterator2, class OutputIterator>
	OutputIterator set_union (InputIterator1 first1, InputIterator1 last1,
                            InputIterator2 first2, InputIterator2 last2,
                            OutputIterator result);
custom (2)	:用户指定的排序方式
	template <class InputIterator1, class InputIterator2,
          class OutputIterator, class Compare>
	OutputIterator set_union (InputIterator1 first1, InputIterator1 last1,
                            InputIterator2 first2, InputIterator2 last2,
                            OutputIterator result, Compare comp);
*/
//版本一：默认是operator<操作的排序方式
template <class _InputIter1, class _InputIter2, class _OutputIter>
_OutputIter set_union(_InputIter1 __first1, _InputIter1 __last1,
                      _InputIter2 __first2, _InputIter2 __last2,
                      _OutputIter __result) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_REQUIRES(typename iterator_traits<_InputIter1>::value_type,
                 _LessThanComparable);
  //两个区间都尚未到达区间尾端，执行以下操作
  while (__first1 != __last1 && __first2 != __last2) {
	  /*
	  在两区间内分别移动迭代器，首先将元素较小者(假设为A区)记录在目标区result
	  移动A区迭代器使其前进；同时另一个区的迭代器不变。然后进行一次新的比较，
	  记录较小值，移动迭代器...直到两区间中有一个到达尾端。若两区间存在元素相等，
	  默认记录第一区间的元素到目标区result.
	  */
    if (*__first1 < *__first2) {//first1小于first2
      *__result = *__first1;//则result初始值为first1
      ++__first1;//继续第一个区间的下一个元素位置
    }
    else if (*__first2 < *__first1) {//first2小于first1
      *__result = *__first2;//第二区间元素值记录到目标区
      ++__first2;//移动第二区间的迭代器
    }
    else {//若两区间存在相等的元素，把第一区间元素记录到目标区
		//同时移动两个区间的迭代器
      *__result = *__first1;
      ++__first1;
      ++__first2;
    }
    ++__result;//更新目标区位置，以备进入下一次记录操作操作
  }
  /*
  只要两区间之中有一个区间到达尾端，就结束上面的while循环
  以下将尚未到达尾端的区间剩余的元素拷贝到目标区
  此刻，[first1, last1)和[first2, last2)至少有一个是空区间
  */
  return copy(__first2, __last2, copy(__first1, __last1, __result));
}
//版本二：用户根据仿函数comp指定排序规则
template <class _InputIter1, class _InputIter2, class _OutputIter,
          class _Compare>
_OutputIter set_union(_InputIter1 __first1, _InputIter1 __last1,
                      _InputIter2 __first2, _InputIter2 __last2,
                      _OutputIter __result, _Compare __comp) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  while (__first1 != __last1 && __first2 != __last2) {
    if (__comp(*__first1, *__first2)) {
      *__result = *__first1;
      ++__first1;
    }
    else if (__comp(*__first2, *__first1)) {
      *__result = *__first2;
      ++__first2;
    }
    else {
      *__result = *__first1;
      ++__first1;
      ++__first2;
    }
    ++__result;
  }
  return copy(__first2, __last2, copy(__first1, __last1, __result));
}
/*例子:
	#include <iostream>     // std::cout
	#include <algorithm>    // std::set_union, std::sort
	#include <vector>       // std::vector

	int main () {
	  int first[] = {5,10,15,20,25};
	  int second[] = {50,40,30,20,10};
	  std::vector<int> v(10);                      // 0  0  0  0  0  0  0  0  0  0
	  std::vector<int>::iterator it;

	  std::sort (first,first+5);     //  5 10 15 20 25
	  std::sort (second,second+5);   // 10 20 30 40 50

	  it=std::set_union (first, first+5, second, second+5, v.begin());
												   // 5 10 15 20 25 30 40 50  0  0
	  v.resize(it-v.begin());                      // 5 10 15 20 25 30 40 50

	  std::cout << "The union has " << (v.size()) << " elements:\n";
	  for (it=v.begin(); it!=v.end(); ++it)
		std::cout << ' ' << *it;
	  std::cout << '\n';

	  return 0;
	}
	Output:
	The union has 8 elements:
	5 10 15 20 25 30 40 50
*/

 //两个集合区间的交集，同样也有两个版本
//求存在于[first1, last1)且存在于[first2, last2)内的所有元素
//注意：输入区间必须是已排序，输出区间的每个元素的相对排序和第一个区间相对排序相同
/*
default (1)	:默认是operator<操作的排序方式
		template <class InputIterator1, class InputIterator2, class OutputIterator>
		OutputIterator set_intersection (InputIterator1 first1, InputIterator1 last1,
                                   InputIterator2 first2, InputIterator2 last2,
                                   OutputIterator result);


custom (2)	:用户指定的排序方式
		template <class InputIterator1, class InputIterator2,
          class OutputIterator, class Compare>
		OutputIterator set_intersection (InputIterator1 first1, InputIterator1 last1,
                                   InputIterator2 first2, InputIterator2 last2,
                                   OutputIterator result, Compare comp);
*/
//版本一：默认是operator<操作的排序方式
template <class _InputIter1, class _InputIter2, class _OutputIter>
_OutputIter set_intersection(_InputIter1 __first1, _InputIter1 __last1,
                             _InputIter2 __first2, _InputIter2 __last2,
                             _OutputIter __result) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_REQUIRES(typename iterator_traits<_InputIter1>::value_type,
                 _LessThanComparable);
  //若两个区间都尚未到达尾端，则执行以下操作
  while (__first1 != __last1 && __first2 != __last2) 
	//在两个区间分别移动迭代器，直到遇到相等元素，记录到目标区
	//继续移动迭代器...直到两区间之中有到达尾端
    if (*__first1 < *__first2) //第一个区间元素小于第二区间元素
      ++__first1;//移动第一区间的迭代器，此时第二区间的迭代器不变
    else if (*__first2 < *__first1) //第二区间的元素小于第一区间元素
      ++__first2;//移动第二区间元素，此时第一区间的迭代器不变
    else {//若第一区间元素等于第二区间元素
      *__result = *__first1;//按第一区间的相对排序记录到目标区
	  //分别移动两区间的迭代器
      ++__first1;
      ++__first2;
	  //更新目标区迭代器，以便继续记录元素
      ++__result;
    }
	//若有区间到达尾部，则停止while循环
	//此时，返回目标区
  return __result;
}
//版本二：用户根据仿函数comp指定排序规则
template <class _InputIter1, class _InputIter2, class _OutputIter,
          class _Compare>
_OutputIter set_intersection(_InputIter1 __first1, _InputIter1 __last1,
                             _InputIter2 __first2, _InputIter2 __last2,
                             _OutputIter __result, _Compare __comp) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);

  while (__first1 != __last1 && __first2 != __last2)
    if (__comp(*__first1, *__first2))
      ++__first1;
    else if (__comp(*__first2, *__first1))
      ++__first2;
    else {
      *__result = *__first1;
      ++__first1;
      ++__first2;
      ++__result;
    }
  return __result;
}
/*例子：
	#include <iostream>     // std::cout
	#include <algorithm>    // std::set_intersection, std::sort
	#include <vector>       // std::vector

	int main () {
	  int first[] = {5,10,15,20,25};
	  int second[] = {50,40,30,20,10};
	  std::vector<int> v(10);                      // 0  0  0  0  0  0  0  0  0  0
	  std::vector<int>::iterator it;

	  std::sort (first,first+5);     //  5 10 15 20 25
	  std::sort (second,second+5);   // 10 20 30 40 50

	  it=std::set_intersection (first, first+5, second, second+5, v.begin());
												   // 10 20 0  0  0  0  0  0  0  0
	  v.resize(it-v.begin());                      // 10 20

	  std::cout << "The intersection has " << (v.size()) << " elements:\n";
	  for (it=v.begin(); it!=v.end(); ++it)
		std::cout << ' ' << *it;
	  std::cout << '\n';

	  return 0;
	}
	Output:
	The intersection has 2 elements:
	10 20
*/

 //两个集合区间的差集，同样也有两个版本
//求存在于[first1, last1)但不存在于[first2, last2)内的所有元素
//注意：输入区间必须是已排序，输出区间的每个元素的相对排序和第一个区间相对排序相同
/*
default (1)	:默认是operator<操作的排序方式
		template <class InputIterator1, class InputIterator2, class OutputIterator>
		OutputIterator set_difference (InputIterator1 first1, InputIterator1 last1,
                                 InputIterator2 first2, InputIterator2 last2,
                                 OutputIterator result);



custom (2)	:用户指定的排序方式
		template <class InputIterator1, class InputIterator2,
          class OutputIterator, class Compare>
		OutputIterator set_difference (InputIterator1 first1, InputIterator1 last1,
                                 InputIterator2 first2, InputIterator2 last2,
                                 OutputIterator result, Compare comp);
*/
//版本一：默认是operator<操作的排序方式
template <class _InputIter1, class _InputIter2, class _OutputIter>
_OutputIter set_difference(_InputIter1 __first1, _InputIter1 __last1,
                           _InputIter2 __first2, _InputIter2 __last2,
                           _OutputIter __result) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_REQUIRES(typename iterator_traits<_InputIter1>::value_type,
                 _LessThanComparable);
  //若两个区间都尚未到达尾端，则执行以下操作
  while (__first1 != __last1 && __first2 != __last2)
	 /*
	 在两个区间分别移动迭代器，当第一区间元素等于第二区间元素时，表示两区间共同存在该元素
	 则同时移动迭代器；
	 当第一区间元素大于第二区间元素时，就让第二区间迭代器前进；
	 第一区间元素小于第二区间元素时，把第一区间元素记录到目标区
	继续移动迭代器...直到两区间之中有到达尾端
	*/
    if (*__first1 < *__first2) {//第一区间元素小于第二区间元素
      *__result = *__first1;//把第一区间元素记录到目标区
      ++__first1;//移动第一区间迭代器
      ++__result;//跟新目标区，以便继续记录数据
    }
    else if (*__first2 < *__first1)//当第一区间的元素大于第二区间的元素
      ++__first2;//移动第二区间迭代器,注意：这里不记录任何元素
    else {//若两区间的元素相等时,同时移动两区间的迭代器
      ++__first1;
      ++__first2;
    }
	//若第二区间先到达尾端，则把第一区间剩余的元素复制到目标区
  return copy(__first1, __last1, __result);
}
//版本二：用户根据仿函数comp指定排序规则
template <class _InputIter1, class _InputIter2, class _OutputIter, 
          class _Compare>
_OutputIter set_difference(_InputIter1 __first1, _InputIter1 __last1,
                           _InputIter2 __first2, _InputIter2 __last2, 
                           _OutputIter __result, _Compare __comp) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);

  while (__first1 != __last1 && __first2 != __last2)
    if (__comp(*__first1, *__first2)) {
      *__result = *__first1;
      ++__first1;
      ++__result;
    }
    else if (__comp(*__first2, *__first1))
      ++__first2;
    else {
      ++__first1;
      ++__first2;
    }
  return copy(__first1, __last1, __result);
}
/*例子：
	#include <iostream>     // std::cout
	#include <algorithm>    // std::set_difference, std::sort
	#include <vector>       // std::vector

	int main () {
	  int first[] = {5,10,15,20,25};
	  int second[] = {50,40,30,20,10};
	  std::vector<int> v(10);                      // 0  0  0  0  0  0  0  0  0  0
	  std::vector<int>::iterator it;

	  std::sort (first,first+5);     //  5 10 15 20 25
	  std::sort (second,second+5);   // 10 20 30 40 50

	  it=std::set_difference (first, first+5, second, second+5, v.begin());
												   //  5 15 25  0  0  0  0  0  0  0
	  v.resize(it-v.begin());                      //  5 15 25

	  std::cout << "The difference has " << (v.size()) << " elements:\n";
	  for (it=v.begin(); it!=v.end(); ++it)
		std::cout << ' ' << *it;
	  std::cout << '\n';

	  return 0;
	}
	Output：
	The difference has 3 elements:
	5 15 25
*/

 //两个集合区间的对称差集，同样也有两个版本
//求存在于[first1, last1)但不存在于[first2, last2)内的所有元素以及出现在[first2, last2)但不出现在[first1, last1)
//注意：输入区间必须是已排序
/*
default (1)	:默认是operator<操作的排序方式
		template <class InputIterator1, class InputIterator2, class OutputIterator>
		OutputIterator set_symmetric_difference (InputIterator1 first1, InputIterator1 last1,
                                           InputIterator2 first2, InputIterator2 last2,
                                           OutputIterator result);


custom (2)	:用户指定的排序方式
		template <class InputIterator1, class InputIterator2,
          class OutputIterator, class Compare>
		OutputIterator set_symmetric_difference (InputIterator1 first1, InputIterator1 last1,
                                           InputIterator2 first2, InputIterator2 last2,
                                           OutputIterator result, Compare comp);
*/
//版本一：默认是operator<操作的排序方式
template <class _InputIter1, class _InputIter2, class _OutputIter>
_OutputIter 
set_symmetric_difference(_InputIter1 __first1, _InputIter1 __last1,
                         _InputIter2 __first2, _InputIter2 __last2,
                         _OutputIter __result) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_REQUIRES(typename iterator_traits<_InputIter1>::value_type,
                 _LessThanComparable);
  //若两个区间都尚未到达尾端，则执行下面的操作
  while (__first1 != __last1 && __first2 != __last2)
	  /*
	  情况1：若两区间元素相等，则同时移动两区间的迭代器.
	  情况2：若第一区间的元素小于第二区间元素，则把第一区间元素记录到目标区，且移动第一区间迭代器.
	  情况3：若第一区间的元素大于第二区间元素，则把第二区间元素记录到目标区，且移动第二区间迭代器.
	  */
    if (*__first1 < *__first2) {//属于情况2
      *__result = *__first1;//把第一区间元素记录到目标区
      ++__first1;//移动第一区间迭代器.此时第二区间迭代器不变
      ++__result;
    }
    else if (*__first2 < *__first1) {//属于情况3
      *__result = *__first2;//把第二区间元素记录到目标区
      ++__first2;//移动第二区间迭代器.此时第一区间迭代器不变
      ++__result;
    }
    else {//属于情况1
		//同时移动两区间的迭代器
      ++__first1;
      ++__first2;
    }
	 /*
  只要两区间之中有一个区间到达尾端，就结束上面的while循环
  以下将尚未到达尾端的区间剩余的元素拷贝到目标区
  此刻，[first1, last1)和[first2, last2)至少有一个是空区间
  */
  return copy(__first2, __last2, copy(__first1, __last1, __result));
}

//版本二：用户根据仿函数comp指定排序规则
template <class _InputIter1, class _InputIter2, class _OutputIter,
          class _Compare>
_OutputIter 
set_symmetric_difference(_InputIter1 __first1, _InputIter1 __last1,
                         _InputIter2 __first2, _InputIter2 __last2,
                         _OutputIter __result,
                         _Compare __comp) {
  __STL_REQUIRES(_InputIter1, _InputIterator);
  __STL_REQUIRES(_InputIter2, _InputIterator);
  __STL_REQUIRES(_OutputIter, _OutputIterator);
  __STL_REQUIRES_SAME_TYPE(
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
       typename iterator_traits<_InputIter1>::value_type,
       typename iterator_traits<_InputIter2>::value_type);
  while (__first1 != __last1 && __first2 != __last2)
    if (__comp(*__first1, *__first2)) {
      *__result = *__first1;
      ++__first1;
      ++__result;
    }
    else if (__comp(*__first2, *__first1)) {
      *__result = *__first2;
      ++__first2;
      ++__result;
    }
    else {
      ++__first1;
      ++__first2;
    }
  return copy(__first2, __last2, copy(__first1, __last1, __result));
}
/*例子：
	#include <iostream>     // std::cout
	#include <algorithm>    // std::set_symmetric_difference, std::sort
	#include <vector>       // std::vector

	int main () {
	  int first[] = {5,10,15,20,25};
	  int second[] = {50,40,30,20,10};
	  std::vector<int> v(10);                      // 0  0  0  0  0  0  0  0  0  0
	  std::vector<int>::iterator it;

	  std::sort (first,first+5);     //  5 10 15 20 25
	  std::sort (second,second+5);   // 10 20 30 40 50

	  it=std::set_symmetric_difference (first, first+5, second, second+5, v.begin());
												   //  5 15 25 30 40 50  0  0  0  0
	  v.resize(it-v.begin());                      //  5 15 25 30 40 50

	  std::cout << "The symmetric difference has " << (v.size()) << " elements:\n";
	  for (it=v.begin(); it!=v.end(); ++it)
		std::cout << ' ' << *it;
	  std::cout << '\n';

	  return 0;
	}
	Output：
	The symmetric difference has 6 elements:
	5 15 25 30 40 50
*/

// min_element and max_element, with and without an explicitly supplied
// comparison function.
//返回序列[first,last)中最大元素的位置
/*
default (1):版本一	
	template <class ForwardIterator>
	ForwardIterator max_element (ForwardIterator first, ForwardIterator last);
custom (2):版本二	
	template <class ForwardIterator, class Compare>
	ForwardIterator max_element (ForwardIterator first, ForwardIterator last,
                               Compare comp);
*/
//版本一：
template <class _ForwardIter>
_ForwardIter max_element(_ForwardIter __first, _ForwardIter __last) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES(typename iterator_traits<_ForwardIter>::value_type,
                 _LessThanComparable);
  if (__first == __last) return __first;//若为空，直接返回
  _ForwardIter __result = __first;//若不为空，从第一个元素开始，即把第一个元素暂时保存为最大值
  while (++__first != __last) //按顺序查找最大值
    if (*__result < *__first)//若有更大的值
      __result = __first;//则更新最大值位置
  return __result;//返回最大值位置
}
//版本二
template <class _ForwardIter, class _Compare>
_ForwardIter max_element(_ForwardIter __first, _ForwardIter __last,
			 _Compare __comp) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
    typename iterator_traits<_ForwardIter>::value_type,
    typename iterator_traits<_ForwardIter>::value_type);
  if (__first == __last) return __first;
  _ForwardIter __result = __first;
  while (++__first != __last) 
    if (__comp(*__result, *__first)) __result = __first;
  return __result;
}
//返回序列[first,last)中最小元素的位置
/*
default (1):版本一	
	template <class ForwardIterator>
	ForwardIterator min_element (ForwardIterator first, ForwardIterator last);
custom (2):版本二	
	template <class ForwardIterator, class Compare>
	ForwardIterator min_element (ForwardIterator first, ForwardIterator last,
                               Compare comp);
*/
//版本一：
template <class _ForwardIter>
_ForwardIter min_element(_ForwardIter __first, _ForwardIter __last) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES(typename iterator_traits<_ForwardIter>::value_type,
                 _LessThanComparable);
  if (__first == __last) return __first;//若为空，直接返回
  _ForwardIter __result = __first;//若不为空，从第一个元素开始，即把第一个元素暂时保存为最小值
  while (++__first != __last) //按顺序查找最小值
    if (*__first < *__result)//若存在更小的值
      __result = __first;//则更新最小值位置
  return __result;//返回最小值位置
}
//版本二
template <class _ForwardIter, class _Compare>
_ForwardIter min_element(_ForwardIter __first, _ForwardIter __last,
			 _Compare __comp) {
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
    typename iterator_traits<_ForwardIter>::value_type,
    typename iterator_traits<_ForwardIter>::value_type);
  if (__first == __last) return __first;
  _ForwardIter __result = __first;
  while (++__first != __last) 
    if (__comp(*__first, *__result))
      __result = __first;
  return __result;
}
//max_element和min_element函数举例
/*
	#include <iostream>     // std::cout
	#include <algorithm>    // std::min_element, std::max_element

	bool myfn(int i, int j) { return i<j; }

	struct myclass {
	  bool operator() (int i,int j) { return i<j; }
	} myobj;

	int main () {
	  int myints[] = {3,7,2,5,6,4,9};

	  // using default comparison:
	  std::cout << "The smallest element is " << *std::min_element(myints,myints+7) << '\n';
	  std::cout << "The largest element is "  << *std::max_element(myints,myints+7) << '\n';

	  // using function myfn as comp:
	  std::cout << "The smallest element is " << *std::min_element(myints,myints+7,myfn) << '\n';
	  std::cout << "The largest element is "  << *std::max_element(myints,myints+7,myfn) << '\n';

	  // using object myobj as comp:
	  std::cout << "The smallest element is " << *std::min_element(myints,myints+7,myobj) << '\n';
	  std::cout << "The largest element is "  << *std::max_element(myints,myints+7,myobj) << '\n';

	  return 0;
	}
	Output:
	The smallest element is 2
	The largest element is 9
	The smallest element is 2
	The largest element is 9
	The smallest element is 2
	The largest element is 9
*/

// next_permutation and prev_permutation, with and without an explicitly 
// supplied comparison function.
//next_permutation获取[first,last)区间所标示序列的下一个排列组合，若果没有下一个排序组合，则返回false;否则返回true;
/*
函数功能：Rearranges the elements in the range [first,last) into the next lexicographically greater permutation.
函数原型：
default (1)	：版本一采用less-than操作符
	template <class BidirectionalIterator>
	bool next_permutation (BidirectionalIterator first,
                         BidirectionalIterator last);
custom (2)	：版本二采用仿函数comp决定
	template <class BidirectionalIterator, class Compare>
	bool next_permutation (BidirectionalIterator first,
                         BidirectionalIterator last, Compare comp);
*/
//版本一
template <class _BidirectionalIter>
bool next_permutation(_BidirectionalIter __first, _BidirectionalIter __last) {
  __STL_REQUIRES(_BidirectionalIter, _BidirectionalIterator);
  __STL_REQUIRES(typename iterator_traits<_BidirectionalIter>::value_type,
                 _LessThanComparable);
  if (__first == __last)
    return false;//若为空，则返回false
  _BidirectionalIter __i = __first;
  ++__i;
  if (__i == __last)//区间只有一个元素
    return false;
  //若区间元素个数不小于两个
  __i = __last;//i指向尾端
  --__i;//不断后移

  for(;;) {
	  //下面两行是让ii和i成为相邻的元素
	  //其中i为第一个元素，ii为第二个元素
    _BidirectionalIter __ii = __i;//
    --__i;
	//以下在相邻元素判断
    if (*__i < *__ii) {//若前一个元素小于后一个元素，
		//则再从最尾端开始往前检查，找出第一个大于*i的元素，令该元素为*j，将*i和*j交换
		//再将ii之后的所有元素颠倒排序
      _BidirectionalIter __j = __last;//令j指向最尾端
      while (!(*__i < *--__j))//由尾端往前检查，直到遇到比*i大的元素
        {}
      iter_swap(__i, __j);//交换迭代器i和迭代器j所指的元素
      reverse(__ii, __last);//将ii之后的元素全部逆向重排
      return true;
    }
    if (__i == __first) {//进行到最前面
      reverse(__first, __last);//整个区间全部逆向重排
      return false;
    }
  }
}
//版本二
template <class _BidirectionalIter, class _Compare>
bool next_permutation(_BidirectionalIter __first, _BidirectionalIter __last,
                      _Compare __comp) {
  __STL_REQUIRES(_BidirectionalIter, _BidirectionalIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
    typename iterator_traits<_BidirectionalIter>::value_type,
    typename iterator_traits<_BidirectionalIter>::value_type);
  if (__first == __last)
    return false;
  _BidirectionalIter __i = __first;
  ++__i;
  if (__i == __last)
    return false;
  __i = __last;
  --__i;

  for(;;) {
    _BidirectionalIter __ii = __i;
    --__i;
    if (__comp(*__i, *__ii)) {
      _BidirectionalIter __j = __last;
      while (!__comp(*__i, *--__j))
        {}
      iter_swap(__i, __j);
      reverse(__ii, __last);
      return true;
    }
    if (__i == __first) {
      reverse(__first, __last);
      return false;
    }
  }
}
//next_permutation函数举例：
/*
	#include <iostream>     // std::cout
	#include <algorithm>    // std::next_permutation, std::sort

	int main () {
	  int myints[] = {1,2,3,4};

	  std::sort (myints,myints+4);

	  std::cout << "The 3! possible permutations with 3 elements:\n";
	  do {
		std::cout << myints[0] << ' ' << myints[1] << ' ' << myints[2] <<' ' << myints[3]<< '\n';
	  } while ( std::next_permutation(myints,myints+4) );
 
	  //std::next_permutation(myints,myints+4);
	  std::cout << "After loop: " << myints[0] << ' ' << myints[1] << ' ' << myints[2] << ' ' << myints[3]<<'\n';

	  return 0;
	}
	Output:
	The 3! possible permutations with 3 elements:
	1 2 3 4
	1 2 4 3
	1 3 2 4
	1 3 4 2
	1 4 2 3
	1 4 3 2
	2 1 3 4
	2 1 4 3
	2 3 1 4
	2 3 4 1
	2 4 1 3
	2 4 3 1
	3 1 2 4
	3 1 4 2
	3 2 1 4
	3 2 4 1
	3 4 1 2
	3 4 2 1
	4 1 2 3
	4 1 3 2
	4 2 1 3
	4 2 3 1
	4 3 1 2
	4 3 2 1
	After loop: 1 2 3 4
*/

//prev_permutation获取[first,last)区间所标示序列的上一个排列组合，若果没有上一个排序组合，则返回false;否则返回true;
/*
函数功能：Rearranges the elements in the range [first,last) into the previous lexicographically-ordered permutation.
函数原型：
default (1)	：版本一采用less-than操作符
	template <class BidirectionalIterator>
	bool prev_permutation (BidirectionalIterator first,
                         BidirectionalIterator last );
custom (2)	：版本二采用仿函数comp
	template <class BidirectionalIterator, class Compare>
	bool prev_permutation (BidirectionalIterator first,
                         BidirectionalIterator last, Compare comp);
*/
//版本一
template <class _BidirectionalIter>
bool prev_permutation(_BidirectionalIter __first, _BidirectionalIter __last) {
  __STL_REQUIRES(_BidirectionalIter, _BidirectionalIterator);
  __STL_REQUIRES(typename iterator_traits<_BidirectionalIter>::value_type,
                 _LessThanComparable);
  if (__first == __last)
    return false;//若区间为空，返回false
  _BidirectionalIter __i = __first;
  ++__i;
  if (__i == __last)//区间只有一个元素
    return false;//返回false
  //若区间元素个数不小于两个
  __i = __last;
  --__i;

  for(;;) {
	  //下面两行是让ii和i成为相邻的元素
	  //其中i为第一个元素，ii为第二个元素
    _BidirectionalIter __ii = __i;
    --__i;
	//以下在相邻元素判断
    if (*__ii < *__i) {//若前一个元素大于后一个元素，
		//则再从最尾端开始往前检查，找出第一个小于*i的元素，令该元素为*j，将*i和*j交换
		//再将ii之后的所有元素颠倒排序
      _BidirectionalIter __j = __last;//令j指向最尾端      
      while (!(*--__j < *__i))//由尾端往前检查，直到遇到比*i小的元素
        {}
      iter_swap(__i, __j); //交换迭代器i和迭代器j所指的元素
      reverse(__ii, __last);//将ii之后的元素全部逆向重排
      return true;
    }
    if (__i == __first) {//进行到最前面
      reverse(__first, __last);//把区间所有元素逆向重排
      return false;
    }
  }
}
//版本二
template <class _BidirectionalIter, class _Compare>
bool prev_permutation(_BidirectionalIter __first, _BidirectionalIter __last,
                      _Compare __comp) {
  __STL_REQUIRES(_BidirectionalIter, _BidirectionalIterator);
  __STL_BINARY_FUNCTION_CHECK(_Compare, bool,
    typename iterator_traits<_BidirectionalIter>::value_type,
    typename iterator_traits<_BidirectionalIter>::value_type);
  if (__first == __last)
    return false;
  _BidirectionalIter __i = __first;
  ++__i;
  if (__i == __last)
    return false;
  __i = __last;
  --__i;

  for(;;) {
    _BidirectionalIter __ii = __i;
    --__i;
    if (__comp(*__ii, *__i)) {
      _BidirectionalIter __j = __last;
      while (!__comp(*--__j, *__i))
        {}
      iter_swap(__i, __j);
      reverse(__ii, __last);
      return true;
    }
    if (__i == __first) {
      reverse(__first, __last);
      return false;
    }
  }
}
//prev_permutation函数举例
/*
	#include <iostream>     // std::cout
	#include <algorithm>    // std::next_permutation, std::sort, std::reverse

	int main () {
	  int myints[] = {1,2,3};

	  std::sort (myints,myints+3);
	  std::reverse (myints,myints+3);

	  std::cout << "The 3! possible permutations with 3 elements:\n";
	  do {
		std::cout << myints[0] << ' ' << myints[1] << ' ' << myints[2] << '\n';
	  } while ( std::prev_permutation(myints,myints+3) );

	  std::cout << "After loop: " << myints[0] << ' ' << myints[1] << ' ' << myints[2] << '\n';

	  return 0;
	}
	Output:
	The 3! possible permutations with 3 elements:
	3 2 1
	3 1 2
	2 3 1
	2 1 3
	1 3 2
	1 2 3
	After loop: 3 2 1
*/

// find_first_of, with and without an explicitly supplied comparison function.
//以[first2,last2)区间内的某些元素为查找目标，寻找他们在[first1,last1)区间首次出现的位置
//find_first_of函数有两个版本：
//版本一：提供默认的equality操作operator==
//版本二：提供用户自行指定的操作规则comp
/*
函数功能：Returns an iterator to the first element in the range [first1,last1) that matches any of the elements in [first2,last2). 
If no such element is found, the function returns last1.
函数原型：
equality (1)：版本一	
	template <class ForwardIterator1, class ForwardIterator2>
	ForwardIterator1 find_first_of (ForwardIterator1 first1, ForwardIterator1 last1,
                                   ForwardIterator2 first2, ForwardIterator2 last2);
predicate (2)：版本二	
	template <class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
	ForwardIterator1 find_first_of (ForwardIterator1 first1, ForwardIterator1 last1,
                                   ForwardIterator2 first2, ForwardIterator2 last2,
                                   BinaryPredicate pred);
*/
//版本一：提供默认的equality操作operator==
template <class _InputIter, class _ForwardIter>
_InputIter find_first_of(_InputIter __first1, _InputIter __last1,
                         _ForwardIter __first2, _ForwardIter __last2)
{
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool, 
     typename iterator_traits<_InputIter>::value_type,
     typename iterator_traits<_ForwardIter>::value_type);

  for ( ; __first1 != __last1; ++__first1) //若序列一不为空，则遍历序列一，每次指定一个元素
	  //以下，根据序列二的每个元素
    for (_ForwardIter __iter = __first2; __iter != __last2; ++__iter)
      if (*__first1 == *__iter)//若序列一的元素等于序列二的元素，则表示找到
        return __first1;//返回找到的位置
  return __last1;//否则没找到
}
//版本二：提供用户自行指定的操作规则comp
template <class _InputIter, class _ForwardIter, class _BinaryPredicate>
_InputIter find_first_of(_InputIter __first1, _InputIter __last1,
                         _ForwardIter __first2, _ForwardIter __last2,
                         _BinaryPredicate __comp)
{
  __STL_REQUIRES(_InputIter, _InputIterator);
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_BinaryPredicate, bool,
     typename iterator_traits<_InputIter>::value_type,
     typename iterator_traits<_ForwardIter>::value_type);

  for ( ; __first1 != __last1; ++__first1) 
    for (_ForwardIter __iter = __first2; __iter != __last2; ++__iter)
      if (__comp(*__first1, *__iter))
        return __first1;
  return __last1;
}
//find_first_of函数举例：
/*
	#include <iostream>     // std::cout
	#include <algorithm>    // std::find_first_of
	#include <vector>       // std::vector
	#include <cctype>       // std::tolower

	bool comp_case_insensitive (char c1, char c2) {
	  return (std::tolower(c1)==std::tolower(c2));
	}

	int main () {
	  int mychars[] = {'a','b','c','A','B','C'};
	  std::vector<char> haystack (mychars,mychars+6);
	  std::vector<char>::iterator it;

	  int needle[] = {'A','B','C'};

	  // using default comparison:
	  it = find_first_of (haystack.begin(), haystack.end(), needle, needle+3);

	  if (it!=haystack.end())
		std::cout << "The first match is: " << *it << '\n';

	  // using predicate comparison:
	  it = find_first_of (haystack.begin(), haystack.end(),
						  needle, needle+3, comp_case_insensitive);

	  if (it!=haystack.end())
		std::cout << "The first match is: " << *it << '\n';

	  return 0;
	}
	Output:
	The first match is: A
	The first match is: a
*/

// find_end, with and without an explicitly supplied comparison function.
// Search [first2, last2) as a subsequence in [first1, last1), and return
// the *last* possible match.  Note that find_end for bidirectional iterators
// is much faster than for forward iterators.

// find_end for forward iterators. 
//若萃取出来的迭代器类型为正向迭代器forward_iterator_tag，则调用此函数
template <class _ForwardIter1, class _ForwardIter2>
_ForwardIter1 __find_end(_ForwardIter1 __first1, _ForwardIter1 __last1,
                         _ForwardIter2 __first2, _ForwardIter2 __last2,
                         forward_iterator_tag, forward_iterator_tag)
{
  if (__first2 == __last2)//若第二个区间为空
    return __last1;//则直接返回第一个区间的尾端
  else {
    _ForwardIter1 __result = __last1;
    while (1) {
		//以下利用search函数查找出某个子序列的首次出现点；若找不到直接返回last1
      _ForwardIter1 __new_result

        = search(__first1, __last1, __first2, __last2);
      if (__new_result == __last1)//若返回的位置为尾端，则表示没找到
        return __result;//返回last1
      else {//若在[first1,last1)中找到[first2,last2)首次出现的位置，继续准备下一次查找
		  
        __result = __new_result;//更新返回的位置
        __first1 = __new_result;//更新查找的起始位置
        ++__first1;//确定正确查找起始位置
      }
    }
  }
}
//版本二：指定规则
template <class _ForwardIter1, class _ForwardIter2,
          class _BinaryPredicate>
_ForwardIter1 __find_end(_ForwardIter1 __first1, _ForwardIter1 __last1,
                         _ForwardIter2 __first2, _ForwardIter2 __last2,
                         forward_iterator_tag, forward_iterator_tag,
                         _BinaryPredicate __comp)
{
  if (__first2 == __last2)
    return __last1;
  else {
    _ForwardIter1 __result = __last1;
    while (1) {
      _ForwardIter1 __new_result
        = search(__first1, __last1, __first2, __last2, __comp);
      if (__new_result == __last1)
        return __result;
      else {
        __result = __new_result;
        __first1 = __new_result;
        ++__first1;
      }
    }
  }
}

// find_end for bidirectional iterators.  Requires partial specialization.
#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION
//若萃取出来的迭代器类型为双向迭代器bidirectional_iterator_tag，则调用此函数
template <class _BidirectionalIter1, class _BidirectionalIter2>
_BidirectionalIter1
__find_end(_BidirectionalIter1 __first1, _BidirectionalIter1 __last1,
           _BidirectionalIter2 __first2, _BidirectionalIter2 __last2,
           bidirectional_iterator_tag, bidirectional_iterator_tag)
{
  __STL_REQUIRES(_BidirectionalIter1, _BidirectionalIterator);
  __STL_REQUIRES(_BidirectionalIter2, _BidirectionalIterator);
  //利用反向迭代器很快就可以找到
  typedef reverse_iterator<_BidirectionalIter1> _RevIter1;
  typedef reverse_iterator<_BidirectionalIter2> _RevIter2;

  _RevIter1 __rlast1(__first1);
  _RevIter2 __rlast2(__first2);
  //查找时将序列一和序列二逆方向
  _RevIter1 __rresult = search(_RevIter1(__last1), __rlast1,
                               _RevIter2(__last2), __rlast2);

  if (__rresult == __rlast1)//表示没找到
    return __last1;
  else {//找到了
    _BidirectionalIter1 __result = __rresult.base();//转会正常迭代器
    advance(__result, -distance(__first2, __last2));//调整回到子序列的起始位置
    return __result;
  }
}
//版本二：指定规则comp
template <class _BidirectionalIter1, class _BidirectionalIter2,
          class _BinaryPredicate>
_BidirectionalIter1
__find_end(_BidirectionalIter1 __first1, _BidirectionalIter1 __last1,
           _BidirectionalIter2 __first2, _BidirectionalIter2 __last2,
           bidirectional_iterator_tag, bidirectional_iterator_tag, 
           _BinaryPredicate __comp)
{
  __STL_REQUIRES(_BidirectionalIter1, _BidirectionalIterator);
  __STL_REQUIRES(_BidirectionalIter2, _BidirectionalIterator);
  typedef reverse_iterator<_BidirectionalIter1> _RevIter1;
  typedef reverse_iterator<_BidirectionalIter2> _RevIter2;

  _RevIter1 __rlast1(__first1);
  _RevIter2 __rlast2(__first2);
  _RevIter1 __rresult = search(_RevIter1(__last1), __rlast1,
                               _RevIter2(__last2), __rlast2,
                               __comp);

  if (__rresult == __rlast1)
    return __last1;
  else {
    _BidirectionalIter1 __result = __rresult.base();
    advance(__result, -distance(__first2, __last2));
    return __result;
  }
}
#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

// Dispatching functions for find_end.
//find_end函数有两个版本：
//版本一：提供默认的equality操作operator==
//版本二：提供用户自行指定的操作规则comp
//注意：这里也有偏特化的知识
/*函数功能：Searches the range [first1,last1) for the last occurrence of the sequence defined by [first2,last2), 
and returns an iterator to its first element, or last1 if no occurrences are found.
函数原型：
equality (1)：版本一	
	template <class ForwardIterator1, class ForwardIterator2>
	ForwardIterator1 find_end (ForwardIterator1 first1, ForwardIterator1 last1,
                              ForwardIterator2 first2, ForwardIterator2 last2);
predicate (2)：版本二
	template <class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
	ForwardIterator1 find_end (ForwardIterator1 first1, ForwardIterator1 last1,
                              ForwardIterator2 first2, ForwardIterator2 last2,
                              BinaryPredicate pred);
*/
//对外接口的版本一
template <class _ForwardIter1, class _ForwardIter2>
inline _ForwardIter1 
find_end(_ForwardIter1 __first1, _ForwardIter1 __last1, 
         _ForwardIter2 __first2, _ForwardIter2 __last2)
{
  __STL_REQUIRES(_ForwardIter1, _ForwardIterator);
  __STL_REQUIRES(_ForwardIter2, _ForwardIterator);
  __STL_REQUIRES_BINARY_OP(_OP_EQUAL, bool,
   typename iterator_traits<_ForwardIter1>::value_type,
   typename iterator_traits<_ForwardIter2>::value_type);
  //首先通过iterator_traits萃取出first1和first2的迭代器类型
  //根据不同的迭代器类型调用不同的函数
  return __find_end(__first1, __last1, __first2, __last2,
                    __ITERATOR_CATEGORY(__first1),
                    __ITERATOR_CATEGORY(__first2));
}
//对外接口的版本一
template <class _ForwardIter1, class _ForwardIter2, 
          class _BinaryPredicate>
inline _ForwardIter1 
find_end(_ForwardIter1 __first1, _ForwardIter1 __last1, 
         _ForwardIter2 __first2, _ForwardIter2 __last2,
         _BinaryPredicate __comp)
{
  __STL_REQUIRES(_ForwardIter1, _ForwardIterator);
  __STL_REQUIRES(_ForwardIter2, _ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_BinaryPredicate, bool,
   typename iterator_traits<_ForwardIter1>::value_type,
   typename iterator_traits<_ForwardIter2>::value_type);
  //首先通过iterator_traits萃取出first1和first2的迭代器类型
  //根据不同的迭代器类型调用不同的函数
  return __find_end(__first1, __last1, __first2, __last2,
                    __ITERATOR_CATEGORY(__first1),
                    __ITERATOR_CATEGORY(__first2),
                    __comp);
}
//find_end函数举例：
/*
	#include <iostream>     // std::cout
	#include <algorithm>    // std::find_end
	#include <vector>       // std::vector

	bool myfunction (int i, int j) {
	  return (i==j);
	}

	int main () {
	  int myints[] = {1,2,3,4,5,1,2,3,4,5};
	  std::vector<int> haystack (myints,myints+10);

	  int needle1[] = {1,2,3};

	  // using default comparison:
	  std::vector<int>::iterator it;
	  it = std::find_end (haystack.begin(), haystack.end(), needle1, needle1+3);

	  if (it!=haystack.end())
		std::cout << "needle1 last found at position " << (it-haystack.begin()) << '\n';

	  int needle2[] = {4,5,1};

	  // using predicate comparison:
	  it = std::find_end (haystack.begin(), haystack.end(), needle2, needle2+3, myfunction);

	  if (it!=haystack.end())
		std::cout << "needle2 last found at position " << (it-haystack.begin()) << '\n';

	  return 0;
	}
	Output:
	needle1 found at position 5
	needle2 found at position 3
*/

// is_heap, a predicate testing whether or not a range is
// a heap.  This function is an extension, not part of the C++
// standard.

template <class _RandomAccessIter, class _Distance>
bool __is_heap(_RandomAccessIter __first, _Distance __n)
{
  _Distance __parent = 0;
  for (_Distance __child = 1; __child < __n; ++__child) {
    if (__first[__parent] < __first[__child]) 
      return false;
    if ((__child & 1) == 0)
      ++__parent;
  }
  return true;
}

template <class _RandomAccessIter, class _Distance, class _StrictWeakOrdering>
bool __is_heap(_RandomAccessIter __first, _StrictWeakOrdering __comp,
               _Distance __n)
{
  _Distance __parent = 0;
  for (_Distance __child = 1; __child < __n; ++__child) {
    if (__comp(__first[__parent], __first[__child]))
      return false;
    if ((__child & 1) == 0)
      ++__parent;
  }
  return true;
}

template <class _RandomAccessIter>
inline bool is_heap(_RandomAccessIter __first, _RandomAccessIter __last)
{
  __STL_REQUIRES(_RandomAccessIter, _RandomAccessIterator);
  __STL_REQUIRES(typename iterator_traits<_RandomAccessIter>::value_type,
                 _LessThanComparable);
  return __is_heap(__first, __last - __first);
}


template <class _RandomAccessIter, class _StrictWeakOrdering>
inline bool is_heap(_RandomAccessIter __first, _RandomAccessIter __last,
                    _StrictWeakOrdering __comp)
{
  __STL_REQUIRES(_RandomAccessIter, _RandomAccessIterator);
  __STL_BINARY_FUNCTION_CHECK(_StrictWeakOrdering, bool, 
         typename iterator_traits<_RandomAccessIter>::value_type, 
         typename iterator_traits<_RandomAccessIter>::value_type);
  return __is_heap(__first, __comp, __last - __first);
}

// is_sorted, a predicated testing whether a range is sorted in
// nondescending order.  This is an extension, not part of the C++
// standard.

template <class _ForwardIter>
bool is_sorted(_ForwardIter __first, _ForwardIter __last)
{
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_REQUIRES(typename iterator_traits<_ForwardIter>::value_type,
                 _LessThanComparable);
  if (__first == __last)
    return true;

  _ForwardIter __next = __first;
  for (++__next; __next != __last; __first = __next, ++__next) {
    if (*__next < *__first)
      return false;
  }

  return true;
}

template <class _ForwardIter, class _StrictWeakOrdering>
bool is_sorted(_ForwardIter __first, _ForwardIter __last,
               _StrictWeakOrdering __comp)
{
  __STL_REQUIRES(_ForwardIter, _ForwardIterator);
  __STL_BINARY_FUNCTION_CHECK(_StrictWeakOrdering, bool, 
        typename iterator_traits<_ForwardIter>::value_type,
        typename iterator_traits<_ForwardIter>::value_type);
  if (__first == __last)
    return true;

  _ForwardIter __next = __first;
  for (++__next; __next != __last; __first = __next, ++__next) {
    if (__comp(*__next, *__first))
      return false;
  }

  return true;
}

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1209
#endif

__STL_END_NAMESPACE

#endif /* __SGI_STL_INTERNAL_ALGO_H */

// Local Variables:
// mode:C++
// End:
