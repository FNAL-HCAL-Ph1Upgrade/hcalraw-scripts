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
    class SummaryPlotInfo
    {
    public:
        std::string name;
        int num;
        double min, max;
    };

    std::string plugin;
    double chi2Min1, chi2Max1;
    std::vector<double> min1, max1;
    std::vector<SummaryPlotInfo> parNames;
    
    double chi2Min2, chi2Max2;
    std::vector<double> min2, max2;
    
    PluginPassInfo(std::string plugin_,
                   double chi2Min1_, double chi2Max1_,
                   std::vector<double> min1_, std::vector<double> max1_, std::vector<SummaryPlotInfo> parNames_)
        : plugin(plugin_), chi2Min1(chi2Min1_), chi2Max1(chi2Max1_),
          min1(min1_), max1(max1_), parNames(parNames_), 
          chi2Min2(-1), chi2Max2(-1), min2({}), max2({}) {}

    PluginPassInfo(std::string plugin_,
                   double chi2Min1_, double chi2Max1_,
                   std::vector<double> min1_, std::vector<double> max1_,
                   double chi2Min2_, double chi2Max2_,
                   std::vector<double> min2_, std::vector<double> max2_, std::vector<SummaryPlotInfo> parNames_)
        : plugin(plugin_), chi2Min1(chi2Min1_), chi2Max1(chi2Max1_),
          min1(min1_), max1(max1_),
          chi2Min2(chi2Min2_), chi2Max2(chi2Max2_),
          min2(min2_), max2(max2_), parNames(parNames_) {}
};

