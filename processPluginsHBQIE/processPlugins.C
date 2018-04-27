#include "TROOT.h"
#include "TObject.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TLegend.h"
#include "TMath.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TList.h"
#include "TLatex.h"
#include "TGraph.h"
#include "TSystem.h"

#include <getopt.h>
#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <string>

double lineFun(double* x, double* p)
{
    //p[0]: Slope of the line
    //p[1]: y-intercept of the line
    return p[0]*x[0] + p[1];
}

void fitHisto(const std::string& plugin, TGraph* hfit, const std::string& histName, const std::string& runNum)
{
    TCanvas c1("c1","c1",800,800);
    gPad->SetTopMargin(0.1);
    gPad->SetBottomMargin(0.12);
    gPad->SetRightMargin(0.05);
    gPad->SetLeftMargin(0.14);
    TLegend* leg = new TLegend(0.3, 0.8, 0.7, 0.9);
    leg->SetTextSize(0.03);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->AddEntry(hfit,"Data","P");
    //c1.SetLogx();
    //c1.SetLogy();
    TH1* temp = new TH1F("dummy","dummy",10,0,1);
    temp->GetXaxis()->SetRangeUser(0, 1);
    temp->GetXaxis()->SetRangeUser(0, 1);
    temp->SetMinimum(0.00000001);
    temp->SetMaximum(1.00001);
    temp->SetStats(false);
    temp->SetTitle( (plugin+"  "+histName).c_str() );
    temp->SetLineColor(kBlack);
    temp->GetYaxis()->SetTitle("y_axis");
    temp->GetXaxis()->SetTitle("x_axis");
    temp->SetTitleOffset(1,"X");
    temp->SetTitleOffset(1.2,"Y");
    temp->SetTitleSize(0.05,"X");
    temp->SetTitleSize(0.05,"Y");
    temp->Draw();
    hfit->SetLineWidth(3);
    hfit->SetMarkerStyle(21);
    //hfit->SetLineColor(2);
    hfit->Draw("same P");
    //hfit->Draw("same AP");
    leg->Draw();
    
    //////////////////////
    //Fitting info
    //////////////////////
                                                     
    double min1   =   0; double max1   = 200;   
    double min2   =  -1; double max2   =   1;   
    double set1   =   6;                         
    double set2   =   0;
    double fitmin =   0; double fitmax = 1;
    
    TF1* fit1 = new TF1("line", lineFun, 0.0, 1.0, 3);
    fit1->SetParLimits(0, min1, max1); 
    fit1->SetParLimits(1, min2, max2); 
    fit1->SetParameter(0, set1);
    fit1->SetParameter(1, set2);
    fit1->SetLineWidth(2);
    fit1->SetLineColor(kRed);
    hfit->Fit(fit1, "RQM", "", fitmin, fitmax);
    fit1->Draw("same");
    leg->AddEntry(fit1,"Fit","l");
    
    printf(
           "Chi^2:%10.4f, P0:%8.4f +/- %8.4f  P1:%8.4f +/- %8.4f\n",
           fit1->GetChisquare(), fit1->GetParameter(0), fit1->GetParError(0), fit1->GetParameter(1), fit1->GetParError(1)
           );

    char chi2[100];
    char slope[100];
    char b[100];
    std::string fix = "";
    if(fit1->GetParameter(1) < 0) fix = " ";

    sprintf(chi2,  "#chi^{2} %18s %.3f"             , "", fit1->GetChisquare() );
    sprintf(slope, "slope %10s %s %.3f #pm %.3f"    , "", fix.c_str(), fit1->GetParameter(0), fit1->GetParError(0));
    sprintf(b,     "y-intercept %2s %.3f #pm %.3f"  , "", fit1->GetParameter(1), fit1->GetParError(1));

    TLatex mark;
    mark.SetNDC(true);
    mark.SetTextAlign(11);
    mark.SetTextSize(0.030);
    //mark.SetTextFont(61);
    mark.DrawLatex( gPad->GetLeftMargin() + 0.1, 1 - (gPad->GetTopMargin() + 0.13        ),  chi2);
    mark.DrawLatex( gPad->GetLeftMargin() + 0.1, 1 - (gPad->GetTopMargin() + 0.13 + 0.03 ), slope);
    mark.DrawLatex( gPad->GetLeftMargin() + 0.1, 1 - (gPad->GetTopMargin() + 0.13 + 0.06 ),     b);    

    gSystem->Exec( ("mkdir -p run"+runNum+"/").c_str() ) ;
    c1.Print(("run"+runNum+"/FittedPlot_"+histName+".png").c_str());

    delete leg;
    delete temp;
    delete fit1;
}

