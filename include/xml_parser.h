#ifndef XML_PARSER_H
#define XML_PARSER_H

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <map>

#include "tinyxml2/tinyxml2.h"


enum class BlockType {
    Inport = 0,
    Outport,
    Sum,
    Gain,
    UnitDelay,
    Unknown
};

BlockType FromString(const std::string& s);
std::string_view ToString(const BlockType& type);


struct Block {
    int sid = -1;
    BlockType type = BlockType::Unknown;
    std::string name;
    std::map<std::string, std::string> params;
};


struct Line {
    int src_id = -1;
    int src_port = -1;
    int dst_id = -1;
    int dst_port = -1;
};


class XMLParser {
public:
    void Parse(const char* filename);
    const std::vector<Block>& Blocks() const { return blocks; }
    const std::vector<Line>& Lines() const { return lines; }
    void Dump() const;

private:
    void ParseBlocks(tinyxml2::XMLElement* root);
    void ParseLines(tinyxml2::XMLElement* root);
    void ParseLineParameters(tinyxml2::XMLElement* xml_line, Line& line);
    std::string FixName(std::string name) const;

    std::vector<Block> blocks;
    std::vector<Line> lines;
};

#endif 
