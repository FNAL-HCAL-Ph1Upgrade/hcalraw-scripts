#include "ProcessPlugins.h"

#include <getopt.h>
#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <string>

std::string split(std::string half, std::string s, std::string h)
{
    std::string token;
    if("first"==half)
    {
        token = s.substr(0, s.find(h));
    }
    else if ("last"==half)
    {
        token = s.substr(s.find(h) + h.length(), std::string::npos);
    }
    return token;
}

int main(int argc, char *argv[])
{
    int opt, option_index = 0;
    std::string runFile = "";
    static struct option long_options[] = {
        {"runFile", required_argument, 0, 'f'},
    };

    while((opt = getopt_long(argc, argv, "f:", long_options, &option_index)) != -1)
    {
        switch(opt)
        {
        case 'f': runFile = optarg; break;
        }
    }

    const std::string& pluginType = split("first", split("last", runFile,"-") ,".root");
    const std::string& runNum  = split("last",  split("first",runFile,"-") ,"run").c_str();
    //const std::vector<const std::string>& SLOT2_FIBERS = {"0", "1", "2", "3", "4", "5", "7", "8"};
    //const int chNum = 8;

    const std::vector<const std::string>& SLOT2_FIBERS = {"2"};
    const int chNum = 1;

    std::string firstPart;
    if(pluginType == "iQi_GselScan" || pluginType == "iQiScan") firstPart = "TS_3_Charge_vs_EvtNum_";
    else if(pluginType == "pedScan" || pluginType == "iQi_phaseScan") firstPart = "Charge_vs_EvtNum_";
    else if(pluginType == "CapIDpedestalScan") firstPart = "CapID_1_Charge_vs_EvtNum_";
    
    for(const auto& fib : SLOT2_FIBERS)
    {
        for(int ch = 0; ch < chNum; ch++)
        {
            std::cout << firstPart+"FED_1776_Crate_41_Slot_2_Fib_" + fib + "_Ch_" + std::to_string(ch) << std::endl;
            RunSummary r = {pluginType, runFile, firstPart, "FED_1776_Crate_41_Slot_2_Fib_"+fib+"_Ch_"+std::to_string(ch), runNum};
            ProcessPlugins p;
            //p.processPlugins(r, "", false);
            p.processPlugins(r, "Error", true);
        }
    }
}
