#include "astra-sim/workload/Workload.hh"
#include "astra-sim/system/Callable.hh"
#include "astra-sim/workload/RequestScheduler.hh"
#include "astra-sim/system/RequestSchedulerData.hh"

using namespace AstraSim;

RequestScheduler::RequestScheduler(uint64_t batch_size, uint64_t cluster_resource){
    this->max_batch_size = batch_size;
    this->current_batch_size = 0;
    this->available_resource = cluster_resource;
}

void RequestScheduler::call(EventType event, CallData* data){
		// .... 
		// queued in the request pool
		if (event == EventType::SchedulerCalled){   				
                auto schd_handler = (AstraSim::RequestSchedulerData*)data;

                // 1. check the available memory capacity
                uint64_t tokens = schd_handler->req_tokens;
                uint64_t remaining = available_resource - tokens;
                // 2. check the max batch size
                if (current_batch_size < max_batch_size && remaining > 0){
                    available_resource = remaining;
                }
				 
				// the memory space is reserved for all tokens at once
				// This needs some proper math here but we'll make it simple now

				// 3. If yes, the request is okay to join the execution
					// current_batch_size++;
					// system[0]->register_event(pipe_req, EventType::RequsetScheduled, ....)

			  // 4. If no, what do we do now?
					// we do nothing because when every iteration ends, 
					// there will be a "systems[sys_id]->register_event(scheduler, EventType::SchedulerCalled, req, current_tick, 0)"
					// from PipelinedStage::call(...)
		}
		// .... 
}
