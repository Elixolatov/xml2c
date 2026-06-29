#ifndef SHEDULER_H
#define SHEDULER_H

#include <vector>
#include <unordered_map>
#include "xml_graph.h"


class Scheduler {
public:
    void Schedule(const std::vector<GraphNode*>& graph);
    const std::vector<GraphNode*>& Order() const { return order; }
    void Dump() const;

private:
    std::vector<GraphNode*> order;
};

#endif
