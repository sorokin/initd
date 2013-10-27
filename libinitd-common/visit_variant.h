#ifndef VISIT_VARIANT_H
#define VISIT_VARIANT_H

#include <utility>
#include <type_traits>
#include <boost/variant.hpp>

template <typename F>
struct member_function_traits;

template <typename R, typename C, typename Arg>
struct member_function_traits<R (C::*)(Arg)>
{
    typedef R result_type;
};

template <typename R, typename C, typename Arg>
struct member_function_traits<R (C::*)(Arg) const>
{
    typedef R result_type;
};

template <typename L>
struct lambda_function_traits
{
    typedef typename member_function_traits<decltype(&L::operator())>::result_type result_type;
};

template <typename ResultType, typename... Lambdas>
struct lambda_visitor
{
    typedef ResultType result_type;

    ResultType operator()(...) = delete;
};

template <typename ResultType, typename Lambda, typename... Lambdas>
struct lambda_visitor<ResultType, Lambda, Lambdas...> : Lambda, lambda_visitor<ResultType, Lambdas...>
{
    typedef ResultType result_type;

    static_assert(std::is_same<ResultType, typename lambda_function_traits<Lambda>::result_type>::value, "result type mismatch");

    lambda_visitor(Lambda lambda, Lambdas... lambdas)
        : Lambda(lambda)
        , lambda_visitor<ResultType, Lambdas...>(lambdas...)
    {}

    using Lambda::operator();
    using lambda_visitor<ResultType, Lambdas...>::operator();
};

template <typename... Lambdas>
struct lambdas_result_type
{};

template <typename Lambda, typename... Lambdas>
struct lambdas_result_type<Lambda, Lambdas...>
{
    typedef typename lambda_function_traits<Lambda>::result_type type;
};


template <typename Variant, typename... Lambdas>
typename lambdas_result_type<Lambdas...>::type visit_variant(Variant v, Lambdas... lambdas)
{
    typedef typename lambdas_result_type<Lambdas...>::type result_type;
    return apply_visitor(lambda_visitor<result_type, Lambdas...>(lambdas...), v);
}

#endif
