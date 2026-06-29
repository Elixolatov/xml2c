#include <iostream>
#include <stdexcept>
#include "xml_graph.h"


// Main method: build graph from parsed XML data and verify it
void XMLGraph::BuildGraph(const std::vector<Block>& blocks, const std::vector<Line>& lines) {
    storage.clear();
    nodes.clear();

    AddBlocks(blocks);
    AddLines(lines);
    Verify();
}


// Create graph nodes for all XML blocks
void XMLGraph::AddBlocks(const std::vector<Block>& blocks) {
    storage.reserve(blocks.size());
    nodes.reserve(blocks.size());
    for (auto& block : blocks) {
        auto node = std::make_unique<GraphNode>();
        node->block = &block;
        nodes.push_back(node.get());
        storage.push_back(std::move(node));
    }
}


// Create graph edges using XML lines
void XMLGraph::AddLines(const std::vector<Line>& lines) {
    // create nodes map by block sid, and verify unique sid
    std::unordered_map<int, GraphNode*> node_by_sid;
    node_by_sid.reserve(storage.size());
    for (auto& node : storage) {
        auto& block = *node->block;
        if (node_by_sid.find(block.sid) != node_by_sid.end()) {
            throw std::runtime_error("Block #" + std::to_string(block.sid) + " is duplicate");
        }
        node_by_sid.emplace(block.sid, node.get());
    }

    // create graph edges
    for (const auto& line : lines) {
        auto* src = node_by_sid[line.src_id];
        if (!src) {
            throw std::runtime_error("Unknown src block #" + std::to_string(line.src_id));
        }
        auto* dst = node_by_sid[line.dst_id];
        if (!dst) {
            throw std::runtime_error("Unknown dst block #" + std::to_string(line.dst_id));
        }

        // order isn't important for outputs
        src->outputs.push_back(dst);

        // it's important to save inputs order
        if (dst->inputs.size() < line.dst_port) {
            dst->inputs.resize(line.dst_port);
        }
        dst->inputs[line.dst_port - 1] = src;
    }
}


// Verify graph structure
void XMLGraph::Verify() const {
    bool has_inport = false;
    bool has_outport = false;

    // verify Input/Outport nodes
    for (auto& node : nodes) {
        if (node->block->type == BlockType::Inport) {
            has_inport = true;
        } else if (node->block->type == BlockType::Outport) {
            has_outport = true;
        }
    }
    if (!has_inport) {
        throw std::runtime_error("Inport block not found");
    }
    if (!has_outport) {
        throw std::runtime_error("Outport block not found");
    }
}


// Dump builded graph
void XMLGraph::Dump() const {
    std::cout << "Dump graph:\n";
    for (const auto& node : nodes) {
        std::cout << "SID:" << node->block->sid << " " << ToString(node->block->type) << " " << node->block->name << "\n";
        for (const auto& in : node->inputs) {
            std::cout << "\tInput: " << in->block->sid << ":" << in->block->name << "\n";
        }
        for (const auto& in : node->outputs) {
            std::cout << "\tOutput: " << in->block->sid << ":" << in->block->name << "\n";
        }
    }
}
