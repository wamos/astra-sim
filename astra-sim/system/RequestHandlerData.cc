/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#include "RequestHandlerData.hh"

using namespace AstraSim;

RequestrHandlerData::RequestrHandlerData() {
  sys_id = 0;
  req_id = 0;
  iter = 0;
  stage_id = 0;
  pipe_req = nullptr;
  pipe_stage = nullptr;
}
