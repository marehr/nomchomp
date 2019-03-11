// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \brief Provides seqan3::detail::alignment_selector.
 * \author Rene Rahn <rene.rahn AT fu-berlin.de>
 */

#pragma once

#include <functional>
#include <tuple>
#include <utility>
#include <vector>

#include <seqan3/alignment/configuration/all.hpp>
#include <seqan3/alignment/pairwise/policy/all.hpp>
#include <seqan3/alignment/pairwise/alignment_algorithm.hpp>
#include <seqan3/alignment/pairwise/align_result_selector.hpp>
#include <seqan3/alignment/pairwise/align_result.hpp>
#include <seqan3/alignment/pairwise/edit_distance_unbanded.hpp>
#include <seqan3/alphabet/gap/gapped.hpp>
#include <seqan3/core/concept/tuple.hpp>
#include <seqan3/core/metafunction/deferred_crtp_base.hpp>
#include <seqan3/core/metafunction/range.hpp>
#include <seqan3/core/metafunction/template_inspection.hpp>
#include <seqan3/core/type_list.hpp>
#include <seqan3/range/view/persist.hpp>

namespace seqan3::detail
{

/*!\brief Provides several contracts to test when configuring the alignment algorithm.
 * \ingroup pairwise_alignment
 * \tparam range_type            The type of the range containing sequences to be aligned.
 * \tparam alignment_config_type The type of the alignment configuration.
 *
 * \details
 *
 * This stateless helper class provides several contract testing functions for the alignment configuration.
 */
template <typename range_type,
          typename alignment_config_type>
struct alignment_contract
{
private:
    /*!\brief Auxiliary member types
     * \{
     */
     //!\brief The type of the first sequence.
    using first_seq_t  = std::tuple_element_t<0, value_type_t<std::ranges::iterator_t<range_type>>>;
    //!\brief The type of the second sequence.
    using second_seq_t = std::tuple_element_t<1, value_type_t<std::ranges::iterator_t<range_type>>>;
    //!\}

public:
    //!\brief Tests whether the value type of `range_type` is a tuple with exactly 2 members.
    constexpr static bool expects_tuple_like_value_type()
    {
        return tuple_like_concept<alignment_config_type> &&
               std::tuple_size_v<value_type_t<std::ranges::iterator_t<range_type>>> == 2;
    }

    //!\brief Tests whether the scoring scheme is set and can be invoked with the sequences passed.
    constexpr static bool expects_valid_scoring_scheme()
    {
        if constexpr (alignment_config_type::template exists<align_cfg::scoring>())
        {
            using scoring_type = std::remove_reference_t<
                                    decltype(get<align_cfg::scoring>(std::declval<alignment_config_type>()).value)
                                 >;
            return static_cast<bool>(scoring_scheme_concept<scoring_type, value_type_t<first_seq_t>,
                                                                          value_type_t<second_seq_t>>);
        }
        else
        {
            return false;
        }
    }
};

/*!\brief Configures the alignment algorithm given the sequences and the configuration object.
 * \ingroup pairwise_alignment
 */
struct alignment_configurator
{
private:

    /*!\brief Transformation trait that chooses the correct matrix policy.
     * \tparam config_type The configuration for which to select the correct matrix policy.
     * \tparam trait_types A template parameter pack with additional traits to augment the selected policy.
     */
    template <typename config_type, typename score_allocator_t, typename trace_allocator_t>
    struct select_matrix_policy
    {
    private:

        //!\brief Selects the correct alignment matrix policy based on the stored config types.
        template <typename config_t>
        static constexpr auto select() noexcept
        {
            // Check whether traceback was requested or not.
            if constexpr (std::is_same_v<typename trace_allocator_t::value_type, ignore_t>)
            {  // No traceback
                if constexpr (config_t::template exists<align_cfg::band>())
                    return deferred_crtp_base<banded_score_dp_matrix_policy, score_allocator_t>{};
                else
                    return deferred_crtp_base<unbanded_score_dp_matrix_policy, score_allocator_t>{};
            }
            else
            {  // requested traceback
                if constexpr (config_t::template exists<align_cfg::band>())
                {
                    return deferred_crtp_base<banded_score_trace_dp_matrix_policy,
                                              score_allocator_t,
                                              trace_allocator_t>{};
                }
                else
                {
                    return deferred_crtp_base<unbanded_score_trace_dp_matrix_policy,
                                              score_allocator_t,
                                              trace_allocator_t>{};
                }
            }
        }

    public:
        //!\brief The matrix policy based on the configurations given by `config_type`.
        using type = decltype(select<config_type>());
    };

