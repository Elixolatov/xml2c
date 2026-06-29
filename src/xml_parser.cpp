#include <iostream>
#include <stdexcept>
#include "xml_parser.h"

using namespace tinyxml2;


// Convert String to BlockType
BlockType FromString(const std::string& s) {
	if (s == "Inport")
        return BlockType::Inport;
	if (s == "Outport")
        return BlockType::Outport;
	if (s == "Sum")
        return BlockType::Sum;
	if (s == "Gain")
        return BlockType::Gain;
	if (s == "UnitDelay")
        return BlockType::UnitDelay;
	return BlockType::Unknown;
}


// Convert BlockType to String
std::string_view ToString(const BlockType& type) {
	switch (type) {
	    case BlockType::Inport:
            return "Inport";
	    case BlockType::Outport:
            return "Outport";
	    case BlockType::Sum:
            return "Sum";
	    case BlockType::Gain:
            return "Gain";
	    case BlockType::UnitDelay:
            return "UnitDelay";
	    default:
            return "Unknown";
	}
}


// Main method: parse xml file to blocks and lines
void XMLParser::Parse(const char* filename) {
    XMLDocument doc;
    if (doc.LoadFile(filename) != XML_SUCCESS) {
        throw std::runtime_error("Cannot load XML file: " + std::string(filename));
    }

    XMLElement* root = doc.FirstChildElement("System");
    if (!root) {
        throw std::runtime_error("System node not found");
    }

    // clear prev result
    blocks.clear();
    lines.clear();

    // fill storages
    ParseBlocks(root);
    ParseLines(root);
}


// parse each <Block> element
void XMLParser::ParseBlocks(XMLElement* root) {
    XMLElement* xml_block = root->FirstChildElement("Block");
    for (; xml_block != nullptr; xml_block = xml_block->NextSiblingElement("Block")) {
        Block block;

        // set block attributes
        if (const char* sid = xml_block->Attribute("SID")) {
            block.sid = std::stoi(sid);
        }
        if (const char* type = xml_block->Attribute("BlockType")) {
            block.type = FromString(type);
        }
        if (const char* name = xml_block->Attribute("Name")) {
            block.name = FixName(name);
        }

        // parse each <P> element of <Block>
        XMLElement* p = xml_block->FirstChildElement("P");
        for (; p != nullptr; p = p->NextSiblingElement("P")) {
            const char* pname = p->Attribute("Name");
            const char* pval = p->GetText();
            if (pname && pval) {
                block.params[pname] = pval;
            }
        }

        // store block
        blocks.push_back(std::move(block));
    }
}


// parse each <Line> element
void XMLParser::ParseLines(XMLElement* root) {
    XMLElement* xml_line = root->FirstChildElement("Line");
    for (; xml_line != nullptr; xml_line = xml_line->NextSiblingElement("Line")) {
        Line line;

        // parse XML parameters of line
        ParseLineParameters(xml_line, line);

        // line doesn't have branches, save only one line
        XMLElement* xml_branch = xml_line->FirstChildElement("Branch");
        if (xml_branch == nullptr) {
            lines.push_back(std::move(line));
            continue;
        }

        // line has branches, save each line branch
        for (; xml_branch != nullptr; xml_branch = xml_branch->NextSiblingElement("Branch")) {
            Line branch_line = line;
            ParseLineParameters(xml_branch, branch_line);
            lines.push_back(std::move(branch_line));
        }
    }
}


// parse each <P> element of <Line>/<Branch>
void XMLParser::ParseLineParameters(XMLElement* xml_line, Line& line) {
    auto parse_p = [](const std::string& str, int& id, int& port) {
        auto pos1 = str.find('#');
        auto pos2 = str.find(':');
        if (pos1 != std::string::npos && pos2 != std::string::npos) {
            id = std::stoi(str.substr(0, pos1));
            port = std::stoi(str.substr(pos2 + 1));
        }
    };

    XMLElement* xml_p = xml_line->FirstChildElement("P");
    for (; xml_p != nullptr; xml_p = xml_p->NextSiblingElement("P")) {
        const char* name = xml_p->Attribute("Name");
        const char* val = xml_p->GetText();
        if (!name || !val) {
            continue;
        }
        if (strcmp(name, "Dst") == 0) {
            parse_p(val, line.dst_id, line.dst_port);
            continue;
        }
        if (strcmp(name, "Src") == 0) {
            parse_p(val, line.src_id, line.src_port);
        }
    }
}


// Dump parsed <Blocks> and <Lines>
void XMLParser::Dump() const {
    std::cout << "Dump <Blocks>:\n";
    for (const auto& block : blocks) {
        std::cout << "SID:" << block.sid << " " << ToString(block.type) << " " << block.name << "\n";
        for (const auto& it : block.params) {
            std::cout << "\t" << it.first << " " << it.second << "\n";
        }
    }
    std::cout << "\nDump <Lines>:\n";
    for (const auto& line : lines) {
        std::cout << line.src_id << " (p" << line.src_port << ") -> " 
                  << line.dst_id << " (p" << line.dst_port << ")\n";
    }
}


// Fix name of <Block>, remove spaces.
std::string XMLParser::FixName(std::string name) const {
    name.erase(std::remove(name.begin(), name.end(), ' '), name.end());
    return name;
}
