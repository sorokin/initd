#ifndef REPORT_DUPLICATES_H
#define REPORT_DUPLICATES_H

#include <map>
#include <boost/next_prior.hpp>

template <typename K, typename V, typename F>
void report_duplicates(std::multimap<K, V> const& m, F f)
{
    if (m.empty())
        return;

    typedef typename std::multimap<K, V>::const_iterator const_iterator;

    bool last_reported = false;

    for (const_iterator i = boost::next(m.begin()); i != m.end(); ++i)
    {
        const_iterator pi = boost::prior(i);
        if (pi->first == i->first)
        {
            if (!last_reported)
                f(pi->second);

            f(i->second);
            last_reported = true;
        }
    }
}

#endif
