// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2020, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2020, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

#include <fstream>

#include <gtest/gtest.h>

#include <seqan3/argument_parser/argument_parser.hpp>
#include <seqan3/argument_parser/detail/format_help.hpp>
#include <seqan3/range/views/get.hpp>

// reused global variables
int option_value{5};
bool flag_value{};
std::vector<std::string> pos_opt_value{};
const char * argv0[] = {"./help_add_test --version-check 0"};
const char * argv1[] = {"./help_add_test --version-check 0", "-h"};
const char * argv2[] = {"./help_add_test --version-check 0", "-hh"};
const char * argv3[] = {"./help_add_test --version-check 0", "--version"};

std::string const basic_options_str = "OPTIONS\n"
                                      "\n"
                                      "  Basic options:\n"
                                      "    -h, --help\n"
                                      "          Prints the help page.\n"
                                      "    -hh, --advanced-help\n"
                                      "          Prints the help page including advanced options.\n"
                                      "    --version\n"
                                      "          Prints the version information.\n"
                                      "    --copyright\n"
                                      "          Prints the copyright/license information.\n"
                                      "    --export-help (std::string)\n"
                                      "          Export the help page information. Value must be one of [html, man].\n"
                                      "    --version-check (bool)\n"
                                      "          Whether to to check for the newest app version. Default: 1.\n";

std::string const version_str = std::to_string(SEQAN3_VERSION_MAJOR) + "."
                                + std::to_string(SEQAN3_VERSION_MINOR) + "."
                                + std::to_string(SEQAN3_VERSION_PATCH);

std::string const basic_version_str = "VERSION\n"
                                      "    Last update: \n"
                                      "    test_parser version: \n"
                                      "    SeqAn version: " + version_str + "\n";

std::string license_text()
{
    std::ifstream license_file{std::string{{SEQAN3_TEST_LICENSE_DIR}} + "/LICENSE.md"};
    std::stringstream buffer;
    buffer << license_file.rdbuf();

    std::string str = buffer.str();
    size_t license_start = str.find("```\n") + 4;
    size_t license_end = str.find("```", license_start);

    return str.substr(license_start, license_end - license_start);
}

