// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2020, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2020, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \brief Provides exceptions used in the I/O module.
 * \author Hannes Hauswedell <hannes.hauswedell AT fu-berlin.de>
 */

#pragma once

#include <ios>
#include <stdexcept>

namespace seqan3
{

/*!\addtogroup io
 * \{
 */
// ----------------------------------------------------------------------------
// file open exceptions
// ----------------------------------------------------------------------------

//!\brief Thrown if there is no format that accepts a given file extension.
struct unhandled_extension_error : std::invalid_argument
{
    //!\brief Constructor that forwards the exception string.
    unhandled_extension_error(std::string const & s) : std::invalid_argument{s}
    {}
};

} // namespace seqan3