    /*!\brief Transformation trait that chooses the correct gap policy.
     * \tparam config_type The configuration for which to select the correct gap policy.
     * \tparam trait_types A template parameter pack with additional traits to augment the selected policy.
     */
    template <typename config_type, typename ... trait_types>
    struct select_gap_policy
    {
    private:

        //!\brief Selects the correct gap policy based on the stored config types.
        template <typename config_t>
        static constexpr auto select() noexcept
        {
            if constexpr (config_t::template exists<align_cfg::band>())
                return deferred_crtp_base<affine_gap_banded_policy, trait_types...>{};
            else
                return deferred_crtp_base<affine_gap_policy, trait_types...>{};
        }

    public:
        //!\brief The matrix policy based on the configurations given by `config_type`.
        using type = decltype(select<config_type>());
    };

    /*!\brief Transformation trait that chooses the correct gap initialisation policy.
     * \tparam config_type The configuration for which to select the correct gap initialisation policy.
     * \tparam trait_types A template parameter pack with additional traits to augment the selected policy.
     */
    template <typename config_type, typename ... trait_types>
    struct select_gap_init_policy
    {
    private:

        //!\brief Selects the correct gap init policy based on the stored config types.
        template <typename config_t>
        static constexpr auto select() noexcept
        {
            if constexpr (config_t::template exists<align_cfg::band>())
                return deferred_crtp_base<affine_gap_banded_init_policy, trait_types...>{};
            else
                return deferred_crtp_base<affine_gap_init_policy, trait_types...>{};
        }

    public:
        //!\brief The matrix policy based on the configurations given by `config_type`.
        using type = decltype(select<config_type>());
    };

public:
    /*!\brief Configures the algorithm.
     * \tparam sequences_t The range type containing the sequence pairs; must model std::ranges::ForwardRange.
     * \tparam config_t    The alignment configuration type; must be a specialisation of seqan3::configuration.
     * \param[in]     seq_range The range over the sequences; The value type must model seqan3::tuple_like_concept.
     * \param[in,out] cfg       The configuration object.
     *
     * \returns std::function wrapper of the configured alignment algorithm.
     *
     * \details
     *
     * This function reads the seqan3::configuration object and generates the corresponding alignment algorithm type.
     * During this process some runtime configurations are converted to static configurations if required. The return
     * type is a std::function which is generated in the following way:
     *
     * \snippet snippet/alignment/pairwise/alignment_configurator.cpp result
     *
     * The arguments to the function object are two ranges, which always need to be passed as lvalue references.
     * Note that even if they are not passed as const lvalue reference (which is not possible, since not all views are
     * const-iterable), they are not modified within the alignment algorithm.
     */
    template <std::ranges::ForwardRange sequences_t, typename config_t>
    //!\cond
        requires tuple_like_concept<value_type_t<std::remove_reference_t<sequences_t>>> && 
                 is_type_specialisation_of_v<remove_cvref_t<config_t>, configuration>
    //!\endcond
    static constexpr auto configure(sequences_t && SEQAN3_DOXYGEN_ONLY(seq_range), config_t const & cfg)
    {
        // ----------------------------------------------------------------------------
        // Configure the type-erased alignment function.
        // ----------------------------------------------------------------------------

        using first_seq_t = std::tuple_element_t<0, value_type_t<std::remove_reference_t<sequences_t>>>;
        using second_seq_t = std::tuple_element_t<1, value_type_t<std::remove_reference_t<sequences_t>>>;

        // Select the result type based on the sequences and the configuration.
        using result_t = align_result<typename align_result_selector<std::remove_reference_t<first_seq_t>,
                                                                     std::remove_reference_t<second_seq_t>,
                                                                     config_t>::type>;
        // Define the function wrapper type.
        using function_wrapper_t = std::function<result_t(first_seq_t &, second_seq_t &)>;

        // ----------------------------------------------------------------------------
        // Test some basic preconditions
        // ----------------------------------------------------------------------------

        using alignment_contract_t = alignment_contract<remove_cvref_t<sequences_t>, config_t>;

        static_assert(alignment_contract_t::expects_tuple_like_value_type(),
                      "Alignment configuration error: "
                      "The value type of the sequence ranges must model the seqan3::detail::tuple_like_concept "
                      "and must contain exactly 2 elements.");

        static_assert(alignment_contract_t::expects_valid_scoring_scheme(),
                      "Alignment configuration error: "
                      "Either the scoring scheme was not configured or the given scoring scheme cannot be invoked with "
                      "the value types of the passed sequences.");

        // ----------------------------------------------------------------------------
        // Configure the algorithm
        // ----------------------------------------------------------------------------

        // Use default edit distance if gaps are not set.
        auto const & gaps = cfg.template value_or<align_cfg::gap>(gap_scheme{gap_score{-1}});
        auto const & scoring_scheme = get<align_cfg::scoring>(cfg).value;

        // Check if edit distance can be used?
        if (gaps.get_gap_open_score() == 0)
        {
            // TODO: Instead of relying on nucleotide scoring schemes we need to be able to determine the edit distance
            //       option via the scheme.
            if constexpr (is_type_specialisation_of_v<remove_cvref_t<decltype(scoring_scheme)>,
                                                      nucleotide_scoring_scheme>)
            {
                if ((scoring_scheme.score('A'_dna15, 'A'_dna15) == 0) &&
                    (scoring_scheme.score('A'_dna15, 'C'_dna15)) == -1)
                    return configure_edit_distance<function_wrapper_t>(cfg);
            }
        }

        // Configure the alignment algorithm.
        return configure_free_ends_initialisation<function_wrapper_t>(cfg);
    }

private:

