// This file is part of the pCloud Console Client.
//
// (c) 2021 Serghei Iakovlev <egrep#protonmail.ch>
//
// For the full copyright and license information, please view
// the LICENSE file that was distributed with this source code.

#include "cli/app.hpp"

#include <gtest/gtest.h>

#include <tuple>
#include <utility>

class AppTest : public testing::TestWithParam<std::tuple<std::string, int>> {
 protected:
  AppTest() : argv(){};
  std::vector<std::string> argv;
};

TEST_P(AppTest, global_options) {
  auto expected = std::get<1>(GetParam());
  const auto& option = std::get<0>(GetParam());

  argv.assign({option});

  auto app = std::make_unique<pcloud::cli::App>(argv);
  auto actual = app->run();

  EXPECT_EQ(expected, actual);
}

INSTANTIATE_TEST_SUITE_P(
    BulkTest, AppTest,
    testing::Values(std::make_tuple("--help", EXIT_SUCCESS),
                    std::make_tuple("--version", EXIT_SUCCESS),
                    std::make_tuple("--vernum", EXIT_SUCCESS),
                    std::make_tuple("-dumpversion", EXIT_SUCCESS),
                    std::make_tuple("--dumpversion", EXIT_SUCCESS)));
