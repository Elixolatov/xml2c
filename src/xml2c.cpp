#include <filesystem>
#include <fstream>
#include <stdexcept>
#include "xml2c.h"


// Main method: generate C source code from XML diagram
void Xml2c::Run(const std::string& input, const std::string& output, const std::string& prefix) {
    VerifyArgs(input, output, prefix);

    parser.Parse(input.data());
    graph.BuildGraph(parser.Blocks(), parser.Lines());
    sheduler.Schedule(graph.Nodes());

    std::ofstream file(output);
    if (!file) {
        throw std::runtime_error("Cannot open output file: " + output);
    }
    codegen.Generate(file, sheduler.Order(), prefix);
}


// Validates xml2c arguments
void Xml2c::VerifyArgs(const std::string& input, const std::string& output, const std::string& prefix) {
    if (!std::filesystem::exists(input)) {
        throw std::runtime_error("Input file not found: " + input);
    }
    if (output.empty()) {
        throw std::runtime_error("Output file is empty");
    }
    if (prefix.empty()) {
        throw std::runtime_error("Prefix is empty");
    }
}