    /*!\brief Configures the edit distance algorithm.
     * \tparam function_wrapper_t The invocable alignment function type-erased via std::function.
     * \tparam config_t           The alignment configuration type.
     * \param[in] cfg             The passed configuration object.
     */
    template <typename function_wrapper_t, typename config_t>
    static constexpr function_wrapper_t configure_edit_distance(config_t const & cfg)
    {
        // ----------------------------------------------------------------------------
        // Unsupported configurations
        // ----------------------------------------------------------------------------

        if constexpr (config_t::template exists<align_cfg::band>())
            throw invalid_alignment_configuration{"Banded alignments are yet not supported."};

        if constexpr (config_t::template exists<align_cfg::aligned_ends>())
            throw invalid_alignment_configuration{"Free end gaps is not yet supported."};

        return function_wrapper_t{edit_distance_wrapper<remove_cvref_t<config_t>>{cfg}};
    }

    /*!\brief Configures the dynamic programming matrix initialisation accoring to seqan3::align_cfg::aligned_ends
     *        settings.
     * \tparam function_wrapper_t The invocable alignment function type-erased via std::function.
     * \tparam config_t           The alignment configuration type.
     * \param[in] cfg   The passed configuration object.
     *
     * \details
     *
     * The matrix initialisation depends on the settings for the leading gaps for the first and the second sequence
     * within the seqan3::align_cfg::aligned_ends configuration element.
     */
    template <typename function_wrapper_t, typename config_t>
    static constexpr function_wrapper_t configure_free_ends_initialisation(config_t const & cfg);

    /*!\brief Configures the search space for the alignment algorithm according to seqan3::align_cfg::aligned_ends
     *        settings.
     * \tparam function_wrapper_t The invocable alignment function type-erased via std::function.
     * \tparam policies_t         A template parameter pack for the already configured policy types.
     * \tparam config_t     The alignment configuration type.
     * \param[in] cfg       The passed configuration object.
     *
     * \details
     *
     * This option is configured in the seqan3::align_cfg::aligned_ends configuration element according to
     * the settings for the trailing gaps of the first and the second sequence.
     */
    template <typename function_wrapper_t, typename ...policies_t, typename config_t>
    static constexpr function_wrapper_t configure_free_ends_optimum_search(config_t const & cfg);

