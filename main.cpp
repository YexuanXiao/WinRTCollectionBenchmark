#include <chrono>
#include <iostream>
#include <ranges>
#include <vector>
#include <winrt/Data.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>

using namespace winrt;

void test()
{
    auto constexpr count{100000000};
    auto pre{decltype(std::chrono::steady_clock::now()){}};
    auto now{decltype(pre){}};
    auto winrt_vector{winrt::single_threaded_vector<winrt::Data::Data>()};
    auto standard_vector{std::vector<winrt::Data::Data>{}};

    std::cout << "Size of Data: " << sizeof(winrt::Data::Data) << '\n';
    std::cout << "Count: " << count << "\n";

    // print to stdout to prevent compiler optimization
    auto accumulate{unsigned long long{}};

    // start
    pre = std::chrono::steady_clock::now();
    for (auto const i : std::views::iota(0, count))
    {
        // since the winrt API usually only accepts constant objects, move and emplace are not used here
        auto const item{winrt::Data::Data{winrt::hstring{}, 1ll, true, 1ul}};
        standard_vector.push_back(item);
    }
    now = std::chrono::steady_clock::now();
    std::cout << "Standard Vector Push Back                     : "
              << std::chrono::duration_cast<std::chrono::milliseconds>(now - pre).count() << " milliseconds\n";

    standard_vector = decltype(standard_vector){};

    pre = std::chrono::steady_clock::now();
    for (auto const i : std::views::iota(0, count))
    {
        standard_vector.emplace_back(winrt::hstring{}, 1ll, true, 1ul);
    }
    now = std::chrono::steady_clock::now();
    std::cout << "Standard Vector Emplace Back                  : "
              << std::chrono::duration_cast<std::chrono::milliseconds>(now - pre).count() << " milliseconds\n";

    pre = std::chrono::steady_clock::now();
    for (auto const i : std::views::iota(0, count))
    {
        winrt_vector.Append({winrt::hstring{}, 1ll, true, 1ul});
    }
    now = std::chrono::steady_clock::now();
    std::cout << "WinRT Vector Apply End                        : "
              << std::chrono::duration_cast<std::chrono::milliseconds>(now - pre).count() << " milliseconds\n";

    pre = std::chrono::steady_clock::now();
    {
        // use a independent block to make std::vector destruct
        auto standard_vector{std::vector<winrt::Data::Data>{}};
        standard_vector.reserve(count);
        for (auto const i : std::views::iota(0, count))
        {
            standard_vector.emplace_back(winrt::hstring{}, 1ll, true, 1ul);
        }
        winrt_vector = winrt::single_threaded_vector(std::move(standard_vector));
    }
    now = std::chrono::steady_clock::now();
    std::cout << "Buid Standard Vector and move to WinRT Vector : "
              << std::chrono::duration_cast<std::chrono::milliseconds>(now - pre).count() << " milliseconds\n";

    winrt_vector = winrt::single_threaded_vector<winrt::Data::Data>();
    pre = std::chrono::steady_clock::now();
    {
        // use a independent block to make std::vector destruct
        auto standard_vector{std::vector<winrt::Data::Data>{}};
        standard_vector.reserve(count);
        for (auto const i : std::views::iota(0, count))
        {
            standard_vector.emplace_back(winrt::hstring{}, 1ll, true, 1ul);
        }
        winrt_vector.ReplaceAll(standard_vector);
    }
    now = std::chrono::steady_clock::now();
    std::cout << "Buid Standard Vector and copy to WinRT Vector : "
              << std::chrono::duration_cast<std::chrono::milliseconds>(now - pre).count() << " milliseconds\n";

    pre = std::chrono::steady_clock::now();
    for (auto i : std::ranges::as_const_view(standard_vector))
    {
        accumulate += i.u8;
    }
    now = std::chrono::steady_clock::now();
    std::cout << "Standard Vector iteration                     : "
              << std::chrono::duration_cast<std::chrono::milliseconds>(now - pre).count() << " milliseconds\n";

    pre = std::chrono::steady_clock::now();
    for (auto i : winrt_vector)
    {
        accumulate += i.u8;
    }
    now = std::chrono::steady_clock::now();
    std::cout << "WinRT Vector iteration                        : "
              << std::chrono::duration_cast<std::chrono::milliseconds>(now - pre).count() << " milliseconds\n";

    pre = std::chrono::steady_clock::now();
    for (auto i : winrt_vector.GetView())
    {
        accumulate += i.u8;
    }
    now = std::chrono::steady_clock::now();
    std::cout << "WinRT Vector iteration use GetView            : "
              << std::chrono::duration_cast<std::chrono::milliseconds>(now - pre).count() << " milliseconds\n";

    std::cout << accumulate << '\n';
}

int main()
{
    for (auto const i : std::views::iota(0, 3))
        test();
}
