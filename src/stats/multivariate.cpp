//!
//! Contains implementation of the base class of all multivariate distributions.
//!
//! \file stats/multivariate.cpp
//! \author Darren Shen
//! \date 2014
//! \license Lesser General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include "multivariate.hpp"

namespace stateline
{
  namespace stats
  {
    Multivariate::Multivariate(std::size_t length)
      : length_(length)
    {
      // Check that the distribution is indeed multivariate
      assert(length > 1);
    }

    std::size_t Multivariate::length() const
    {
      return length_;
    }
  }
}
