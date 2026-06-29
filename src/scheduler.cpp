#include <queue>
#include <iostream>
#include "scheduler.h"


// Main method: build operation order from XML graph
void Scheduler::Schedule(const std::vector<GraphNode*>& graph) {
    order.clear();

    // calculates inputs for each GraphNode
    std::unordered_map<GraphNode*, int> inputs_size;
    for (auto& node : graph) {
        // ingore UnitDelay nodes, it requires special handling
        if (node->block->type == BlockType::UnitDelay) {
            continue;
        }

        inputs_size[node] = 0;
        for (auto* input : node->inputs) {
            if (input->block->type == BlockType::UnitDelay)
                continue;
            inputs_size[node]++;
        }
    }

    // find inputs
    std::queue<GraphNode*> q;
    for (const auto& [node, size] : inputs_size) {
        if (size == 0) {
            q.push(node);
        }
    }

    // shedule nodes
    order.reserve(inputs_size.size());
    while (!q.empty()) {
        GraphNode *cur = q.front();
        q.pop();
        order.push_back(cur);
        for (auto out : cur->outputs) {
            inputs_size[out]--;
            if (inputs_size[out] == 0) {
                q.push(out);
            }
        }
    }

    // add UnitDelay in back of order
    for (auto& node : graph) {
        if (node->block->type == BlockType::UnitDelay) {
            order.push_back(node);
        }
    }
}


// Dump execution order
void Scheduler::Dump() const {
    std::cout << "Execution order:\n";
    for (size_t i = 0; i < order.size(); i++) {
        auto block = order[i]->block;
        std::cout << i << ") SID:" << block->sid << " " << ToString(block->type) << " " << block->name << "\n";
    }
}
