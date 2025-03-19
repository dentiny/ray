// Copyright 2025 The Ray Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>

#include "ray/common/cgroup/cgroup_setup.h"
#include "ray/common/test/testing.h"

namespace ray::internal {

namespace {

// Precondition: cgroup V2 has already been mounted as rw.
//
// Setup command:
// sudo umount /sys/fs/cgroup/unified
// sudo mount -t cgroup2 cgroup2 /sys/fs/cgroup/unified -o rw
TEST(CgroupV2UtilsTest, CgroupV2MountPrepared) {
  // Happy path.
  RAY_ASSERT_OK(IsCgroupV2Prepared("/sys/fs/cgroup"));
}

TEST(CgroupV2UtilsTest, CgroupV2DirectoryNotExist) {
  EXPECT_FALSE(IsCgroupV2Prepared("/tmp/non_existent_folder").ok());
}

TEST(CgroupV2UtilsTest, CgroupV2DirectoryNotWritable) {
  EXPECT_FALSE(IsCgroupV2Prepared("/").ok());
}

TEST(CgroupV2UtilsTest, CgroupV2DirectoryNotOfCgroupV2Type) {
  EXPECT_FALSE(IsCgroupV2Prepared("/tmp").ok());
}

}  // namespace

}  // namespace ray::internal
