#include <cstdio>
#include <mutex>

#include <Locked.hpp>

std::mutex                          mutex;
std::timed_mutex                    timed_mutex;

Locked<int>                         g_i(1);
Locked<int, std::mutex&>            g_li(mutex, 2);
Locked<int, std::timed_mutex>       g_ti(5);
Locked<int, std::timed_mutex&>      g_tli(timed_mutex, 42);

Locked<int, std::recursive_mutex>   g_ri(11);

#if __cplusplus >= 201703
#include <shared_mutex>

std::shared_mutex                       shared_mutex;
std::shared_timed_mutex                 shared_timed_mutex;

Locked<int, std::shared_mutex>          g_si(101);
Locked<int, std::shared_mutex&>         g_sli(shared_mutex, 102);
Locked<int, std::shared_timed_mutex>    g_tsi(105);
Locked<int, std::shared_timed_mutex&>   g_tsli(shared_timed_mutex, 142);
#endif // __cplusplus >= 201703

int
get(const int* i)
{
    return *i;
}

void
set(int* i, int value)
{
    *i = value;
}

int
main()
{
    Locked<int>::pointer                            pi(g_i);
    Locked<int, std::mutex&>::const_pointer         ci(g_li);
    Locked<int, std::timed_mutex>::pointer          pti(g_ti);
    Locked<int, std::timed_mutex&>::const_pointer   cti(g_tli);

    printf("g_i %d g_li %d g_ti %d g_tli %d\n", *pi, get(ci), *pti, *cti);

    // *pi = 6;
    set(pi, 6);
    // *ci = 7; // Uncomment for compile-time test of const_pointer.
    // set(ci, 7); // Uncomment for compile-time test of const_pointer.
    printf("g_i %d g_li %d g_ti %d g_tli %d\n", *pi, get(ci), *pti, *cti);

    *pti = 9;
    // *cti = 10; // Uncomment for compile-time test of const_pointer.
    printf("g_i %d g_li %d g_ti %d g_tli %d\n", *pi, get(ci), *pti, *cti);

    Locked<int, std::recursive_mutex>::pointer          pri(g_ri);
    Locked<int, std::recursive_mutex>::const_pointer    cri(g_ri);

    printf("g_ri %d\n", *cri);
    *pri = 23;
    printf("g_ri %d\n", *cri);

#if __cplusplus >= 201703
    Locked<int, std::shared_mutex>::pointer                 spi(g_si);
    Locked<int, std::shared_mutex&>::const_pointer          sci(g_sli);
    Locked<int, std::shared_timed_mutex>::pointer           spti(g_tsi);
    Locked<int, std::shared_timed_mutex&>::const_pointer    scti(g_tsli);

    printf("g_si %d g_sli %d g_tsi %d g_tsli %d\n", *spi, *sci, *spti, *scti);

    set(spi, 106);
    // *ci = 7; // Uncomment for compile-time test of const_pointer.
    // set(ci, 7); // Uncomment for compile-time test of const_pointer.
    printf("g_si %d g_sli %d g_tsi %d g_tsli %d\n", *spi, *sci, *spti, *scti);

    *spti = 109;
    // *cti = 10; // Uncomment for compile-time test of const_pointer.
    printf("g_si %d g_sli %d g_tsi %d g_tsli %d\n", *spi, *sci, *spti, *scti);
#endif // __cplusplus >= 201703

    return 0;
}
