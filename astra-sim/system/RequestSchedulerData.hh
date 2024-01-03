/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#ifndef __REQ_SCHED_HANDLER_DATA_HH__
#define __REQ_SCHED_HANDLER_DATA_HH__

#include "astra-sim/system/AstraNetworkAPI.hh"
#include "astra-sim/system/BasicEventHandlerData.hh"
// we'll have to make the header include so the pointers could work

namespace AstraSim {

class Request;
class PipelinedRequest;
class PipelinedStage;

class RequestSchedulerData : public BasicEventHandlerData, public MetaData {
 public:
  uint64_t req_tokens;
  RequestSchedulerData();
};

} // namespace AstraSim

#endif /* __REQ_SCHED_HANDLER_DATA_HH__ */