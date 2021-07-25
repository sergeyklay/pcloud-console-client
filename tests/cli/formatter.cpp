// This file is part of the pCloud Console Client.
//
// (c) 2021 Serghei Iakovlev <egrep#protonmail.ch>
//
// For the full copyright and license information, please view
// the LICENSE file that was distributed with this source code.

#include "formatter.hpp"

#include <gtest/gtest.h>

class FormatterTest : public ::testing::Test {
 protected:
  FormatterTest() = default;
};

TEST_F(FormatterTest, make_usage) {
  auto app(std::make_shared<CLI::App>());
  auto formatter(std::make_shared<pcloud::cli::Formatter>());
  auto usage = formatter->make_usage(app.get(), "not used");

  EXPECT_EQ(usage, "Usage:\n   [OPTIONS] [--] [ARGUMENTS]\n");
}
