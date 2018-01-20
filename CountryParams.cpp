//////////////////////////////////////////////////////////
//                                                      //
//                  CountryParams.cpp                   //
//            Created by Pablo on 20/10/2017.           //
//  Copyright © 2017 Mikaela Smit. All rights reserved. //
//                                                      //
//////////////////////////////////////////////////////////


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "errorcoutmacro.h"
#include "CountryParams.hpp"
#include "LoadParams.h"

using namespace std;


extern int UN_Pop;
extern int init_pop;
extern int total_population;
extern int ART_start_yr;
extern int factor;
extern int ageAdult;
extern int age_atrisk_hpv;
extern int age_tostart_CCscreening;
extern double Sex_ratio;

extern double HPV_Prevalence;
//extern double CIN1_Rates[2];
//extern double CIN2_3_Rates[2];
//extern double CIS_Rates[2];
//extern double ICC_Rates[2];
extern int HPV_Status_HPV;
extern int HPV_Status_CIN1;
extern int HPV_Status_CIN2_3;
extern int HPV_Status_CIS;
extern int HPV_Status_ICC;
extern int HPV_Status_Recovered;
extern double hpv_date_after_death;
extern double no_hpv_infection;
extern double HPV_Screening_coverage;

extern double ARTbuffer;
extern double MortAdj;
extern double background_d;
extern double HIV_d;
extern double IHD_d;
extern double Depression_d;
extern double Asthma_d;
extern double Stroke_d;
extern double Diabetes_d;
extern double CKD_d;
extern double Colo_d;
extern double Liver_d;
extern double Oeso_d;
extern double Prostate_d;
extern double OtherCan_d;
extern string ParamDirectory;

