/******************************************************************************
 *                       ____    _    _____                                   *
 *                      / ___|  / \  |  ___|    C++                           *
 *                     | |     / _ \ | |_       Actor                         *
 *                     | |___ / ___ \|  _|      Framework                     *
 *                      \____/_/   \_|_|                                      *
 *                                                                            *
 * Copyright 2011-2020 Dominik Charousset                                     *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 *                                                                            *
 * If you did not receive a copy of the license files, see                    *
 * http://opensource.org/licenses/BSD-3-Clause and                            *
 * http://www.boost.org/LICENSE_1_0.txt.                                      *
 ******************************************************************************/

#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <utility>

#include "caf/detail/make_meta_object.hpp"
#include "caf/detail/meta_object.hpp"
#include "caf/span.hpp"
#include "caf/type_id.hpp"

namespace caf::detail {

template <uint16_t First, uint16_t Second>
struct type_id_pair {};

template <class Range, uint16_t... Is>
struct type_id_sequence_helper;

template <uint16_t End, uint16_t... Is>
struct type_id_sequence_helper<type_id_pair<End, End>, Is...> {
  using type = std::integer_sequence<uint16_t, Is...>;
};

template <uint16_t Begin, uint16_t End, uint16_t... Is>
struct type_id_sequence_helper<type_id_pair<Begin, End>, Is...> {
  using type = typename type_id_sequence_helper<type_id_pair<Begin + 1, End>,
                                                Is..., Begin>::type;
};

template <uint16_t Begin, uint16_t End>
using make_type_id_sequence =
  typename type_id_sequence_helper<type_id_pair<Begin, End>>::type;

} // namespace caf::detail

namespace caf {

/// @warning calling this after constructing any ::actor_system is unsafe and
///          causes undefined behavior.
template <uint16_t... Is>
void init_global_meta_objects_impl(std::integer_sequence<uint16_t, Is...>,
                                   uint16_t first_id) {
  detail::meta_object src[] = {
    detail::make_meta_object<type_by_id_t<Is>>(type_name_by_id_v<Is>)...,
  };
  detail::set_global_meta_objects(first_id, make_span(src));
}

/// Initializes the global meta object table with all types in `ProjectIds`.
/// @warning calling this after constructing any ::actor_system is unsafe and
///          causes undefined behavior.
template <class ProjectIds>
void init_global_meta_objects() {
  static constexpr uint16_t begin = ProjectIds::first;
  static constexpr uint16_t end = ProjectIds::last + 1;
  init_global_meta_objects_impl(detail::make_type_id_sequence<begin, end>{},
                                begin);
}

} // namespace caf
