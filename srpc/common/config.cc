
#include "common/config.h"


namespace srpc{
static Config::ptr g_config = nullptr;



void ListAllMember(const std::string& prefix, const YAML::Node& root,std::map<std::string,YAML::Node> &mp){
    mp.insert({prefix,root});
    if(root.IsScalar()){
    }else if(root.IsMap()) {
        for(auto it = root.begin(); it!= root.end();++it){
            ListAllMember(prefix.empty() ? it->first.Scalar() : (prefix + "." + it->first.Scalar()), it->second, mp);
        }  
    }else if(root.IsSequence()){

    }      
}



void Config::LoadFromYaml(const char* path){
    YAML::Node root = YAML::LoadFile(path);
    Config::ptr conf = GetGlobalConfig();
    ListAllMember("",root,conf->c_nodes);

    conf->c_listenPort = conf->c_nodes["server.listen_port"].as<int16_t>();
    conf->c_logLevel = conf->c_nodes["log.level"].as<std::string>();

}
Config::ptr Config::GetGlobalConfig(){
    if(g_config!=nullptr){
        return g_config;
    }
    g_config.reset(new Config);
    return g_config;
}

}