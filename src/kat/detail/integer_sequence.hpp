///////////////////////////////////////////////////////////////////////////////
//  Copyright (c)      2018 NVIDIA Corporation
//  Copyright (c) 2015-2018 Bryce Adelstein Lelbach aka wash
//  Copyright (c) 2020 Eyal Rozenberg
//
//  Distributed under the Boost Software License, Version 1.0. (See copy
//  at http://www.boost.org/LICENSE_1_0.txt)
///////////////////////////////////////////////////////////////////////////////

/** @file integer_sequence.hpp
 *
 * @brief An implementation of the C++14 standard library's `integer_sequence`
 * and associated helper aliases plus some extensions. Copied fromm NVIDIA's
 * thrust library's file `integer_sequence.h`, 2020-03-11.
 */

#ifndef CUDA_KAT_INTEGER_SEQUENCE_HPP_
#define CUDA_KAT_INTEGER_SEQUENCE_HPP_

#include <kat/common.hpp>

#include <type_traits>
#include <utility>
#include <cstdint>

namespace kat {

#if __cplusplus >= 201402L

// A compile-time sequence of integral constants of type T.
template <typename T, T... Is>
using integer_sequence = std::integer_sequence<T, Is...>;

// A compile-time sequence of size_t constants.
template <size_t... Is>
using index_sequence = std::index_sequence<Is...>;

// Create a new integer_sequence with elements 0, 1, 2, ..., N - 1.
template <typename T, size_t N>
using make_integer_sequence = std::make_integer_sequence<T, N>;

// Create a new index_sequence with elements 0, 1, 2, ..., N - 1.
template <size_t N>
using make_index_sequence = std::make_index_sequence<N>;

///////////////////////////////////////////////////////////////////////////////

#else // Older than C++14.

// A compile-time sequence of integral constants of type T.
template <typename T, T... Is>
struct integer_sequence;

// A compile-time sequence of size_t constants.
template <size_t... Is>
using index_sequence = integer_sequence<size_t, Is...>;

///////////////////////////////////////////////////////////////////////////////

namespace detail
{

// Create a new integer_sequence containing the elements of Sequence0 followed
// by the elements of Sequence1. Sequence0::size() is added to each element from
// Sequence1 in the new sequence.
template <typename Sequence0, typename Sequence1>
  struct merge_and_renumber_integer_sequences_impl;
template <typename Sequence0, typename Sequence1>
  using merge_and_renumber_integer_sequences =
      typename merge_and_renumber_integer_sequences_impl<
          Sequence0, Sequence1
      >::type;

// Create a new integer_sequence with elements 0, 1, 2, ..., N - 1.
template <typename T, size_t N>
  struct make_integer_sequence_impl;


} // namespace detail

///////////////////////////////////////////////////////////////////////////////

// Create a new integer_sequence with elements 0, 1, 2, ..., N - 1.
template <typename T, size_t N>
using make_integer_sequence =
  typename detail::make_integer_sequence_impl<T, N>::type;

// Create a new index_sequence with elements 0, 1, 2, ..., N - 1.
template <size_t N>
using make_index_sequence =
  make_integer_sequence<size_t, N>;

///////////////////////////////////////////////////////////////////////////////

template <typename T, T... Is>
struct integer_sequence
{
  using type = integer_sequence;
  using value_type = T;
  using size_type = size_t;

