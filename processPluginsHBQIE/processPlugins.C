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

    const std::string& runNum  = split("last",  split("first",runFile,"-") ,"run").c_str();
    //const std::vector<const std::string>& SLOT2_FIBERS = {"0", "1", "2", "3", "4", "5", "7", "8"};
    //const int chNum = 8;
    const std::vector<const std::string>& SLOT2_FIBERS = {"2"};
    const int chNum = 1;
    const std::vector<const std::string>& plugins = {"gselScan","iQiScan","pedestalScan","phaseScan","capID0pedestal", "capID1pedestal", "capID2pedestal", "capID3pedestal"};


    std::map<std::string, std::vector<FitResults*>> resultsMap;
    for(const auto& fib : SLOT2_FIBERS)
    {
        for(int ch = 0; ch < chNum; ch++)
        {
            std::string channel = "Slot_2_Fib_"+fib+"_Ch_"+std::to_string(ch);
            std::vector<FitResults*> results;
            for(const auto& plugin : plugins)
            {
                std::string firstPart = plugin + "_Charge_vs_EvtNum_";
                if(plugin == "phaseScan") firstPart = "_Charge_vs_EvtNum_";

                //std::cout << firstPart + "Slot_2_Fib_" + fib + "_Ch_" + std::to_string(ch) << std::endl;
                RunSummary rs = {plugin, runFile, firstPart, channel, runNum};
                ProcessPlugins p;
                //p.processPlugins(r, "", false);
                p.processPlugins(rs, "Error", false);
                FitResults* r = p.getFitResults();
                results.push_back(r);                
            }
            resultsMap.insert( std::pair<std::string, std::vector<FitResults*>>(channel, results) );
        }
    }
    
    for(const auto& ch : resultsMap)
    {
        int index = -1;
        for(const auto* r : ch.second)
        {
            index++;
            if(r->fit2 != nullptr)
            {
                std::cout<<ch.first<<" "<<plugins[index]<<" "<<r->fit1->GetChisquare()<<" "<<r->fit2->GetChisquare()<<std::endl;
            }
            else
            {
                std::cout<<ch.first<<" "<<plugins[index]<<" "<<r->fit1->GetChisquare()<<std::endl;
            }
            delete r;
        }
    }
}
