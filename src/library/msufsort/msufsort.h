#pragma once

#include <vector>
#include <stdint.h>
#include "./thread_pool.h"


namespace maniscalco
{

    class msufsort
    {
    public:

        using suffix_index = int32_t;
        using suffix_array = std::vector<suffix_index>;

        msufsort
        (
            int32_t
        );

        ~msufsort();

        suffix_array make_suffix_array
        (
	        uint8_t const *,
            uint8_t const *,
            int32_t
        );

        int32_t forward_burrows_wheeler_transform
        (
	        uint8_t *,
            uint8_t *,
            int32_t
        );

        void reverse_burrows_wheeler_transform
        (
	        uint8_t *,
            uint8_t *,
            int32_t
        );
 
    protected:

    private:

        // flags used in ISA
        static int32_t constexpr is_tandem_repeat_flag  = 0x80000000;
        static int32_t constexpr is_bstar_suffix_flag   = 0x40000000;
        static int32_t constexpr isa_flag_mask = (is_tandem_repeat_flag | is_bstar_suffix_flag);        
        static int32_t constexpr isa_index_mask = ~isa_flag_mask;

        // flags used in SA
        static int32_t constexpr preceding_suffix_is_type_a_flag = 0x80000000;
        static int32_t constexpr sa_index_mask = ~preceding_suffix_is_type_a_flag;
        static int32_t constexpr min_length_before_tandem_repeat_check = (int32_t)(2 + sizeof(uint64_t) + sizeof(uint64_t));

        static constexpr int32_t insertion_sort_threshold = 16;

        enum suffix_type 
        {
            a,
            b,
            bStar
        };

        struct partition_info
        {
            int32_t size_;
            int32_t matchLength_;
            uint64_t startingPattern_;
            bool     potentialTandemRepeats_;
        };

        uint64_t get_value
        (
            uint8_t const *,
            suffix_index
        ) const;

        suffix_type get_suffix_type
        (
            uint8_t const *
        );

        bool compare_suffixes
        (
            uint8_t const *,
            suffix_index,
            suffix_index
        ) const;

        void insertion_sort
        (
            suffix_index *,
            suffix_index *,
            int32_t,
            uint64_t
        );

        bool tandem_repeat_sort
        (
            suffix_index *,
            suffix_index *,
            int32_t,
            uint64_t
        );

        void second_stage_its
        (
            int32_t
        );

        int32_t second_stage_its_as_burrows_wheeler_transform
        (
            int32_t
        );

        void first_stage_its
        (
            int32_t
        );

        void multikey_quicksort
        (
            suffix_index *,
            suffix_index *,
            int32_t,
            uint64_t
        );

        uint8_t const * inputBegin_;

        uint8_t const * inputEnd_;

        int32_t         inputSize_;

        uint8_t const * getValueEnd_;

        suffix_index    getValueMaxIndex_;

        uint8_t         copyEnd_[sizeof(uint64_t) << 1];

        suffix_index *  suffixArrayBegin_;

        suffix_index *  suffixArrayEnd_;

        suffix_index *  inverseSuffixArrayBegin_;

        suffix_index *  inverseSuffixArrayEnd_;

        int32_t         frontBucketOffset_[0x100];

        int32_t         backBucketOffset_[0x10000];

        bool const      tandemRepeatSortEnabled_ = true;

        std::unique_ptr<thread_pool>    threadPool_;

    }; // class msufsort


    template <typename input_iter>
    msufsort::suffix_array make_suffix_array
    (
        input_iter,
        input_iter,
        int32_t = 1
    );

    template <typename input_iter>
    int32_t forward_burrows_wheeler_transform
    (
        input_iter,
        input_iter,
        int32_t = 1
    );

    template <typename input_iter>
    void reverse_burrows_wheeler_transform
    (
        input_iter,
        input_iter,
        int32_t,
        int32_t = 1
    );

} // namespace maniscalco


//==============================================================================
template <typename input_iter>
maniscalco::msufsort::suffix_array maniscalco::make_suffix_array
(
    input_iter begin,
    input_iter end,
    int32_t numThreads
)
{
    return msufsort(numThreads).make_suffix_array((uint8_t const *)&*begin, (uint8_t const *)&*end, numThreads);
}


//==============================================================================
template <typename input_iter>
int32_t maniscalco::forward_burrows_wheeler_transform
(
    input_iter begin,
    input_iter end,
    int32_t numThreads
)
{
    return msufsort(numThreads).forward_burrows_wheeler_transform((uint8_t *)&*begin, (uint8_t *)&*end, numThreads);
}


//==============================================================================
template <typename input_iter>
void maniscalco::reverse_burrows_wheeler_transform
(
    input_iter begin,
    input_iter end,
    int32_t sentinelIndex,
    int32_t numThreads
)
{
    msufsort(numThreads).reverse_burrows_wheeler_transform((uint8_t *)&*begin, (uint8_t *)&*end, sentinelIndex);
}
