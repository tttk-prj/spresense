/****************************************************************************
 * audio_player_post/worker/src/userproc/src/rcgproc.cpp
 *
 *   Copyright 2018 Sony Semiconductor Solutions Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of Sony Semiconductor Solutions Corporation nor
 *    the names of its contributors may be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#include "rcgproc.h"

/*--------------------------------------------------------------------*/
/*                                                                    */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
void RcgProc::init(InitRcgParam *param)
{
  /* Init signal process. */

  param->result.result_code = CustomprocCommand::ExecOk;
}

/*--------------------------------------------------------------------*/
void RcgProc::exec(ExecRcgParam *param)
{
  /* Execute signal process to input audio data. */

  memcpy(param->exec_cmd.output.addr,
         param->exec_cmd.input.addr,
         param->exec_cmd.input.size);

  param->exec_cmd.output.size = param->exec_cmd.input.size;

  static int s_inform_cnt = 0;
  param->exec_cmd.recog.inform_req = ((s_inform_cnt % 100) == 0) ? 1 : 0;
  param->exec_cmd.recog.param[0] = (s_inform_cnt / 100);
  param->exec_cmd.recog.param[1] = 1;
  param->exec_cmd.recog.param[2] = 2;
  param->exec_cmd.recog.param[3] = 3;
  param->exec_cmd.recog.param[4] = 4;
  param->exec_cmd.recog.param[5] = 5;
  param->exec_cmd.recog.param[6] = 6;
  s_inform_cnt++;

  param->result.result_code = CustomprocCommand::ExecOk;
}

/*--------------------------------------------------------------------*/
void RcgProc::flush(FlushRcgParam *param)
{
  /* Flush signal process. */

  param->flush_cmd.output.size = 0;

  param->result.result_code = CustomprocCommand::ExecOk;
}

/*--------------------------------------------------------------------*/
void RcgProc::set(SetRcgParam *param)
{
  /* Set signal process parameters.
   * Enable/Disable and Coef.
   */

  m_enable = param->enable;

  param->result.result_code = CustomprocCommand::ExecOk;
}

