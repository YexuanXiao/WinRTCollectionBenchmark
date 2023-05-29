# Conclusion

The winrt::IVector has acceptable enough performance in most tests that I don't think using winrt::IVector directly is a bad choice.

For appending elements without pre-allocation, std::vector performs 0.7 to 0.5 times better than winrt::IVector, so winrt::IVector is not too slow.

Because types of WinRT objects are mostly runtime classes (e.g. all UI elements, and all types that have abstract interfaces), emplace_back is not always available, so the emplace_back data is for reference only.

For iteration, std::vector is always slower than winrt::IVector in the first time, about 2/3 to 9/10 of the performance of winrt::IVector. But from the second or third time on, std::vector will iterate faster, to the point where the performance is 3 times that of winrt::IVector, I'm not sure why, but the result is indeed stably reproducible.

Iterate use winrt::IVector::GetView has the same performance as direct iteration, and according to my many tests, there is only margin of errors.

If you know the number of elements, constructing a winrt::IVector from a pre-allocated std::vector is the fastest, due to the reduction of about 45 reallocations (the growth factor of std::vectors in Microsoft STL is 1.5). If you do not need to have winrt::IVector as a binding source for XAML, then you can move std::vector to winrt::IVector directly. If you need to have winrt::IVector as a binding source for XAML (this means that you cannot move std::vector to winrt::IVector), construct a std::vector first and use winrt::IVector::ReplaceAll to copy all elements still with very high performance (but with a slight increase in temporary memory).

On my Intel test machine with the following typical output:

Size of Data: 24
Count: 100000000
Standard Vector Push Back                     : 4866 milliseconds
Standard Vector Emplace Back                  : 3826 milliseconds
WinRT Vector Apply End                        : 7216 milliseconds
Buid Standard Vector and move to WinRT Vector : 3113 milliseconds
Buid Standard Vector and copy to WinRT Vector : 2231 milliseconds
Standard Vector iteration                     : 3235 milliseconds
WinRT Vector iteration                        : 1788 milliseconds
WinRT Vector iteration use GetView            : 1671 milliseconds
300000000
Size of Data: 24
Count: 100000000
Standard Vector Push Back                     : 4271 milliseconds
Standard Vector Emplace Back                  : 4167 milliseconds
WinRT Vector Apply End                        : 6311 milliseconds
Buid Standard Vector and move to WinRT Vector : 1592 milliseconds
Buid Standard Vector and copy to WinRT Vector : 2243 milliseconds
Standard Vector iteration                     : 1098 milliseconds
WinRT Vector iteration                        : 1529 milliseconds
WinRT Vector iteration use GetView            : 1417 milliseconds
300000000
Size of Data: 24
Count: 100000000
Standard Vector Push Back                     : 4158 milliseconds
Standard Vector Emplace Back                  : 4308 milliseconds
WinRT Vector Apply End                        : 6394 milliseconds
Buid Standard Vector and move to WinRT Vector : 1318 milliseconds
Buid Standard Vector and copy to WinRT Vector : 2126 milliseconds
Standard Vector iteration                     : 545 milliseconds
WinRT Vector iteration                        : 1770 milliseconds
WinRT Vector iteration use GetView            : 1724 milliseconds
300000000