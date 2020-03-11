// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2020, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2020, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \brief Provides various type traits on generic types.
 * \author Hannes Hauswedell <hannes.hauswedell AT fu-berlin.de>
 */

#pragma once

#include <tuple>
#include <seqan3/std/type_traits>

#include <seqan3/core/platform.hpp>

// ----------------------------------------------------------------------------
// is_constexpr
// ----------------------------------------------------------------------------

/*!\brief Returns true if the expression passed to this macro can be evaluated at compile time, false otherwise.
 * \ingroup type_traits
 * \returns true or false.
 */
#define SEQAN3_IS_CONSTEXPR(...) std::integral_constant<bool, __builtin_constant_p((__VA_ARGS__, 0))>::value

namespace seqan3
{
template <typename t>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<t>>;
}

namespace seqan3::detail
{

/*!\addtogroup type_traits
 * \{
 */

// ----------------------------------------------------------------------------
// deferred_type
// ----------------------------------------------------------------------------

/*!\brief Return the type identity; further arguments are ignored, but can make this type dependent if they are.
 * \implements seqan3::transformation_trait
 * \tparam t The type to operate on.
 * \tparam dependent_ts Any provided types are ignored.
 * \see seqan3::detail::deferred_type_t
 */
template <typename t, typename ...dependent_ts>
struct deferred_type
{
    //!\brief The type identity.
    using type = t;
};

/*!\brief Return the type identity; further arguments are ignored, but can make this type dependent if they are
 *        (transformation_trait shortcut).
 * \tparam t The type to operate on.
 * \tparam dependent_ts Any provided types are ignored.
 * \see seqan3::detail::deferred_type
 */
template <typename t, typename ...dependent_ts>
using deferred_type_t = typename deferred_type<t, dependent_ts...>::type;

//!\}

} // namespace seqan3::detail
