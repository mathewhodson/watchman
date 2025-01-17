/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <thread>
#include "watchman/ChildProcess.h"
#include "watchman/PubSub.h"

enum trigger_input_style { input_dev_null, input_json, input_name_list };

class watchman_event;
struct watchman_root;
struct w_query;

struct watchman_trigger_command {
  w_string triggername;
  std::shared_ptr<w_query> query;
  json_ref definition;
  json_ref command;
  watchman::ChildProcess::Environment env;

  bool append_files;
  enum trigger_input_style stdin_style;
  uint32_t max_files_stdin;

  int stdout_flags;
  int stderr_flags;
  std::string stdout_name;
  std::string stderr_name;

  /* While we are running, this holds the pid
   * of the running process */
  std::unique_ptr<watchman::ChildProcess> current_proc;

  watchman_trigger_command(
      const std::shared_ptr<watchman_root>& root,
      const json_ref& trig);
  watchman_trigger_command(const watchman_trigger_command&) = delete;
  ~watchman_trigger_command();

  void stop();
  void start(const std::shared_ptr<watchman_root>& root);

 private:
  std::thread triggerThread_;
  std::shared_ptr<watchman::Publisher::Subscriber> subscriber_;
  std::unique_ptr<watchman_event> ping_;
  bool stopTrigger_{false};

  void run(const std::shared_ptr<watchman_root>& root);
  bool maybeSpawn(const std::shared_ptr<watchman_root>& root);
  bool waitNoIntr();
};

void w_assess_trigger(
    const std::shared_ptr<watchman_root>& root,
    struct watchman_trigger_command* cmd);
std::unique_ptr<watchman_trigger_command> w_build_trigger_from_def(
    const std::shared_ptr<watchman_root>& root,
    const json_ref& trig,
    char** errmsg);
