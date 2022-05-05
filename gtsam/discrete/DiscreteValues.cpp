/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation,
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 *  @file DiscreteValues.cpp
 *  @date January, 2022
 *  @author Frank Dellaert
 */

#include <gtsam/discrete/DiscreteValues.h>

#include <sstream>

using std::cout;
using std::endl;
using std::string;
using std::stringstream;

namespace gtsam {

void DiscreteValues::print(const string& s,
                           const KeyFormatter& keyFormatter) const {
  cout << s << ": ";
  for (auto&& kv : *this)
    cout << "(" << keyFormatter(kv.first) << ", " << kv.second << ")";
  cout << endl;
}

string DiscreteValues::Translate(const Names& names, Key key, size_t index) {
  if (names.empty()) {
    stringstream ss;
    ss << index;
    return ss.str();
  } else {
    return names.at(key)[index];
  }
}

string DiscreteValues::markdown(const KeyFormatter& keyFormatter,
                                const Names& names) const {
  stringstream ss;

  // Print out header and separator with alignment hints.
  ss << "|Variable|value|\n|:-:|:-:|\n";

  // Print out all rows.
  for (const auto& kv : *this) {
    ss << "|" << keyFormatter(kv.first) << "|"
       << Translate(names, kv.first, kv.second) << "|\n";
  }

  return ss.str();
}

string DiscreteValues::html(const KeyFormatter& keyFormatter,
                            const Names& names) const {
  stringstream ss;

  // Print out preamble.
  ss << "<div>\n<table class='DiscreteValues'>\n  <thead>\n";

  // Print out header row.
  ss << "    <tr><th>Variable</th><th>value</th></tr>\n";

  // Finish header and start body.
  ss << "  </thead>\n  <tbody>\n";

  // Print out all rows.
  for (const auto& kv : *this) {
    ss << "    <tr>";
    ss << "<th>" << keyFormatter(kv.first) << "</th><td>"
       << Translate(names, kv.first, kv.second) << "</td>";
    ss << "</tr>\n";
  }
  ss << "  </tbody>\n</table>\n</div>";
  return ss.str();
}

string markdown(const DiscreteValues& values, const KeyFormatter& keyFormatter,
                const DiscreteValues::Names& names) {
  return values.markdown(keyFormatter, names);
}

string html(const DiscreteValues& values, const KeyFormatter& keyFormatter,
            const DiscreteValues::Names& names) {
  return values.html(keyFormatter, names);
}

}  // namespace gtsam
