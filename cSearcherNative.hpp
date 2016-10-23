#pragma once

#include <vector>
#include <immintrin.h>

#include "iSearcher.hpp"

constexpr int PATTERN_SIZE_MAX       = 64;
constexpr int ALIGNMENT              = 32;

struct AVX { char byte[ 16 ]; };
struct AVX2 { char byte[ 32 ]; };

template < class T >
struct simd_traits
{
    typedef T type;
    static const size_t size = 1;
};


template <>
struct simd_traits< AVX >
{
    typedef __m128i type;
    static const size_t size = sizeof( AVX );
};


template <>
struct simd_traits< AVX2 >
{
    typedef __m256i type;
    static const size_t size = sizeof( AVX2 );
};


template< typename T >
class cSearcherNative : public iSearcher
{
public:

    typedef typename simd_traits< T >::type vec_type;

    cSearcherNative( std::string & pattern );
    ~cSearcherNative();

    inline vec_type vector_load( const vec_type * );
    inline vec_type vector_compare( vec_type &, vec_type & );
    inline unsigned int vector_to_bitmask( vec_type & );
    inline unsigned int int_bits_count( unsigned int & );

    virtual std::vector< sMatchInstance > process( std::string & filename );

    static const int REGISTERS_REQUIRED = PATTERN_SIZE_MAX / simd_traits< T >::size;

private:

    void populatePatternVariables();
    void insertRecord( const char *, const char *, int, std::vector< sMatchInstance > & );

    vec_type firstLetterRepated;
    vec_type nl_vec;
    vec_type sp_vec[ simd_traits< T >::size ][ REGISTERS_REQUIRED ];

    std::string m_pattern;
};