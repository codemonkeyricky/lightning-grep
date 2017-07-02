#include <string>
#include <algorithm> 
#include <vector>
#include <queue>
#include <fstream>
#include <iostream>
#include <cstring>

int LevenshteinDistance(
    std::string target, 
    std::string candidate
    )
{
    std::vector< uint8_t > initial = 
    { 
        0, 1, 2, 3, 4, 5, 6, 7,
        8, 9, 10, 11, 12, 13, 14, 15
    }; 

    std::vector< uint8_t > t; 
    t.resize( 16 ); 

    auto targetLen = target.length();
    auto candidateLen = candidate.length();
    target.resize( 16 ); 
    __m128i c   = _mm_load_si128( ( const __m128i * ) &initial[ 0 ] );
    __m128i ones= _mm_set1_epi8( 1 ); 
    __m128i s   = _mm_load_si128( ( const __m128i * ) &target[ 0 ] ); 
    __m128i e, a; 
    for ( auto i = 0; i < candidateLen; i ++ )
    {
        // edit remains.
        e       = c; 

        // shift add to align with edit. 
        a       = _mm_srli_si128( c, 1 );

        // Only need to substitute if character is different.
        auto r  = _mm_set1_epi8( candidate[ i ] );

        // m = compare entire word with current letter
        // e = m * e + !m * ( min( a, e ) + 1 ); 

        // 
        // Find the chars that just carries over -> p1
        // 

        auto m  = _mm_cmpeq_epi8( s, r );
        auto p1 = _mm_and_si128( m, e ); 

        // 
        // Find the minimum between edits and adds, then +1 -> p2 
        // 

        auto min    = _mm_min_epi8( a, e ); 
        min         = _mm_add_epi8( min, ones );
        auto nm     = ~m; 
        auto p2     = _mm_and_si128( nm, min ); 

        auto sum    = _mm_add_epi8( p1, p2 ); 
        sum         = _mm_slli_si128( sum, 1 );

        // 
        // Now account for deletes -> c 
        // 

        _mm_store_si128( ( __m128i * ) &t[ 0 ], sum ); 

        t[ 0 ] = i + 1; 
        for ( auto j = 1; j < targetLen; j ++ )
        {
            t[ j ] = std::min( (uint8_t) ( t[ j - 1 ] + 1 ), t[ j ] ); 
        }

        c  = _mm_load_si128( ( const __m128i * ) &t[ 0 ] );
    }

    return t[ targetLen ]; 
}

struct FileEntry
{
    FileEntry( std::string f, std::string p ) 
        : f( f ), p( p )
    { }

    std::string f;  ///< filename 
    std::string p;  ///< fullpath
};


struct Element
{
    Element( int d, const std::string f, const std::string & p ) 
        : d( d ), f( f ), p( p )
    { }

    int d;          ///< distance
    std::string f;  ///< filename
    std::string p;  ///< path
}; 


struct Compare
{
    bool operator() (
        const Element   &e1,
        const Element   &e2
        )
    {
        return e1.d < e2.d; 
    }
};


int main(
    int argc, 
    char **argv
    )
{
    std::string pattern = argv[ 1 ]; 

    std::vector< FileEntry > list;
    std::fstream f( "filelist" );
    std::string fullpath;
    while ( true )
    {
        std::getline( f, fullpath ); 
        if ( !f )
            break; 

        auto lastIndex  = fullpath.find_last_of( "/" );
        auto filename   = fullpath.substr( lastIndex + 1 );

        list.emplace_back( filename, fullpath ); 
    }

#if 0
    std::priority_queue< Element, std::vector< Element >, Compare > heap; 
    for ( auto &p : list ) 
    {
#endif 
        std::string t( "bread" ); 
        std::string p( "fred" ); 
        auto d = LevenshteinDistance( t, p );

#if 0
        if ( heap.size() < 10 )
        {
            heap.emplace( d, p.f, p.p );
        }
        else if ( d < heap.top().d )
        {
            heap.pop(); 
            heap.emplace( d, p.f, p.p ); 
        }
    }

    while ( heap.size() > 0 )
    {
        std::cout << heap.top().d << " : " << heap.top().p << std::endl; heap.pop();
    }
    #endif
}