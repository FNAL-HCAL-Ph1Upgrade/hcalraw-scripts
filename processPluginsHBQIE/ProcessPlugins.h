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
#include "TROOT.h"

#include <string>
#include <vector>
#include <math.h>

//Adding an ugly global for the phase scan 
std::vector<int> whichTS;

class FitResults
{
public:
    TF1* fit1;
    TF1* fit2;
    double mean;
    double sigma;
    std::string uniqueID;
    std::string iglooType;
    bool isIncreasing;    

    void setVar(const std::string& name, TF1* var)
    {
        if(name == "fit1") fit1 = var;
        else if(name == "fit2") fit2 = var;
    }

    void setVar(const std::string& name, double var)
    {
        if(name == "mean") mean = var;
        else if(name == "sigma") sigma = var;
    }

    void setVar(const std::string& name, std::string var)
    {
        if(name == "uniqueID") uniqueID = var;
        else if(name == "iglooType") iglooType = var;
    }
    
    FitResults() : fit1(nullptr), fit2(nullptr), mean(0), sigma(0), uniqueID(""), isIncreasing(true)
    {
    }

    ~FitResults()
    {
        delete fit1;
        delete fit2;
    }
};

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
                   double chi2Min2_, double chi2Max2_,
                   std::vector<SummaryPlotInfo> parNames_)
        : plugin(plugin_), chi2Min1(chi2Min1_), chi2Max1(chi2Max1_),
          min1({}), max1({}), parNames(parNames_), 
          chi2Min2(chi2Min2_), chi2Max2(chi2Max2_), min2({}), max2({}) {}

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

class RunSummary
{
public:
    const std::string plugin;
    const std::string file;
    const std::string histVar;
    const std::string channel;
    const std::string runNum;
    const std::string uniqueID;
    const std::string iglooType;
    const PluginPassInfo* ppi;
};

class ProcessPlugins
{
private:
    FitResults* fitResults;
    
    class PluginSummary
    {
    public:
        std::vector<double> known;
        std::string plugin, histName, histNameX, histNameY, runNum, channel, uniqueID, iglooType;
        int nEvents;
        bool verb;
        TH1* scan;
        //Info for main line fit
        bool first;
        double min1, max1, set1, min2, max2, set2, min3, max3, set3, min4, max4, set4;
        double min5, max5, set5, min6, max6, set6; //,min7, max7, set7, min8, max8, set8,
        double fitmin, fitmax;
        //Info for second line fit
        bool second;
        double min12, max12, set12, min22, max22, set22, min32, max32, set32, fitmin2, fitmax2;
        //Graph info
        double gxmin, gxmax, gymin, gymax;
        std::vector<double> mean, rms, sigma, tdc;
        bool doMeanTS;
        TH1* TDC;
        const PluginPassInfo* ppi;
	bool isIncreasing;        

        void setGraphInfo(std::vector<double> m, std::vector<double> r, std::vector<double> s, std::vector<double> t)
        {
            mean  = m;
            rms   = r;
            sigma = s;
            tdc   = t;
        }
        
        void set(std::vector<double> known_, std::string plugin_, std::string histName_, std::string histNameX_, std::string histNameY_, std::string runNum_, std::string channel_, std::string uniqueID_, std::string iglooType_,
                 int nEvents_, bool verb_, TH1* scan_,
                 bool first_,
                 double min1_,  double max1_,  double set1_,  double min2_,  double max2_,  double set2_,  double min3_,  double max3_,  double set3_,  double min4_, double max4_, double set4_,
                 double min5_,  double max5_,  double set5_,  double min6_,  double max6_,  double set6_,  /*double min7_,  double max7_,  double set7_,  double min8_, double max8_, double set8_,*/
                 double fitmin_,  double fitmax_,
                 bool second_,
                 double min12_, double max12_, double set12_, double min22_, double max22_, double set22_, double min32_, double max32_, double set32_, double fitmin2_, double fitmax2_,
                 double gxmin_, double gxmax_, double gymin_, double gymax_,
                 TH1* TDC_ = nullptr,
                 const PluginPassInfo* ppi_ = nullptr)
        {
            known = known_; plugin = plugin_; histName = histName_; histNameX = histNameX_; histNameY = histNameY_; runNum = runNum_; channel = channel_; uniqueID = uniqueID_; iglooType = iglooType_;
            nEvents = nEvents_; verb = verb_; scan = scan_; 
            first = first_;
            min1 = min1_;   max1 = max1_;   set1 = set1_;   min2 = min2_;   max2 = max2_;   set2 = set2_;   min3 = min3_;   max3 = max3_;   set3 = set3_;   min4 = min4_; max4 = max4_; set4 = set4_;
            min5 = min5_;   max5 = max5_;   set5 = set5_;   min6 = min6_;   max6 = max6_;   set6 = set6_;   /*min7 = min7_;   max7 = max7_;   set7 = set7_;   min8 = min8_; max8 = max8_; set8 = set8_;*/
            fitmin = fitmin_; fitmax = fitmax_; 
            second = second_;
            min12 = min12_; max12 = max12_; set12 = set12_; min22 = min22_; max22 = max22_; set22 = set22_; min32 = min32_; max32 = max32_; set32 = set32_; fitmin2 = fitmin2_; fitmax2 = fitmax2_; 
            gxmin = gxmin_; gxmax = gxmax_; gymin = gymin_; gymax = gymax_;
            TDC = TDC_; ppi = ppi_;
        }

