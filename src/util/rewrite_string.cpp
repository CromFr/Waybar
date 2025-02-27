#include "util/rewrite_string.hpp"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <regex>

namespace waybar::util {
std::string rewriteString(const std::string& value, const Json::Value& rules) {
  if (!rules.isObject()) {
    return value;
  }

  std::string res = value;

  for (auto it = rules.begin(); it != rules.end(); ++it) {
    if (it.key().isString() && it->isString()) {
      try {
        // malformated regexes will cause an exception.
        // in this case, log error and try the next rule.
        const std::regex rule{it.key().asString(), std::regex_constants::icase};
        if (std::regex_match(value, rule)) {
          res = std::regex_replace(res, rule, it->asString());
        }
      } catch (const std::regex_error& e) {
        spdlog::error("Invalid rule {}: {}", it.key().asString(), e.what());
      }
    }
  }

  return res;
}

std::string rewriteStringOnce(const std::string& value, const Json::Value& rules,
                              bool& matched_any) {
  if (!rules.isObject()) {
    return value;
  }

  matched_any = false;

  std::string res = value;

  for (auto it = rules.begin(); it != rules.end(); ++it) {
    if (it.key().isString() && it->isString()) {
      try {
        const std::regex rule{it.key().asString(), std::regex_constants::icase};
        if (std::regex_match(value, rule)) {
          matched_any = true;
          return std::regex_replace(res, rule, it->asString());
        }
      } catch (const std::regex_error& e) {
        spdlog::error("Invalid rule {}: {}", it.key().asString(), e.what());
      }
    }
  }

  return value;
}
}  // namespace waybar::util
