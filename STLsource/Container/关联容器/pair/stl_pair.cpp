#ifndef __SGI_STL_INTERNAL_PAIR_H
#define __SGI_STL_INTERNAL_PAIR_H

__STL_BEGIN_NAMESPACE

/*
pair在关联容器中使用很广泛,它是STL的模板类型,可以存储两个成员变量
pair采用的是struct结构,struct的成员默认属性是public
*/
template <class _T1, class _T2>
struct pair {
  typedef _T1 first_type;
  typedef _T2 second_type;

  //pair的两个成员变量,其属性是public
  _T1 first;
  _T2 second;
  //以下是构造函数
  pair() : first(_T1()), second(_T2()) {}
  pair(const _T1& __a, const _T2& __b) : first(__a), second(__b) {}

#ifdef __STL_MEMBER_TEMPLATES
  //兼容性的拷贝构造函数
  //兼容性是指两个pair的类型可以不同,但是必须可以转换
  template <class _U1, class _U2>
  pair(const pair<_U1, _U2>& __p) : first(__p.first), second(__p.second) {}
  //注意:用pair初始化另一个pair时,只能通过拷贝构造函数进行,不能通过赋值进行
  //因为这里没有提供operator=操作符的重载
#endif
};

//operator==操作符重载
//两个pair相等时，意味着两个成员变量都对应相等
template <class _T1, class _T2>
inline bool operator==(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
{ 
  return __x.first == __y.first && __x.second == __y.second; 
}

//operator<操作符重载
//比较两个pair时,以第一个成员变量first为主,若第一个成员变量first不能判断表达式的大小
//则对其第二个成员变量second进行比较
template <class _T1, class _T2>
inline bool operator<(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
{ 
  return __x.first < __y.first || 
         (!(__y.first < __x.first) && __x.second < __y.second); 
}

#ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER

//下面的操作符重载都是基于上面operator<和operator==操作符的.
//operator!=,operator>,operator<=,operator>=操作符的重载
template <class _T1, class _T2>
inline bool operator!=(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y) {
  return !(__x == __y);
}

template <class _T1, class _T2>
inline bool operator>(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y) {
  return __y < __x;
}

template <class _T1, class _T2>
inline bool operator<=(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y) {
  return !(__y < __x);
}

template <class _T1, class _T2>
inline bool operator>=(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y) {
  return !(__x < __y);
}

#endif /* __STL_FUNCTION_TMPL_PARTIAL_ORDER */

//根据两个数值，构造一个pair
template <class _T1, class _T2>
inline pair<_T1, _T2> make_pair(const _T1& __x, const _T2& __y)
{
  return pair<_T1, _T2>(__x, __y);
}

__STL_END_NAMESPACE

#endif /* __SGI_STL_INTERNAL_PAIR_H */

// Local Variables:
// mode:C++
// End:
