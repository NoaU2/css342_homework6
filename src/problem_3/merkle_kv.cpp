#include <stack>
#include "merkle_kv.h"
#include "sha256/sha256.h"

MerkleKV::MerkleKV(const MerkleKV &other_merkle_kv) {
    this->map_sorted = other_merkle_kv.map_sorted;
    this->meta_tree = other_merkle_kv.meta_tree;
    collect_keys();
}

void MerkleKV::put(const std::string &key, const std::string &val) {
    if (map_sorted.find(key) == map_sorted.end()) {
        return; // does not add new key
    }
    if (map_sorted[key] != val) {
        map_sorted[key] = val;
        update_meta_tree();
    }
}

bool MerkleKV::get(const std::string &key, std::string *val) {
    if (val == nullptr || map_sorted.find(key) == map_sorted.end()) {
        return false;
    }
    *val = map_sorted[key];
    return true;
}

void MerkleKV::update_meta_tree() {

    SHA256 sha256;

    // get hashes on the leaves
    for (const auto &kv: map_sorted) {
        meta_tree.push_back(sha256(kv.second));
    }

    // construct the non-leaf tree nodes
    int nodes_on_curr_level = map_sorted.size() / 2;
    int prev_level_map_sorted_offset = 0;
    while (nodes_on_curr_level != 0) {
        for (int i = 0; i < nodes_on_curr_level; i++) {
            std::string left = meta_tree[prev_level_map_sorted_offset];
            std::string right = meta_tree[prev_level_map_sorted_offset + 1];
            meta_tree.push_back(sha256(left + right));
            prev_level_map_sorted_offset += 2;
        }
        nodes_on_curr_level /= 2;
    }
}

void MerkleKV::collect_keys() {
    for (const auto&[key, value]: map_sorted) {
        keys_sorted.push_back(key);
    }
}

#define ITERATIVE

std::set<std::string> MerkleKV::compare(const MerkleKV &other_kv) {
    if (meta_tree.size() != other_kv.meta_tree.size()) {
        return {};
    }
    std::set<std::string> diffs;

    // iterative compare
#ifdef ITERATIVE
    compare(other_kv.meta_tree, diffs);
#else
    // recursive compare
    int root_idx = meta_tree.size() - 1;
    compare(other_kv.meta_tree, root_idx, diffs);
#endif

    return diffs;
}

// Refernce https://en.wikipedia.org/wiki/Heap_(data_structure) for the calculation of left() and right
int MerkleKV::left(int node_idx) {

    int num_tree_nodes = meta_tree.size() - 1;
    return num_tree_nodes - ((num_tree_nodes - node_idx) * 2 + 2);

}

int MerkleKV::right(int node_idx) {
    int num_tree_nodes = meta_tree.size() - 1;
    return num_tree_nodes - ((num_tree_nodes - node_idx) * 2 + 1);
}

void MerkleKV::compare(const std::vector<std::string> &other_meta_tree,
                       int node_idx,
                       std::set<std::string> &diff_keys) {
    if (meta_tree[node_idx] == other_meta_tree[node_idx]) {
        return;
    }

    int num_of_keys = keys_sorted.size();

    if (node_idx < num_of_keys) {
        diff_keys.insert(keys_sorted[node_idx]);
        return;
    }

    compare(other_meta_tree, left(node_idx), diff_keys);
    compare(other_meta_tree, right(node_idx), diff_keys);
}

void MerkleKV::compare(const std::vector<std::string> &other_meta_tree,
                       std::set<std::string> &diff_keys) {

    std::stack<int> tree_node_stack;
    int num_of_keys = keys_sorted.size();

    int root_idx = meta_tree.size() - 1;
    if (meta_tree[root_idx] == other_meta_tree[root_idx]) {
        return;
    }

    tree_node_stack.push(root_idx);
    while (!tree_node_stack.empty()) {
        auto idx = tree_node_stack.top();
        tree_node_stack.pop();

        if (meta_tree[idx] == other_meta_tree[idx]) {
            continue;
        }

        if (idx < num_of_keys) {
            diff_keys.insert(keys_sorted[idx]);
            continue;
        }

        tree_node_stack.push(left(idx));
        tree_node_stack.push(right(idx));
    }
}