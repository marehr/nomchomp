// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2020, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2020, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \author Svenja Mehringer <svenja.mehringer AT fu-berlin.de>
 * \brief Provides helper functions for file operations.
 */

#pragma once

#include <fstream>

#include <seqan3/std/filesystem>

namespace seqan3::detail
{

/*!\brief Checks if the given path is writable.
 * \param path The path to check.
 * \returns `true` if writable, otherwise `false`.
 * \throws std::filesystem::filesystem_error on underlying OS API errors.
 */
inline bool writeable_file(std::filesystem::path const & path)
{
    std::ofstream file{path};
    bool write_permissions = file.is_open() && file.good();
    file.close();
    std::filesystem::remove(path);

    return write_permissions;
}

} // namespace seqan3::detail