void checkFit(const FitResults* r, const PluginPassInfo& p, std::vector<bool>& flags, std::vector<TH1F*>& summaryVec)
{
    double chi2Fit1 = r->fit1->GetChisquare();
    (*summaryVec[0]).Fill(chi2Fit1);
    bool flag = (p.chi2Min1 < chi2Fit1 && chi2Fit1 < p.chi2Max1) ? true : false; 
    flags.push_back(flag);
    
    for(int pram = 0; pram < p.min1.size(); pram++ )
    {
        double val = r->fit1->GetParameter(pram);
        (*summaryVec[1+pram]).Fill(val);
        bool flag = (p.min1[pram] < val && val < p.max1[pram]) ? true : false; 
        flags.push_back(flag);
    }
    
    if(r->fit2 != nullptr)
    {
        double chi2Fit2 = r->fit2->GetChisquare();
        (*summaryVec[p.min1.size()+1]).Fill(chi2Fit2);        
        bool flag = (p.chi2Min2 < chi2Fit2 && chi2Fit2 < p.chi2Max2) ? true : false; 
        flags.push_back(flag);
        
        for(int pram = 0; pram < p.min2.size(); pram++ )
        {
            double val = r->fit2->GetParameter(pram);
            (*summaryVec[p.min1.size()+2+pram]).Fill(val);        
            bool flag = (p.min2[pram] < val && val < p.max2[pram]) ? true : false; 
            flags.push_back(flag);
        }       
    }
}

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
    const std::vector<PluginPassInfo>& plugins = {
        {"gselScan",       0.0,   3.0, {0.95,  -0.01}, {1.05,   0.01}, {{"chi2Fit1",20,0,10}, {"slope",20,0,2}, {"y-intercept",20,-10,5}}},
        //{"iQiScan",        0.0,   4.5, {0.95,  -0.01}, {1.05,   0.01}, {{"chi2Fit1"}, {"slope"},{"y-intercept"}}},
        //{"pedestalScan",   0.0, 360.0, {2.30, -81.00}, {2.50, -75.00}, {{"chi2Fit1"}, {"slope"},{"y-intercept"}}},
        //{"phaseScan",      0.0,  75.0, {20.0, 40.0, 70.0, 89.0, -4.3, -4.3}, {21.0, 45.0, 71.0, 91.0 , -3.8, -3.8}, {{"chi2Fit1"},{"switch1"},{"switch2"},{"switch3"},{"switch4"},{"timeConst1"},{"timeConst2"}}},
        //{"capID0pedestal", 0.0,  25.0, { 1.4,  4.5}, { 1.6,  5.5},
        // 0.0,  18.0, {-1.5, 19.0}, {-1.4, 20.0}, {{"chi2Fit1"}, {"slope1"}, {"y-intercept1"}, {"chi2Fit2"}, {"slope2"}, {"y-intercept2"}}},
        //{"capID1pedestal", 0.0,  30.0, { 1.4,  4.0}, { 1.6,  5.0},
        // 0.0,  20.0, {-1.5, 19.0}, {-1.3, 20.0}, {{"chi2Fit1"}, {"slope1"}, {"y-intercept1"}, {"chi2Fit2"}, {"slope2"}, {"y-intercept2"}}},
        //{"capID2pedestal", 0.0,  32.0, { 1.4,  1.0}, { 1.6,  3.0},
        // 0.0,  14.0, {-1.6, 16.0}, {-1.3, 18.0}, {{"chi2Fit1"}, {"slope1"}, {"y-intercept1"}, {"chi2Fit2"}, {"slope2"}, {"y-intercept2"}}},
        //{"capID3pedestal", 0.0,  34.0, { 1.4,  1.0}, { 1.6,  2.0},
        // 0.0,  25.0, {-1.5, 15.0}, {-1.3, 17.0}, {{"chi2Fit1"}, {"slope1"}, {"y-intercept1"}, {"chi2Fit2"}, {"slope2"}, {"y-intercept2"}}},
    };

    std::vector<std::vector<TH1F*>> summaryPlots;
    for(const auto& p : plugins)
    {
        std::vector<TH1F*> summaryVec;
        for(const auto& s : p.parNames)
        {
            std::string name = p.plugin+" "+s.name;
            TH1F* summary = new TH1F(name.c_str(),name.c_str(),s.num,s.min,s.max);
            summaryVec.push_back(summary);
        }
        summaryPlots.push_back(summaryVec);
    }
    
    // ---------------------------------------------------
    // Loop over all of the channels and make the fit map
    // ---------------------------------------------------

    gErrorIgnoreLevel = kWarning;
    std::map<std::string, std::vector<FitResults*>> resultsMap;
    for(const auto& sf : SLOTS_FIBERS)
    {
        for(int fib = 0; fib <= sf.second; fib++)
        {
            for(int ch = 0; ch <= chNum; ch++)
            {
                std::string channel = "Slot_"+sf.first+"_Fib_"+std::to_string(fib)+"_Ch_"+std::to_string(ch);
                std::cout<<channel<<std::endl;
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

    gErrorIgnoreLevel = kPrint;
    TCanvas* c = new TCanvas("c","c",800,800);
    //TH1F* slopeH = new TH1F("gsel slope","gsel slope",20, 0.5, 1.5);
    for(const auto& ch : resultsMap)
    {
        std::cout<<"-------------"<<ch.first<<"-------------"<<std::endl;
        int index = -1;
        for(const auto* r : ch.second)
        {
            index++;
            std::vector<bool> flags;
            checkFit(r, plugins[index], flags, summaryPlots[index]);
            int i = -1;
            for(const auto& f : flags)
            {
                i++;
                std::cout<<plugins[index].plugin<<" "<<plugins[index].parNames[i].name<<" "<<f<<std::endl;
            }

            //if(plugins[index].plugin == "gselScan")
            //{
            //    double slope = r->fit1->GetParameter(0);
            //    slopeH->Fill(slope);
            //}
            
            delete r;
        }
    }

    for(const auto& v : summaryPlots)
    {
        for(auto* s : v)
        {
            std::string name = s->GetName();
            s->SetLineColor(kBlack);
            s->Draw();
            const std::string& first  = split("first", name, " ");
            const std::string& last  = split("last", name, " ");
            c->Print(("Summary"+first+"_"+last+".png").c_str());
            delete s;
        }
    }
    
    //slopeH->Draw();
    //c->Print("chi2Dist_gselScan.png");
    //delete slopeH;
    delete c;
}
