// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2020, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2020, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \brief Stream concepts.
 * \author Rene Rahn <rene.rahn AT fu-berlin.de>
 */

#pragma once

#include <seqan3/std/concepts>
#include <iosfwd>
#include <type_traits>

#include <seqan3/core/platform.hpp>

namespace seqan3
{
/*!\interface seqan3::output_stream_over <>
 * \ingroup stream
 * \brief Concept for output streams.
 *
 * An object is an output stream if it inherits from the [std::ios_base](https://en.cppreference.com/w/cpp/io/ios_base)
 * and supports the (un)formatted output function (`operator<<`) for a l-value of a given `value_type`.
 * It further needs to define the public member types as described in the [STD](https://en.cppreference.com/w/cpp/io/basic_ios).
 */
//!\cond
template <typename stream_type, typename value_type>
SEQAN3_CONCEPT output_stream_over = std::is_base_of_v<std::ios_base, std::remove_reference_t<stream_type>> &&
                         requires (stream_type & os, value_type & val)
{
    typename std::remove_reference_t<stream_type>::char_type;
    typename std::remove_reference_t<stream_type>::traits_type;
    typename std::remove_reference_t<stream_type>::int_type;
    typename std::remove_reference_t<stream_type>::pos_type;
    typename std::remove_reference_t<stream_type>::off_type;

    SEQAN3_RETURN_TYPE_CONSTRAINT(os << val,
                                  std::same_as,
                                  std::basic_ostream<typename std::remove_reference_t<stream_type>::char_type,
                                                     typename std::remove_reference_t<stream_type>::traits_type> &);
};
//!\endcond

/*!\interface seqan3::input_stream_over <>
 * \ingroup stream
 * \brief Concept for input streams.
 *
 * An object is an input stream if it inherits from the [std::ios_base](https://en.cppreference.com/w/cpp/io/ios_base)
 * and supports the (un)formatted input function (`operator>>`) for a l-value of a given `value_type`.
 * It further needs to define the public member types as described in the [STD](https://en.cppreference.com/w/cpp/io/basic_ios).
 */
//!\cond
template <typename stream_type, typename value_type>
SEQAN3_CONCEPT input_stream_over = std::is_base_of_v<std::ios_base, std::remove_reference_t<stream_type>> &&
                         requires (stream_type & is, value_type & val)
{
    typename std::remove_reference_t<stream_type>::char_type;
    typename std::remove_reference_t<stream_type>::traits_type;
    typename std::remove_reference_t<stream_type>::int_type;
    typename std::remove_reference_t<stream_type>::pos_type;
    typename std::remove_reference_t<stream_type>::off_type;


    SEQAN3_RETURN_TYPE_CONSTRAINT(is >> val,
                                  std::same_as,
                                  std::basic_istream<typename std::remove_reference_t<stream_type>::char_type,
                                                     typename std::remove_reference_t<stream_type>::traits_type> &);
};
//!\endcond

} // namespace seqan3
