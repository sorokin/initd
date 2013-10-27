#ifndef OPTIONAL_CAST_H
#define OPTIONAL_CAST_H

#include <boost/optional.hpp>

template <typename Target, typename Source>
boost::optional<Target> optional_cast(boost::optional<Source> const& source)
{
    if (!source)
        return boost::none;

    return static_cast<Target>(*source);
}

#endif
