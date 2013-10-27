#ifndef MAKE_UNIQUE_H
#define MAKE_UNIQUE_H

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

#endif // MAKE_UNIQUE_H
