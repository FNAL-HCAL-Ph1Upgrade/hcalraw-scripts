#include "TH1.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TLegend.h"
#include "TMath.h"
#include "TFile.h"
#include "TLatex.h"
#include "TGraphErrors.h"
#include "TSystem.h"
#include "TAxis.h"

#include <getopt.h>
#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <string>

class RunSummary
{
public:
    const std::string& plugin;
    const std::string& file;
    const std::string& histVar;
    const std::string& histName;    
    const std::string& runNum;
};
    
class PluginSummary
{
public:
    std::vector<double> known;
    std::string plugin;
    std::string histName;
    std::string histNameX;
    std::string histNameY;
    std::string runNum;
    int nEvents;

    //Info for line fit
    double min1;
    double max1;
    double min2;
    double max2;
    double set1;
    double set2;
    double fitmin;
    double fitmax;
};

double lineFun(double* x, double* p)
{
    //p[0]: Slope of the line
    //p[1]: y-intercept of the line
    return p[0]*x[0] + p[1];
}

template<typename G> void fitHisto(const PluginSummary& p, G* hfit)
{
    TCanvas* c1 = new TCanvas("c1","c1",800,800);
    gPad->SetTopMargin(0.1);
    gPad->SetBottomMargin(0.12);
    gPad->SetRightMargin(0.05);
    gPad->SetLeftMargin(0.14);
    TLegend* leg = new TLegend(0.3, 0.8, 0.7, 0.9);
    leg->SetTextSize(0.03);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->AddEntry(hfit,"Scan Setting","P");
    //c1.SetLogx();
    //c1.SetLogy();
    TH1* temp = new TH1F("dummy","dummy",10,0,1);
    temp->GetXaxis()->SetRangeUser(0, 1);
    temp->GetXaxis()->SetRangeUser(0, 1);
    temp->SetMinimum(0.00000001);
    temp->SetMaximum(1.00001);
    temp->SetStats(false);
    temp->SetTitle(0);
    temp->SetLineColor(kBlack);
    temp->GetYaxis()->SetTitle(p.histNameY.c_str());
    temp->GetXaxis()->SetTitle(p.histNameX.c_str());
    temp->SetTitleOffset(1,"X");
    temp->SetTitleOffset(1.2,"Y");
    temp->SetTitleSize(0.04,"X");
    temp->SetTitleSize(0.04,"Y");
    temp->Draw();
    c1->Modified();
    hfit->SetLineWidth(3);
    hfit->SetMarkerStyle(21);
    //hfit->SetLineColor(2);
    hfit->Draw("same PE");
    //hfit->Draw("same AP");
    leg->Draw();
    
    //////////////////////
    //Fitting info
    //////////////////////
    TF1* fit1 = new TF1("line", lineFun, p.fitmin, p.fitmax, 2);
    fit1->SetParLimits(0, p.min1, p.max1); 
    fit1->SetParLimits(1, p.min2, p.max2); 
    fit1->SetParameter(0, p.set1);
    fit1->SetParameter(1, p.set2);
    fit1->SetLineWidth(2);
    fit1->SetLineColor(kRed);
    hfit->Fit(fit1, "RQM", "", p.fitmin, p.fitmax);
    fit1->Draw("same");
    leg->AddEntry(fit1,"Linear Fit","l");
    
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
    mark.DrawLatex( gPad->GetLeftMargin() + 0.1, 1 - (gPad->GetTopMargin() + 0.13        ),  chi2);
    mark.DrawLatex( gPad->GetLeftMargin() + 0.1, 1 - (gPad->GetTopMargin() + 0.13 + 0.03 ), slope);
    mark.DrawLatex( gPad->GetLeftMargin() + 0.1, 1 - (gPad->GetTopMargin() + 0.13 + 0.06 ),     b);    
    mark.SetTextSize(0.031);
    mark.SetTextFont(1);
    mark.DrawLatex( 0.09, 0.95, p.histName.c_str());
    
    gSystem->Exec( ("mkdir -p run"+p.runNum+"/").c_str() ) ;
    c1->Print(("run"+p.runNum+"/"+p.histName+".png").c_str());

    delete c1;
    delete leg;
    delete temp;
    delete fit1;
}

