#include "gtest/gtest.h"
#include "problem_3/merkle_kv.h"
#include "unit_test_merkle_kv.h"
#include <chrono>

using namespace std::chrono;

TEST(problem_3, sorted_map_keys) {
    std::map<std::string, std::string> data = {
            {"b",    "y"},
            {"a1",   "x1"},
            {"c12",  "z11"},
            {"a",    "x"},
            {"a111", "x1111"},
            {"c1",   "z11"},
            {"c11",  "z11"},
            {"a2",   "y1"},
            {"c11a", "z11"},
            {"b1",   "y11"},
            {"b1",   "y11"},
    };
    std::vector<std::string> keys, keys_sorted;

    // collect all the keys from the input data
    // here keys collected should be in sorted order
    // inside MerkleKV, the same method is used to collect keys in sorted order
    for (const auto&[key, value]: data) {
        keys.push_back(key);
    }

    // manually sort all the keys for comparison
    keys_sorted = keys; // https://stackoverflow.com/questions/14977632/copying-stdvector-prefer-assignment-or-stdcopy
    sort(keys_sorted.begin(), keys_sorted.end());

    // being a little paranoid here
    // to make sure keys collected are indeed in sorted order
    ASSERT_EQ(keys_sorted, keys);

    // verify keys_sorted contains sorted keys
    MerkleKV kv(data);
    auto &kv_keys = kv.get_keys_sorted();
    ASSERT_EQ(keys_sorted, kv_keys);
}

TEST(problem_3, merkle_put_get) {
    std::map<std::string, std::string> expect = {
            {"x", "9"},
            {"a", "1"},
            {"b", "2"},
    };

    MerkleKV kv(expect);

    for (const auto&[key, value]: expect) {
        kv.put(key, value + "x");
    }

    ASSERT_EQ(expect.size(), kv.size());

    for (const auto&[key, value]: expect) {
        std::string actual;
        ASSERT_TRUE(kv.get(key, &actual));
        ASSERT_EQ(expect[key] + "x", actual);
    }
}

TEST(problem_3, merkle_tree_constructors) {
    std::map<std::string, std::string> expect = {
            {"x", "9"},
            {"a", "1"},
            {"b", "2"},
    };

    // test regular constructor
    MerkleKV kv(expect);

    ASSERT_EQ(expect.size(), kv.size());

    for (const auto&[key, value]: expect) {
        std::string actual;
        ASSERT_TRUE(kv.get(key, &actual));
        ASSERT_EQ(expect[key], actual);
    }

    // test copy constructor
    MerkleKV kv_copy(kv);

    ASSERT_EQ(expect.size(), kv_copy.size());

    for (const auto&[key, value]: expect) {
        std::string actual;
        ASSERT_TRUE(kv_copy.get(key, &actual));
        ASSERT_EQ(expect[key], actual);
    }

    // test deep copy
    std::string kv_v1, kv_v2;
    std::string key = "a";
    std::string new_val = "123";

    kv.put(key, new_val);

    kv.get(key, &kv_v1);
    kv_copy.get(key, &kv_v2);
    ASSERT_EQ(new_val, kv_v1);
    ASSERT_EQ(expect[key], kv_v2);
}

TEST(problem_3, update_meta_tree) {
    std::vector<MerkleKVMetaTestCase> test_cases;
    update_meta_tree_test_cases(test_cases);

    for (const auto &test_case: test_cases) {
        MerkleKV kv_copy(test_case.data);
        ASSERT_EQ(test_case.expect_meta_tree, kv_copy.get_meta_tree());
    }
}

TEST(problem_3, diff_keys) {
    std::vector<MerkleKVDiffTestCase> test_cases;
    diff_key_test_cases(test_cases);

    for (const auto &test_case: test_cases) {
        MerkleKV kv1(test_case.kv1);
        MerkleKV kv2(test_case.kv2);

        ASSERT_EQ(test_case.expect, kv1.compare(kv2));
        ASSERT_EQ(test_case.expect, kv2.compare(kv1));
    }
}

TEST(problem_3, diff_keys_large_kv) {
    std::vector<MerkleKVDiffTestCase> test_cases;
    diff_key_test_cases_large_kv(test_cases, 32768); // later to convert to time comparison

    for (const auto &test_case: test_cases) {
        MerkleKV kv1(test_case.kv1);
        MerkleKV kv2(test_case.kv2);

        ASSERT_EQ(test_case.expect, kv1.compare(kv2));
        ASSERT_EQ(test_case.expect, kv2.compare(kv1));
    }
}

TEST(problem_3, diff_keys_compare_time_large_kv) {
    std::vector<MerkleKVDiffTestCase> test_cases;
    const int SIZE = 32768;
    diff_key_test_cases_large_kv_no_diff(test_cases, SIZE); // later to convert to time comparison
    diff_key_test_cases_large_kv(test_cases, SIZE); // later to convert to time comparison

    for (const auto &test_case: test_cases) {
        MerkleKV kv1(test_case.kv1);
        MerkleKV kv2(test_case.kv2);

        // time the merkle tree compare
        auto start = high_resolution_clock::now();

        auto diff_keys = kv1.compare(kv2);

        auto stop = high_resolution_clock::now();
        auto duration_merkle = duration_cast<microseconds>(stop - start);

        ASSERT_EQ(test_case.expect, diff_keys);

        diff_keys.clear();
        std::map<std::string, std::string> map_1 = test_case.kv1;
        std::map<std::string, std::string> map_2 = test_case.kv2;

        // time the key-wise compare
        start = high_resolution_clock::now();
        for (auto const&[key, v1]: map_1) {
            auto v2 = map_2[key];
            if (v1 != v2) {
                diff_keys.insert(key);
            }
        }
        stop = high_resolution_clock::now();
        auto duration_primitive = duration_cast<microseconds>(stop - start);

        ASSERT_EQ(test_case.expect, diff_keys);

        std::cout << "time: merkle(" +
                     std::to_string(duration_merkle.count()) + "), primitive(" +
                     std::to_string(duration_primitive.count()) + ") ms" << std::endl;
    }
}

/*
 * TODO: add your analysis explaining the result of the test diff_keys_compare_time_large_kv
 *
 *
 *
 */