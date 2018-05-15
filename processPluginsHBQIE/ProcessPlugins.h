#ifndef PROCESSPLUGINS_H
#define PROCESSPLUGINS_H

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

#include <string>
#include <vector>

class RunSummary
{
public:
    const std::string& plugin;
    const std::string& file;
    const std::string& histVar;
    const std::string& histName;    
    const std::string& runNum;
};

class ProcessPlugins
{
private:
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
        bool verb;

        //Info for main line fit
        double min1;
        double max1;
        double min2;
        double max2;
        double set1;
        double set2;
        double fitmin;
        double fitmax;

        //Info for second line fit
        bool second;
        double min12;
        double max12;
        double min22;
        double max22;
        double set12;
        double set22;
        double fitmin2;
        double fitmax2;

        //Graph info
        double gxmin, gxmax, gymin, gymax;
        std::vector<double> mean, rms, sigma;
        
        void setGraphInfo(std::vector<double> m, std::vector<double> r, std::vector<double> s)
        {
            mean  = m;
            rms   = r;
            sigma = s;
        }        
    };

    static double lineFun(double* x, double* p)
    {
        //p[0]: Slope of the line
        //p[1]: y-intercept of the line
        return p[0]*x[0] + p[1];
    }

    void printFitInfo(TF1* fit)
    {
        printf(
            "Chi^2:%10.4f, P0:%8.4f +/- %8.4f  P1:%8.4f +/- %8.4f\n",
            fit->GetChisquare(), fit->GetParameter(0), fit->GetParError(0), fit->GetParameter(1), fit->GetParError(1)
            );                
    }

    void drawFitInfo(TF1* fit, double x, double y)
    {
        char chi2[100], slope[100], b[100];
        std::string fix = "";
        if(fit->GetParameter(1) < 0) fix = " ";
        
        sprintf(chi2,  "#chi^{2} %18s %.3f"             , "", fit->GetChisquare() );
        sprintf(slope, "slope %10s %s %.3f #pm %.3f"    , "", fix.c_str(), fit->GetParameter(0), fit->GetParError(0));
        sprintf(b,     "y-intercept %2s %.3f #pm %.3f"  , "", fit->GetParameter(1), fit->GetParError(1));

        TLatex mark;
        mark.SetNDC(true);
        mark.SetTextAlign(11);
        mark.SetTextSize(0.030);
        mark.DrawLatex( gPad->GetLeftMargin() + x, 1 - (gPad->GetTopMargin() + y        ),  chi2);
        mark.DrawLatex( gPad->GetLeftMargin() + x, 1 - (gPad->GetTopMargin() + y + 0.03 ), slope);
        mark.DrawLatex( gPad->GetLeftMargin() + x, 1 - (gPad->GetTopMargin() + y + 0.06 ),     b);    
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
        TH1* temp = new TH1F("dummy","dummy",10,p.gxmin,p.gxmax);
        //temp->GetXaxis()->SetRangeUser(0, 1);
        //temp->GetXaxis()->SetRangeUser(0, 1);
        temp->SetMinimum(p.gymin);
        temp->SetMaximum(p.gymax);
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

        TLatex mark;
        mark.SetNDC(true);
        mark.SetTextAlign(11);
        mark.SetTextSize(0.031);
        mark.SetTextFont(1);
        mark.DrawLatex( 0.09, 0.95, p.histName.c_str());
    
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
        drawFitInfo(fit1, 0.1, 0.13);
        if(p.verb) printFitInfo(fit1);
        
        if(p.second)
        {
            TF1* fit2 = new TF1("line2", lineFun, p.fitmin2, p.fitmax2, 2);
            fit2->SetParLimits(0, p.min12, p.max12); 
            fit2->SetParLimits(1, p.min22, p.max22); 
            fit2->SetParameter(0, p.set12);
            fit2->SetParameter(1, p.set22);
            fit2->SetLineWidth(2);
            fit2->SetLineColor(kBlue);
            hfit->Fit(fit2, "RQM", "", p.fitmin2, p.fitmax2);
            fit2->Draw("same");
            leg->AddEntry(fit2,"Linear Fit","l");
            drawFitInfo(fit2, 0.1, 0.23);
            if(p.verb) printFitInfo(fit2);
        }
    
        gSystem->Exec( ("mkdir -p run"+p.runNum+"/").c_str() ) ;
        c1->Print(("run"+p.runNum+"/"+p.histName+".png").c_str());
        
        delete c1;
        delete leg;
        delete temp;
        delete fit1;
    }

    template<typename G> G* makeTGraph(const PluginSummary& p,
                                       const std::vector<double>& mVec, const std::vector<double>& mVecError,
                                       const std::vector<double>& kVec, const std::vector<double>& kVecError)
    {
        int n = mVec.size();
        double x[n], ex[n], y[n], ey[n];
        for(int i = 0; i < n; i++)
        {
            x[i] = mVec[i]; ex[i] = mVecError[i];
            y[i] = kVec[i]; ey[i] = kVecError[i];
            if(p.verb) std::cout<<x[i]<<" +/- "<<ex[i]<<" "<<y[i]<<" +/- "<<ey[i]<<std::endl;
        }
    
        G* gFit = nullptr;
        if(typeid(G) == typeid(TGraph))
            gFit = static_cast<G*>( new TGraph (n, x, y) );    
        else if(typeid(G) == typeid(TGraphErrors))
            gFit = static_cast<G*>( new TGraphErrors (n, x, y, ex, ey) );

        return gFit;
    }

    template<typename G> void processRatios(const PluginSummary& p)
    {
        std::vector<double> ratio,      ratioError;
        std::vector<double> knownRatio, knownRatioError;
        double meanMax  = *max_element(p.mean.begin(), p.mean.end());
        double knownMax = *max_element(p.known.begin(), p.known.end());
        
        for(int index = 0; index < p.known.size(); index++)
        {
            if(p.verb) std::cout<<"Measured: "<<p.mean[index]<<" +/- "<<p.sigma[index]<<"  Ref: "<<p.known[index]<<std::endl;
            ratio.push_back( p.mean[index] / meanMax );
            ratioError.push_back( p.sigma[index] / meanMax );
            knownRatio.push_back( p.known[index] / knownMax );
            knownRatioError.push_back(0.01);
        }
        
        G* gFit = makeTGraph<G>(p, ratio, ratioError, knownRatio, knownRatioError);
        fitHisto<G>(p, gFit);
        
        delete gFit;
    }

    template<typename G> void processMeans(const PluginSummary& p)
    {
        std::vector<double> x, xError, yError;
        for(int index = 0; index < p.mean.size(); index++)
        {
            if(p.verb) std::cout<<"Measured: "<<p.mean[index]<<" +/- "<<p.sigma[index]<<std::endl;
            x.push_back(index + 1);
            xError.push_back(0.1);
        }

        G* gFit = makeTGraph<G>(p, x, xError, p.mean, p.sigma);
        fitHisto<G>(p, gFit);
        
        delete gFit;        
    }
    
