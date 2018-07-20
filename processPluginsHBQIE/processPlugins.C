#include "ProcessPlugins.h"

#include <getopt.h>
#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <json/json.h>

std::string color(const std::string& text, const std::string& color)
{
    std::string c;
    if(color=="red") c = "31";
    else if(color=="green") c = "32";
    else if(color=="yellow") c = "33";
    else if(color=="blue") c = "34";
    else if(color=="white") c = "37";
    
    return "\033[1;"+c+"m"+ text +"\033[0m";
}

int main(int argc, char *argv[])
{
    int opt, option_index = 0;
    std::string runFile = "";
    std::string tName = "";
    std::string comments = "";
    static struct option long_options[] = {
        {"runFile", required_argument, 0, 'f'},
    };

    while((opt = getopt_long(argc, argv, "f:c:n:", long_options, &option_index)) != -1)
    {
        switch(opt)
	{
            case 'f': runFile  = optarg; break;
            case 'c': comments = optarg; break;
            case 'n': tName    = optarg; break;
	}
    }

    // -------------------------------------------------
    // Define everything needed to process the plugins
    // -------------------------------------------------
    
    const std::string& runNum  = split("last",  split("first",runFile,"-") ,"run").c_str();
    const std::map<std::string, int> SLOTS_FIBERS = { {"1", 23}, {"2", 23} };
    const int chNum = 7;
    whichTS = {1,2,3};
    //const std::map<std::string, int> SLOTS_FIBERS = { {"1" , 7} };
    //const int chNum = 0;

    //--------------------------------------------------------------
    // Change pass fail requirements here
    // {"Name of Scan"} , MinOfCHi2, MaxOfCHi2, {MinOfPram1, MinOfPram2, ...} , {MaxOfPram1, MaxOfPram2, ...}, {{"Name of Pram",BinsOfSummaryPlot,BinMin,BinMax},...}  
    //--------------------------------------------------------------
    const std::vector<PluginPassInfo>& plugins = {
        {"gselScan",       0.0,   25.0, {0.90,  -0.05}, {1.10,   0.01}, {{"chi2Fit1",1000000,0,1000000},{"slope",50, -1, 3},{"y-intercept",50,-2,2}}},
        {"iQiScan",        0.0,   25.0, {0.90,  -0.05}, {1.10,   0.01}, {{"chi2Fit1",1000000,0,1000000},{"slope",50, -1, 3},{"y-intercept",50,-2,2}}},
        {"pedestalScan",   0.0,  350.0, {2.20, -10.00}, {2.80,  15.00}, {{"chi2Fit1", 100000,0,1000000},{"slope",50, -1,10},{"y-intercept",100,-50,50}}},
        {"capID0pedestal", 0.0,   25.0, {1.28,   0.00}, {1.80,  50.00}, {{"chi2Fit1",1000000,0,1000000},{"slope",50,-30,30},{"y-intercept",50,-20,100}}},
        {"capID1pedestal", 0.0,   25.0, {1.28,   0.00}, {1.80,  50.00}, {{"chi2Fit1",1000000,0,1000000},{"slope",50,-30,30},{"y-intercept",50,-20,100}}},
        {"capID2pedestal", 0.0,   25.0, {1.28,   0.00}, {1.80,  50.00}, {{"chi2Fit1",1000000,0,1000000},{"slope",50,-30,30},{"y-intercept",50,-20,100}}},
        {"capID3pedestal", 0.0,   25.0, {1.28,   0.00}, {1.80,  50.00}, {{"chi2Fit1",1000000,0,1000000},{"slope",50,-30,30},{"y-intercept",50,-20,100}}},
        {"phaseScan",      0.0,  600.0, {20.0, 30.0, 70.0, 81.0, -5.3, -5.3}, {23.0, 45.0, 73.0, 91.0 , -3.5, -3.5}, {{"chi2Fit1",1000000, 0,10000000},{"switch1",   50,10,33},
                                                                                                                      {"switch2", 50,20,55},      {"switch3",   50,55,85},
                                                                                                                      {"switch4", 50,65,100},     {"timeConst1",50,-8,-1},{"timeConst2",50,-8,-1}}},
        {"pedestal",       0,30, 0,1, {{"mean",50,0,40},{"sigma",50,-2,4}}},
    };

    std::vector<std::vector<TH1F*>> summaryPlots;
    for(const auto& p : plugins)
    {
        std::vector<TH1F*> summaryVec;
        for(const auto& s : p.parNames)
        {
            std::string name = p.plugin+" "+s.name;
            TH1F* summary = new TH1F(name.c_str(),name.c_str(),s.num,s.min,s.max);
            summary->GetYaxis()->SetTitle("Channels");
            summaryVec.push_back(summary);
        }
        summaryPlots.push_back(summaryVec);
    }
    
    // ----------------------------------------------------
    // Loop over all of the channels and make the fit map
    // ----------------------------------------------------
    std::cout<<"//////////////////////////////////////////////////////"<<std::endl;
    std::cout<<"//////////////Making Map of the Fit Results///////////"<<std::endl;
    std::cout<<"//////////////////////////////////////////////////////"<<std::endl;
    gErrorIgnoreLevel = kWarning;
    std::set<std::string> uniqueIDs;
    std::map<std::string, std::vector<FitResults*>> resultsMap;
    for(const auto& sf : SLOTS_FIBERS)
    {
        for(int fib = 0; fib <= sf.second; fib++)
        {
            TH1::AddDirectory(false);
            TFile* f = TFile::Open( runFile.c_str() );
	    std::string uIDName = "UniqueID_Slot_"+sf.first+"_Fib_"+std::to_string(fib);
	    if (!f->GetListOfKeys()->Contains(uIDName.c_str())) continue;
            TH1* id = (TH1*)f->Get(uIDName.c_str());
            f->Close();
            delete f;
	    std::string uIDresult = split("first", static_cast<std::string>(id->GetTitle()), " ");
            std::string uniqueID  = split("first", split("last", static_cast<std::string>(id->GetTitle()), " "), " ");
            std::string iglooInfo = split("last", split("last", static_cast<std::string>(id->GetTitle()), " "), " ");
            std::string iglooType = split("first", iglooInfo, " ");
            std::string major     = split("first", split("last", iglooInfo, " "), "_");
            std::string minor     = split("last",  split("last", iglooInfo, " "), "_");
            if(uIDresult == "FAIL" || uniqueID.length() == 0 || uniqueID == "0xFFFFFFFF_0xFFFFFF70") continue;
            std::cout<<"Linktest mode: "<<uIDresult<<" UniqueID: "<<uniqueID<<" Igloo type: "<<iglooType<<" Major: "<<major<<" Minor: "<<minor<<std::endl;
            for(int ch = 0; ch <= chNum; ch++)
            {
                std::string channel = "Slot_"+sf.first+"_Fib_"+std::to_string(fib)+"_Ch_"+std::to_string(ch);
                std::cout<<"-------------"<<channel<<"-------------"<<std::endl;
                std::vector<FitResults*> results;
                for(const auto& info : plugins)
                {
                    std::string firstPart = info.plugin + "_Charge_vs_EvtNum_";
                    if(info.plugin == "phaseScan") firstPart = "_Charge_vs_EvtNum_";
    
                    //std::cout << firstPart + channel << std::endl;
                    RunSummary rs = {info.plugin, runFile, firstPart, channel, runNum, uniqueID, iglooType, &info};
                    uniqueIDs.insert(uniqueID);
                    ProcessPlugins p;
                    //p.processPlugins(r, "", false);
                    p.processPlugins(rs, "Error", false);
                    FitResults* r = p.getFitResults();
                    results.push_back(r);
                }
                resultsMap.insert( std::pair<std::string, std::vector<FitResults*>>(channel, results) );
            }
            delete id;
        }
    }

    //Prepare jsonMap for each QIE card
    std::map<std::string, Json::Value> jsonMap;
    for(auto& id : uniqueIDs)
    {
        Json::Value j;
        jsonMap.insert( std::pair<std::string, Json::Value>(id, j) );
    }
    
    // ---------------------------------------------------
    // Check each fit and decide if it passed or failed
    // ---------------------------------------------------
    std::cout<<"//////////////////////////////////////////////////////"<<std::endl;
    std::cout<<"///Checking if fits passed and filling summary plots//"<<std::endl;
    std::cout<<"//////////////////////////////////////////////////////"<<std::endl;
    //gErrorIgnoreLevel = kPrint;
    Json::Value cJson;
    TCanvas* c = new TCanvas("c","c",800,800);
    for(const auto& ch : resultsMap)
    {
        //std::cout<<"-------------"<<ch.first<<"-------------"<<std::endl;
        int index = -1;
        for(const auto* r : ch.second)
        {
            index++;
            std::vector<std::vector<double>> flags;
            checkFit(r, plugins[index], flags, summaryPlots[index]);
            int i = -1;
            for(const auto& f : flags)
            {
                i++;
                //std::cout<<plugins[index].plugin<<" "<<plugins[index].parNames[i].name<<" "<<f<<std::endl;
                Json::Value vec(Json::arrayValue);
                vec.append(Json::Value(f[0]));
                vec.append(Json::Value(f[1]));
		//Json per run
                cJson[r->uniqueID][r->iglooType][ch.first][plugins[index].plugin][plugins[index].parNames[i].name] = vec;
		//Json per card
                jsonMap[r->uniqueID][r->uniqueID][r->iglooType][ch.first][plugins[index].plugin][plugins[index].parNames[i].name] = vec;
		jsonMap[r->uniqueID]["Unique_ID"] = r->uniqueID;
		jsonMap[r->uniqueID]["RunNum"] = runNum;
		jsonMap[r->uniqueID]["Comments"] = comments;
		jsonMap[r->uniqueID]["Tester_Name"] = tName;
		if(int(f[1]) == 0)
		{ 
  		    std::string failInfo = "FAIL: "+r->uniqueID+" "+r->iglooType+" "+ch.first+" "+plugins[index].plugin+" "+plugins[index].parNames[i].name+" "+std::to_string(f[0]);
		    std::cout<<color(failInfo,"red")<<std::endl;
		}
            }
            delete r;
        }
    }

    //Make and fill the Json file
    std::ofstream file_id("qcTestResults/QC_run"+runNum+"/run"+runNum+"_QC.json");
    Json::StreamWriterBuilder wbuilder;
    //wbuilder.settings_["indentation"] = "";
    std::string outputString = Json::writeString(wbuilder, cJson);
    file_id << outputString << std::endl;
    for(auto& j : jsonMap)
    {
        std::ofstream file_id("qcTestResults/QC_run"+runNum+"/"+j.first+"/"+j.first+"_QC.json");
        Json::StreamWriterBuilder wbuilder;
        wbuilder.settings_["indentation"] = "";
        std::string outputString = Json::writeString(wbuilder, j.second);
        file_id << outputString << std::endl;        
    }

    //Draw and save the summary plots
    for(const auto& v : summaryPlots)
    {
        for(auto* s : v)
        {
            TCanvas* canvas = new TCanvas("canvas","canvas",800,800);
            std::string name = s->GetName();
            s->SetLineColor(kRed);
	    s->SetLineWidth(2.0);
            s->Draw();
            const std::string& first  = split("first", name, " ");
            const std::string& last  = split("last", name, " ");
            s->GetXaxis()->SetTitle(last.c_str());;
            if(split("last",name," ")=="chi2Fit1" || split("last",name," ")=="chi2Fit2") 
	    {
		canvas->SetLogx();
		canvas->SetLogy();
	    }
	    canvas->Print(("qcTestResults/QC_run"+runNum+"/Summary_"+first+"_"+last+".png").c_str());
            delete s;
            delete canvas;
        }
    }
    delete c;

    std::cout<<color("Finished processing output of Hcalraw plugins Run"+runNum,"green")<<std::endl;
}