        void setMeanTS(bool m) {doMeanTS = m;}
        
        PluginSummary() : scan(nullptr), doMeanTS(false), TDC(nullptr), isIncreasing(true)
        {
        }
        ~PluginSummary()
        {
            delete scan;
        }
    };

    static double lineFun(double* x, double* p)
    {
        //p[0]: Slope of the line, p[1]: y-intercept of the line
        return p[0]*x[0] + p[1];
    }

    static double gamma(double x)
    {
        return 1 / (1 - exp(x));
    }
    
    static double meanTS(double* x, double* p)
    {
        //p[0]: switch 1, p[1]: switch 2, p[2]: switch 3, p[3]: switch 4, p[4]: time const 1, p[5]: time const 2
        double val = 0;
        if(x[0] < p[0] )
            val = whichTS[2]; 
        else if(p[0] < x[0] && x[0] < p[1])
            val = exp((x[0]-p[0])/p[4]) + whichTS[1];
        else if(p[1] < x[0] && x[0] < p[2])
            val = whichTS[1];
        else if(p[2] < x[0] && x[0] < p[3])
            val = exp((x[0]-p[2])/p[5]) + whichTS[0];
        else
            val = whichTS[0];
        //if(x[0] < p[0] )
        //    val = 3; 
        //else if(p[0] < x[0] && x[0] < p[1])
        //    val = gamma( (p[1]-p[0])/p[4] )*( exp((x[0]-p[0])/p[4]) - 1 );
        //else if(p[1] < x[0] && x[0] < p[2])
        //    val = 2;
        //else if(p[2] < x[0] && x[0] < p[3])
        //    val = gamma( (p[3]-p[2])/p[5] )*( exp((x[0]-p[2])/p[5]) - 1 );
        //else
        //    val = 1;            
        return val;
    }
        
    static double expFunDecay(double* x, double* p)
    {
        //p[0]: norm, p[1]: time const, p[2]: phase        
        return (x[0] < p[2] + 1) ? p[0]: p[0]*exp((x[0]-p[2])/p[1]);
    }

    static double expFunRise(double* x, double* p)
    {
        //p[0]: norm, p[1]: time const, p[2]: phase        
        return (x[0] < p[2] + 1) ? p[0] : p[0]*(1 - exp((x[0]-p[2])/p[1]) );
    }

    void printFitInfo(TF1* fit)
    {
        printf(
            "Chi^2:%10.4f, P0:%8.4f +/- %8.4f  P1:%8.4f +/- %8.4f P2:%8.4f +/- %8.4f\n",
            fit->GetChisquare(), fit->GetParameter(0), fit->GetParError(0), fit->GetParameter(1), fit->GetParError(1), fit->GetParameter(2), fit->GetParError(2)
            );                
    }

