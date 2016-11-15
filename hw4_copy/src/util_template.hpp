#ifndef UTIL_TEMPLATE_HPP
#define UTIL_TEMPLATE_HPP

#include <functional>
#include <algorithm>

/********************************* Template Util ******************************/

template<class T, class... ArgTypes>
using result_of_t = typename std::result_of<T(ArgTypes ...)>::type;

template<bool B, class T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

template<bool B, class T, class F>
using conditional_t = typename std::conditional<B, T, F>::type;

template<typename... Conds>
struct all_true : std::true_type {};

template<typename Cond, typename... Conds>
struct all_true<Cond, Conds ...> :
    conditional_t<Cond::value, all_true<Conds ...>, std::false_type>
{};

template<typename... A>
using are_arithmetic = all_true<std::is_arithmetic<A> ...>;

#endif
