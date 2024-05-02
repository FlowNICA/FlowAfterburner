R__LOAD_LIBRARY(libMcIniData.so)

using namespace std;

void convert(TString inputFileName="test.root", TString outputFileName="out.root")
{
  TFile *fIn = new TFile(inputFileName, "read");
  URun *run = (URun *)fIn->Get("run");
  float sqrtSnn = run->GetNNSqrtS();
  int aProj = run->GetAProj();
  TTree *tree = (TTree *)fIn->Get("events");
  //EventInitialState *iniState{nullptr};
  UEvent *event{nullptr};
  //tree->SetBranchAddress("iniState", &iniState);
  tree->SetBranchAddress("event", &event);

  // TH2F *h2Part = new TH2F("h2Part", "Participant position distribution;x (fm); y (fm)", 300, -15, 15, 300, -15, 15);
  // TH1F *hSpect =
  //     new TH1F("hSpect", "Projectile spectator final rapidity distribution;#it{y}; Number of particles", 500, -5, 5);

  //List of histograms and Ntuples....
  TFile *fo = new TFile(outputFileName.Data(),"recreate");
 
  TTree *mctree;

  float     d_bimp;      // impact parameter
  int       d_npart;    // number of participants
  float     d_phi2;
  float     d_phi3;
  float     d_ecc2;  // eccentricity for v2
  float     d_ecc3;  // eccentricity for v3

  static const int max_nh = 25000;

  int           d_nh;
  float         d_momx[max_nh];   //[momentum x]
  float         d_momy[max_nh];   //[momentum y]
  float         d_momz[max_nh];   //[momentum z]
  float         d_ene[max_nh];   //[energy]
  int           d_pdg[max_nh];   //[particle data group code]
  int           d_hid[max_nh];   //[history id]
  short         d_charge[max_nh];   //[electric charge]

  mctree = new TTree("mctree","Hadrons tree from MC RHIC models");

  mctree->Branch("bimp",&d_bimp,"bimp/F"); // impact parametr
  mctree->Branch("phi2",&d_phi2,"phi2/F"); // phiRP2
  mctree->Branch("phi3",&d_phi3,"phi3/F"); // phiRP3
  mctree->Branch("ecc2",&d_ecc2,"ecc2/F"); // eccentricity2
  mctree->Branch("ecc3",&d_ecc3,"ecc3/F"); // eccentricity3


  mctree->Branch("npart",&d_npart,"npart/I"); // number of participants
  mctree->Branch("nh",&d_nh,"nh/I");  // number of particles
  mctree->Branch("momx",&d_momx,"momx[nh]/F");
  mctree->Branch("momy",&d_momy,"momy[nh]/F");
  mctree->Branch("momz",&d_momz,"momz[nh]/F");
  mctree->Branch("ene",&d_ene,"ene[nh]/F");//[energy]
  mctree->Branch("hid",&d_hid,"hid[nh]/I");//[histrory id]
  mctree->Branch("pdg",&d_pdg,"pdg[nh]/I");//[particle data group code]
  mctree->Branch("charge",&d_charge,"charge[nh]/S");//[electric charge]

  Double_t aveRsqr, aveCos2, aveSin2, aveCos3, aveSin3;
  Double_t aveX, aveY, aveZ;
  Int_t    size;
  Double_t ecc2, ecc3;
  TLorentzVector vCentroid;

  int nEvents = tree->GetEntries();
  for(int iEvent = 0; iEvent < nEvents; iEvent++)
  {
    tree->GetEntry(iEvent);
    //if (iEvent%100 == 0)
      std::cout << "Event [" << iEvent << "/" << nEvents << "]" << std::endl;
    if (!event) continue;

    d_bimp = event->GetB();
    d_npart= 0.;
    d_phi2 = event->GetPhi();
    d_phi3 = 0.;
    ecc2 = 0.;
    ecc2 = 0.;
    
    d_nh = event->GetNpa();
    for(int iPart = 0; iPart < d_nh; iPart++)
    {
      //UParticle *particle = event->GetParticle(iPart);
      /*TLorentzVector momentum = particle->GetMomentum();

      d_momx[iPart]  = momentum.Px();
      d_momy[iPart]  = momentum.Py();
      d_momz[iPart]  = momentum.Pz();
      d_ene[iPart]   = momentum.E();
      d_pdg[iPart]   = particle->GetPdg();
      d_hid[iPart]   = particle->GetParent();
      auto pdgparticle = TDatabasePDG::Instance()->GetParticle(particle->GetPdg());
      if (pdgparticle)
        d_charge[iPart]= 1./3.*pdgparticle->Charge();
      else
        d_charge[iPart]= -999.;*/
    }
    std::cout << "\tcheck " << iEvent << std::endl;
    mctree->Fill();
  }
  fo->cd();
  mctree->Write();
  fo->Close();
}
