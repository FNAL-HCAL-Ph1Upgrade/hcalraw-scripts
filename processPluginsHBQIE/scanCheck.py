#!/usr/bin/env python
from ROOT import *
import os

# Valid Gsel settings
GSEL_CODES = [0b00000, 0b00001, 0b00010, 0b00100, 0b01000, 0b10000, 0b10010, \
              0b10100, 0b11000, 0b11010, 0b11100, 0b11110, 0b11111]
nAdcMax = 256 

# scanType: iqi or gsel
scanType = "iqi"
if scanType == "iqi":
    SCAN_BINS = 8
elif scanType == "gsel":
    SCAN_BINS = 13



EVENTS_PER_BIN = 100
SOI = 3  # Sample (time slice) of interest

#inF = "run178-iQi_GselScan.root"
inF = "run179-iQiScan.root"
f = TFile.Open(inF, "read")

# plotType: ADC or Charge
#plotType = "Charge"
plotType = "ADC"


# Make output directory
#outDir = "resultsScanCheck"
outDir = "scan_%s" % inF[:inF.find(".")]
os.system("mkdir -p %s/%s" % (outDir, plotType)) 


# Other slot 2 links will be ignored
SLOT2_FIBERS = [0, 1, 2, 3, 4, 5, 7, 8]

results = {}
for fib in SLOT2_FIBERS:
    results[fib] = {}   # Create new entry
    for ch in xrange(8):
	results[fib][ch] = f.Get("TS_%d_%s_vs_EvtNum_FED_1776_Crate_41_Slot_2_Fib_%d_Ch_%d_1D" % (SOI, plotType, fib, ch))  

histos = {}
for scan_bin in xrange(SCAN_BINS):
    if scanType == "iqi":
        hName = "Setting_%d_ChargeInjectDAC_%d" % (scan_bin+1, scan_bin)
        hTitle = "Setting %d: ChargeInjectDAC %d" % (scan_bin+1, scan_bin)
    elif scanType == "gsel":
        hName = "Setting_%d_Gsel_%d" % (scan_bin+1,GSEL_CODES[scan_bin])
        hTitle = "Setting %d: Gsel %d" % (scan_bin+1, GSEL_CODES[scan_bin])

    histos[scan_bin] = TH1F(hName, hTitle, nAdcMax, -0.5, (nAdcMax-0.5) if plotType == "ADC" else 14999.5 )
    

    histos[scan_bin].SetXTitle("ADC" if plotType == "ADC" else "Charge [fC]")

for fib in SLOT2_FIBERS:
    for ch in xrange(8):
	for evt in xrange(1, EVENTS_PER_BIN * SCAN_BINS+1):
	    histos[(evt-1)/EVENTS_PER_BIN].Fill(results[fib][ch].GetBinContent(evt))

c = TCanvas("c","c",1200,800)
for scan_bin in xrange(SCAN_BINS):
    histos[scan_bin].Draw("HIST")
    c.SaveAs("%s/%s/%s_setting_%d.png" % (outDir, plotType, plotType, scan_bin+1))
