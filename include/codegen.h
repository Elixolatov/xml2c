#ifndef CODEGEN_H
#define CODEGEN_H

#include <vector>
#include "xml_graph.h"


class Codegen {
public:
    void Generate(std::ostream& out, const std::vector<GraphNode*>& order, const std::string& prefix);

private:
    void GenerateStorage(const std::vector<GraphNode*>& order) const;
    void GenerateInit(const std::vector<GraphNode*>& order) const;
    void GenerateStep(const std::vector<GraphNode*>& order) const;
    void GeneratePorts(const std::vector<GraphNode*>& order) const;

    std::string GetGain(GraphNode* node) const;
    std::string GetSum(GraphNode* node) const;
    std::string GetUnitDelay(GraphNode* node) const;
    std::string GetField(const Block& block) const;

    std::ostream* out = nullptr;
    std::string prefix;
};

#endif