void processPlugins(const RunSummary& r, const std::string& gType = "", const bool verb = true)
{
    TH1::AddDirectory(false);
    TFile* f = TFile::Open( r.file.c_str() );
    TH1* scan = (TH1*)f->Get( (r.histVar+r.histName).c_str() );

    f->Close();
    delete f;
    
    PluginSummary p;
    if(r.plugin == "iQi_GselScan")
    {
        p = {
            {1/3.10, 1/4.65, 1/6.20, 1/9.30, 1/12.40, 1/15.50, 1/18.60, 1/21.70, 1/24.80, 1/27.90, 1/31.00, 1/34.10, 1/35.65},
            r.plugin, "Run"+r.runNum+"_"+r.plugin+"_"+r.histName, "Measured Gain", "Reference Gain", r.runNum, 100,
            0, 2, -1, 1, 1, 0, 0, 1
        };
    }
    else if(r.plugin == "iQiScan")
    {
        p = {
            //{90, 180, 360, 620, 1540, 2880, 5760, 8640}, //test
            {90, 180, 360, 720, 1440, 2880, 5760, 8640},
            r.plugin, "Run"+r.runNum+"_"+r.plugin+"_"+r.histName, "Measured: Charge / Max Charge", "Reference: Charge / Max Charge", r.runNum, 100,
            0, 2, -1, 1, 1, 0, 0, 1
        };
    }

    std::vector<double> mean, rms, sigma;
    for(int index = 0; index < scan->GetNbinsX()/p.nEvents; index++)
    {
        double m = 0, m2 = 0; int n = 0;
        for(int bin = 1 + p.nEvents*index; bin <= (p.nEvents) + p.nEvents*index; bin++)
        {
            double c = scan->GetBinContent(bin);
            m += c; m2 += c*c; n++;
            if(verb) std::cout<<index<<"  "<<bin<<"  "<<scan->GetBinContent(bin)<<std::endl;
        }
        if(verb) std::cout<<n<<std::endl;
        mean.push_back( m/n );
        rms.push_back( sqrt(m2/n) );
        if( sqrt(m2/n) - m/n > 0)
            sigma.push_back( sqrt( sqrt(m2/n) - m/n) );
        else
            sigma.push_back(0);
    }
    
    std::vector<double> ratio, ratioError;
    std::vector<double> knownRatio;
    double meanMax = *max_element(mean.begin(), mean.end());
    double knownMax = *max_element(p.known.begin(), p.known.end());

    for(int index = 0; index < p.known.size(); index++)
    {
        if(verb) std::cout<<"Measured: "<<mean[index]<<" +/- "<<sigma[index]<<"  Ref: "<<p.known[index]<<std::endl;
        ratio.push_back( mean[index] / meanMax );
        ratioError.push_back( sigma[index] / meanMax );
        knownRatio.push_back( p.known[index] / knownMax );
    }
    
    int n = ratio.size();
    double x[n], ex[n], y[n], ey[n];
    for(int i = 0; i < ratio.size(); i++)
    {
        x[i] = ratio[i]; ex[i] = ratioError[i];
        y[i] = knownRatio[i]; ey[i] = 0.01;
        if(verb) std::cout<<x[i]<<" +/- "<<ex[i]<<" "<<y[i]<<" +/- "<<ey[i]<<std::endl;
    }
    
    TGraphErrors* gFit = nullptr;
    if(gType == "")
        gFit = static_cast<TGraphErrors*>( new TGraph (n, x, y) );    
    else if(gType == "Error")
        gFit = new TGraphErrors (n, x, y, ex, ey);
    
    fitHisto<TGraphErrors>(p, gFit);
    
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
    //const std::vector<const std::string>& SLOT2_FIBERS = {"0", "1", "2", "3", "4", "5", "7", "8"};
    //const int chNum = 8;

    const std::vector<const std::string>& SLOT2_FIBERS = {"0"};
    const int chNum = 1;
    
    for(const auto& fib : SLOT2_FIBERS)
    {
        for(int ch = 0; ch < chNum; ch++)
        {
            //std::cout << "TS_3_Charge_vs_EvtNum_FED_1776_Crate_41_Slot_2_Fib_" + fib + "_Ch_" + std::to_string(ch) << std::endl;
            RunSummary r = {pluginType, runFile, "TS_3_Charge_vs_EvtNum_", "FED_1776_Crate_41_Slot_2_Fib_"+fib+"_Ch_"+std::to_string(ch), runNum};
            //processPlugins(r, "", false);
            processPlugins(r, "Error", false);
        }
    }
}
