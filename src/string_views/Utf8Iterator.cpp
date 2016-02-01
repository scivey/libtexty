#include "string_views/Utf8Iterator.h"
#include <utf8/unchecked.h>

using namespace std;

namespace texty { namespace string_views {

pair<size_t, uint32_t> Utf8Iterator::dereference() const {
  return std::make_pair(distanceFromStart_, utf8::unchecked::peek_next(position_));
}

}} // texty::string_views
