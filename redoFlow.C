R__LOAD_LIBRARY(libMcIniData.so)

void redoFlow(std::string iName="", std::string oName="", std::string iFuncFileName="", float NeutronFlowFactor=1.)
{
  TStopwatch timer;
  timer.Start();
  
  TFile *iFile = new TFile(iName.c_str(), "read");
  if (!iFile){
    std::cerr << "Error: no input file was found!" << std::endl;
    return;
  }
  TTree *iTree = (TTree*) iFile->Get("events");
  if (!iTree){
    std::cerr << "Error: no input TTree was found in the input file!" << std::endl;
    return;
  }

  TFile *iFuncFile = new TFile(iFuncFileName.c_str(), "read");
  TProfile3D *p_v1 = (TProfile3D*)iFuncFile->Get("v1/p3_v1_PID_b_pt_y_3");
  TProfile3D *p_v2 = (TProfile3D*)iFuncFile->Get("v2/p3_v2_PID_b_pt_y_3");
  TProfile3D *p_v3 = (TProfile3D*)iFuncFile->Get("v3/p3_v3_PID_b_pt_y_3");
  TProfile3D *p_v4 = (TProfile3D*)iFuncFile->Get("v4/p3_v4_PID_b_pt_y_3");

  if (!p_v1 || !p_v2 || !p_v3 || !p_v4){
    std::cerr << "Error: no input vn TProfile3D were found!" << std::endl;
    return;
  }

  TF1 *func = new TF1("func", "1./(2.*TMath::Pi())*( 1. + 2.*[1]*cos(1.*(x - [0])) + 2.*[2]*cos(2.*(x - [0])) + 2.*[3]*cos(3.*(x - [0])) + 2.*[4]*cos(4.*(x - [0])) ) + [5]", -1.*TMath::Pi(), TMath::Pi());

  URun *iRun = (URun*)iFile->Get("run");
  URun *oRun = (URun*)iRun->Clone();
  UEvent *iEvent{nullptr};
  iTree->SetBranchAddress("event", &iEvent);

  TFile *oFile = new TFile(oName.c_str(), "recreate");
  TTree *oTree = new TTree(iTree->GetName(), iTree->GetTitle());
  UEvent *oEvent = new UEvent();
  oTree->Branch("event", "UEvent", oEvent);

  long nev = (long)iTree->GetEntriesFast();
  for (long i=0; i<nev; ++i){
    iTree->GetEntry(i);
    if (i%100 == 0)
      std::cout << "Event [" << i << "/" << nev << "]" << std::endl;

    oEvent->SetB(iEvent->GetB());
    oEvent->SetPhi(iEvent->GetPhi());
    oEvent->SetEventNr(iEvent->GetEventNr());
    oEvent->SetNes(iEvent->GetNes());
    oEvent->SetStepNr(iEvent->GetStepNr());
    oEvent->SetStepT(iEvent->GetStepT());

    auto npart = iEvent->GetNpa();
    for (int ip = 0; ip < npart; ++ip){
      UParticle *iParticle = iEvent->GetParticle(ip);
      TLorentzVector iMom = iParticle->GetMomentum();
      TLorentzVector oMom;
      UParticle *oParticle = new UParticle(*iParticle);
      if (iParticle->GetPdg() == 2212 || iParticle->GetPdg() == 2112){
        auto y = iMom.Rapidity();
        auto pt = iMom.Pt();
        auto b = iEvent->GetB();

        if (abs(y) < 2. && pt < 5. && b < 16.){

          auto v1 = p_v1->GetBinContent(p_v1->GetXaxis()->FindBin(pt),
                                        p_v1->GetYaxis()->FindBin(y),
                                        p_v1->GetZaxis()->FindBin(b));
          auto v2 = p_v1->GetBinContent(p_v1->GetXaxis()->FindBin(pt),
                                        p_v1->GetYaxis()->FindBin(y),
                                        p_v1->GetZaxis()->FindBin(b));
          auto v3 = p_v1->GetBinContent(p_v1->GetXaxis()->FindBin(pt),
                                        p_v1->GetYaxis()->FindBin(y),
                                        p_v1->GetZaxis()->FindBin(b));
          auto v4 = p_v1->GetBinContent(p_v1->GetXaxis()->FindBin(pt),
                                        p_v1->GetYaxis()->FindBin(y),
                                        p_v1->GetZaxis()->FindBin(b));
          
          auto factor = 1.f;
          if (iParticle->GetPdg() == 2112)
            factor = NeutronFlowFactor;
          
          func->SetParameter(0, iEvent->GetPhi());
          func->SetParameter(1, v1*factor);
          func->SetParameter(2, v2*factor);
          func->SetParameter(3, v3*factor);
          func->SetParameter(4, v4*factor);
          func->SetParameter(5, 0.);
          
          auto min_value = (double)func->GetMinimum();
          if (min_value < 0.)
            func->SetParameter(5, abs(1.001*min_value));
          
          auto phi = (double)func->GetRandom();
          auto px = pt * cos(phi);
          auto py = pt * sin(phi);
          auto pz = iMom.Pz();
          auto e = iMom.E();
          oMom.SetPxPyPzE(px, py, pz, e);
          oParticle->Momentum( oMom );
        }
      }
      oEvent->AddParticle(*oParticle);
    }
    oTree->Fill();
  }
  oFile->cd();
  oRun->Write();
  oTree->Write();
  oFile->Close();

  timer.Stop();
  timer.Print();
}