    void drawFitInfo(TF1* fit, std::vector<double>* v, const std::vector<std::string>& names ,double x, double y, const PluginPassInfo* ppi)
    {
        char st[100];
        TLatex mark;
        mark.SetNDC(true);
        mark.SetTextAlign(11);
        mark.SetTextSize(0.030);
        
        std::string fix = "";
        if(v==nullptr)
        {
            if(fit->GetParameter(1) < 0) fix = " ";            
            sprintf(st,  "#chi^{2} %16s %.3f [%.2lf, %.2lf]"             , "", fit->GetChisquare(), ppi->chi2Min1, ppi->chi2Max1 );
            mark.DrawLatex( gPad->GetLeftMargin() + x, 1 - (gPad->GetTopMargin() + y ), st);
            
            int index = -1;
            for(const auto& name : names)
            {
                index++;
                char ch[50];
                sprintf(ch, "%-16s %.3f #pm %.3f [%.2lf, %.2lf]"    ,name.c_str(), fit->GetParameter(index), fit->GetParError(index), ppi->min1[index], ppi->max1[index]);
                mark.DrawLatex( gPad->GetLeftMargin() + x, 1 - (gPad->GetTopMargin() + y + 0.03*(index+1) ), ch);
            }
        }
        else if(fit==nullptr)
        {
            sprintf(st, "%-10s %.3f [%.2lf, %.2lf]"    ,names[0].c_str(), (*v)[0], ppi->chi2Min1, ppi->chi2Max1);
            mark.DrawLatex( gPad->GetLeftMargin() + x, 1 - (gPad->GetTopMargin() + y ), st);
            sprintf(st, "%-10s %.3f [%.2lf, %.2lf]"    ,names[1].c_str(), (*v)[1], ppi->chi2Min2, ppi->chi2Max2);
            mark.DrawLatex( gPad->GetLeftMargin() + x, 1 - (gPad->GetTopMargin() + y + 0.03 ), st);
        }
    }
    
    template<typename G> void fitHisto(const PluginSummary* p, G* graph)
    {
        TCanvas* c1 = new TCanvas("c1","c1",800,800);
        gPad->SetTopMargin(0.1);
        gPad->SetBottomMargin(0.12);
        gPad->SetRightMargin(0.05);
        gPad->SetLeftMargin(0.15);
        TLegend* leg = new TLegend(0.3, 0.8, 0.7, 0.9);
        leg->SetTextSize(0.03);
        leg->SetBorderSize(0);
        leg->SetFillStyle(0);
        //c1.SetLogx();
        //c1.SetLogy();
        TH1* temp = new TH1F("dummy","dummy",10,p->gxmin,p->gxmax);
	if(graph->InheritsFrom("TH1"))
	{
	    temp->GetYaxis()->SetRangeUser(0.1 , graph->GetMaximum()*10);
	}
	else 
	{
  	    leg->AddEntry(graph,"Scan Setting","P");
	    temp->SetMinimum(p->gymin);
	    temp->SetMaximum(p->gymax);
	}
	temp->SetStats(false);
        temp->SetTitle(0);
        temp->SetLineColor(kBlack);
        temp->GetYaxis()->SetTitle(p->histNameY.c_str());
        temp->GetXaxis()->SetTitle(p->histNameX.c_str());
        temp->SetTitleOffset(1,"X");
        temp->SetTitleOffset(1.6,"Y");
        temp->SetTitleSize(0.04,"X");
        temp->SetTitleSize(0.04,"Y");
        temp->Draw();
        c1->Modified();
        graph->SetLineWidth(3);
        graph->SetMarkerStyle(kFullCircle);
        //graph->SetMarkerStyle(kFullSquare);
	if(graph->InheritsFrom("TH1"))
	{ 
	    graph->Draw("same hist");        
	}
        else graph->Draw("same PE");
        leg->Draw();

        TLatex mark;
        mark.SetNDC(true);
        mark.SetTextAlign(11);
        mark.SetTextSize(0.031);
        mark.SetTextFont(1);
        mark.DrawLatex( 0.25, 0.95, p->histName.c_str());
    
        //////////////////////
        //Fitting info
        //////////////////////
        std::vector<std::string> names = {"slope","y-intercept"};
        TF1* fit1 = nullptr;
        if(p->first)
        {
            if (p->plugin == "phaseScan")
            {
                if(p->doMeanTS)
                {
                    fit1 = new TF1("exp", meanTS, p->fitmin, p->fitmax, 6);
                    fit1->SetParLimits(2, p->min3, p->max3); 
                    fit1->SetParameter(2, p->set3);
                    fit1->SetParLimits(3, p->min4, p->max4); 
                    fit1->SetParameter(3, p->set4);
                    fit1->SetParLimits(4, p->min5, p->max5); 
                    fit1->SetParameter(4, p->set5);
                    fit1->SetParLimits(5, p->min6, p->max6); 
                    fit1->SetParameter(5, p->set6);
                    names = {"switch 1","switch 2","switch 3","switch 4","time const 1","time const 2"};
                }
                else
                {
                    fit1 = new TF1("exp", expFunDecay, p->fitmin, p->fitmax, 3);
                    fit1->SetParLimits(2, p->min3, p->max3); 
                    fit1->SetParameter(2, p->set3);
                    names = {"norm","time const","phase"};                
                }
            }
            else
                fit1 = new TF1("line", lineFun, p->fitmin, p->fitmax, 2);
            fit1->SetParLimits(0, p->min1, p->max1); 
            fit1->SetParLimits(1, p->min2, p->max2); 
            fit1->SetParameter(0, p->set1);
            fit1->SetParameter(1, p->set2);
            fit1->SetLineWidth(2);
            fit1->SetLineColor(kRed);
            graph->Fit(fit1, "RQM", "", p->fitmin, p->fitmax);
            fit1->Draw("same");
            leg->AddEntry(fit1,"Fit","l");
            drawFitInfo(fit1, nullptr, names, 0.05, 0.13, p->ppi);
            if(p->verb) printFitInfo(fit1);
        }
        
        TF1* fit2 = nullptr;
        if(p->second)
        {
            if (p->plugin == "phaseScan")
            {
                fit2 = new TF1("exp2", expFunRise, p->fitmin2, p->fitmax2, 3);
                fit2->SetParLimits(2, p->min32, p->max32); 
                fit2->SetParameter(2, p->set32);
                names = {"norm","time const","phase"};
            }
            else
                fit2 = new TF1("line2", lineFun, p->fitmin2, p->fitmax2, 2);
            fit2->SetParLimits(0, p->min12, p->max12); 
            fit2->SetParLimits(1, p->min22, p->max22); 
            fit2->SetParameter(0, p->set12);
            fit2->SetParameter(1, p->set22);
            fit2->SetLineWidth(2);
            fit2->SetLineColor(kBlue);
            graph->Fit(fit2, "RQM", "", p->fitmin2, p->fitmax2);
            fit2->Draw("same");
            leg->AddEntry(fit2,"Fit","l");
            drawFitInfo(fit2, nullptr, names, 0.05, 0.25, p->ppi);
            if(p->verb) printFitInfo(fit2);
        }

        fitResults = new FitResults();
        fitResults->setVar("fit1", fit1);
        fitResults->setVar("fit2", fit2);
        fitResults->setVar("uniqueID",  p->uniqueID);
        fitResults->setVar("iglooType", p->iglooType);
        if (p->plugin == "pedestal")
        {
	    fitResults->setVar("mean", graph->GetMean());
	    fitResults->setVar("sigma", graph->GetRMS());
            names = {"mean", "sigma"};
            auto* v = new std::vector<double>({graph->GetMean(),graph->GetRMS()});
            drawFitInfo(nullptr, v, names, 0.35, 0.05, p->ppi);
            c1->SetLogy();
        }
   
	fitResults->isIncreasing = p->isIncreasing;
        std::string path = "qcTestResults/QC_run"+p->runNum+"/"+p->uniqueID+"/"+p->plugin+"/";
        gSystem->Exec( ("mkdir -p "+path).c_str() );
        c1->Print((path+p->histName+".png").c_str());
            
        delete c1;
        delete leg;
        delete temp;
    }

