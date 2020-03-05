#pragma once

namespace seqan3::detail
{

template <typename value_type>
std::stringstream & stringify(std::stringstream & stream, value_type && value)
{
    stream << value;
    return stream;
}

template <typename option_type>
//!\cond
    requires named_enumeration<remove_cvref_t<option_type>>
//!\endcond
std::stringstream & stringify(std::stringstream & stream, option_type && op)
{
    for (auto & [key, value] : enumeration_names<option_type>)
    {
        if (op != value)
            continue;

        stream << key;
        return stream;
    }

    stream << "<UNKNOWN_VALUE>";
    return stream;
}

template <typename string_range_t>
std::string join_strings(string_range_t strings, std::string delim)
{
    std::ostringstream joined{};
    std::copy(strings.begin(), strings.end(), std::ostream_iterator<std::string>{joined, delim.c_str()});
    std::string join = joined.str();
    if (join.size() >= delim.size()) // remove last delimiter
        join.erase(join.size() - delim.size());
    return join;
}

template <typename ...value_type>
std::string as_string(value_type && ...values)
{
    std::stringstream stream{};
    (stringify(stream, std::forward<value_type>(values)), ...);
    return stream.str();
}

} // namespace seqan3::detail
