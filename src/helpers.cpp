#include "helpers.hpp"
#include <numeric>
namespace Helpers {

// Take a vector of strings and return a single string with the strings
// delimited by the given string
std::string combineVecStrWithDelim(std::vector<std::string> vecStr,
                                   const std::string &delimiter) {
    auto delimit_fold = [delimiter](std::string accumulator,
                                    const std::string &addition) {
        return std::move(accumulator) + delimiter + addition;
    };

    if (vecStr.size() != 0) {
        return std::accumulate(std::next(vecStr.begin()), vecStr.end(),
                               vecStr[0], delimit_fold);
    } else {
        return "";
    }
}

} // namespace Helpers
