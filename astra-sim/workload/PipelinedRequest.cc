/******************************************************************************
This file is part of infsim.

infsim is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

infsim is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with infsim. If not, see <https://www.gnu.org/licenses/>.
*******************************************************************************/

#include "astra-sim/workload/Workload.hh"
#include "astra-sim/workload/Request.hh"
#include "astra-sim/workload/PipelinedRequest.hh"
#include "astra-sim/workload/PipelinedStage.hh"
#include "astra-sim/system/RequestHandlerData.hh"
#include "astra-sim/system/Sys.hh"



#include <iostream>
#include <unistd.h>
#include <stdlib.h>

using namespace std;
using namespace AstraSim;

// Note stingw: static batching, the basline of ORCA, will be implemented not here 
// but in the main.cc on top of requests

PipelinedRequest::PipelinedRequest(uint64_t req_id, uint64_t arrival_time, uint32_t prefill_tokens, 
    uint32_t generation_iterations){
    this->req_id = req_id;
    this->arrival_time = arrival_time;
    this->prefill_tokens = prefill_tokens;
    this->generation_iterations = generation_iterations;
    this->current_iterations = 0;
    this->is_finished = false;   
    this->reqhd = nullptr; 
}

PipelinedRequest::~PipelinedRequest(){
    delete reqhd;
    delete stage;
}

void PipelinedRequest::set_system(Sys** systems, uint32_t num_npus, uint32_t max_num_stage){
    this->num_npus = num_npus;
    this->systems = systems;
    this->num_npus_per_stage = num_npus/max_num_stage;
    // RequestrHandlerData will be needed if systems does not run ecah request to completion
    this->reqhd = new AstraSim::RequestrHandlerData[num_npus];
    this->stage = new AstraSim::PipelinedStage(0, max_num_stage);
    stage->set_system(systems, num_npus, num_npus/max_num_stage);

    for (uint32_t i = 0; i < num_npus; i++) {
        // pre-fill the default setup for all systems here 
        reqhd[i].req_id = req_id;
        reqhd[i].sys_id = i;                
        reqhd[i].iter = 0;
        reqhd[i].stage_id = i/num_npus_per_stage;
        reqhd[i].pipe_req = this;
        // reqhd[i].pipe_iter = this->iteration;
        reqhd[i].pipe_stage = this->stage;
    }

}

// per-request call on all systems of a stage
void PipelinedRequest::call(EventType event, CallData* data){
    Tick current_tick = Sys::boostedTick();

    // how do we know which system to launch for
    // It depends on the pipeline stage 
    // cout << "arrival_time:" << arrival_time << "\n";
    // cout << "current_tick:" << current_tick << "\n";
    cout << "req_id:" << req_id << "\n";
    if (event == EventType::RequsetScheduled){   
        if (arrival_time > current_tick){
            // Common case: register event in the future 
            for (uint32_t i = 0; i < num_npus_per_stage; i++) {
                cout << "registering npu[" << i << "] for stage 0\n";
                // pre-fill the default setup for all systems here 
                systems[i]->register_event(this, EventType::PipelineStageScheduled, &reqhd[i], current_tick, arrival_time);
            }
        }
        else {
            // Common case: launch the first stage of the first iteration
            // launch all systems for a stage
            for (uint32_t i = 0; i < num_npus_per_stage; i++) {
                cout << "launching npu[" << i << "] for stage 0\n";
                // register event for "PipelineStageScheduled"
                systems[i]->register_event(stage, EventType::PipelineStageScheduled, &reqhd[i], current_tick, 0);
            }

        }
        // TODO: for preemtive scheduler, register event for the scheduler 
        // to check if resources are avaialble for another request
    }
    else if (event == EventType::IterationFinished){
        // TODO: this will be the place to collect per iteration stats
        // stingw: the possible location to have preemptive scheduling per iteration 
        std::cout << "Iteration Finished\n";
        auto rqhd = (AstraSim::RequestrHandlerData*)data;
        int sys_id = rqhd->sys_id;
        
        // stignw:
        // the iteration counting should be here to ensure
        // the termination
        std::cout << "current_iterations:" << current_iterations << "\n";
        std::cout << "generation_iterations:" << generation_iterations << "\n";

        if  (current_iterations == generation_iterations){
            // TODO: collect all the cycle number 
            std::cout << "Request Finished\n";
            is_finished = true;
            return;
        }

        systems[sys_id]->register_event(stage, EventType::PipelineStageScheduled, &reqhd[sys_id], current_tick, 0);
    }        
}

// for the prep before 1st iteration
void PipelinedRequest::fire(){  
    Tick current_tick = Sys::boostedTick();
    // AstraSim::timespec_t timespec; 
    // timespec.time_res = AstraSim::NS;
    // timespec.time_val = current_tick; // in ns

    cout << "Request::fire(), req_id:" << req_id << "\n";
    // this is the way to bypass scheduler to directly schedule the request
    //is_scheduled = true;
    call(EventType::RequsetScheduled, nullptr);
}

void PipelinedRequest::set_arrival_time(uint64_t arrival_time){
    this->arrival_time = arrival_time;
}