    template<typename G> G* makeTGraph(const PluginSummary* p,
                                       const std::vector<double>& mVec, const std::vector<double>& mVecError,
                                       const std::vector<double>& kVec, const std::vector<double>& kVecError)
    {
        int n = mVec.size();
        double x[n], ex[n], y[n], ey[n];
        for(int i = 0; i < n; i++)
        {
            x[i] = mVec[i]; ex[i] = mVecError[i];
            y[i] = kVec[i]; ey[i] = kVecError[i];
            if(p->verb) std::cout<<x[i]<<" +/- "<<ex[i]<<" "<<y[i]<<" +/- "<<ey[i]<<std::endl;
        }
    
        G* gFit = nullptr;
        if(typeid(G) == typeid(TGraph))
            gFit = static_cast<G*>( new TGraph (n, x, y) );    
        else if(typeid(G) == typeid(TGraphErrors))
            gFit = static_cast<G*>( new TGraphErrors (n, x, y, ex, ey) );

        return gFit;
    }

    template<typename G> void processRatios(const PluginSummary* p)
    {
        std::vector<double> ratio,      ratioError;
        std::vector<double> knownRatio, knownRatioError;
        double meanMax  = *max_element(p->mean.begin(), p->mean.end());
        double knownMax = *max_element(p->known.begin(), p->known.end());
        
        for(int index = 0; index < p->known.size(); index++)
        {
            if(p->verb) std::cout<<"Measured: "<<p->mean[index]<<" +/- "<<p->sigma[index]<<"  Ref: "<<p->known[index]<<std::endl;
            ratio.push_back( p->mean[index] / meanMax );
            ratioError.push_back( p->sigma[index] / meanMax );
            knownRatio.push_back( p->known[index] / knownMax );
            knownRatioError.push_back(0.01);
	    //if(p->plugin == "gselScan") std::cout<<"Measured: "<<p->mean[index] / meanMax<<"  Ref: "<<p->known[index] / knownMax<<std::endl;
        }
        
        G* gFit = makeTGraph<G>(p, ratio, ratioError, knownRatio, knownRatioError);
        fitHisto<G>(p, gFit);

        delete gFit;
    }

