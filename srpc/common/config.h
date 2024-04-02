#pragma once
#include <string>
#include <memory>
#include <yaml-cpp/yaml.h>
#include <map>
namespace srpc{

class Config{
public:
    typedef std::shared_ptr<Config> ptr;

public:

    static void LoadFromYaml(const char* path);
    static Config::ptr GetGlobalConfig();
public:
    std::string c_logLevel; 
    int16_t     c_listenPort;
    std::map<std::string,YAML::Node> c_nodes;



};



}