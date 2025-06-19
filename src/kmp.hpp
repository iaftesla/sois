#ifndef KMP_HPP
#define KMP_HPP

#include <string>
#include <vector>

namespace kmp {

/** Build prefix table for pattern. */
std::vector<int> buildPrefix(const std::string& pattern);

/** Detect pattern occurrences using KMP algorithm.
 *  Returns boolean vector with true at ending index of each occurrence.
 */
std::vector<bool> detect(const std::string& pattern, const std::string& text);

}

#endif