TEST(help_page_printing, short_help)
{
    // Empty call with no options given. For seqan3::detail::format_short_help
    seqan3::argument_parser parser0{"empty_options", 1, argv0};
    parser0.info.synopsis.push_back("./some_binary_name synopsis");

    testing::internal::CaptureStdout();
    EXPECT_EXIT(parser0.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
    std::string std_cout = testing::internal::GetCapturedStdout();

    std::string expected = "empty_options\n"
                           "=============\n"
                           "    ./some_binary_name synopsis\n"
                           "    Try -h or --help for more information.\n";
    EXPECT_EQ(std_cout, expected);
}

TEST(help_page_printing, no_information)
{
    // Empty help call with -h
    seqan3::argument_parser parser1{"test_parser", 2, argv1};

    testing::internal::CaptureStdout();
    EXPECT_EXIT(parser1.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
    std::string std_cout = testing::internal::GetCapturedStdout();

    std::string expected = "test_parser\n"
                           "===========\n"
                           "\n" +
                           basic_options_str +
                           "\n"
                           "  \n"
                           "\n" +
                           basic_version_str;
    EXPECT_EQ(std_cout, expected);
}

TEST(help_page_printing, with_short_copyright)
{
    // Again, but with short copyright, long copyright, and citation.
    seqan3::argument_parser short_copy("test_parser", 2, argv1);
    short_copy.info.short_copyright = "short";

    testing::internal::CaptureStdout();
    EXPECT_EXIT(short_copy.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
    std::string std_cout = testing::internal::GetCapturedStdout();

    std::string expected = "test_parser\n"
                           "===========\n"
                           "\n" +
                           basic_options_str +
                           "\n"
                           "  \n"
                           "\n" +
                           basic_version_str +
                           "\n" +
                           "LEGAL\n"
                           "    test_parser Copyright: short\n"
                           "    SeqAn Copyright: 2006-2015 Knut Reinert, FU-Berlin; released under the 3-clause BSDL.\n";
    EXPECT_EQ(std_cout, expected);
}

TEST(help_page_printing, with_long_copyright)
{
    seqan3::argument_parser long_copy("test_parser", 2, argv1);
    long_copy.info.long_copyright = "long";

    testing::internal::CaptureStdout();
    EXPECT_EXIT(long_copy.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
    std::string std_cout = testing::internal::GetCapturedStdout();

    std::string expected = "test_parser\n"
                           "===========\n"
                           "\n" +
                           basic_options_str +
                           "\n"
                           "  \n"
                           "\n" +
                           basic_version_str +
                           "\n" +
                           "LEGAL\n"
                           "    SeqAn Copyright: 2006-2015 Knut Reinert, FU-Berlin; released under the 3-clause BSDL.\n"
                           "    For full copyright and/or warranty information see --copyright.\n";
    EXPECT_EQ(std_cout, expected);
}

TEST(help_page_printing, with_citation)
{
    seqan3::argument_parser citation("test_parser", 2, argv1);
    citation.info.citation = "citation";

    testing::internal::CaptureStdout();
    EXPECT_EXIT(citation.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
    std::string std_cout = testing::internal::GetCapturedStdout();

    std::string expected = "test_parser\n"
                           "===========\n"
                           "\n" +
                           basic_options_str +
                           "\n"
                           "  \n"
                           "\n" +
                           basic_version_str +
                           "\n" +
                           "LEGAL\n"
                           "    SeqAn Copyright: 2006-2015 Knut Reinert, FU-Berlin; released under the 3-clause BSDL.\n"
                           "    In your academic works please cite: citation\n";
    EXPECT_EQ(std_cout, expected);
}

TEST(help_page_printing, empty_advanced_help)
{
    // Empty help call with -hh
    seqan3::argument_parser parser2{"test_parser", 2, argv2};

    testing::internal::CaptureStdout();
    EXPECT_EXIT(parser2.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
    std::string std_cout = testing::internal::GetCapturedStdout();

    std::string expected = "test_parser\n"
                           "===========\n"
                           "\n" +
                           basic_options_str +
                           "\n"
                           "  \n"
                           "\n" +
                           basic_version_str;
    EXPECT_EQ(std_cout, expected);
}

TEST(help_page_printing, empty_version_call)
{
    // Empty version call
    seqan3::argument_parser parser3{"test_parser", 2, argv3};

    testing::internal::CaptureStdout();
    EXPECT_EXIT(parser3.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
    std::string std_cout = testing::internal::GetCapturedStdout();

    std::string expected = "test_parser\n"
                           "===========\n"
                           "\n" +
                           basic_version_str;
    EXPECT_EQ(std_cout, expected);
}

TEST(help_page_printing, version_call)
{
    // Version call with url and options.
    seqan3::argument_parser parser4{"test_parser", 2, argv3};
    parser4.info.url = "www.seqan.de";
    parser4.add_option(option_value, 'i', "int", "this is a int option.");
    parser4.add_flag(flag_value, 'f', "flag", "this is a flag.");
    parser4.add_positional_option(pos_opt_value, "this is a positional option.");

    testing::internal::CaptureStdout();
    EXPECT_EXIT(parser4.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
    std::string std_cout = testing::internal::GetCapturedStdout();

    std::string expected = "test_parser\n"
                           "===========\n"
                           "\n" +
                           basic_version_str +
                           "URL\n"
                           "    www.seqan.de\n"
                           "\n";
    EXPECT_EQ(std_cout, expected);
}

TEST(help_page_printing, do_not_print_hidden_options)
{
    // Add an option and request help.
    seqan3::argument_parser parser5{"test_parser", 2, argv1};
    parser5.add_option(option_value, 'i', "int", "this is a int option.", seqan3::option_spec::HIDDEN);
    parser5.add_flag(flag_value, 'f', "flag", "this is a flag.", seqan3::option_spec::HIDDEN);

    testing::internal::CaptureStdout();
    EXPECT_EXIT(parser5.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
    std::string std_cout = testing::internal::GetCapturedStdout();

    std::string expected = "test_parser\n"
                           "===========\n"
                           "\n" +
                           basic_options_str +
                           "\n"
                           "  \n"
                           "\n" +
                           basic_version_str;
    EXPECT_EQ(std_cout, expected);
}

enum class foo
{
    one,
    two,
    three
};

auto enumeration_names(foo)
{
    return std::unordered_map<std::string_view, foo>{{"one", foo::one}, {"two", foo::two}, {"three", foo::three}};
}

TEST(help_page_printing, full_information)
{
    int8_t required_option{};
    int8_t non_list_optional{1};
    foo enum_option_value{};

    // Add synopsis, description, short description, positional option, option, flag, and example.
    seqan3::argument_parser parser6{"test_parser", 2, argv1};
    parser6.info.synopsis.push_back("./some_binary_name synopsis");
    parser6.info.synopsis.push_back("./some_binary_name synopsis2");
    parser6.info.description.push_back("description");
    parser6.info.description.push_back("description2");
    parser6.info.short_description = "so short";
    parser6.add_option(option_value, 'i', "int", "this is a int option.");
    parser6.add_option(enum_option_value, 'e', "enum", "this is an enum option.", seqan3::option_spec::DEFAULT,
                       seqan3::value_list_validator{seqan3::enumeration_names<foo> | seqan3::views::get<1>});
    parser6.add_option(required_option, 'r', "required-int", "this is another int option.",
                       seqan3::option_spec::REQUIRED);
    parser6.add_section("Flags");
    parser6.add_subsection("SubFlags");
    parser6.add_line("here come all the flags");
    parser6.add_flag(flag_value, 'f', "flag", "this is a flag.");
    parser6.add_positional_option(non_list_optional, "this is not a list.");
    parser6.add_positional_option(pos_opt_value, "this is a positional option.");
    parser6.info.examples.push_back("example");
    parser6.info.examples.push_back("example2");

    testing::internal::CaptureStdout();
    EXPECT_EXIT(parser6.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
    std::string std_cout = testing::internal::GetCapturedStdout();

    std::string expected = "test_parser - so short\n"
                           "======================\n"
                           "\n"
                           "SYNOPSIS\n"
                           "    ./some_binary_name synopsis\n"
                           "    ./some_binary_name synopsis2\n"
                           "\n"
                           "DESCRIPTION\n"
                           "    description\n"
                           "\n"
                           "    description2\n"
                           "\n"
                           "POSITIONAL ARGUMENTS\n"
                           "    ARGUMENT-1 (signed 8 bit integer)\n"
                           "          this is not a list.\n"
                           "    ARGUMENT-2 (List of std::string's)\n"
                           "          this is a positional option. Default: [].\n"
                           "\n" +
                           basic_options_str +
                           "\n"
                           "  \n"
                           "    -i, --int (signed 32 bit integer)\n"
                           "          this is a int option. Default: 5.\n"
                           "    -e, --enum (foo)\n"
                           "          this is an enum option. Default: one. Value must be one of\n"
                           "          [three,two,one].\n"
                           "    -r, --required-int (signed 8 bit integer)\n"
                           "          this is another int option.\n"
                           "\n"
                           "FLAGS\n"
                           "\n"
                           "  SubFlags\n"
                           "    here come all the flags\n"
                           "    -f, --flag\n"
                           "          this is a flag.\n"
                           "\n"
                           "EXAMPLES\n"
                           "    example\n"
                           "\n"
                           "    example2\n"
                           "\n" +
                           basic_version_str;
    EXPECT_EQ(std_cout, expected);
}

TEST(help_page_printing, copyright)
{
    // Tests the --copyright call.
    const char * argvCopyright[] = {"./copyright", "--copyright"};
    seqan3::argument_parser copyright("myApp", 2, argvCopyright);

    std::string license_string = license_text();

    // Test --copyright with empty short and long copyright info.
    {
        testing::internal::CaptureStdout();
        EXPECT_EXIT(copyright.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
        std::string std_cout = testing::internal::GetCapturedStdout();

        std::string expected = "================================================================================\n"
                               "Copyright information for myApp:\n"
                               "--------------------------------------------------------------------------------\n"
                               "myApp copyright information not available.\n"
                               "================================================================================\n"
                               "This program contains SeqAn code licensed under the following terms:\n"
                               "--------------------------------------------------------------------------------\n"
                               + license_string;

        EXPECT_EQ(std_cout, expected);
    }

    // Test --copyright with a non-empty short copyright and an empty long copyright.
    copyright.info.short_copyright = "short copyright line 1\nshort copyright line 2";
    {
        testing::internal::CaptureStdout();
        EXPECT_EXIT(copyright.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
        std::string std_cout = testing::internal::GetCapturedStdout();

        std::string expected = "================================================================================\n"
                               "Copyright information for myApp:\n"
                               "--------------------------------------------------------------------------------\n"
                               "myApp full copyright information not available. Displaying short copyright information instead:\n"
                               "short copyright line 1\n"
                               "short copyright line 2\n"
                               "================================================================================\n"
                               "This program contains SeqAn code licensed under the following terms:\n"
                               "--------------------------------------------------------------------------------\n"
                               + license_string;

        EXPECT_EQ(std_cout, expected);
    }

    // Test --copyright with a non-empty short copyright and a non-empty long copyright.
    copyright.info.long_copyright = "long copyright line 1\nlong copyright line 2";
    {
        testing::internal::CaptureStdout();
        EXPECT_EXIT(copyright.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
        std::string std_cout = testing::internal::GetCapturedStdout();

        std::string expected = "================================================================================\n"
                               "Copyright information for myApp:\n"
                               "--------------------------------------------------------------------------------\n"
                               "long copyright line 1\n"
                               "long copyright line 2\n"
                               "================================================================================\n"
                               "This program contains SeqAn code licensed under the following terms:\n"
                               "--------------------------------------------------------------------------------\n"
                               + license_string;

        EXPECT_EQ(std_cout, expected);
    }
}

TEST(parse_test, subcommand_argument_parser)
{
    int option_value{};
    std::string option_value2{};

    const char * argv[]{"./test_parser", "-h"};
    seqan3::argument_parser top_level_parser{"test_parser", 2, argv, true, {"sub1", "sub2"}};
    top_level_parser.info.description.push_back("description");
    top_level_parser.add_option(option_value, 'f', "foo", "foo bar.");

    testing::internal::CaptureStdout();
    EXPECT_EXIT(top_level_parser.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
    std::string std_cout = testing::internal::GetCapturedStdout();

    std::string expected = "test_parser\n"
                           "===========\n"
                           "\n"
                           "DESCRIPTION\n"
                           "    description\n"
                           "\n"
                           "SUBCOMMANDS\n"
                           "    This program must be invoked with one of the following subcommands:\n"
                           "    - sub1\n"
                           "    - sub2\n"
                           "    See the respective help page for further details (e.g. by calling\n"
                           "    test_parser sub1 -h).\n"
                           "\n"
                           "    The following options below belong to the top-level parser and need to be\n"
                           "    specified before the subcommand key word. Every argument after the\n"
                           "    subcommand key word is passed on to the corresponding sub-parser.\n"
                           "\n" +
                           basic_options_str +
                           "\n"
                           "  \n"
                           "    -f, --foo (signed 32 bit integer)\n"
                           "          foo bar. Default: 0.\n"
                           "\n" +
                           basic_version_str;

    EXPECT_EQ(std_cout, expected);
}
