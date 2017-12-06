////////////////////////////////////////////////////////////////
//    Mikaela Epidemiology Model							  //
//    Created by Mikaela Smit on 7/11/2014.				      //
//    Copyright (c) 2014 Mikaela Smit. All rights reserved.   //
////////////////////////////////////////////////////////////////

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <ctime>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <cmath>
#include <math.h>
#include "person.h"
#include "event.h"
#include "eventQ.h"
#include "eventfunctions.h"
#include "errorcoutmacro.h"
#include "LoadParams.h"
#include "CParamReader.hpp"
#include "CountryParams.hpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                                   VARIABLE PARAMETERS                                                 //////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// STEP 1 --- SELECT THE COUNTRY TO RUN THE MODEL
// 1=KENYA      2=ZIMBABWE      3=MALAWI      4=KENYA - UG
int country=1;

// STEP 2 --- NAME THE DIRECTORY AND TAG FOR THE OUTPUT FILE
string OutputFileDirectory="/Users/rachelcassidy/Dropbox/Latest.csv";

/// STEP 3 --- AT WHAT FACTOR SHOULD WE RUN THE POPULATION?
int factor=100; //county = 1, country = 100

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                                   MODIFY IF NEEDED PARAMETERS                                        //////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double StartYear=1950;                                                                                          //////////
int EndYear=2035;                                                                                               //////////
const long long int final_number_people=100000000;                                                              //////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                      DO NOT MODIFY - CRITICAL PARAMETERS AND FUNCTIONS                               //////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////                                                POINTERS                                                //////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double      *p_GT;                                                                                              //////////
int         *p_PY;                                                                                              //////////
int         PY=0;                                                                                               //////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////                    --- COUNTRY-SPECIFIC PARAMETERS: ADJUST IN CountryParams.cpp!!!!                    //////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int         UN_Pop;                                                                                             //////////
int         init_pop;                                                                                           //////////
int         total_population;                                                                                   //////////
double      Sex_ratio;
double      HPV_Prevalence;
double      CIN1_Prevalence;
double      CIN2_3_Prevalence;
double      CIS_Prevalence;
double      ICC_Prevalence;
double no_hpv_infection;
double HPV_Status_HPV;
double HPV_Status_CIN1;
double HPV_Status_CIN2_3;
double HPV_Status_CIS;
double HPV_Status_ICC;
double HPV_Status_Recovered;
double hpv_date_after_death;


