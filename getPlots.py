#!/usr/bin/python

import ROOT
import argparse
import sys
import code


def main():
    # Choose options from the list or add your own
    ranges = {"UniqueID":[1,100],
             "pedestal":[101,1100],
             "capIDpedestal":[1101,2700],
             "pedestalScan":[2701, 9100],
             "iqiScan":[9101, 9900],
             "gselScan":[9901, 11200],
             "phaseScan":[11201, 21200],
             "all":[1, 21200]
             }
    types = list(key for key in ranges)
    parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("--type",       "-t", default="all", help="run type for selecting events")
    parser.add_argument("--eventmin",   "-emin", default="", help="minevt for selecting events")
    parser.add_argument("--eventmax",   "-emax", default="", help="maxevt type for selecting events")
    parser.add_argument("--run",        "-r", default="", help="run number from run control")
    parser.add_argument("--slot",       "-s", default="", help="uHTR slot number")
    parser.add_argument("--fiber",      "-f", default="", help="uHTR fiber number")
    parser.add_argument("--channel",    "-c", default="", help="uHTR channel number")
    parser.add_argument("--list_types", "-l", action="store_true", help="list available run types")
    
    options = parser.parse_args()
    run_type = options.type
    emin = options.eventmin
    emax = options.eventmax
    run = options.run
    slot = options.slot
    fiber = options.fiber
    channel = options.channel
    list_types = options.list_types

    if list_types:
        sys.exit("types: {0}".format(" ".join(types)))

    if run_type not in ranges:
        print "Run type not recognized...."
        exit()
    
    try:
        points = ranges[run_type]
    except:
        print "Error: Incorrect Run Type"
        exit()

    if emin and emax:
        points = [int(emin),int(emax)]
        run_type = "eventmin {0}, eventmax {1}".format(emin,emax)

    f = ROOT.TFile.Open("/home/hcalpro/hcalraw/output/run{0}-master.root".format(run))
    if not f: 
        print "Run number entered doesn't have histograms"
        exit()
    nbins = points[1]-points[0]+1
    h2=ROOT.TH2F("h2",run_type,nbins,points[0]-0.5,points[1]+0.5,7,0.5,7.5)
    for ts in xrange(1,8):
        h=f.Get("TS_{0}_Charge_vs_EvtNum_Slot_{1}_Fib_{2}_Ch_{3}".format(ts,slot,fiber,channel))
        #print "time slice: {0}".format(ts) 

        if not h:
            print "Histogram doesn't load properly"
            exit()
        for event in xrange(points[0],points[1]+1):
            Bin = event - points[0]+1 
            fc=h.GetBinContent(event)
            h2.SetBinContent(Bin,ts,fc)
    h2.GetYaxis().SetTitle("TS #")
    h2.GetXaxis().SetTitle("Event #")
    h2.SetDirectory(0)
    f.Close()
    return h2

if __name__=="__main__":
    c1 = ROOT.TCanvas("c1","multigraph",0,100,1200,900)
    h=main()
    #print h
    c1.SetRightMargin(0.15)
    h.Draw("colz")
    ROOT.gStyle.SetOptStat(0)
    c1.Update()
    code.interact(local=locals())
    #print h