    void processPedestal(const PluginSummary* p)
    {
        TH1F* hFit = new TH1F("Pedestal","Pedestal",40, 0, 3.1*10);
        for(auto v : p->mean)
        {
    	  hFit->Fill(v);
        }
	hFit->SetLineColor(kBlack);
	hFit->SetStats(true);
	    
	fitHisto<TH1>(p, static_cast<TH1*>(hFit));
    
        delete hFit;        
    }

    template<typename G> void processMeans(PluginSummary* p)
    {
        std::vector<double> x, xError, yError;
        std::vector<std::pair<int, double>> pairVec;
        for(int index = 0; index < p->mean.size(); index++)
        {
            if(p->verb) std::cout<<"Measured: "<<p->mean[index]<<" +/- "<<p->sigma[index]<<std::endl;
            //Adding in bitwise and logic for the signed bit settings
            int setting;
            if(p->plugin == "pedestalScan") setting = ((index & 0x20) ? 1 : -1)*(index & 0x1f);
            else setting = ((index & 0x8) ? 1 : -1)*(index & 0x7); 
	    pairVec.push_back( std::make_pair(setting, p->mean[index]) );
            x.push_back(setting);
            xError.push_back(0.1);
        }

	std::sort(pairVec.begin(), pairVec.end());
	bool setMin = true;
	int index = -1;
	for(auto& pair : pairVec)
	{
	    if(pair.second > 2.5 && setMin)
	    {
	        p->fitmin = pair.first;
		setMin = false;
	    }                
	    if(!(index == -1) && p->isIncreasing) p->isIncreasing = (pairVec[index].second <= pair.second) ? true : false;
	    index++;
	}

        G* gFit = makeTGraph<G>(p, x, xError, p->mean, p->sigma);
        fitHisto<G>(p, gFit);

        delete gFit;        
    }

    template<typename G> void processPhaseScan(const std::vector<PluginSummary*>& pVec)
    {
        for(auto* p : pVec)
        {
            std::vector<double> x, xError, yError;
            for(int index = 0; index < p->mean.size(); index++)
            {
                if(p->verb) std::cout<<"Measured: "<<p->mean[index]<<" +/- "<<p->sigma[index]<<std::endl;
                x.push_back(index + 1);
                xError.push_back(0.1);
            }

            G* gFit1 = makeTGraph<G>(p, x, xError, p->mean, p->sigma);
            fitHisto<G>(p, gFit1);
        
            delete gFit1;
        }
    }
    
    template<typename G> void processPhaseScan(PluginSummary* p, const std::vector<PluginSummary*>& pVec)
    {
        std::vector<double> x, y, xError, yError;
        p->setMeanTS(true);
        for(int t = 0; t < pVec[0]->mean.size(); t++)
        {
	    double num = 0, den = 0, numErrorSquared = 0, denErrorSquared = 0;
            int index = -1;
            double factor = 1;
            for(auto* p : pVec)
            {
  	        index++;
                num += whichTS[index]*p->mean[t];
                den += p->mean[t];
		numErrorSquared += pow(whichTS[index]*p->sigma[t], 2);
		denErrorSquared += pow(p->sigma[t], 2);
            }
            x.push_back(t); 
	    y.push_back(num/den);
            xError.push_back(0.05);
            yError.push_back(0.01);
	    //yError.push_back((num/den)*sqrt(numErrorSquared/(num*num) + denErrorSquared/(den*den)));
        }
        G* gFit = makeTGraph<G>(p, x, xError, y, yError);
        fitHisto<G>(p, gFit);

        delete gFit;
    }
        
public:
    FitResults* getFitResults()
    {
        return fitResults;
    }

