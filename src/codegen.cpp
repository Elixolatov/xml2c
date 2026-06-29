#include <iostream>
#include "codegen.h"


// Main method: generate C code from operation order
void Codegen::Generate(std::ostream& out, const std::vector<GraphNode*>& order, const std::string& prefix) {
    this->out = &out;
    this->prefix = prefix;

    GenerateStorage(order);
    GenerateInit(order);
    GenerateStep(order);
    GeneratePorts(order);
}


// Generate storage structure
void Codegen::GenerateStorage(const std::vector<GraphNode*>& order) const {
    *out << "#include \"" << prefix << "_run.h\"\n";
    *out << "#include <math.h>\n\n";

    *out << "static struct\n";
    *out << "{\n";
    for (const auto& node : order) {
        const Block& block = *node->block;
        if (block.type == BlockType::Outport) {
            continue;
        }
        *out << "\tdouble " << block.name << ";\n";
    }
    *out << "} " << prefix << ";\n\n";
}


// Generate init function
void Codegen::GenerateInit(const std::vector<GraphNode*>& order) const {
    *out << "void " << prefix << "_generated_init()\n";
    *out << "{ \n";
    for (auto* node : order) {
        const Block& block = *node->block;
        if (block.type != BlockType::UnitDelay) {
            continue;
        }
        *out << "\t" << GetField(block) << " = 0;\n";
    }
    *out << "}\n\n";
}


// Generate step function
void Codegen::GenerateStep(const std::vector<GraphNode*>& order) const {
    *out << "void " << prefix << "_generated_step()\n";
    *out << "{\n";
    for (auto* node : order) {
        const Block& block = *node->block;
        switch (block.type) {
            case BlockType::Inport:
            case BlockType::Outport:
                break;
            case BlockType::Gain:
                *out << "\t" << GetGain(node) << "\n";
                break;
            case BlockType::Sum:
                *out << "\t" << GetSum(node) << "\n";
                break;
            case BlockType::UnitDelay:
                *out << "\t" << GetUnitDelay(node) << "\n";
                break;
        }
    }
    *out << "}\n\n";
}


// Generate input/output ports array
void Codegen::GeneratePorts(const std::vector<GraphNode*>& order) const {
    *out << "static const " << prefix << "_ExtPort ext_ports[] =\n";
    *out << "{\n";
    for (auto* node : order) {
        const Block& block = *node->block;
        if (block.type == BlockType::Inport) {
            *out << "\t{ \"" << block.name << "\", &" << GetField(block) << ", 1 },\n";
            continue;
        }
        if (block.type == BlockType::Outport) {
            auto* source = node->inputs.front();
            *out << "\t{ \"" << block.name << "\", &" << GetField(*source->block) << ", 0 },\n";
        }
    }
    *out << "\t{ 0, 0, 0 },\n";
    *out << "};\n\n";

    *out << "const " << prefix << "_ExtPort* const\n\t" << prefix << "_generated_ext_ports = ext_ports;\n";
    *out << "const size_t\n\t" << prefix << "_generated_ext_ports_size = sizeof(ext_ports);\n\n";
}


// Get Gain instruction
std::string Codegen::GetGain(GraphNode* node) const {
    auto* input = node->inputs.front();
    const auto& block = *node->block;
    return GetField(block) + " = " + 
           GetField(*input->block) + " * " + 
           block.params.at("Gain") + ";";
}


// Get Sum instruction
std::string Codegen::GetSum(GraphNode* node) const {
    std::string res = "";
    const Block& block = *node->block;

    std::string sign;
    auto it = block.params.find("Inputs");
    if (it != block.params.end()) {
        sign = it->second;
    } else {
        sign.resize(node->inputs.size(), '+');
    }

    // generate rvalue and first operand
    res = GetField(block) + " = ";
    if (sign[0] == '-') {
        res += "-";
    }
    res += GetField(*node->inputs[0]->block);

    // generate other operands
    for (size_t i = 1; i < node->inputs.size(); i++) {
        const Block& input = *node->inputs[i]->block;
        res = res + " " + sign[i] + " " + GetField(input);
    }
    res += ";";
    return res;
}


// Get UnitDelay updating instuction
std::string Codegen::GetUnitDelay(GraphNode* node) const {
    auto* input = node->inputs.front();
    return GetField(*node->block) + " = " +
           GetField(*input->block) + ";";
}


// Get accessing structure field
std::string Codegen::GetField(const Block& block) const {
    return prefix + "." + block.name;
}