void processPlugins(const std::string& plugin, const std::string& file, const std::string& histName, const std::string& runNum, const bool verb = true, const int nEvents = 100, const int hmax = 10000)
{
    TH1::AddDirectory(false);
    gROOT->SetStyle("Plain");

    TFile *f = TFile::Open( file.c_str() );
    TH1* scan = (TH1*)f->Get( histName.c_str() );
    
    std::vector<double> known;
    if(plugin == "iQi_GselScan")
    {
        known = {1/3.10, 1/4.65, 1/6.20, 1/9.30, 1/12.40, 1/15.50, 1/18.60, 1/21.70, 1/24.80, 1/27.90, 1/31.00, 1/34.10, 1/35.65};
    }
    else if(plugin == "iQiScan")
    {
        known = {90, 180, 360, 720, 1440, 2880, 5760, 8640};        
    }
    
    std::vector<double> mean;
    for(int index = 0; index < known.size(); index++)
    {
        double m = 0;
        int n = 0;
        for(int bin = 1 + nEvents*index; bin <= (nEvents) + nEvents*index; bin++)
        {
            n++;
            m += scan->GetBinContent(bin);
            if(verb) std::cout<<index<<"  "<<bin<<"  "<<scan->GetBinContent(bin)<<std::endl;
        }
        if(verb) std::cout<<n<<std::endl;
        mean.push_back( m/n );
    }

    std::vector<double> ratio;
    std::vector<double> knownRatio;
    double meanMax = *max_element(mean.begin(), mean.end());
    double knownMax = *max_element(known.begin(), known.end());

    for(int index = 0; index < known.size(); index++)
    {
        if(verb) std::cout<<"Measured: "<<mean[index]<<"  Ref: "<<known[index]<<std::endl;
        ratio.push_back(mean[index]/meanMax);
        knownRatio.push_back( known[index] / knownMax );
    }
    
    int n = ratio.size();
    double x[n], y[n];
    for(int i = 0; i < ratio.size(); i++)
    {
        x[i] = ratio[i];
        y[i] = knownRatio[i];
        if(verb) std::cout<<x[i]<<" "<<y[i]<<std::endl;
    }

    TGraph* gFit = new TGraph (n, x, y);

    fitHisto(plugin, gFit, histName, runNum);

    delete gFit;
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

    const std::string& pluginType = split("first", split("last", runFile,"-") ,".root");
    const std::string& runNum  = split("last",  split("first",runFile,"-") ,"run").c_str();
    const std::vector<const std::string>& SLOT2_FIBERS = {"0", "1", "2", "3", "4", "5", "7", "8"};
    const int& chNum = 8;
    
    for(const auto& fib : SLOT2_FIBERS)
    {
        for(int ch = 0; ch < chNum; ch++)
        {
            //std::cout<<"TS_3_Charge_vs_EvtNum_FED_1776_Crate_41_Slot_2_Fib_" + fib + "_Ch_" + std::to_string(ch) + "_1D"<<std::endl;
            processPlugins(pluginType, runFile, "TS_3_Charge_vs_EvtNum_FED_1776_Crate_41_Slot_2_Fib_"+fib+"_Ch_"+std::to_string(ch)+"_1D", runNum, false);
        }
    }
}