    void processPlugins(const RunSummary& r, const std::string& gType = "", const bool verb = true)
    {
        TH1::AddDirectory(false);
        TFile* f = TFile::Open( r.file.c_str() );
        std::vector<PluginSummary*> pVec;
        PluginSummary* phaseInfo = nullptr;
        std::vector<TH1*> tdcVec;
        
        if(r.plugin == "gselScan")
        {
            PluginSummary* p = new PluginSummary();
            TH1* s = (TH1*)f->Get( (r.histVar+r.channel).c_str() );
            //p->set({1/3.10, 1/4.65, 1/6.20, 1/9.30, 1/12.40, 1/15.50, 1/18.60, 1/21.70, 1/24.80, 1/27.90, 1/31.00, 1/34.10, 1/35.65},
            p->set({1.0, 0.667602857348108, 0.506330479564591, 0.341836005540379, 0.258321000061532, 0.205926892499078, 0.171592581374011, 0.147594903128095, 0.12953563333741, 0.11507657813958, 0.103777746736405, 0.0942977010756129, 0.0901185583084523},
                   r.plugin, "Run"+r.runNum+"_"+r.plugin+"_"+r.iglooType+"_"+r.channel, "Measured Gain", "Reference Gain", r.runNum, r.channel, r.uniqueID, r.iglooType, 100, verb, s,
                   true,
                   0,2,1, -1,1,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, //0,0,0, 0,0,0,
                   0, 0.99,
                   false,
                   0,0,0, 0,0,0, 0,0,0,
                   0, 0,
                   0, 1.01, 0.00000001, 1.01,
                   nullptr, r.ppi
                );
            pVec.push_back(p);
        }
        else if(r.plugin == "iQiScan")
        {
            PluginSummary* p = new PluginSummary();
            TH1* s = (TH1*)f->Get( (r.histVar+r.channel).c_str() );
            p->set({90, 180, 360, 720, 1440, 2880, 5760, 8640},
                   //{90, 180, 360, 62, 15400, 2880, 5760, 8640}, //test
                   r.plugin, "Run"+r.runNum+"_"+r.plugin+"_"+r.iglooType+"_"+r.channel, "Measured: Charge / Max Charge", "Reference: Charge / Max Charge", r.runNum, r.channel, r.uniqueID, r.iglooType, 100, verb, s,
                   true,
                   0,2,1, -1,1,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, //0,0,0, 0,0,0,
                   0, 0.99,
                   false,
                   0,0,0, 0,0,0, 0,0,0,
                   0, 0,
                   0, 1.01, 0.00000001, 1.01,
                   nullptr, r.ppi
                );
            pVec.push_back(p);
        }
        else if(r.plugin == "pedestalScan")
        {
            PluginSummary* p = new PluginSummary();
            TH1* s = (TH1*)f->Get( (r.histVar+r.channel).c_str() );
            p->set({},
                   r.plugin, "Run"+r.runNum+"_"+r.plugin+"_"+r.iglooType+"_"+r.channel, "Setting", "Charge [fC]", r.runNum, r.channel, r.uniqueID, r.iglooType, 100, verb, s,
                   true,
                   0,10,2.4, -100,20,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, //0,0,0, 0,0,0,
                   0, 31,
                   false,
                   0,0,0, 0,0,0, 0,0,0,
                   0, 0,
                   -40, 40, 0, 100,
                   nullptr, r.ppi
                );
            pVec.push_back(p);
        }
        else if(r.plugin == "capID0pedestal" || r.plugin == "capID1pedestal" ||
                r.plugin == "capID2pedestal" || r.plugin == "capID3pedestal")
    
        {
            PluginSummary* p = new PluginSummary();
            TH1* s = (TH1*)f->Get( (r.histVar+r.channel).c_str() );
            p->set({},
                   r.plugin, "Run"+r.runNum+"_"+r.plugin+"_"+r.iglooType+"_"+r.channel, "Setting", "Charge [fC]", r.runNum, r.channel, r.uniqueID, r.iglooType, 100, verb, s,
                   true,
                   1,3,1.5, 0,50,30, 0,0,0, 0,0,0, 0,0,0, 0,0,0, //0,0,0, 0,0,0,
                   -7, 7,
                   false,
                   -20,0,-1, -10,100,20, 0,0,0,
                   1, 8,
                   -10, 10, 0, 70,
                   nullptr, r.ppi
                );
            pVec.push_back(p);
        }
        else if(r.plugin == "phaseScan")
        {
            PluginSummary* p1 = new PluginSummary();
            PluginSummary* p2 = new PluginSummary();
            PluginSummary* p3 = new PluginSummary();
            TH1* s1 = (TH1*)f->Get( (r.plugin+"_TS_"+std::to_string(whichTS[0])+r.histVar+r.channel).c_str() );
            TH1* s2 = (TH1*)f->Get( (r.plugin+"_TS_"+std::to_string(whichTS[1])+r.histVar+r.channel).c_str() );
            TH1* s3 = (TH1*)f->Get( (r.plugin+"_TS_"+std::to_string(whichTS[2])+r.histVar+r.channel).c_str() );
            TH1* t1 = (TH1*)f->Get( (r.plugin+"_TS_"+std::to_string(whichTS[0])+"_TDC_vs_EvtNum_"+r.channel).c_str() ); tdcVec.push_back(t1);
            TH1* t2 = (TH1*)f->Get( (r.plugin+"_TS_"+std::to_string(whichTS[1])+"_TDC_vs_EvtNum_"+r.channel).c_str() ); tdcVec.push_back(t2);
            TH1* t3 = (TH1*)f->Get( (r.plugin+"_TS_"+std::to_string(whichTS[3])+"_TDC_vs_EvtNum_"+r.channel).c_str() ); tdcVec.push_back(t3);
            p1->set({},
                    r.plugin, "Run"+r.runNum+"_TS_"+std::to_string(whichTS[0])+"_"+r.plugin+"_"+r.iglooType+"_"+r.channel, "Setting", "Charge [fC]", r.runNum, r.channel, r.uniqueID, r.iglooType, 50, verb, s1,
                    false,
                    0,100,0, -6,-4,-5, 0,100,50, 0,0,0, 0,0,0, 0,0,0, //0,0,0, 0,0,0,
                    0, 72,
                    true,
                    6000,7000,6500, -6,-4,-5, 0,100,50,
                    72, 110,
                    0, 100, 0, 12000,
                    t1, r.ppi
                );
            p2->set({},
                    r.plugin, "Run"+r.runNum+"_TS_"+std::to_string(whichTS[1])+"_"+r.plugin+"_"+r.iglooType+"_"+r.channel, "Setting", "Charge [fC]", r.runNum, r.channel, r.uniqueID, r.iglooType, 50, verb, s2,
                    true,
                    6000,7000,6500, -6,-4,-5, 0,100,50, 0,0,0, 0,0,0, 0,0,0, //0,0,0, 0,0,0,
                    55, 90,
                    true,
                    6000,7000,6500, -6,-4,-5, 0,100,50,
                    22, 55,
                    0, 100, 0, 12000,
                    t2, r.ppi
                );
            p3->set({},
                    r.plugin, "Run"+r.runNum+"_TS_"+std::to_string(whichTS[2])+"_"+r.plugin+"_"+r.iglooType+"_"+r.channel, "Setting", "Charge [fC]", r.runNum, r.channel, r.uniqueID, r.iglooType, 50, verb, s3,
                    true,
                    6000,7000,6500, -100,-3,-5, 0,30,25.5, 0,0,0, 0,0,0, 0,0,0, //0,0,0, 0,0,0,
                    0, 40,
                    false,
                    0,0,0, 0,0,0, 0,0,0,
                    0, 0,
                    0, 100, 0, 12000,
                    t3, r.ppi
                );
            pVec.push_back(p1);
            pVec.push_back(p2);
            pVec.push_back(p3);

            phaseInfo = new PluginSummary();
            phaseInfo->set({},
                           r.plugin, "Run"+r.runNum+"_"+r.plugin+"_"+r.iglooType+"_"+r.channel, "Setting", "Charge Weighted TS", r.runNum, r.channel, r.uniqueID, r.iglooType, 50, verb, s1,
                           true,
                           20,23,21, 30,45,33, 65,75,70, 75,90,85, -5.5,-3.5,-4, -5.5,-3.5,-4,
                           0, 114,
                           false,
                           0,0,0, 0,0,0, 0,0,0,
                           0, 0,
                           0, 100, whichTS[2]-3, whichTS[2]+2.5,
                           nullptr, r.ppi
                );
        }
        else if(r.plugin == "pedestal")
        {
            PluginSummary* p = new PluginSummary();
            TH1* s = (TH1*)f->Get( (r.histVar+r.channel).c_str() );
            p->set({},
                   r.plugin, "Run"+r.runNum+"_"+r.plugin+"_"+r.iglooType+"_"+r.channel, "Charge [fC]", "Events", r.runNum, r.channel, r.uniqueID, r.iglooType, 1, verb, s,
                   false,
                   0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, //0,0,0, 0,0,0,
                   0, 0,
                   false,
                   0,0,0, 0,0,0, 0,0,0,
                   0, 0,
                   0, 35, 0, 50,
                   nullptr, r.ppi
                );
            pVec.push_back(p);
        }
        f->Close();
        delete f;

        for(auto* p : pVec)
        {
            std::vector<double> mean, rms, sigma, error, tdc;
            for(int index = 0; index < p->scan->GetNbinsX()/p->nEvents; index++)
            {
                double m = 0, m2 = 0, me2 = 0; int n = 0, t = 0;
                for(int bin = 1 + p->nEvents*index; bin <= (p->nEvents) + p->nEvents*index; bin++)
                {
                    double c = p->scan->GetBinContent(bin);
                    double cE = p->scan->GetBinError(bin);
                    m += c; m2 += c*c; me2 += cE*cE; n++;
                    if(p->TDC  != nullptr) t += p->TDC->GetBinContent(bin);
                    //if(verb) std::cout<<index<<"  "<<bin<<"  "<<p->scan->GetBinContent(bin)<<std::endl;
                }
                //if(verb) std::cout<<n<<std::endl
                mean.push_back( m/n );
                rms.push_back( sqrt(m2/n) );
                sigma.push_back( sqrt( m2/n - (m/n)*(m/n)) );
                error.push_back( sqrt(me2/n) );
                tdc.push_back( t/n );
            }
            if(r.plugin == "pedestalScan") p->setGraphInfo(mean, rms, error, tdc);
            else p->setGraphInfo(mean, rms, sigma, tdc);
        }
        
        if(r.plugin == "gselScan" || r.plugin == "iQiScan")
        {
            if(gType == "")
                processRatios<TGraph>(pVec[0]);
            else if(gType == "Error")
                processRatios<TGraphErrors>(pVec[0]);
        }
        else if(r.plugin == "pedestalScan"   ||
                r.plugin == "capID0pedestal" || r.plugin == "capID1pedestal" ||
                r.plugin == "capID2pedestal" || r.plugin == "capID3pedestal")
        {
            if(gType == "")
                processMeans<TGraph>(pVec[0]);
            else if(gType == "Error")
                processMeans<TGraphErrors>(pVec[0]);
        }
	else if(r.plugin == "pedestal")
	{
	    processPedestal(pVec[0]);
	}
        else if(r.plugin == "phaseScan")
        {
            if(gType == "")
            {
                processPhaseScan<TGraph>(pVec);
                processPhaseScan<TGraph>(phaseInfo, pVec);
            }
            else if(gType == "Error")
            {
	        //processPhaseScan<TGraphErrors>(pVec);
                processPhaseScan<TGraphErrors>(phaseInfo, pVec);
            }
        }
        
        for(auto* p : pVec){delete p;}
        for(auto* p : tdcVec){delete p;}
    }

