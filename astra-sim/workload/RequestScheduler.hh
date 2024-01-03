/******************************************************************************
This file is part of infsim.

infsim is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

infsim is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with infsim. If not, see <https://www.gnu.org/licenses/>.
*******************************************************************************/

#ifndef __REQUEST_SCHED_HH__
#define __REQUEST_SCHED_HH__

#include <iostream>
#include <unistd.h>
#include <stdlib.h>


#include "astra-sim/workload/Workload.hh"
#include "astra-sim/system/Callable.hh"
#include "astra-sim/system/CommunicatorGroup.hh"
#include "astra-sim/workload/HardwareResource.hh"
#include "extern/graph_frontend/chakra/et_feeder/et_feeder.h"
#include "astra-sim/system/RequestHandlerData.hh"
//#include "astra-sim/workload/PipelinedIteration.hh"

namespace AstraSim {

class Sys;

class RequestScheduler: public Callable {
 public:
  RequestScheduler(uint64_t batch_size, uint64_t cluster_resource);
  ~RequestScheduler();

  // event-based simulation
  void call(EventType event, CallData* data);

  Sys** systems;
  uint32_t num_npus;
  uint64_t max_batch_size;
  uint64_t current_batch_size;
  uint64_t available_resource;

  // or a different kind of handler
  RequestrHandlerData* reqhd;
};

} // namespace AstraSim

#endif /* __REQUEST_SCHED_HH__ */