  KAT_HD
  static constexpr size_type size() noexcept
  {
    return sizeof...(Is);
  }
};
///////////////////////////////////////////////////////////////////////////////

namespace detail
{

template <typename T, T... Is0, T... Is1>
struct merge_and_renumber_integer_sequences_impl<
  integer_sequence<T, Is0...>, integer_sequence<T, Is1...>
>
{
  using type = integer_sequence<T, Is0..., (sizeof...(Is0) + Is1)...>;
};

///////////////////////////////////////////////////////////////////////////////

template <typename T, size_t N>
struct make_integer_sequence_impl
{
  using type = merge_and_renumber_integer_sequences<
    make_integer_sequence<T, N / 2>
  , make_integer_sequence<T, N - N / 2>
  >;
};

template <typename T>
struct make_integer_sequence_impl<T, 0>
{
  using type = integer_sequence<T>;
};

template <typename T>
struct make_integer_sequence_impl<T, 1>
{
  using type = integer_sequence<T, 0>;
};

} // namespace detail

#endif // THRUST_CPP_DIALECT >= 2014

///////////////////////////////////////////////////////////////////////////////

namespace detail
{

// Create a new integer_sequence containing the elements of Sequence0 followed
// by the elements of Sequence1. Sequence1::size() is added to each element from
// Sequence0 in the new sequence.
template <typename Sequence0, typename Sequence1>
  struct merge_and_renumber_reversed_integer_sequences_impl;
template <typename Sequence0, typename Sequence1>
  using merge_and_renumber_reversed_integer_sequences =
      typename merge_and_renumber_reversed_integer_sequences_impl<
          Sequence0, Sequence1
      >::type;

// Create a new integer_sequence with elements N - 1, N - 2, N - 3, ..., 0.
template <typename T, size_t N>
struct make_reversed_integer_sequence_impl;

// Add a new element to the front of an integer_sequence<>.
template <typename T, T I, typename Sequence>
struct integer_sequence_push_front_impl;

// Add a new element to the back of an integer_sequence<>.
template <typename T, T I, typename Sequence>
struct integer_sequence_push_back_impl;

}

///////////////////////////////////////////////////////////////////////////////

// Create a new integer_sequence with elements N - 1, N - 2, N - 3, ..., 0.
template <typename T, size_t N>
using make_reversed_integer_sequence =
  typename detail::make_reversed_integer_sequence_impl<T, N>::type;

// Create a new index_sequence with elements N - 1, N - 2, N - 3, ..., 0.
template <size_t N>
using make_reversed_index_sequence =
  make_reversed_integer_sequence<size_t, N>;

// Add a new element to the front of an integer_sequence<>.
template <typename T, T I, typename Sequence>
using integer_sequence_push_front =
  typename detail::integer_sequence_push_front_impl<T, I, Sequence>::type;

// Add a new element to the back of an integer_sequence<>.
template <typename T, T I, typename Sequence>
using integer_sequence_push_back =
  typename detail::integer_sequence_push_back_impl<T, I, Sequence>::type;

///////////////////////////////////////////////////////////////////////////////

namespace detail
{

template <typename T, T... Is0, T... Is1>
struct merge_and_renumber_reversed_integer_sequences_impl<
  integer_sequence<T, Is0...>, integer_sequence<T, Is1...>
>
{
  using type = integer_sequence<T, (sizeof...(Is1) + Is0)..., Is1...>;
};

///////////////////////////////////////////////////////////////////////////////

template <typename T, size_t N>
struct make_reversed_integer_sequence_impl
{
  using type = merge_and_renumber_reversed_integer_sequences<
      make_reversed_integer_sequence<T, N / 2>
    , make_reversed_integer_sequence<T, N - N / 2>
  >;
};

///////////////////////////////////////////////////////////////////////////////

template <typename T>
struct make_reversed_integer_sequence_impl<T, 0>
{
  using type = integer_sequence<T>;
};

template <typename T>
struct make_reversed_integer_sequence_impl<T, 1>
{
  using type = integer_sequence<T, 0>;
};

///////////////////////////////////////////////////////////////////////////////

template <typename T, T I0, T... Is>
struct integer_sequence_push_front_impl<T, I0, integer_sequence<T, Is...> >
{
  using type = integer_sequence<T, I0, Is...>;
};

///////////////////////////////////////////////////////////////////////////////

template <typename T, T I0, T... Is>
struct integer_sequence_push_back_impl<T, I0, integer_sequence<T, Is...> >
{
  using type = integer_sequence<T, Is..., I0>;
};

///////////////////////////////////////////////////////////////////////////////

} // namespace detail

} // namespace kat

#endif // CUDA_KAT_INTEGER_SEQUENCE_HPP_

