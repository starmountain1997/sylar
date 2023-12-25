//
// Created by guozr on 23-12-21.
//

#include <gtest/gtest.h>
#include "sylar/config.h"


TEST(Test_Config, BasicAssertions) {
    sylar::ConfigVar<int>::ptr g_int_value_config =
            sylar::Config::Lookup("system.port", (int) 8080, "system port");

    sylar::ConfigVar<float>::ptr g_int_valuex_config =
            sylar::Config::Lookup("system.port", (float) 8080, "system port");

    sylar::ConfigVar<float>::ptr g_float_value_config =
            sylar::Config::Lookup("system.value", (float) 10.2f, "system value");

    sylar::ConfigVar<std::vector<int> >::ptr g_int_vec_value_config =
            sylar::Config::Lookup("system.int_vec", std::vector<int>{1, 2}, "system int vec");

    sylar::ConfigVar<std::list<int> >::ptr g_int_list_value_config =
            sylar::Config::Lookup("system.int_list", std::list<int>{1, 2}, "system int list");

    sylar::ConfigVar<std::set<int> >::ptr g_int_set_value_config =
            sylar::Config::Lookup("system.int_set", std::set<int>{1, 2}, "system int set");

    sylar::ConfigVar<std::unordered_set<int> >::ptr g_int_uset_value_config =
            sylar::Config::Lookup("system.int_uset", std::unordered_set<int>{1, 2}, "system int uset");

    sylar::ConfigVar<std::map<std::string, int> >::ptr g_str_int_map_value_config =
            sylar::Config::Lookup("system.str_int_map", std::map<std::string, int>{{"k", 2}}, "system str int map");

    sylar::ConfigVar<std::unordered_map<std::string, int> >::ptr g_str_int_umap_value_config =
            sylar::Config::Lookup("system.str_int_umap", std::unordered_map<std::string, int>{{"k", 2}},
                                  "system str int map");


}