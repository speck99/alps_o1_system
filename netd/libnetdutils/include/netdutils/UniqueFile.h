/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NETDUTILS_UNIQUEFILE_H
#define NETDUTILS_UNIQUEFILE_H

#include <stdio.h>
#include <memory>

#include "netdutils/Fd.h"

namespace android {
namespace netdutils {

struct UniqueFileDtor {
    void operator()(FILE* file);
};

using UniqueFile = std::unique_ptr<FILE, UniqueFileDtor>;

}  // namespace netdutils
}  // namespace android

#endif /* NETDUTILS_UNIQUEFILE_H */