    ProcessPlugins() : fitResults(nullptr)
    {
    }
};

void checkFit(const FitResults* r, const PluginPassInfo& p, std::vector<std::vector<double>>& flags, std::vector<TH1F*>& summaryVec)
{
    if(p.plugin == "pedestal")
    {
        (*summaryVec[0]).Fill(r->mean);
        (*summaryVec[1]).Fill(r->sigma);
        double flag = (p.chi2Min1 < r->mean && r->mean < p.chi2Max1) ? 1 : 0;
        flags.push_back({r->mean, flag});
        flag = (p.chi2Min2 < r->sigma && r->sigma < p.chi2Max2) ? 1 : 0;
        flags.push_back({r->sigma, flag});        
    }
    
    if(r->fit1 != nullptr)
    {
        double chi2Fit1 = r->fit1->GetChisquare();
        (*summaryVec[0]).Fill(chi2Fit1);
        double flag = (p.chi2Min1 < chi2Fit1 && chi2Fit1 < p.chi2Max1) ? 1 : 0; 
        flags.push_back({chi2Fit1, flag});
    
        for(int pram = 0; pram < p.min1.size(); pram++ )
        {
            double val = r->fit1->GetParameter(pram);
	    if(pram == p.min1.size() - 1 && (p.plugin == "pedestalScan" || p.plugin == "capID0pedestal" || p.plugin == "capID1pedestal" || p.plugin == "capID2pedestal" || p.plugin == "capID3pedestal"))
	    {
		val = static_cast<double>(r->isIncreasing);
	    }
            (*summaryVec[1+pram]).Fill(val);
            double flag = (p.min1[pram] <= val && val <= p.max1[pram]) ? 1 : 0; 
            flags.push_back({val, flag});
        }
    }
    
    if(r->fit2 != nullptr)
    {
        double chi2Fit2 = r->fit2->GetChisquare();
        (*summaryVec[p.min1.size()+1]).Fill(chi2Fit2);        
        double flag = (p.chi2Min2 < chi2Fit2 && chi2Fit2 < p.chi2Max2) ? 1 : 0; 
        flags.push_back({chi2Fit2, flag});
        
        for(int pram = 0; pram < p.min2.size(); pram++ )
        {
            double val = r->fit2->GetParameter(pram);
            (*summaryVec[p.min1.size()+2+pram]).Fill(val);        
            double flag = (p.min2[pram] < val && val < p.max2[pram]) ? 1 : 0; 
            flags.push_back({val, flag});
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

#endif