/////////////////// FUNCTION: IF LOOP FOR COUNTRY-SPECIFIC PARAMETERS //////////////////////
void loadCountryParams(int x){
    if (x == 1){                                        // KENYA
        cout << "You have selected Kenya" << endl;
        UN_Pop=5909800;
        init_pop=UN_Pop/factor;
        total_population=init_pop;
        ART_start_yr=2004;
        ageAdult=15;
        age_atrisk_hpv=17;
        age_tostart_CCscreening=25;
        Sex_ratio=0.495639296;
        HPV_Prevalence=0.8;
        HPV_Screening_coverage=0.03;
        //CIN1_Rates[2]= {0.2,0.8};
        //CIN2_3_Rates[2]={0.4,0.6};
        //CIS_Rates[2]={0.65,0.35};
        //ICC_Rates[2]={1.0,0.0};
        no_hpv_infection=-988;
        hpv_date_after_death = -977;
        HPV_Status_HPV=1;
        HPV_Status_CIN1=2;
        HPV_Status_CIN2_3=3;
        HPV_Status_CIS=4;
        HPV_Status_ICC=5;
        HPV_Status_Recovered=6;
        ARTbuffer=1.01;
        MortAdj=1;
        // Mortality percentages from GBD 2016
        background_d =71.32;
        HIV_d        =15.56;
        IHD_d         =3.99;
        Depression_d =0.0;
        Asthma_d     =0.45;
        Stroke_d     =3.92;
        Diabetes_d   =1.27;
        CKD_d        =1.5;
        Colo_d       =0.39;
        Liver_d      =0.34;
        Oeso_d       =0.32;
        Prostate_d    =0.31;
        OtherCan_d   =0.71;
    }
    
 else if (x == 2){                                          // ZIMBABWE
     cout << "You have selected Zimbabwe" << endl;
     UN_Pop=2565000;
     init_pop=UN_Pop/factor;
     total_population=init_pop;
     ART_start_yr=2004;
     ageAdult=15;
     age_atrisk_hpv=17;
     Sex_ratio=0.4986;
     ARTbuffer=1;
     MortAdj=1;
     // Mortality percentages from GBD 2013
     background_d =56.6;
     HIV_d        =29.6;
     IHD_d         =1.00;
     Depression_d =0.0;
     Asthma_d     =0.7;
     Stroke_d     =4.6;
     Diabetes_d   =1.7;
     CKD_d        =1.3;
     Colo_d       =0.3;
     Liver_d      =0.3;
     Oeso_d       =0.6;
     Prostate_d    =0.2;
     OtherCan_d   =3.1;
     }

    


else if (x == 3){                                          // MALAWI need to replace params
    cout << "You have selected MALAWI" << endl;
    UN_Pop=2565000;
    init_pop=UN_Pop/factor;
    total_population=init_pop;
    ART_start_yr=2004;
    ageAdult=15;
    age_atrisk_hpv=17;
    Sex_ratio=0.4986;
    ARTbuffer=1;
    MortAdj=1;
    // Mortality percentages from GBD 2013
    background_d =56.6;
    HIV_d        =29.6;
    IHD_d         =1.00;
    Depression_d =0.0;
    Asthma_d     =0.7;
    Stroke_d     =4.6;
    Diabetes_d   =1.7;
    CKD_d        =1.3;
    Colo_d       =0.3;
    Liver_d      =0.3;
    Oeso_d       =0.6;
    Prostate_d    =0.2;
    OtherCan_d   =3.1;
    }
    
else if (x == 4){                                          // Uasin Gishu - KENYA
    cout << "You have selected Kenya - Uasin Gishu" << endl;
    UN_Pop=99075;  //99075
    init_pop=UN_Pop/factor;
    total_population=init_pop;
    ART_start_yr=2004;
    ageAdult=15;
    age_atrisk_hpv=17;
    Sex_ratio=0.50237783022;
    HPV_Prevalence=0.388; //normally 0.388
    //CIN1_Rates[2]={0.2,0.8};
    //CIN2_3_Rates[2]={0.4,0.6};
    //CIS_Rates[2]={0.65,0.35};
    //ICC_Rates[2]={1.0,0.0};
    no_hpv_infection=-988;
    hpv_date_after_death = -977;
    HPV_Status_HPV=1;
    HPV_Status_CIN1=2;
    HPV_Status_CIN2_3=3;
    HPV_Status_CIS=4;
    HPV_Status_ICC=5;
    HPV_Status_Recovered=6;
    ARTbuffer=1.01;
    MortAdj=1;
    // Mortality percentages from GBD 2016
    background_d =71.32;
    HIV_d        =15.56;
    IHD_d         =3.99;
    Depression_d =0.0;
    Asthma_d     =0.45;
    Stroke_d     =3.92;
    Diabetes_d   =1.27;
    CKD_d        =1.5;
    Colo_d       =0.39;
    Liver_d      =0.34;
    Oeso_d       =0.32;
    Prostate_d    =0.31;
    OtherCan_d   =0.71;
    
}
    
    }



void getParamsString(int x){
    if (x == 1){
        ParamDirectory="/Users/Monkeyface/Dropbox/Ageing in Kenya and Zimbabwe - project/Model_wHPV/Model_Africa_HPV/HIVModelZimbabwe/Kenya/";
    }

    else if (x == 2){
        ParamDirectory="/Users/Monkeyface/Dropbox/Ageing in Kenya and Zimbabwe - project/Model_wHPV/Model_Africa_HPV/HIVModelZimbabwe/Zimbabwe/";
    }
    else if (x == 3){
        ParamDirectory="/Users/Monkeyface/Dropbox/Ageing in Kenya and Zimbabwe - project/Model_wHPV/Model_Africa_HPV/HIVModelZimbabwe/Malawi/";
    }
    
    else if (x == 4){
        ParamDirectory="/Users/Monkeyface/Dropbox/Ageing in Kenya and Zimbabwe - project/Model_wHPV/Model_Africa_HPV/HIVModelZimbabwe/Kenya_UG/";
    }
    
    
}

