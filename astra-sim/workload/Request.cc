/******************************************************************************
This file is part of infsim.

infsim is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

infsim is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with infsim. If not, see <https://www.gnu.org/licenses/>.
*******************************************************************************/

#include "astra-sim/workload/Workload.hh"
#include "astra-sim/workload/Request.hh"
#include "astra-sim/system/RequestHandlerData.hh"
//#include "astra-sim/system/WorkloadLayerHandlerData.hh"
#include "astra-sim/system/Sys.hh"

#include <iostream>
#include <unistd.h>
#include <stdlib.h>

using namespace std;
using namespace AstraSim;

// *** Request launches a set of systems with their workload accroding to the pipeline stages
// *** once the stage is completed, it launches another stage
// *** We'll do run-to-completion first

Request::Request(uint64_t req_id, uint64_t arrival_time, uint32_t prefill_tokens, 
    uint32_t generation_iterations){
    this->req_id = req_id;
    this->arrival_time = arrival_time;
    this->prefill_tokens = prefill_tokens;
    this->generation_iterations = generation_iterations;
    this->current_iterations = 0;
    this->is_finished = false;   
    this->reqhd = nullptr; 
}

Request::~Request(){
    delete reqhd;
}

void Request::set_system(Sys** systems, int num_npus){
    this->num_npus = num_npus;
    this->systems = systems;
    // RequestrHandlerData will be needed if systems does not run ecah request to completion
    this->reqhd = new AstraSim::RequestrHandlerData[num_npus];
    this->iteration_finished_systems = (uint32_t) num_npus;
}

// per-iteration call
void Request::call(EventType event, CallData* data){
    Tick current_tick = Sys::boostedTick();

    // how do we know which system to launch for
    // It depends on the pipeline stage 
    cout << "arrival_time:" << arrival_time << "\n";
    cout << "current_tick:" << current_tick << "\n";
    cout << "req_id:" << req_id << "\n";
    if (arrival_time > current_tick){
        // for each system in systems: 
        //  register an event of Request at the `arrival_time`
        for (int i = 0; i < num_npus; i++) { // no stage yet
            cout << "registering npu[" << i << "]\n";
            // TODO: assign reqhd[i].req_id = the req id of current request
            reqhd[i].sys_id = i;
            systems[i]->register_event(this, EventType::RequsetScheduled, &reqhd[i], 0, arrival_time);
        }
    }
    else {
        if (event == EventType::RequsetScheduled){            
            AstraSim::RequestrHandlerData* rqhd = (AstraSim::RequestrHandlerData*)data;
            int i = rqhd->sys_id;
            cout << "RequsetScheduled for npu[" << i << "]\n";
            systems[i]->workload->fire();
            iteration_finished_systems--;
        }
        else if (event == EventType::General){
            for (int i = 0; i < num_npus; i++) { // no stage yet
                systems[i]->workload->fire();
                iteration_finished_systems--;
            }
        }
        else{
            cout << "Request call() can't recongnize this event type\n";
        }

        // register events for the next iteration
        if (iteration_finished_systems == 0){
            iteration_finished_systems = num_npus;
            current_iterations++;
            cout << "current_iterations:" << current_iterations << "\n";
            if  (current_iterations == generation_iterations){
                return;
            }

            for (int i = 0; i < num_npus; i++) { // no stage yet
                cout << "registering npu[" << i << "] for next iteration" << current_iterations << "\n";
                reqhd[i].sys_id = i;
                systems[i]->register_event(this, EventType::RequsetScheduled, &reqhd[i], current_tick, 0);
            }            
        }
    }
    // else { // for requests that is about to be lauched or has been launched 
    //     for (int i = 0; i< num_npus; i++) { // no stage yet
    //         systems[i]->workload->fire();
    //         // systems[i]->register_event(this, EventType::General, nullptr, current_tick, 0);
    //         // stingw TODO: we'll need to reinitilize the workload
    //         // and this needs code in the Workload class
    //     }
    //     current_iterations++;
    //     // stingw TODO: later insert "Iteration_Finished" event to trigger the ORCA scheduler.
    //     // it will use "is_scheduled" to determine if a request can run
    //     if (current_iterations < generation_iterations){
    //         call(EventType::General, nullptr); 
    //     }
    // }    
}

// for the prep before 1st iteration
void Request::fire(){  
    Tick current_tick = Sys::boostedTick();
    AstraSim::timespec_t timespec; 
    timespec.time_res = AstraSim::NS;
    timespec.time_val = current_tick; // in ns

    cout << "Request::fire(), req_id:" << req_id << "\n";

    // This will be needed when we do ORCA scheduler
    // for (int i = 0; i< num_npus; i++) { // no stage yet
    //     reqhd[i].req_id = req_id;
    //     reqhd[i].req = this;
    //     reqhd[i].timestamp = timespec; // the timestamp for the fire()
    // }

    call(EventType::General, nullptr);
}

// this should be called from call()
// 1. a pipeline stage means that a set of systems but not all systems
// will be lanched for a stage
// 2. call() needs to differeniate non-pipeline and pipeline execution
// 3. adding new event types: PipelineStageScheduled and PipelineStageFinished
void Request::run_pipeline_stage(){
    // Run a pipeline stage, we'll use a fix stage in code at first.
    call(EventType::PipelineStageScheduled, nullptr);

    // is_scheduled = true;

    // if  (current_iterations == generation_iterations){
    //     is_finished = true;
    // }

    // if is_finished == false: (later with && hardware memory capity is available)
    //  register an event of Request right-away at the cuurrent time for the next stage
    //  this should go to the else{} of call()
}

void Request::pause(){
    is_scheduled = false;
}

void Request::set_arrival_time(uint64_t arrival_time){
    this->arrival_time = arrival_time;
}











// if (current_iterations < generation_iterations && is_scheduled){
//     workload->call(EventType::General, NULL);
//     // this is not nessecry true if we have more than a stage of execcution
//     current_iterations++;
// }

// it's called because it's time for this to be popped out of event queue
// If the Request is not happending right now, register an event of Request at the `arrival_time`
// else we call run() function 

// how do we determine if a request gets to call run()
// 1. the hardware is avialable (like for a stage of pipeline)
// 2. Request is not finished yet
// this will call workload->fire()on the system/hardware of a stage of pipeline 