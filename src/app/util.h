//
// Created by bone on 06.04.18.
//

#pragma once

#include <algorithm>
#include <cassert>
#include <iterator>
#include <numeric>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

namespace minuseins::util {
constexpr unsigned int make_positive(const int num) {
  if (0 >= num) {
    return 0;
  } else {
    return static_cast<unsigned int>(num);
  }
}
constexpr unsigned int ensure_positive(const int num) {
  assert(0 <= num);
  return static_cast<unsigned int>(num);
}
namespace detail {
template <class Iterator>
struct enumerate_iterator {
  using iterator = Iterator;
  using index_type = typename std::iterator_traits<iterator>::difference_type;
  using reference = typename std::iterator_traits<iterator>::reference;

  enumerate_iterator(index_type index, iterator iterator)
      : index(index), iter(iterator) {}

  enumerate_iterator &operator++() {
    ++index;
    ++iter;
    return *this;
  }

  bool operator!=(const enumerate_iterator &other) const {
    return iter != other.iter;
  }

  std::pair<index_type &, reference> operator*() { return {index, *iter}; }

 private:
  index_type index;
  iterator iter;
};

template <class Iterator>
struct enumerate_range {
  using index_type = typename std::iterator_traits<Iterator>::difference_type;
  using iterator = enumerate_iterator<Iterator>;

  enumerate_range(Iterator first, Iterator last, index_type initial)
      : first(first), last(last), initial(initial) {}

  iterator begin() const { return iterator(initial, first); }

  iterator end() const { return iterator(0, last); }

 private:
  Iterator first;
  Iterator last;
  index_type initial;
};

}  // namespace detail
template <class Iterator>
decltype(auto) enumerate(
    Iterator first, Iterator last,
    typename std::iterator_traits<Iterator>::difference_type initial) {
  return detail::enumerate_range(first, last, initial);
}

template <class Container>
decltype(auto) enumerate(Container &content) {
  return detail::enumerate_range(std::begin(content), std::end(content), 0);
}

template <typename IntType>
std::vector<IntType> range(IntType stop) {
  std::vector<IntType> x(stop);
  std::iota(x.begin(), x.end(), IntType(0));
  return x;
}
}  // namespace minuseins::util