int         ageAdult;                                                                                           //////////
double      ARTbuffer;                                                                                          //////////
double      MortAdj;                                                                                            //////////
int         ART_start_yr;                                                                                       //////////
double      background_d;                                                                                       //////////
double      HIV_d;                                                                                              //////////
double      IHD_d;                                                                                              //////////
double      Depression_d;                                                                                       //////////
double      Asthma_d;                                                                                           //////////
double      Stroke_d;                                                                                           //////////
double      Diabetes_d;                                                                                         //////////
double      CKD_d;                                                                                              //////////
double      Colo_d;                                                                                             //////////
double      Liver_d;                                                                                            //////////
double      Oeso_d;                                                                                             //////////
double      Prostate_d;                                                                                         //////////
double      OtherCan_d;                                                                                         //////////
extern double MortRisk[6];              // Adjust in eventsfunctions.cpp                                        //////////
extern double MortRisk_Cancer[5];       // Adjust in eventsfunctions.cpp                                        //////////
string ParamDirectory;                                                                                          //////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                                  POINTER TO EVENT QUEUE                                              //////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
priority_queue<event*, vector<event*>, timeComparison> *p_PQ;				                                    //////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                      person** is a POINTER to a pointer (address)                                    //////////
//////////                      'new person*' is a pointer to the actual person below                           //////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
person** MyArrayOfPointersToPeople = new person*[final_number_people];                                          //////////
vector<event *> Events;                                                                                         //////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                      Function for random number generator between min and max                        //////////
/////////                       !!!!Note: if min=0 and max=4 it will generate 0,1,2,3,4                         //////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double RandomMinMax_3(int min, int max){                                                                        //////////
    return rand()%(max-min+1)+min;                                                                              //////////
}                                                                                                               //////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////                        LET'S RUN THE MAIN FUNCTION OF THE MODEL!!!                                   //////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(){
    
    srand(time(NULL));														      // Random Number generator using PC time
    
    cout << MortRisk << endl;
    cout << "Testing changes" << endl;
    
    
    cout << endl << "Jambo / Hello / Hola!" << endl << endl ;								  // Check if model is running
    
    getParamsString(country); // Gets the ParamDirectory string to pull country-specific files
    
    cout << "Country files are being accessed..." << endl << endl;
    
    // Call the function that loads country-specific parameters
    loadCountryParams(country);
    cout << "Population was of " << UN_Pop << " in 1950, with a sex ratio of " << Sex_ratio << ", per UN estimates." << endl;
    cout << "Model calibrated to: 1) Run at a " << factor << "th of the population (N=" << init_pop << "); 2) Adult = "
    << ageAdult << " years of age on; 3) Mortality adjustment = "
    << MortAdj << "; 4) ART buffer = " << ARTbuffer << "; 5) ART was introduced in " << ART_start_yr << endl << endl;


    
    
    //// --- Load parameters --- ////
    cout << "Section 1 - We are loading the arrays" << endl;
    // Load HIV Arrays
    loadCD4StartArray();
    loadCD4ProgArray();
    loadCD4DeathArray();
    loadCD4ARTArray();
    
    // Load ART Arrays
    loadARTKidsArray();
    loadARTWomenArray();
    loadARTMenArray();
    loadARTMen_sumArray();
    loadARTWomen_sumArray();
    
    // Load Demographic Arrays
    loadAgeDistribution();
    loadAgeMin();
    loadAgeMax();
    loadNrChildren();
    loadNrChildrenProb();
    
    // Load Large Arrays
    loadBirthArray();
    loadDeathArray_Women();
    loadDeathArray_Men();
    loadHIVArray_Women();
    loadHIVArray_Men();
    loadNCDArray();
    loadCancerArray();
    
    
    
    
    cout << "Section 2 - All arrays loaded successfully" << endl;
    
    
    //// ---- Warning Code --- ////
    E(cout << "NOTE: The Macro for error code is working" << endl << endl;)
    if (StartYear > 1954){cout << "CAREFUL!! Fertility at the beginning is based on a TRF_init of 0, based on GlobalTime being between 1950 and 1954" << endl << endl;}
    if (EndYear > 2010){cout << "CAREFUL!! The model runs after 2010 and output things need to be updated to avoid errors" << endl << endl;}
    
    
    //// --- Some pointers ---- ////
    double GlobalTime=StartYear;											// Define Global Time and set it to 0 at the beginning of the model
    p_GT=&GlobalTime;														// Define the location the pointer to Global time is pointing to
    
    priority_queue<event*, vector<event*>, timeComparison> iQ;				// Define th ePriority Q
    p_PQ=&iQ;																// Define pointer to event Q
    p_PY=&PY;
    cout << p_PY << endl;

    
    //// --- MAKING POPULATION--- ////
    
    cout << "Section 3 - We're going to create a population" << endl;
    
    for(int i=0; i<total_population; i++){									// REMEMBER: this needs to stay "final_number_people" or it will give error with CSV FILES!!!!
        MyArrayOfPointersToPeople[i]=new person();							// The 'new person' the actual new person
        int a=i;
        (MyArrayOfPointersToPeople[i])->PersonIDAssign(a);					// --- Assign PersonID ---
    }
    // cout << "We have assigned a new ID to " << endl;
    
    for(int i=0; i<total_population; i++){
        (MyArrayOfPointersToPeople[i])->Alive=1;							// --- Assign Life Status ---
        (MyArrayOfPointersToPeople[i])->GenderDistribution();				// --- Assign Sex- ---
        (MyArrayOfPointersToPeople[i])->GetMyDoB();							// --- Assign DoB/Age ---
        (MyArrayOfPointersToPeople[i])->GetDateOfDeath();					// --- Assign date of death ---
        (MyArrayOfPointersToPeople[i])->GetMyDateOfHPVInfection();            // --- Assign date of HPV infection ---
        if (MyArrayOfPointersToPeople[i]->Sex == 2 && MyArrayOfPointersToPeople[i]->Age<50 && MyArrayOfPointersToPeople[i]->AgeAtDeath>=15) {(MyArrayOfPointersToPeople[i])->GetDateOfBaby();}              // --- Assign Birth of all Children- ---
        (MyArrayOfPointersToPeople[i])->GetMyDateNCD();                     // --- Get date of NCDs ---
        (MyArrayOfPointersToPeople[i])->GetMyDateCancers();                     // --- Get date of NCDs ---
        (MyArrayOfPointersToPeople[i])->GetMyDateOfHIVInfection();           // ---Get date of HIV infection ---
    }
    
    
    cout << "Section 4 - We've finished creating a population" << endl;
    
    
    //// --- EVENTQ --- ////
    cout << "Section 5 - We are going to create the annual events" << endl;
    
    event * TellNewYear = new event;										// --- Tell me every time  a new year start ---
    Events.push_back(TellNewYear);
    TellNewYear->time = StartYear;
    TellNewYear->p_fun = &EventTellNewYear;
    iQ.push(TellNewYear);

    
    //// --- LETS RUN THE EVENTQ --- ////
    cout << endl << endl << "The characteristics of the event queue:" << endl;
    cout << "the first event will ocurr in " << iQ.top()->time << ".  " << endl;
    cout << "the size of the event queue is " << iQ.size() << endl;
    
    while(iQ.top()->time< EndYear +1){                                      // this loop throws up error because no recurrent birthday pushing gt over 5 yrs and iq.pop means gt cannot be updated after pop
        GlobalTime=iQ.top()->time;											// careful with order of global time update - do not touch or touch and check!!
        iQ.top()-> p_fun(iQ.top()->person_ID);
        iQ.pop();
    }
    
    
    //// --- Output the results in a csv file --- ////
    FILE* ProjectZim;
    ProjectZim = fopen(OutputFileDirectory.c_str(),"w");
    
    
    
    for (int i=0; i<total_population; i++) {								// Note: If adding more variables to be output, need to adapt the %x
        fprintf(ProjectZim,"%d, %d, %f, %f, %d, %d, %f, %d, %f, %d, %d, %f, %f, %f, %f, %f, %d, %f, %f, %f, %f, %f, %f, %f, %d, %f, %f, %f, %f, %f, %f, %f, %f, %d, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f \n",
                MyArrayOfPointersToPeople[i]->PersonID,
                MyArrayOfPointersToPeople[i]->Sex,
                MyArrayOfPointersToPeople[i]->DoB,
                MyArrayOfPointersToPeople[i]->Age,
                MyArrayOfPointersToPeople[i]->MotherID,
                MyArrayOfPointersToPeople[i]->DatesBirth.size(),
                MyArrayOfPointersToPeople[i]->DateOfDeath,
                MyArrayOfPointersToPeople[i]->AgeAtDeath,
                MyArrayOfPointersToPeople[i]->HIV,
                MyArrayOfPointersToPeople[i]->CD4_cat,
                MyArrayOfPointersToPeople[i]->ART,
                MyArrayOfPointersToPeople[i]->HT,
                MyArrayOfPointersToPeople[i]->Depression,
                MyArrayOfPointersToPeople[i]->Asthma,
                MyArrayOfPointersToPeople[i]->Stroke,
                MyArrayOfPointersToPeople[i]->Diabetes,
                MyArrayOfPointersToPeople[i]->CD4_cat_ARTstart,
                MyArrayOfPointersToPeople[i]->CD4_change.at(0),
                MyArrayOfPointersToPeople[i]->CD4_change.at(1),
                MyArrayOfPointersToPeople[i]->CD4_change.at(2),
                MyArrayOfPointersToPeople[i]->CD4_change.at(3),
                MyArrayOfPointersToPeople[i]->CD4_change.at(4),
                MyArrayOfPointersToPeople[i]->CD4_change.at(5),
                MyArrayOfPointersToPeople[i]->CD4_change.at(6),
                MyArrayOfPointersToPeople[i]->CauseOfDeath,
                MyArrayOfPointersToPeople[i]->CKD,
                MyArrayOfPointersToPeople[i]->Breast,
                MyArrayOfPointersToPeople[i]->Cervical,
                MyArrayOfPointersToPeople[i]->Colo,
                MyArrayOfPointersToPeople[i]->Liver,
                MyArrayOfPointersToPeople[i]->Oeso,
                MyArrayOfPointersToPeople[i]->Prostate,
                MyArrayOfPointersToPeople[i]->OtherCan,
                MyArrayOfPointersToPeople[i]->Stroke_status,             // Check if used and, if not, remove
                MyArrayOfPointersToPeople[i]->HPV_Status,
                MyArrayOfPointersToPeople[i]->HPV_DateofInfection,
                MyArrayOfPointersToPeople[i]->CIN1_DateofInfection,
                MyArrayOfPointersToPeople[i]->CIN2_3_DateofInfection,
                MyArrayOfPointersToPeople[i]->CIS_DateofInfection,
                MyArrayOfPointersToPeople[i]->ICC_DateofInfection,
                MyArrayOfPointersToPeople[i]->HPV_DateofRecovery,
                MyArrayOfPointersToPeople[i]->CIN1_DateofRecovery,
                MyArrayOfPointersToPeople[i]->CIN2_3_DateofRecovery,
                MyArrayOfPointersToPeople[i]->CIS_DateofRecovery
                );}
    fclose(ProjectZim);
    
    // COUNT OUTPUT FOR FITTING
    int count_2016deaths=0;
    int count_causeofdeath[14]={0};
    
    for (int i=0; i<total_population; i++) {
        if (MyArrayOfPointersToPeople[i]->DateOfDeath>=2013 && MyArrayOfPointersToPeople[i]->DateOfDeath<2014)
        {
            count_2016deaths++;
            count_causeofdeath[MyArrayOfPointersToPeople[i]->CauseOfDeath-1]++;
        }
    }
    
    // Calculate percentages
    cout << "FINAL: total: " << count_2016deaths << " and nr background " << count_causeofdeath[0] << endl;
    
    double background_m =(count_causeofdeath[0]/(double)count_2016deaths)*100;
    double HIV_m        =(count_causeofdeath[1]/(double)count_2016deaths)*100;
    double HT_m         =(count_causeofdeath[2]/(double)count_2016deaths)*100;
    double Depression_m =(count_causeofdeath[3]/(double)count_2016deaths)*100;
    double Asthma_m     =(count_causeofdeath[4]/(double)count_2016deaths)*100;
    double Stroke_m     =(count_causeofdeath[5]/(double)count_2016deaths)*100;
    double Diabetes_m   =(count_causeofdeath[6]/(double)count_2016deaths)*100;
    double CKD_m        =(count_causeofdeath[7]/(double)count_2016deaths)*100;
    double Colo_m       =(count_causeofdeath[8]/(double)count_2016deaths)*100;
    double Liver_m      =(count_causeofdeath[9]/(double)count_2016deaths)*100;
    double Oeso_m       =(count_causeofdeath[10]/(double)count_2016deaths)*100;
    double Prostate_m    =(count_causeofdeath[11]/(double)count_2016deaths)*100;
    double OtherCan_m   =(count_causeofdeath[12]/(double)count_2016deaths)*100;
    double HPV_Status_m        =(count_causeofdeath[13]/(double)count_2016deaths)*100;
    double HPV_DateofInfection_m        =(count_causeofdeath[13]/(double)count_2016deaths)*100;
    double CIN1_DateofInfection_m        =(count_causeofdeath[13]/(double)count_2016deaths)*100;
    double CIN2_3_DateofInfection_m        =(count_causeofdeath[13]/(double)count_2016deaths)*100;
    double CIS_DateofInfection_m        =(count_causeofdeath[13]/(double)count_2016deaths)*100;
    double ICC_DateofInfection_m        =(count_causeofdeath[13]/(double)count_2016deaths)*100;
    double HPV_DateofRecovery_m        =(count_causeofdeath[13]/(double)count_2016deaths)*100;
    double CIN1_DateofRecovery_m        =(count_causeofdeath[13]/(double)count_2016deaths)*100;
    double CIN2_3_DateofRecovery_m        =(count_causeofdeath[13]/(double)count_2016deaths)*100;
    double CIS_DateofRecovery_m        =(count_causeofdeath[13]/(double)count_2016deaths)*100;
    
    // Output the model percentages
    cout << "Background " << background_m << endl;
    cout << "HIV "        << HIV_m << endl;
    cout << "HT "         << HT_m << endl;
    cout << "Depression " << Depression_m << endl;
    cout << "Asthma "     << Asthma_m << endl;
    cout << "Stroke "     << Stroke_m << endl;
    cout << "Diabetes "   << Diabetes_m << endl;
    cout << "CKD "        << CKD_m << endl;
    cout << "Colo "       << Colo_m << endl;
    cout << "Liver "      << Liver_m << endl;
    cout << "Oeso "       << Oeso_m << endl;
    cout << "Prostate "    << Prostate_m << endl;
    cout << "OtherCan "   << OtherCan_m << endl;
    cout << "HPV_Status_m "        << HPV_Status_m << endl;
    cout << "HPV_DateofInfection "        << HPV_DateofInfection_m << endl;
    cout << "CIN1_DateofInfection "        << CIN1_DateofInfection_m << endl;
    cout << "CIN2_3_DateofInfection "        << CIN2_3_DateofInfection_m << endl;
    cout << "CIS_DateofInfection "        << CIS_DateofInfection_m << endl;
    cout << "ICC_DateofInfection "        << ICC_DateofInfection_m << endl;
    cout << "HPV_DateofRecovery "        << HPV_DateofRecovery_m << endl;
    cout << "CIN1_DateofRecovery "        << CIN1_DateofRecovery_m << endl;
    cout << "CIN2_3_DateofRecovery "        << CIN2_3_DateofRecovery_m << endl;
    cout << "CIS_DateofRecovery "        << CIS_DateofRecovery_m << endl;

    
    // Least square calculation
    double sum_MinLik=  pow ((background_m  - background_d),2) +
    pow ((HIV_m         - HIV_d),2) +
    pow ((HT_m          - IHD_d),2) +
    pow ((Depression_m  - Depression_d),2) +
    pow ((Asthma_m      - Asthma_d),2) +
    pow ((Stroke_m      - Stroke_d),2) +
    pow ((Diabetes_m    - Diabetes_d),2) +
    pow ((CKD_m         - CKD_d),2) +
    pow ((Colo_m        - Colo_d),2) +
    pow ((Liver_m       - Liver_d),2) +
    pow ((Oeso_m        - Oeso_d),2) +
    pow ((Prostate_m     - Prostate_d),2) +
    pow ((OtherCan_m    - OtherCan_d),2) ;
    
    
    cout << "Least Square " << sum_MinLik << endl;
    

    
    
    //// --- LETS AVOID MEMORY LEAKS AND DELETE ALL NEW EVENTS --- ////
    cout << "Lets delete the heap! " << endl;
    for(int i=0; i<Events.size()-1; i++){
        E(cout << "Event " << Events.size() << " is " << Events.at(i)->time << endl;)
        delete Events.at(i);
        E(cout << "Event " << Events.size() << " is " << Events.at(i)->time << endl;)
    }
    
    for(int i=0; i<total_population; i++){									// REMEMBER: this needs to stay "final_number_people" or it will give error with CSV FILES!!!!
        delete MyArrayOfPointersToPeople[i];								// The 'new person' the actual new person
    }
    
    // --- End of code ---
    cout << endl << "Kwaheri / Goodbye / Adios!\n";
    system("pause");
    return 0;
    
    
}
