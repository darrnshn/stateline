//!
//! Contains comms data structures representing jobs and results.
//!
//! \file comms/datatypes.hpp
//! \author Lachlan McCalman
//! \date 2014
//! \license Lesser General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include <string>
#include <chrono>

namespace stateline
{
  namespace comms
  {

    //! Numeric Job type.
    typedef uint JobType;
   
    //! Numeric Job ID associated with each job type.
    typedef uint JobID;

    //! High resolution clock used for heartbeating
    typedef std::chrono::high_resolution_clock hrc;

    //! Abstraction of job specification.
    struct JobData
    {
      //! Type of job
      JobType type;

      //! Data common to all jobs
      std::string globalData;

      //! Data specific to this job
      std::string jobData;

      JobData() { }

      JobData(const JobData &) = default;

      JobData(JobType type, const std::string &globalData, const std::string &jobData)
        : type(type), globalData(globalData), jobData(jobData) { }

      JobData(JobType type, std::string&& globalData, std::string&& jobData)
        : type(type), globalData(globalData), jobData(jobData) { }

      JobData(JobData&& other)
        : type(other.type), globalData(std::move(other.globalData)), jobData(std::move(other.jobData)) { }
    };

    //! Abstraction of job results.
    struct ResultData
    {
      //! Type of job
      JobType type;

      //! Results data
      std::string data;

      ResultData() { }

      ResultData(const ResultData &) = default;

      ResultData(JobType type, const std::string &data)
        : type(type), data(data) { }

      ResultData(JobType type, std::string&& data)
        : type(type), data(std::move(data)) { }

      ResultData(ResultData&& other)
        : type(other.type), data(std::move(other.data)) { }

      ResultData &operator= (ResultData&& other)
      {
        if (this == &other) return *this;

        type = other.type;
        data = std::move(other.data);
        return *this;
      }
    };
    

  } // namespace comms
} // namespace stateline
