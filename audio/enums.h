#pragma once

namespace natus
{
    namespace audio
    {
        enum class channels
        {
            undefined,
            mono,
            stereo
        };

        enum class frequency
        {
            undefined,
            freq_48k,
            freq_96k,
            freq_128k,
            max_frequencies
        };

        static size_t to_number( frequency const f ) noexcept
        {
            static size_t const _numbers[] = { 0, 48000, 96000, 128000 } ;
            return _numbers[ size_t(f) < size_t(frequency::max_frequencies) ? size_t(f) : 0 ] ;
        }

        static size_t to_number( channels const c ) noexcept
        {
            return size_t( c ) ;
        }
    }
}