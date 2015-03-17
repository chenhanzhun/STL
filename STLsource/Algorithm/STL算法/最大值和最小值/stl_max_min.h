
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