/******************************************************************************
This file is part of infsim.

infsim is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

infsim is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with infsim. If not, see <https://www.gnu.org/licenses/>.
*******************************************************************************/

#include "astra-sim/workload/Workload.hh"
#include "astra-sim/workload/PipelinedRequest.hh"
#include "astra-sim/workload/PipelinedStage.hh"
#include "astra-sim/system/RequestHandlerData.hh"
#include "astra-sim/system/Sys.hh"

#include <iostream>
#include <unistd.h>
#include <stdlib.h>

using namespace std;
using namespace AstraSim;

PipelinedStage::PipelinedStage(uint32_t stage_id, uint32_t max_num_stage){
    this->current_stage_id = stage_id;
    this->max_num_stage = max_num_stage; 
}

PipelinedStage::~PipelinedStage(){
}

void PipelinedStage::set_system(Sys** systems, uint32_t num_npus, uint32_t num_npus_per_stage){ 
    this->num_npus = num_npus;
    this->systems = systems;
    this->num_npus_per_stage = num_npus_per_stage;
    this->stage_running_systems = num_npus_per_stage;   
}

// per stage call on a single system 
void PipelinedStage::call(EventType event, CallData* data){
    Tick current_tick = Sys::boostedTick();
    if (event == EventType::PipelineStageScheduled){
        std::cout << "PipelineStageScheduled\n";
        auto rqhd = (AstraSim::RequestrHandlerData*)data;
        int sys_id = rqhd->sys_id;
        uint32_t stage_id = rqhd->stage_id;

        std::cout << "stage:" << stage_id << ",sys:"<< sys_id << "\n";
        systems[sys_id]->workload->fire(); 
        rqhd->iter++; 

          

        //std::cout << "stage:" << stage_id << ",sys:"<< sys_id << "\n";            

        // stingw: register event for PipelineStageScheduled
        // trigger the next stage with stage_id + 1
        // TODO: do we need to register PipelineStageFinished?
        stage_id = stage_id + 1;
        if (stage_id < max_num_stage){
            // calculate the system id for the next stage
            int next_sys_id = sys_id + (int) num_npus_per_stage;
            AstraSim::RequestrHandlerData* next_req = &rqhd->pipe_req->reqhd[next_sys_id];
            // we don't need next_req->stage_id++ because the stage_id is pre-set when initilizing
            stage_running_systems--;
            if (stage_running_systems == 0){
                // current_stage_id is a global state
                // it increments when all systesm of this stage is finished
                current_stage_id++;
                stage_running_systems = num_npus_per_stage; 
            }
            std::cout << "sys:"<< sys_id << "triggers" << next_sys_id << "\n";
            // stingw: should we launch all the systems of the next stage together or alone?
            systems[next_sys_id]->register_event(this, EventType::PipelineStageScheduled, next_req, current_tick, 0);
        }
        else{
            // go back to the first sys-id
            // e.g. sys id = 5, 13, 21. for 21 to go back to 5. 21/8=2. 21%8=5
            int next_sys_id = sys_id % num_npus_per_stage; 
            // (Callable*) iter for registering IterationFinished
            auto req = rqhd->pipe_req;

            stage_running_systems--;
            if (stage_running_systems == 0){  
                // restore the global state of stage_id and stage_running_systems
                current_stage_id = 0;
                stage_running_systems = num_npus_per_stage;
                req->current_iterations++;
            }
            std::cout << "last stage: sys:"<< sys_id << "triggers" << next_sys_id << "\n";

            AstraSim::RequestrHandlerData* next_req = &rqhd->pipe_req->reqhd[next_sys_id];

            // register event for IterationFinished when all items of the last stage is finished
            systems[next_sys_id]->register_event((Callable*) req, EventType::IterationFinished, next_req, current_tick, 0);
        }
    }    

    // TODO: when a "PipelineStageFinished", update the cycle count of a request here using rqhd->req directly
}