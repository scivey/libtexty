#pragma once
#include <gumbo.h>

namespace texty { namespace html {

class GumboVectorWrapper {
 protected:
  const GumboVector *gumboVec_;
 public:
  GumboVectorWrapper();
  GumboVectorWrapper(const GumboVector *);
  bool good() const;
  operator bool() const;
  const GumboNode* at(size_t idx) const;
  const GumboNode* operator[](size_t idx) const;
  size_t size() const;

  class Iterator {
    GumboVectorWrapper *parent_;
    size_t idx_;
   public:
    Iterator(GumboVectorWrapper*, size_t);
    bool operator!=(const Iterator&other) const;
    Iterator& operator++();
    Iterator operator++(int);
    const GumboNode* operator*();
    const GumboNode* operator->();
  };
  friend class Iterator;

  Iterator begin();
  Iterator end();
};


}} // texty::html
