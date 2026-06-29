#ifndef XML_GRAPH_H
#define XML_GRAPH_H

#include <vector>
#include <unordered_map>
#include "xml_parser.h"


struct GraphNode {
    const Block* block = nullptr;
    std::vector<GraphNode*> inputs;
    std::vector<GraphNode*> outputs;
};


class XMLGraph {
public:
    void BuildGraph(const std::vector<Block>& blocks, const std::vector<Line>& lines);
    const std::vector<GraphNode*>& Nodes() const { return nodes; }
    void Dump() const;

private:
    void AddBlocks(const std::vector<Block>& blocks);
    void AddLines(const std::vector<Line>& lines);
    void Verify() const;

    std::vector<std::unique_ptr<GraphNode>> storage;
    std::vector<GraphNode*> nodes;
};


#endif
