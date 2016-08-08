#include <type_traits>
#include <utility>

#include <iostream>

struct static_if_
{
    template <typename Cond>
    auto operator()(Cond const &) const -> decltype(auto)
    {
        return select(std::integral_constant<bool, std::decay_t<Cond>::value>{});
    }

private:
    template <typename Cb>
    struct result_t_
    {
        Cb _callback;

        template <typename... Args>
        auto operator()(Args... args) const -> decltype(auto)
        {
            return _callback(std::forward(args)...);
        }
    };

    struct true_t_
    {
        template <typename Cb>
        struct resolver_t_
        {
            Cb _callback;

            struct elif_helper_t_
            {
                Cb _callback;

                template <typename OCb>
                resolver_t_<Cb> then(OCb &&) const { return { _callback }; }
            };

            template <typename Cond>
            elif_helper_t_ elif(Cond const &) const { return { _callback }; }

            template <typename OCb>
            result_t_<Cb> else_(OCb &&) const { return { _callback }; }
        };

        template <typename Cb>
        resolver_t_<Cb> then(Cb && cb) const
        {
            return { cb };
        }
    };

    struct false_t_
    {
        struct resolver_t_
        {
            template <typename Cond>
            auto elif(const Cond &) const -> decltype(auto) { return static_if_{}.select(std::integral_constant<bool, std::decay_t<Cond>::value>{}); }

            template <typename Cb>
            result_t_<Cb> else_(Cb && cb) const { return { cb }; }
        };

        template <typename Cb>
        resolver_t_ then(Cb && cb) { return {}; }
    };

    true_t_ select(std::true_type) const { return {}; }
    false_t_ select(std::false_type) const { return {}; }
};

static constexpr static_if_ static_if{};

int main()
{
    static_if(std::false_type{})
        .then([](){ std::cout << 1 << '\n'; })
    .elif(std::true_type{})
        .then([](){ std::cout << 2 << '\n'; })
    .else_([]() { })
    ();
}