    /*!\brief Determines the trace type.
     * \tparam config_t The configuration type.
     */
    template <typename config_t>
    struct configure_trace_type
    {
        //!\brief If traceback is enabled resolves to seqan3::detail::trace_directions,
        //!\      otherwise seqan3::detail::ignore_t.
        using type = std::conditional_t<config_t::template exists<align_cfg::result<with_trace_type>>() ||
                                        config_t::template exists<align_cfg::result<with_begin_position_type>>(),
                                        trace_directions,
                                        ignore_t>;
    };

};

//!\cond
// This function returns a std::function object which can capture runtime dependent alignment algorithm types through
// a fixed invocation interface which is already defined by the caller of this function.
template <typename function_wrapper_t, typename config_t>
constexpr function_wrapper_t alignment_configurator::configure_free_ends_initialisation(config_t const & cfg)
{
    // ----------------------------------------------------------------------------
    // score and cell type
    // ----------------------------------------------------------------------------

    using score_type = int32_t;
    using trace_type = typename configure_trace_type<config_t>::type;
    using cell_type = std::tuple<score_type, score_type, trace_type>;

    // ----------------------------------------------------------------------------
    // dynamic programming matrix
    // ----------------------------------------------------------------------------

    using dp_matrix_t = typename select_matrix_policy<config_t,
                                                      std::allocator<cell_type>,
                                                      std::allocator<trace_type>>::type;

    // ----------------------------------------------------------------------------
    // affine gap kernel
    // ----------------------------------------------------------------------------

    using affine_t = typename select_gap_policy<config_t, cell_type>::type;

    // ----------------------------------------------------------------------------
    // configure initialisation policy
    // ----------------------------------------------------------------------------

    // Get the value for the sequence ends configuration.
    auto align_ends_cfg = cfg.template value_or<align_cfg::aligned_ends>(align_cfg::none_ends_free);
    using align_ends_cfg_t = decltype(align_ends_cfg);

    // This lambda augments the initialisation policy of the alignment algorithm
    // with the aligned_ends configuration from before.
    auto configure_leading_both = [&](auto first_seq, auto second_seq) constexpr
    {
        // Define the trait for the initialisation policy
        struct policy_trait_type
        {
            using free_first_leading_t  [[maybe_unused]] = decltype(first_seq);
            using free_second_leading_t [[maybe_unused]] = decltype(second_seq);
        };

        // Make initialisation policy a deferred CRTP base and delegate to configure the find optimum policy.
        using init_t = typename select_gap_init_policy<config_t, policy_trait_type>::type;
        return configure_free_ends_optimum_search<function_wrapper_t, affine_t, dp_matrix_t, init_t>(cfg);
    };

    // This lambda determines the initialisation configuration for the second sequence given
    // the leading gap property for it.
    auto configure_leading_second = [&](auto first) constexpr
    {
        // If possible use static information.
        if constexpr (align_ends_cfg_t::template is_static<2>())
        {
            return configure_leading_both(first,
                                          std::integral_constant<bool, align_ends_cfg_t::template get_static<2>()>{});
        }
        else
        {   // Resolve correct property at runtime.
            if (align_ends_cfg[2])
                return configure_leading_both(first, std::true_type{});
            else
                return configure_leading_both(first, std::false_type{});
        }
    };

    // Here the initialisation configuration for the first sequence is determined given
    // the leading gap property for it.
    // If possible use static information.
    if constexpr (align_ends_cfg_t::template is_static<0>())
    {
        return configure_leading_second(std::integral_constant<bool, align_ends_cfg_t::template get_static<0>()>{});
    }
    else
    {  // Resolve correct property at runtime.
        if (align_ends_cfg[0])
            return configure_leading_second(std::true_type{});
        else
            return configure_leading_second(std::false_type{});
    }
}
//!\endcond

//!\cond
// This function returns a std::function object which can capture runtime dependent alignment algorithm types through
// a fixed invocation interface which is already defined by the caller of this function.
template <typename function_wrapper_t, typename ...policies_t, typename config_t>
constexpr function_wrapper_t alignment_configurator::configure_free_ends_optimum_search(config_t const & cfg)
{
    // Get the value for the sequence ends configuration.
    auto align_ends_cfg = cfg.template value_or<align_cfg::aligned_ends>(align_cfg::none_ends_free);
    using align_ends_cfg_t = decltype(align_ends_cfg);

    // This lambda augments the find optimum policy of the alignment algorithm with the
    // respective aligned_ends configuration.
    auto configure_trailing_both = [&](auto first_seq, auto second_seq) constexpr
    {
        struct policy_trait_type
        {
            using find_in_every_cell_type  [[maybe_unused]] = std::false_type;
            using find_in_last_row_type    [[maybe_unused]] = decltype(first_seq);
            using find_in_last_column_type [[maybe_unused]] = decltype(second_seq);
        };

        using find_optimum_t = deferred_crtp_base<find_optimum_policy, policy_trait_type>;
        return function_wrapper_t{alignment_algorithm<config_t, policies_t..., find_optimum_t>{cfg}};
    };

    // This lambda determines the lookup configuration for the second sequence given
    // the trailing gap property for it.
    auto configure_trailing_second = [&](auto first) constexpr
    {
        // If possible use static information.
        if constexpr (align_ends_cfg_t::template is_static<3>())
        {
            return configure_trailing_both(first,
                                           std::integral_constant<bool, align_ends_cfg_t::template get_static<3>()>{});
        }
        else
        { // Resolve correct property at runtime.
            if (align_ends_cfg[3])
                return configure_trailing_both(first, std::true_type{});
            else
                return configure_trailing_both(first, std::false_type{});
        }
    };

    // Here the lookup configuration for the first sequence is determined given
    // the trailing gap property for it.
    // If possible use static information.
    if constexpr (align_ends_cfg_t::template is_static<1>())
    {
        return configure_trailing_second(std::integral_constant<bool, align_ends_cfg_t::template get_static<1>()>{});
    }
    else
    { // Resolve correct property at runtime.
        if (align_ends_cfg[1])
            return configure_trailing_second(std::true_type{});
        else
            return configure_trailing_second(std::false_type{});
    }
}
//!\endcond
} // namespace seqan3::detail