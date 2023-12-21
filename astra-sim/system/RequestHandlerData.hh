/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#ifndef __WORKLOAD_LAYER_HANDLER_DATA_HH__
#define __WORKLOAD_LAYER_HANDLER_DATA_HH__

#include "astra-sim/system/AstraNetworkAPI.hh"
#include "astra-sim/system/BasicEventHandlerData.hh"
// we'll have to make the header include so the pointers could work

namespace AstraSim {

class Request;
class PipelinedRequest;
class PipelinedStage;

class RequestrHandlerData : public BasicEventHandlerData, public MetaData {
 public:
  uint64_t req_id;
  uint32_t iter;  
  uint32_t stage_id;
  // stingw: okay I know these pointers are just ugly but 
  // where do we allocate an array of RequestrHandlerData
  // the decision now is to make it belong to PipelinedRequest/Request
  // the construtor/desturctor of them will handle it  
  Request* req;
  PipelinedRequest* pipe_req;
  PipelinedStage* pipe_stage;    
  RequestrHandlerData();
};

} // namespace AstraSim

#endif /* __WORKLOAD_LAYER_HANDLER_DATA_HH__ */