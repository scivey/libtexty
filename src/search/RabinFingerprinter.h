#pragma once
#include <algorithm>
#include <string>
#include <type_traits>
#include <glog/logging.h>
namespace texty { namespace search {

template<typename TIter>
uint64_t rabinFingerprint(uint32_t alpha, size_t count,
    TIter start, TIter end) {
  if (count == 0) {
    return 0;
  }
  size_t expo = count - 1;
  uint64_t acc = 0;
  for (TIter it = start; it != end; ++it) {
    auto codePoint = (uint32_t) *it;
    acc += std::pow(codePoint, expo);
    if (expo == 0) {
      break;
    }
    expo--;
  }
  return acc;
}

class RabinFingerprinter {
 protected:
  uint32_t alpha_ {0};
 public:
  RabinFingerprinter(uint32_t alpha);
  uint32_t alpha() const;
  uint64_t hash(const char*, const char*) const;
  uint64_t hash(const char*, size_t) const;

  template<typename TIter>
  uint64_t hash(size_t count, TIter start, TIter end) const {
    return rabinFingerprint(alpha_, count, start, end);
  }

  template<typename TCollection>
  uint64_t hash(const TCollection &collection) const {
    return hash(collection.size(),
      collection.begin(),
      collection.end()
    );
  }

  template<typename TCollection>
  uint64_t hash(const TCollection &collection,
      size_t start, size_t end) const {
    if (end <= start) {
      return 0;
    }
    size_t dist = end - start;
    auto startIt = collection.begin() + start;
    auto endIt = startIt + dist; 
    return hash(dist, startIt, endIt);
  } 
};


}} // texty::search
