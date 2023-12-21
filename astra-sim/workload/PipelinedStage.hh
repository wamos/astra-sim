/******************************************************************************
This file is part of infsim.

infsim is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

infsim is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with infsim. If not, see <https://www.gnu.org/licenses/>.
*******************************************************************************/

#ifndef __PIPELINESTAGE_HH__
#define __PIPELINESTAGE_HH__

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <vector>

#include "astra-sim/workload/Workload.hh"
#include "astra-sim/system/Callable.hh"
#include "astra-sim/system/CommunicatorGroup.hh"
#include "astra-sim/workload/HardwareResource.hh"
#include "extern/graph_frontend/chakra/et_feeder/et_feeder.h"
//#include "astra-sim/system/RequestHandlerData.hh"

namespace AstraSim {

class Sys;
class RequestrHandlerData;  
class PipelinedRequest;

class PipelinedStage : public Callable {
 public:
  PipelinedStage(uint32_t stage_id, uint32_t max_num_stage);
  ~PipelinedStage();

  // event-based simulation
  void call(EventType event, CallData* data);
  void set_system(Sys** systems, uint32_t num_npus, uint32_t num_npus_per_stage);

  Sys** systems;

  uint32_t num_npus;
  uint32_t num_npus_per_stage;

  // std::vector<uint32_t> stage_system_ids;
  // Request* req;
  // RequestrHandlerData* reqhd; 

  uint32_t stage_running_systems;
  uint32_t current_stage_id;
  uint32_t max_num_stage;
};

} // namespace AstraSim

#endif /* __PIPELINESTAGE_HH__ */