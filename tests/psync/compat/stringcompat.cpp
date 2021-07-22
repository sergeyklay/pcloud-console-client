// This file is part of the pCloud Console Client.
//
// (c) 2021 Serghei Iakovlev <egrep#protonmail.ch>
//
// For the full copyright and license information, please view
// the LICENSE file that was distributed with this source code.

#include <gtest/gtest.h>
#include <string.h>

#include "config.h"

#include "pcloudcc/psync/stringcompat.h"

class CompatTest : public ::testing::Test {
 protected:
  CompatTest() = default;
};

TEST(CompatTest, stringcompat) {
  struct {
    char buf1[16];
    char buf2[16];
  } s;

  memset(&s, '#', sizeof(s));
  EXPECT_EQ(6, strlcpy(s.buf1, "Hello!", 0));
  EXPECT_EQ(0, memcmp(&s, "################################", sizeof(s)));

  memset(&s, '#', sizeof(s));
  EXPECT_EQ(6, strlcpy(s.buf1, "Hello!", 42));
  EXPECT_EQ(0, memcmp(&s, "Hello!\0#########################", sizeof(s)));

  memset(&s, '#', sizeof(s));
  EXPECT_EQ(6, strlcpy(s.buf1, "Hello!", sizeof(s.buf1)));
  EXPECT_EQ(0, memcmp(&s, "Hello!\0#########################", sizeof(s)));

  memset(&s, '#', sizeof(s));
  EXPECT_EQ(15, strlcpy(s.buf1, "Hello, world!12", sizeof(s.buf1)));
  EXPECT_EQ(0, memcmp(&s, "Hello, world!12\0#########################", sizeof(s)));

  memset(&s, '#', sizeof(s));
  EXPECT_EQ(16, strlcpy(s.buf1, "Hello, world!123", sizeof(s.buf1)));
  EXPECT_EQ(0, memcmp(&s, "Hello, world!12\0#########################", sizeof(s)));

  memset(&s, '#', sizeof(s));
  EXPECT_EQ(20, strlcpy(s.buf1, "Hello, world!1234567", sizeof(s.buf1)));
  EXPECT_EQ(0, memcmp(&s, "Hello, world!12\0#########################", sizeof(s)));
}