public:
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
                r.plugin, "Run"+r.runNum+"_"+r.plugin+"_"+r.histName, "Measured Gain", "Reference Gain", r.runNum, 100, verb,
                0, 2, -1, 1, 1, 0, 0, 1,
                false, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 1, 0.00000001, 1.00001,
            };
        }
        else if(r.plugin == "iQiScan")
        {
            p = {
                //{90, 180, 360, 620, 1540, 2880, 5760, 8640}, //test
                {90, 180, 360, 720, 1440, 2880, 5760, 8640},
                r.plugin, "Run"+r.runNum+"_"+r.plugin+"_"+r.histName, "Measured: Charge / Max Charge", "Reference: Charge / Max Charge", r.runNum, 100, verb,
                0, 2, -1, 1, 1, 0, 0, 1,
                false, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 1, 0.00000001, 1.00001,
            };
        }
        else if(r.plugin == "pedScan")
        {
            p = {
                {},
                r.plugin, "Run"+r.runNum+"_"+r.plugin+"_"+r.histName, "Setting", "Charge [fC]", r.runNum, 100, verb,
                0, 20, -100, 10, 1, 0, 33, 65,
                false, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 66, 0, 100,
            };
        }
        else if(r.plugin == "CapIDpedestalScan")
        {
            p = {
                {},
                r.plugin, "Run"+r.runNum+"_"+r.plugin+"_"+r.histName, "Setting", "Charge [fC]", r.runNum, 100, verb,
                0, 20, -100, 10, 1, 0, 9, 16,
                true, -20, 0, -10, 100, -1, 0, 1, 8,
                0, 17, 0, 50,
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
        p.setGraphInfo(mean, rms, sigma);
                
        if(r.plugin == "iQi_GselScan" || r.plugin == "iQiScan")
        {
            if(gType == "")
            {
                processRatios<TGraph>(p);
            }
            else if(gType == "Error")
            {
                processRatios<TGraphErrors>(p);
            }
        }
        else if(r.plugin == "pedScan" || r.plugin == "CapIDpedestalScan")
        {
            if(gType == "")
            {
                processMeans<TGraph>(p);
            }
            else if(gType == "Error")
            {
                processMeans<TGraphErrors>(p);
            }
        }
    }
};

#endif
