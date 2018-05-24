#include "ProcessPlugins.h"

#include <getopt.h>
#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <string>

class PluginPassInfo
{
public:
    std::string plugin;
    double min;
    double max;    
};

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

    // -------------------------------------------------
    // Define everything needed to process the plugins
    // -------------------------------------------------

    const std::string& runNum  = split("last",  split("first",runFile,"-") ,"run").c_str();
    //const std::map<std::string, int> SLOTS_FIBERS = { {"1", 23}, {"2", 7} };
    //const int chNum = 7;
    const std::map<std::string, int> SLOTS_FIBERS = { {"2" , 0} };
    const int chNum = 0;
    const std::vector<PluginPassInfo>& plugins = {{"gselScan",       0.0,   3.0},
                                                  {"iQiScan",        0.0,   3.5},
                                                  {"pedestalScan",   0.0, 360.0},
                                                  {"phaseScan",      0.0,  70.0},
                                                  {"capID0pedestal", 0.0,  25.0},
                                                  {"capID1pedestal", 0.0,  15.0},
                                                  {"capID2pedestal", 0.0,  23.0},
                                                  {"capID3pedestal", 0.0,  21.0},
    };

    // ---------------------------------------------------
    // Loop over all of the channels and make the fit map
    // ---------------------------------------------------
    
    std::map<std::string, std::vector<FitResults*>> resultsMap;
    for(const auto& sf : SLOTS_FIBERS)
    {
        for(int fib = 0; fib <= sf.second; fib++)
        {
            for(int ch = 0; ch <= chNum; ch++)
            {
                std::string channel = "Slot_"+sf.first+"_Fib_"+std::to_string(fib)+"_Ch_"+std::to_string(ch);
                std::vector<FitResults*> results;
                for(const auto& info : plugins)
                {
                    std::string firstPart = info.plugin + "_Charge_vs_EvtNum_";
                    if(info.plugin == "phaseScan") firstPart = "_Charge_vs_EvtNum_";

                    //std::cout << firstPart + channel << std::endl;
                    RunSummary rs = {info.plugin, runFile, firstPart, channel, runNum};
                    ProcessPlugins p;
                    //p.processPlugins(r, "", false);
                    p.processPlugins(rs, "Error", false);
                    FitResults* r = p.getFitResults();
                    results.push_back(r);                
                }
                resultsMap.insert( std::pair<std::string, std::vector<FitResults*>>(channel, results) );
            }
        }
    }

    // ---------------------------------------------------
    // Check each fit and decide if it passed or failed
    // ---------------------------------------------------

    for(const auto& ch : resultsMap)
    {
        int index = -1;
        for(const auto* r : ch.second)
        {
            index++;
            double chi2 = r->fit1->GetChisquare();
            if(r->fit2 != nullptr)
            {
                std::cout<<ch.first<<" "<<plugins[index].plugin<<" "<<chi2<<" "<<r->fit2->GetChisquare()<<std::endl;
                if(plugins[index].min < chi2 && chi2 < plugins[index].max) std::cout<<"pass"<<std::endl;                    
                else std::cout<<"fail"<<std::endl;
            }
            else
            {
                std::cout<<ch.first<<" "<<plugins[index].plugin<<" "<<r->fit1->GetChisquare()<<std::endl;
                if(plugins[index].min < chi2 && chi2 < plugins[index].max) std::cout<<"pass"<<std::endl;                    
                else std::cout<<"fail"<<std::endl;
            }            
            delete r;
        }
    }
}
