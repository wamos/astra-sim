/******************************************************************************
This file is part of infsim.

infsim is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

infsim is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with infsim. If not, see <https://www.gnu.org/licenses/>.
*******************************************************************************/

#ifndef __REQUEST_HH__
#define __REQUEST_HH__

#include <iostream>
#include <unistd.h>
#include <stdlib.h>


#include "astra-sim/workload/Workload.hh"
#include "astra-sim/system/Callable.hh"
#include "astra-sim/system/CommunicatorGroup.hh"
#include "astra-sim/workload/HardwareResource.hh"
#include "extern/graph_frontend/chakra/et_feeder/et_feeder.h"
#include "astra-sim/system/RequestHandlerData.hh"

namespace AstraSim {

class Sys;

class Request : public Callable {
 public:
  Request(uint64_t req_id, uint64_t arrival_time, uint32_t prefill_tokens, 
    uint32_t generation_iterations);
  ~Request();

  // event-based simulation
  void call(EventType event, CallData* data);
  // This should trigger the start when time is right  
  void set_arrival_time(uint64_t arrival); 
  void fire();
  void run_pipeline_stage();
  void pause();
  void set_system(Sys** systems, int num_npus);

  // Request is a wrapper for LLM inference workload, i.e. model.
  // The workload on each system/host won't change over time. 
  // The request uses the system/host before it finishes or swapped 
  // out by the scheduler.
  // 
  // A request executes the workload N + 1 times, where
  // N is generation_iterations and 1 is the run for running
  // prefilling for input tokens.
  Sys** systems;
  int num_npus;


  uint64_t req_id;
  RequestrHandlerData* reqhd; 
  uint32_t iteration_finished_systems;
  // absoulate time relative to the start of the simulation
  uint64_t arrival_time; 
  uint32_t prefill_tokens;
  uint32_t generation_iterations;   
  uint32_t current_iterations;

  bool is_scheduled; // true = scheduled, false = paused 
  bool is_finished;
};

} // namespace AstraSim

#endif /* __REQUEST_HH__ */