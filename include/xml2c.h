#ifndef XML2C_H
#define XML2C_H

#include <string>
#include <memory>
#include "xml_parser.h"
#include "xml_graph.h"
#include "scheduler.h"
#include "codegen.h"


class Xml2c {
public:
	void Run(const std::string& input, const std::string& output, const std::string& prefix);

private:
	void VerifyArgs(const std::string& input, const std::string& output, const std::string& prefix);

	XMLParser parser;
	XMLGraph graph;
	Scheduler sheduler;
	Codegen codegen;
};

#endif
