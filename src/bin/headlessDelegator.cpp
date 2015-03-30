//!
//! A demo using Stateline to sample from a Gaussian mixture.
//!
//! This file aims to be a tutorial on setting up a MCMC simulation using
//! the C++ server API of Stateline.
//!
//! \file demoDelegator.cpp
//! \author Lachlan McCalman
//! \author Darren Shen
//! \date 2014
//! \licence Lesser General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include <iostream>
#include <functional>
#include <fstream>
#include <boost/program_options.hpp>

#include <chrono>

#include "infer/sampler.hpp"
#include "infer/adaptive.hpp"
#include "infer/diagnostics.hpp"
#include "infer/logging.hpp"
#include "app/logging.hpp"
#include "app/serial.hpp"
#include "app/signal.hpp"
#include "app/commandline.hpp"
#include "comms/hdelegator.hpp"

// Alias namespaces for conciseness
namespace sl = stateline;
namespace ph = std::placeholders;
namespace po = boost::program_options;
namespace ch = std::chrono;

po::options_description commandLineOptions()
{
  auto opts = po::options_description("Demo Options");
  opts.add_options()
  ("loglevel,l", po::value<int>()->default_value(0), "Logging level")
  ("port,p",po::value<uint>()->default_value(5555), "Port on which to accept worker connections") 
  ;
  return opts;
}

int main(int ac, char *av[])
{
  // --------------------------------------------------------------------------
  // Settings for the demo
  // --------------------------------------------------------------------------

  po::variables_map vm = sl::parseCommandLine(ac, av, commandLineOptions());

  // This allows the user to interrupt the MCMC using Ctrl-C.
  sl::init::initialiseSignalHandler();

  // Initialise the logging settings
  sl::initLogging("server", vm["loglevel"].as<int>(), true, "");
  
  
  std::string globalSpec = "whaaat";
  std::map<sl::comms::JobType, std::string> jobSpecs = {};
  
  uint port = vm["port"].as<uint>();
  auto settings = sl::DelegatorSettings::Default(port);
  settings.heartbeat.msRate = 100000;
  settings.heartbeat.msTimeout = 200000;
  sl::comms::HDelegator delegator(globalSpec, jobSpecs, settings);

  while(!sl::global::interruptedBySignal)
  {
    std::this_thread::sleep_for(ch::milliseconds(500));
  }

  return 0;
}