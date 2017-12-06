/////////////////////////////////////////////////////////////////
//    eventfunction.cpp                                        //
//    Created by Mikaela Smit on 07/11/2014.				   //
//    Copyright (c) 2014 Mikaela Smit. All rights reserved.    //
//    These event are added to the EventQ.					   //
/////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <ctime>
#include <math.h>                                                          
#include <vector>
#include <random>
#include <cmath>
#include "eventfunctions.h"
#include "event.h"
#include "eventQ.h"
#include "person.h"
#include "errorcoutmacro.h"
#include "CParamReader.hpp"
#include "LoadParams.h"
#include "CountryParams.hpp"

using namespace std;


//// --- OUTSIDE INFORMATION --- ////
extern double *p_GT;
extern int EndYear;
extern priority_queue<event*, vector<event*>, timeComparison> *p_PQ;
extern int total_population;
extern person** MyArrayOfPointersToPeople;
extern int *p_PY;
extern vector<event*> Events;
extern int ART_start_yr;
extern int country;

extern double*** CD4_startarray;
extern double**  CD4_prog_rates;
extern double*** Death_CD4_rates;
extern double*** ART_CD4_rates;
extern int* ARTKids;
extern int** ARTMen;
extern int* ARTMen_sum;
extern int** ARTWomen;
extern int* ARTWomen_sum;

extern double**  NCDArray;
extern int*      NCDAgeArrayMin;
extern int*      NCDAgeArrayMax;

extern double** DeathArray_Women;
extern double** DeathArray_Men;

extern double   HPV_Prevalence;
extern double   CIN1_Prevalence;
extern double   CIN2_3_Prevalence;
extern double   CIS_Prevalence;
extern double   ICC_Prevalence;
extern double   hpv_date_after_death;
extern double   no_hpv_infection;

extern double DateOfDeath;
extern int HPV_Status_HPV;
extern int HPV_Status_CIN1;
extern int HPV_Status_CIN2_3;
extern int HPV_Status_CIS;
extern int HPV_Status_ICC;
extern int HPV_Status_Recovered;


//// --- Important Internal informtaion --- ////
int RandomMinMax_2(int min, int max){							// Provide function for random number generator between min and max number
    return rand()%(max-min+1)+min;							    // Note: if min=0 and max=4 it will generate 0,1,2,3,4
}


//// --- NCD PARAMETERS HERE --- ////
//// NCD INTERACTION PARAMETERS ////
double Risk_DiabHC=1.12;                                        // Having high cholesterol given diabtes etc ...
double Risk_DiabHT=1.4;
double Risk_DiabCKD=1.5;
double Risk_DiabCVD=2.31;

double Risk_HCHT=1.277;
double Risk_HCCVD=1.41;

double Risk_HTCKD=1.69;
double Risk_HTCVD=1.26;


/// Mortality for NCDs Parameters ///
double MortRisk[6]= {0, 0, 0.85, 1.3, 1.1, 0.8}; //{0.087, 0, 1.4, 670.87, 12.23, 5};         // Original values from Smith et al Factors associated with : 1.52 (HT), 1.77 (diabetes)
double MortRisk_Cancer[5]= {1, 1, 1, 1, 1.05};                   //{0.087, 0, 1.4, 670.87, 12.23};   // Both this and above needs to be fitted
extern double MortAdj;
extern double ARTbuffer;
 

/// HIV Increased risk for NCD Parameter ///
double Risk_HIVHT=1.49;                                       // Increased risk of HT given HIV (and CKD given HIV below) - from Schouten et al CID 2016
double Risk_HIVCKD=2;

double Risk_NCDHIV[2]={Risk_HIVHT, Risk_HIVCKD};
int relatedNCD_HIV[2]={0, 5};
int nr_NCD_HIV=sizeof(relatedNCD_HIV)/sizeof(relatedNCD_HIV[0]);
extern int ageAdult;
extern double ARTbuffer;


//// Tidy up
double Risk_NCD_Diabetes[3]={Risk_DiabHT, Risk_DiabCVD, Risk_DiabCKD};
int relatedNCDs_Diab[3]={0, 3, 5};
int nr_NCD_Diab=sizeof(relatedNCDs_Diab)/sizeof(relatedNCDs_Diab[0]);


double Risk_NCD_HT[2]={Risk_HTCVD, Risk_HTCKD};
int relatedNCDs_HT[2]={3, 5};
int nr_NCD_HT=sizeof(relatedNCDs_HT)/sizeof(relatedNCDs_HT[0]);


// Count to compare to sum and CD4-specific numbers
int count_ARTMen[7]={0, 0, 0, 0, 0, 0, 0};        // Count by CD4 count category
int count_ARTAdult_Women[7]={0, 0, 0, 0, 0, 0, 0};      // Count by CD4 count category
int count_ARTMen_sum=0;                                 // Count the sum for the year - men
int count_ARTWomen_sum=0;                               // Count the sum for the year - women
int count_ARTKids=0;


// Index for the ART year
int ART_index=0;


// To get random index and only chekc HIV+
int HIV_Ref_PersonID[500000]={};
int countHIVRef=0;

int KIDS_HIV_Ref_PersonID[70000]={};
int countKIDSHIVRef=0;

// Count real available patients for error message
int Elig_Men=0;
int Elig_Women=0;
int Elig_Kids=0;



//////////////////////////////////////
//// --- FUNCTIONS FOR EVENTS --- ////
//////////////////////////////////////

//// --- NEW YEAR FUNCTION --- ////                                     // includes count of adults/kids on ART for error/infinite loop problems
void EventTellNewYear(person *MyPointerToPerson){
    
    cout << "A new year has started, it is now " << *p_GT << endl;      // Tells us a new year has started
    
    
    // Lets set the first people on ART
    if (*p_GT>=2004){
        
        // Add people on ART until we hit our aim
        while (count_ARTKids<ARTKids[ART_index] || ARTbuffer*count_ARTMen_sum<ARTMen_sum[ART_index] || ARTbuffer*count_ARTWomen_sum<ARTWomen_sum[ART_index]){
            
            //cout << endl << "Its " << *p_GT << " KIDS: There have " << Elig_Kids << " eligible kids and we need to reach " << ARTKids[ART_index] << " and only have " << count_ARTKids   << endl;
            //cout << "Men: There have " << Elig_Men << " eligible men and we need to reach " << ARTMen_sum[ART_index] << " and only have " << count_ARTMen_sum << " times 10% " << ARTbuffer*count_ARTMen_sum << endl;
            //cout << "Women: There have " << Elig_Women << " eligible women and we need to reach " << ARTWomen_sum[ART_index] << " and only have " << count_ARTWomen_sum << " times 10% " << ARTbuffer*count_ARTWomen_sum<< endl;
            
            
            if (count_ARTKids<ARTKids[ART_index] ){
               
                int a=(RandomMinMax_2(1,countKIDSHIVRef-1));    // Get a random person and update age
                int i=KIDS_HIV_Ref_PersonID[a]-1;
                //cout << "totalpopulation " << total_population << " a " << a <<  " Kids ref " << KIDS_HIV_Ref_PersonID[a] << endl;
                //cout << "I: " << i << " Age " << MyArrayOfPointersToPeople[i]->Age << " GT " << *p_GT << endl;
                MyArrayOfPointersToPeople[i]->Age=(*p_GT - MyArrayOfPointersToPeople[i]->DoB);
                
                
                // Lets get some kids onto ART
                if (((MyArrayOfPointersToPeople[i]->Age<ageAdult && *p_GT<2012)|| (MyArrayOfPointersToPeople[i]->Age<ageAdult && *p_GT>=2012)) && MyArrayOfPointersToPeople[i]->HIV>0 && MyArrayOfPointersToPeople[i]->HIV<=*p_GT && MyArrayOfPointersToPeople[i]->ART==-999 && MyArrayOfPointersToPeople[i]->Alive==1 && count_ARTKids<ARTKids[ART_index] ){
                    
                    MyArrayOfPointersToPeople[i]->ART=*p_GT;            // Lets set ART date
                    MyArrayOfPointersToPeople[i]->CD4_cat_ARTstart=MyArrayOfPointersToPeople[i]->CD4_cat;       // Lets set CD4 cat at ART start
                    count_ARTKids++;                                    // Update our counter
                    Elig_Kids=Elig_Kids-1;
                }
            }
            
            
            // Lets see if adults start ART
            if (ARTbuffer*count_ARTMen_sum<ARTMen_sum[ART_index] || ARTbuffer*count_ARTWomen_sum<ARTWomen_sum[ART_index]){
                
                int a=(RandomMinMax_2(1,countHIVRef-1));
                //cout << "Random number: " << a << " counter: " << countHIVRef << endl;
                int i=HIV_Ref_PersonID[a]-1;
                MyArrayOfPointersToPeople[i]->Age=(*p_GT - MyArrayOfPointersToPeople[i]->DoB);
                
                // Lets let men start ART
                if (MyArrayOfPointersToPeople[i]->Sex==1 && MyArrayOfPointersToPeople[i]->Age>=ageAdult && MyArrayOfPointersToPeople[i]->HIV>0 && MyArrayOfPointersToPeople[i]->HIV<*p_GT && MyArrayOfPointersToPeople[i]->ART==-999 &&MyArrayOfPointersToPeople[i]->Alive==1 && count_ARTMen_sum<ARTMen_sum[ART_index] ){
                    
                    
                    
                    if (ARTbuffer*ARTMen[ART_index][MyArrayOfPointersToPeople[i]->CD4_cat] > count_ARTMen[MyArrayOfPointersToPeople[i]->CD4_cat]){
                        
                        MyArrayOfPointersToPeople[i]->ART=*p_GT;                        // Lets set ART date
                        MyArrayOfPointersToPeople[i]->CD4_cat_ARTstart=MyArrayOfPointersToPeople[i]->CD4_cat;       // Lets set CD4 cat at ART start
                        count_ARTMen[MyArrayOfPointersToPeople[i]->CD4_cat]++;    // Update our counter CD4/ART array
                        count_ARTMen_sum++;                                             // Update the sum counter
                        Elig_Men=Elig_Men-1;
                    }
                    
                }
                
                
                // Lets start women on ART
                if (MyArrayOfPointersToPeople[i]->Sex==2 && MyArrayOfPointersToPeople[i]->Age>=ageAdult && MyArrayOfPointersToPeople[i]->HIV>0 && MyArrayOfPointersToPeople[i]->HIV<*p_GT && MyArrayOfPointersToPeople[i]->ART==-999 &&MyArrayOfPointersToPeople[i]->Alive==1 && count_ARTWomen_sum<ARTWomen_sum[ART_index]){
                    
                    
                    if (ARTbuffer*ARTWomen[ART_index][MyArrayOfPointersToPeople[i]->CD4_cat] > count_ARTAdult_Women[MyArrayOfPointersToPeople[i]->CD4_cat]){
                        
                        
                        MyArrayOfPointersToPeople[i]->ART=*p_GT;            // Lets set ART date
                        MyArrayOfPointersToPeople[i]->CD4_cat_ARTstart=MyArrayOfPointersToPeople[i]->CD4_cat;       // Lets set CD4 cat at ART start
                        count_ARTAdult_Women[MyArrayOfPointersToPeople[i]->CD4_cat]++;  // Update our counter CD4/ART array
                        count_ARTWomen_sum++;                               // Update the sum counter
                        Elig_Women=Elig_Women-1;
                       
                    }
                    
                }
            }
            
        }
        
//        cout << endl << " we have finished assigning ART KIDS: There have " << Elig_Kids << " eligible kids and we need to reach " << ARTKids[ART_index] << " and only have " << count_ARTKids   << endl;
//        cout << "Men: There have " << Elig_Men << " eligible men and we need to reach " << ARTMen_sum[ART_index] << " and only have " << count_ARTMen_sum << " times 20% " << 1.2*count_ARTMen_sum << endl;
//        cout << "Women: There have " << Elig_Women << " eligible women and we need to reach " << ARTWomen_sum[ART_index] << " and only have " << count_ARTWomen_sum << " times 20% " << 1.2*count_ARTWomen_sum<< endl;
        
        // Lets update the ART index
        ART_index++;
        if (ART_index>12){ART_index=12;}
        
    }
    
    
    
    // Lets get the pointer to the right year range
    if (*p_GT<1955){*p_PY = 0; };
    if (*p_GT >= 1955 && *p_GT<1960){*p_PY = 1; };
    if (*p_GT >= 1960 && *p_GT<1965){*p_PY = 2; };
    if (*p_GT >= 1965 && *p_GT<1970){*p_PY = 3; };
    if (*p_GT >= 1970 && *p_GT<1975){*p_PY = 4; };
    if (*p_GT >= 1975 && *p_GT<1980){*p_PY = 5; };
    if (*p_GT >= 1980 && *p_GT<1985){*p_PY = 6; };
    if (*p_GT >= 1985 && *p_GT<1990){*p_PY = 7; };
    if (*p_GT >= 1990 && *p_GT<1995){*p_PY = 8; };
    if (*p_GT >= 1995 && *p_GT<2000){*p_PY = 9; };
    if (*p_GT >= 2000 && *p_GT<2005){*p_PY = 10;};
    if (*p_GT >= 2005){*p_PY = 11; };
    E(cout << "Let's check relevant things have been updated... *p_PY: " << *p_PY << " and Global Time: " << *p_GT << endl;)
    
    // Schedule event for next year
    event * RecurrentTellNewYear = new event;
    Events.push_back(RecurrentTellNewYear);
    RecurrentTellNewYear->time = *p_GT + 1;
    RecurrentTellNewYear->p_fun = &EventTellNewYear;
    p_PQ->push(RecurrentTellNewYear);
    
    E(cout << "We have finished telling you the new year and setting fertility variables for the year." << endl;)	// Error message - can be switched on/off
}

//// --- ART Category switch from Kids to Adult --- ////
void EventARTCatSwitch(person *MyPointerToPerson){
    
    E(cout << "We are switching our kids to adult ART count: " << endl;)       // Error message - can be switched on/off
    
    if(MyPointerToPerson->Alive == 1) {
        
        
        // If they are not on ART
        if (MyPointerToPerson->ART==-999){
            
            // First we need to add them to eligible adults for selection
            HIV_Ref_PersonID[countHIVRef]=MyPointerToPerson->PersonID;         // And the patientID reference
            countHIVRef++;                                                     // Then we need to updated the adult counter
            Elig_Kids=Elig_Kids-1;
            if (MyPointerToPerson->Sex==1){Elig_Men++;}
            if (MyPointerToPerson->Sex==2){Elig_Women++;}
        }
        
        // If they are on ART
        if (MyPointerToPerson->ART>0){
            
            if (count_ARTKids>0) {count_ARTKids=count_ARTKids-1;}              // This count how many children are still on ART
            
            
            if (MyPointerToPerson->Sex==1){
                count_ARTMen_sum++;                                             // Lets add a count to the adult category
                count_ARTMen[4]++;                                        // Lets update the ART CD4 array counter
            }
            
            if (MyPointerToPerson->Sex==2){
                count_ARTWomen_sum++;                                           // Lets add a count to the adult category
                count_ARTAdult_Women[4]++;                                      // Lets update the ART CD4 array counter
            }
        }
    }
}


//// --- DEATH EVENT --- ////
void EventMyDeathDate(person *MyPointerToPerson){
    
    
    // Lets kills people
    if (MyPointerToPerson->Alive==1){
        
        MyPointerToPerson->Alive=0;
        
        if (MyPointerToPerson->CauseOfDeath==-999)
        {
            MyPointerToPerson->CauseOfDeath=1;
        }
        
        
        // Lets also update ART information as people die
        // 1. If they are not on ART
        if (MyPointerToPerson->HIV>0 && MyPointerToPerson->HIV<*p_GT && MyPointerToPerson->ART==-999){
            
            // First we need to update age
            MyPointerToPerson->Age=(*p_GT - MyPointerToPerson->DoB);  // First we update age
            
            if (MyPointerToPerson->Sex==1 && MyPointerToPerson->Age>=ageAdult){Elig_Men=Elig_Men-1;}
            if (MyPointerToPerson->Sex==2 && MyPointerToPerson->Age>=ageAdult){Elig_Women=Elig_Women-1;}
            if (MyPointerToPerson->Age<ageAdult){Elig_Kids=Elig_Kids-1;}
        }
        
        
        // 2. If they are on ART
        if (MyPointerToPerson->HIV>0 && MyPointerToPerson->HIV<*p_GT && MyPointerToPerson->ART>0){
            
            // First we need to update age
            MyPointerToPerson->Age=(*p_GT - MyPointerToPerson->DoB);  // First we update age
            
            // If it is a child
            if (MyPointerToPerson->Age<ageAdult){
                if (count_ARTKids>0) {count_ARTKids=count_ARTKids-1;}
            }
            
            // If it is a man
            if (MyPointerToPerson->Sex==1 && MyPointerToPerson->Age>=ageAdult){
                count_ARTMen[MyPointerToPerson->CD4_cat_start]=count_ARTMen[MyPointerToPerson->CD4_cat_start]-1;
                count_ARTMen_sum=count_ARTMen_sum-1;
            }
            
            // If it is a woman
            if (MyPointerToPerson->Sex==2 && MyPointerToPerson->Age>=ageAdult){
                count_ARTAdult_Women[MyPointerToPerson->CD4_cat_start]=count_ARTAdult_Women[MyPointerToPerson->CD4_cat_start]-1;
                count_ARTWomen_sum=count_ARTWomen_sum-1;
            }
        }
    }
    
    
    E(cout << "Person " << MyPointerToPerson->PersonID << " just died. Their life status now is: " << MyPointerToPerson->Alive << endl;)
}


//// --- BIRTH EVENT AND MAKING NEW PERSON --- ////
void EventBirth(person *MyPointerToPerson){
    
    E(cout << "A birth is about to happen and my life status: " << endl;)		// Error message - can be switched on/off
    
    if(MyPointerToPerson->Alive == 1) {											// Only let woman give birth if she is still alive
        
        total_population=total_population+1;									// Update total population for output and for next new entry
        MyPointerToPerson->Age= (*p_GT - MyPointerToPerson->DoB);				// Update age to get age at birth for output
        
        
        // Creating a new person
        MyArrayOfPointersToPeople[total_population-1]=new person();
        (MyArrayOfPointersToPeople[total_population-1])->PersonIDAssign(total_population-1);
        (MyArrayOfPointersToPeople[total_population-1])->Alive=1;
        (MyArrayOfPointersToPeople[total_population-1])->GenderDistribution();
        (MyArrayOfPointersToPeople[total_population-1])->GetMyDoBNewEntry();
        (MyArrayOfPointersToPeople[total_population-1])->GetDateOfDeath();
        (MyArrayOfPointersToPeople[total_population-1])->GetDateOfBaby();
        (MyArrayOfPointersToPeople[total_population-1])->GetMyDateNCD();
        (MyArrayOfPointersToPeople[total_population-1])->GetMyDateCancers();
        (MyArrayOfPointersToPeople[total_population-1])->GetMyDateOfHPVInfection();
        (MyArrayOfPointersToPeople[total_population-1])->GetMyDateOfHIVInfection();
    
        
        
        // Link Mother and Child
        (MyArrayOfPointersToPeople[total_population-1])->MotherID=MyPointerToPerson->PersonID;			// Give child their mothers ID
        MyPointerToPerson->ChildIDVector.push_back((MyArrayOfPointersToPeople[total_population-1]));	// Give mothers their child's ID
        
        E(cout << "We have finished giving birth " << endl;)					// Error message - can be switched on/off
        
    }
}


//// --- HPV EVENT --- ////

void EventMyHPVInfection(person *MyPointerToPerson){
    
    E(cout << "Somebody is about to get infected with HPV or recover: " << endl;)
    if(MyPointerToPerson->Alive == 1 ) {                                            // Only execute this is patient is still alove
        MyPointerToPerson->Age= (*p_GT - MyPointerToPerson->DoB);                // Update age to get correct parameter below
        
        ///Decide if person recovers or moves on to CIN1
        if (MyPointerToPerson->HPV_DateofInfection>0){
            MyPointerToPerson->HPV_Status=HPV_Status_HPV;
            
            int year = floor(*p_GT);
            double months = floor(((1-(*p_GT-year+0.01))*12));
            
            int j=0;
            float TestCIN1Date=0;
            std::random_device rd;
            std::mt19937 gen{rd()};
            std::uniform_int_distribution<> dis{0, 3};
            j = dis(gen);
            
            double YearFraction=-999;
            if(months>=1){YearFraction=(RandomMinMax_2(0,months))/12.1;}            // This gets month of birth as a fraction of a year
            if(months<1){YearFraction=0;}
            double    h = ((double)rand() / (RAND_MAX));                // Get a random number between 0 and 1.  NB/ THIS SHOULD HAVE A PRECISION OF 15 decimals which should be enough but lets be careful!!
            TestCIN1Date=(MyPointerToPerson->HPV_DateofInfection+j)+YearFraction;
            if (h>CIN1_Prevalence){MyPointerToPerson->CIN1_DateofInfection=no_hpv_infection;}{MyPointerToPerson->HPV_DateofRecovery=TestCIN1Date;}                // In case they recover from HPV
            
            event * HPV_DateofRecoveryEvent = new event;
            Events.push_back(HPV_DateofRecoveryEvent);
            HPV_DateofRecoveryEvent->time = MyPointerToPerson->HPV_DateofRecovery;
            HPV_DateofRecoveryEvent->p_fun = &EventMyHPVRecovery;
            HPV_DateofRecoveryEvent->person_ID = MyPointerToPerson;
            p_PQ->push(HPV_DateofRecoveryEvent);
            
            if (h<=CIN1_Prevalence){                        // In case they progress to CIN1
                 if (TestCIN1Date<MyPointerToPerson->DateOfDeath){MyPointerToPerson->CIN1_DateofInfection=TestCIN1Date;}{MyPointerToPerson->HPV_DateofRecovery=-977;}
                 if (TestCIN1Date>=MyPointerToPerson->DateOfDeath) {MyPointerToPerson->CIN1_DateofInfection=hpv_date_after_death;}
                
                event * CIN1_DateofInfectionEvent = new event;
                Events.push_back(CIN1_DateofInfectionEvent);
                CIN1_DateofInfectionEvent->time = MyPointerToPerson->CIN1_DateofInfection;
                CIN1_DateofInfectionEvent->p_fun = &EventMyCIN1Infection;
                CIN1_DateofInfectionEvent->person_ID = MyPointerToPerson;
                p_PQ->push(CIN1_DateofInfectionEvent);
                
                }
            }
    }
    
    E(cout << "Somebody has just been infected with HPV!" << endl;)                // Error message - can be switched on/off
}

void EventMyCIN1Infection(person *MyPointerToPerson){
    E(cout << "Somebody is about to get infected with CIN1 infection or recover: " << endl;)            // Error message - can be switched on/off
    if(MyPointerToPerson->Alive == 1) {       // Only execute if patient is still alive
        
        MyPointerToPerson->Age= (*p_GT - MyPointerToPerson->DoB);                // Update age to get correct parameter below
        
        ///Decide if person recovers or moves on to CIN2/3
        if (MyPointerToPerson->CIN1_DateofInfection>0){
            MyPointerToPerson->HPV_Status=HPV_Status_CIN1;
            int year = floor(*p_GT);
            double months = floor(((1-(*p_GT-year+0.01))*12));
            
            int j=0;
            float TestCIN2_3Date=0;
            std::random_device rd;
            std::mt19937 gen{rd()};
            std::uniform_int_distribution<> dis{0, 3};
            j = dis(gen);
            
            double YearFraction=-999;
            if(months>=1){YearFraction=(RandomMinMax_2(0,months))/12.1;}            // This gets month of birth as a fraction of a year
            if(months<1){YearFraction=0;}
            double    h = ((double)rand() / (RAND_MAX));                // Get a random number between 0 and 1.  NB/ THIS SHOULD HAVE A PRECISION OF 15 decimals which should be enough but lets be careful!!
            TestCIN2_3Date=(MyPointerToPerson->CIN1_DateofInfection+j)+YearFraction;
            
            if (h>CIN2_3_Prevalence){MyPointerToPerson->CIN2_3_DateofInfection=no_hpv_infection;}{MyPointerToPerson->CIN1_DateofRecovery=TestCIN2_3Date;}                // In case they recover from CIN1
            
            event * CIN1_DateofRecoveryEvent = new event;
            Events.push_back(CIN1_DateofRecoveryEvent);
            CIN1_DateofRecoveryEvent->time = MyPointerToPerson->CIN1_DateofRecovery;
            CIN1_DateofRecoveryEvent->p_fun = &EventMyHPVRecovery;
            CIN1_DateofRecoveryEvent->person_ID = MyPointerToPerson;
            p_PQ->push(CIN1_DateofRecoveryEvent);
            
            if (h<=CIN2_3_Prevalence){                        // In case they progress to CIN1
                
                if (TestCIN2_3Date<MyPointerToPerson->DateOfDeath){MyPointerToPerson->CIN2_3_DateofInfection=TestCIN2_3Date;}{MyPointerToPerson->CIN1_DateofRecovery=-977;}
                if (TestCIN2_3Date>=MyPointerToPerson->DateOfDeath) {MyPointerToPerson->CIN2_3_DateofInfection=hpv_date_after_death;}
                
                event * CIN2_3_DateofInfectionEvent = new event;
                Events.push_back(CIN2_3_DateofInfectionEvent);
                CIN2_3_DateofInfectionEvent->time = MyPointerToPerson->CIN2_3_DateofInfection;
                CIN2_3_DateofInfectionEvent->p_fun = &EventMyCIN2_3Infection;
                CIN2_3_DateofInfectionEvent->person_ID = MyPointerToPerson;
                p_PQ->push(CIN2_3_DateofInfectionEvent);
            }
        }

        

    }
    
    E(cout << "Somebody has just been infected with CIN1!" << endl;)                // Error message - can be switched on/off
}

void EventMyCIN2_3Infection(person *MyPointerToPerson){
    E(cout << "Somebody is about to get infected with CIN2_3 infection or recover: " << endl;)            // Error message - can be switched on/off
    if(MyPointerToPerson->Alive == 1) {       // Only execute if patient is still alive and has CIN1
        
        MyPointerToPerson->Age= (*p_GT - MyPointerToPerson->DoB);                // Update age to get correct parameter below
        
        ///Decide if person recovers or moves on to CIN2/3
        if (MyPointerToPerson->CIN2_3_DateofInfection>0){
            MyPointerToPerson->HPV_Status=HPV_Status_CIN2_3;
            int year = floor(*p_GT);
            double months = floor(((1-(*p_GT-year+0.01))*12));
            
            int j=0;
            float TestCISDate=0;
            std::random_device rd;
            std::mt19937 gen{rd()};
            std::uniform_int_distribution<> dis{0, 3};
            j = dis(gen);
            
            double YearFraction=-999;
            if(months>=1){YearFraction=(RandomMinMax_2(0,months))/12.1;}            // This gets month of birth as a fraction of a year
            if(months<1){YearFraction=0;}
            double    h = ((double)rand() / (RAND_MAX));                // Get a random number between 0 and 1.  NB/ THIS SHOULD HAVE A PRECISION OF 15 decimals which should be enough but lets be careful!!
            TestCISDate=(MyPointerToPerson->CIN2_3_DateofInfection+j)+YearFraction;
            if (h>CIS_Prevalence){MyPointerToPerson->CIS_DateofInfection=no_hpv_infection;}{MyPointerToPerson->CIN2_3_DateofRecovery=TestCISDate;}                // In case they recover from CIN2_3
            
            event * CIN2_3_DateofRecoveryEvent = new event;
            Events.push_back(CIN2_3_DateofRecoveryEvent);
            CIN2_3_DateofRecoveryEvent->time = MyPointerToPerson->CIN2_3_DateofRecovery;
            CIN2_3_DateofRecoveryEvent->p_fun = &EventMyHPVRecovery;
            CIN2_3_DateofRecoveryEvent->person_ID = MyPointerToPerson;
            p_PQ->push(CIN2_3_DateofRecoveryEvent);
            
            if (h<=CIS_Prevalence){                        // In case they progress to CIS
                
                if (TestCISDate<MyPointerToPerson->DateOfDeath){MyPointerToPerson->CIS_DateofInfection=TestCISDate;}{MyPointerToPerson->CIN2_3_DateofRecovery=-977;}
                if (TestCISDate>=MyPointerToPerson->DateOfDeath) {MyPointerToPerson->CIS_DateofInfection=hpv_date_after_death;}
                
                event * CIS_DateofInfectionEvent = new event;
                Events.push_back(CIS_DateofInfectionEvent);
                CIS_DateofInfectionEvent->time = MyPointerToPerson->CIS_DateofInfection;
                CIS_DateofInfectionEvent->p_fun = &EventMyCISInfection;
                CIS_DateofInfectionEvent->person_ID = MyPointerToPerson;
                p_PQ->push(CIS_DateofInfectionEvent);
            }
        }

        

    }
    
    E(cout << "Somebody has just been infected with CIN2_3!" << endl;)                // Error message - can be switched on/off
}

void EventMyCISInfection(person *MyPointerToPerson){
    E(cout << "Somebody is about to get infected with CIS infection: " << endl;)            // Error message - can be switched on/off
    if(MyPointerToPerson->Alive == 1) {       // Only execute if patient is still alive and has CIN1

        MyPointerToPerson->Age= (*p_GT - MyPointerToPerson->DoB);                // Update age to get correct parameter below
        
        ///Decide if person recovers or moves on to CIN2/3
        if (MyPointerToPerson->CIS_DateofInfection>0){
            MyPointerToPerson->HPV_Status=HPV_Status_CIS;
            int year = floor(*p_GT);
            double months = floor(((1-(*p_GT-year+0.01))*12));
            
            int j=0;
            float TestICCDate=0;
            std::random_device rd;
            std::mt19937 gen{rd()};
            std::uniform_int_distribution<> dis{0, 3};
            j = dis(gen);
            
            double YearFraction=-999;
            if(months>=1){YearFraction=(RandomMinMax_2(0,months))/12.1;}            // This gets month of birth as a fraction of a year
            if(months<1){YearFraction=0;}
            double    h = ((double)rand() / (RAND_MAX));                // Get a random number between 0 and 1.  NB/ THIS SHOULD HAVE A PRECISION OF 15 decimals which should be enough but lets be careful!!
            TestICCDate=(MyPointerToPerson->CIS_DateofInfection+j)+YearFraction;
            if (h>ICC_Prevalence){MyPointerToPerson->ICC_DateofInfection=no_hpv_infection;}{MyPointerToPerson->CIS_DateofRecovery=TestICCDate;}                // In case they recover from CIS
            
            event * CIS_DateofRecoveryEvent = new event;
            Events.push_back(CIS_DateofRecoveryEvent);
            CIS_DateofRecoveryEvent->time = MyPointerToPerson->CIS_DateofRecovery;
            CIS_DateofRecoveryEvent->p_fun = &EventMyHPVRecovery;
            CIS_DateofRecoveryEvent->person_ID = MyPointerToPerson;
            p_PQ->push(CIS_DateofRecoveryEvent);
            
            if (h<=ICC_Prevalence){                        // In case they progress to CIS
                
                if (TestICCDate<MyPointerToPerson->DateOfDeath){MyPointerToPerson->ICC_DateofInfection=TestICCDate;}{MyPointerToPerson->CIS_DateofRecovery=-977;}
                if (TestICCDate>=MyPointerToPerson->DateOfDeath) {MyPointerToPerson->ICC_DateofInfection=hpv_date_after_death;}
                
                event * ICC_DateofInfectionEvent = new event;
                Events.push_back(ICC_DateofInfectionEvent);
                ICC_DateofInfectionEvent->time = MyPointerToPerson->ICC_DateofInfection;
                ICC_DateofInfectionEvent->p_fun = &EventMyICCInfection;
                ICC_DateofInfectionEvent->person_ID = MyPointerToPerson;
                p_PQ->push(ICC_DateofInfectionEvent);
            }
        }

        

    }
    
    E(cout << "Somebody has just been infected with CIS!" << endl;)                // Error message - can be switched on/off
}


void EventMyICCInfection(person *MyPointerToPerson){
    E(cout << "Somebody is about to get infected with ICC infection (no recovery): " << endl;)            // Error message - can be switched on/off
    if(MyPointerToPerson->Alive == 1) {       // Only execute if patient is still alive
        
        MyPointerToPerson->HPV_Status=HPV_Status_ICC;
            }
    
    E(cout << "Somebody has just been infected with ICC!" << endl;)                // Error message - can be switched on/off
}


void EventMyHPVRecovery(person *MyPointerToPerson){
    E(cout << "Somebody is about to recover from a stage of HPV infection: " << endl;)            // Error message - can be switched on/off
    if(MyPointerToPerson->Alive == 1) {       // Only execute if patient is still alive and has CIN1
        
        if (MyPointerToPerson->HPV_DateofRecovery>0){
        MyPointerToPerson->HPV_Status=HPV_Status_Recovered;
        }
        
        if (MyPointerToPerson->CIN1_DateofRecovery>0){
        MyPointerToPerson->HPV_Status=HPV_Status_Recovered;
        }
        
        if (MyPointerToPerson->CIN2_3_DateofRecovery>0){
        MyPointerToPerson->HPV_Status=HPV_Status_Recovered;
        }
        
        if (MyPointerToPerson->CIS_DateofRecovery>0){
        MyPointerToPerson->HPV_Status=HPV_Status_Recovered;
        }
        
}

}



//// --- HIV EVENT --- ////
void EventMyHIVInfection(person *MyPointerToPerson){
    
    E(cout << "Somebody is about to get infected with HIV: " << endl;)			// Error message - can be switched on/off
    
    if(MyPointerToPerson->Alive == 1) {											// Only execute this is patient is still alove
        
        if (MyPointerToPerson->DateOfDeath<MyPointerToPerson->HIV){
            cout << "Error! GT " << *p_GT << " PersonID: " << MyPointerToPerson->PersonID << " Death: " << MyPointerToPerson->DateOfDeath << " HIV: " << MyPointerToPerson->HIV<< endl;
        }
        
        MyPointerToPerson->Age= (*p_GT - MyPointerToPerson->DoB);				// Update age to get correct parameter below
        
        //// Update key parameters for ART
        if (MyPointerToPerson->Age>ageAdult){
            HIV_Ref_PersonID[countHIVRef]=MyPointerToPerson->PersonID;
            countHIVRef++;
            if (MyPointerToPerson->Sex==1){Elig_Men++;}
            if (MyPointerToPerson->Sex==2){Elig_Women++;}
        }
        
        if (MyPointerToPerson->Age<ageAdult) {
            KIDS_HIV_Ref_PersonID[countKIDSHIVRef]=MyPointerToPerson->PersonID;
            countKIDSHIVRef++;
            Elig_Kids++;
            
            // Now we need to schedule an event where we move them to adult category
            double DateCD4CatSwitch=(MyPointerToPerson->DoB+ageAdult);
            
            // Schedule event for moving from kids ART to adult ART
            event * ARTCatMove = new event;
            Events.push_back(ARTCatMove);
            ARTCatMove->time = DateCD4CatSwitch;
            ARTCatMove->p_fun = &EventARTCatSwitch;
            ARTCatMove->person_ID = MyPointerToPerson;
            p_PQ->push(ARTCatMove);
        }
        
        //// --- Get my CD4 count at start --- ////
        double	h = ((double)rand() / (RAND_MAX));								// Gets a random number between 0 and 1.
        int i=0;
        int j=0;
        int a=25;
        
        while (MyPointerToPerson->Age>=a && a<46){a=a+10; i++;};				// To get the right age-specific row in the above sex-specific arrays
        while (h>CD4_startarray[MyPointerToPerson->Sex-1][i][j] && j<3){j++;}	// To get the corresponding CD4 count for correct age and sex from random 'h' generated above
        
        MyPointerToPerson->CD4_cat=0+j;											// CD4 count cat (variable over time)
        MyPointerToPerson->CD4_cat_start=0+j;									// CD4 count cat at start (to keep in records)
        MyPointerToPerson->CD4_change.at(MyPointerToPerson->CD4_cat)=*p_GT;
        
        //// --- Let's see what will happen next (Death, CD4 count progression or ART initiation) ---- ////
        double FindART_CD4_rate = ART_CD4_rates[MyPointerToPerson->Sex-1][i][MyPointerToPerson->CD4_cat];
        double FindCD4_rate = CD4_prog_rates[MyPointerToPerson->Sex-1][MyPointerToPerson->CD4_cat];
        double FindDeath_CD4_rate = Death_CD4_rates[MyPointerToPerson->Sex-1][i][MyPointerToPerson->CD4_cat];
        
        // Lets see when CD4 count progression would start
        double CD4_test = 0;
        double cd4 = ((double)rand() / (RAND_MAX));
        CD4_test = (-1/FindCD4_rate) * log(cd4);
        
        // Lets see when death would happen
        double death_test = 0;
        double dd = ((double)rand() / (RAND_MAX));
        death_test = (-1/FindDeath_CD4_rate) * log(dd);
        
        
        //  If they do not start ART then we need to evaluate if they die or decrease in CD4 count
        if (MyPointerToPerson->ART<0){
            
            if (CD4_test<death_test){
                event * CD4change = new event;
                Events.push_back(CD4change);
                CD4change->time = *p_GT+CD4_test;
                CD4change->p_fun = &EventCD4change;
                CD4change->person_ID = MyPointerToPerson;
                p_PQ->push(CD4change);
                
            }
            
            if (death_test<=CD4_test){
                
                double death_test_date = *p_GT +death_test;                  // Get the actual date, not just time until death
                
                if (death_test_date<MyPointerToPerson->DateOfDeath && death_test_date>*p_GT){   // Check HIV deaths happens before natural death
                    
                    MyPointerToPerson->DateOfDeath=death_test_date;
                    
                    event * DeathEvent = new event;						    // In that case we need to add the new death date to the EventQ
                    Events.push_back(DeathEvent);
                    DeathEvent->time = MyPointerToPerson->DateOfDeath;
                    DeathEvent->p_fun = &EventMyDeathDate;
                    DeathEvent->person_ID = MyPointerToPerson;
                    p_PQ->push(DeathEvent);
                    
                    MyPointerToPerson->CauseOfDeath=2;                      // and we will need to update Cause of Death
                }
            }
        }
        
        // Now we need to update NCD risks for HT and CKD
        E(cout << endl << endl << "We are assigning NCDs for HIV+ upon infection!" << endl;)
        
        double age_at_death=MyPointerToPerson->DateOfDeath-MyPointerToPerson->DoB;
        
        if (age_at_death>18)
        {
            
            // Some basic code and finding index for not getting NCDs
            int ncd_nr=0;                                                  // Assisgn all the possible NCDs in this code
            double DateNCD=-997;
            
            
            // Re-evaluate NCD related to HIV
            while (ncd_nr<nr_NCD_HIV){
                double r = ((double) rand() / (RAND_MAX));                 // Get a random number for each NCD
                
                
                // If we are getting an NCD lets get age and date
                if (r<=NCDArray[relatedNCD_HIV[ncd_nr]][120]*Risk_NCDHIV[ncd_nr])             // If they will get and NCD lets get the age and date and also update mortality
                {
                    
                    // Lets get the index for age at NCD
                    int i=0;
                    while (r>NCDArray[relatedNCD_HIV[ncd_nr]][i]*Risk_NCDHIV[ncd_nr]){i++;}
                    
                    // Lets get the age and date they will have the NCD
                    double YearFraction=(RandomMinMax_2(1,12))/12.1;                          // This gets month of birth as a fraction of a year
                    DateNCD=MyPointerToPerson->DoB+i+YearFraction;
                    
                    // Lets see if this pushed NCD date forward
                    if (DateNCD>=*p_GT && DateNCD<MyPointerToPerson->NCD_DatesVector.at(relatedNCD_HIV[ncd_nr]))
                    {
                        // Lets update the Date everywhere and add to the queue
                        MyPointerToPerson->NCD_DatesVector.at(relatedNCD_HIV[ncd_nr])=DateNCD;
                        
                        
                        if (ncd_nr==0)
                        {
                            MyPointerToPerson->HT=DateNCD;
                            //// --- Lets feed Hypertension into the eventQ --- ////
                            int p=MyPointerToPerson->PersonID-1;
                            event * HTEvent = new event;
                            Events.push_back(HTEvent);
                            HTEvent->time = MyPointerToPerson->HT;
                            HTEvent->p_fun = &EventMyHyptenDate;
                            HTEvent->person_ID = MyArrayOfPointersToPeople[p];
                            p_PQ->push(HTEvent);
                        }
                        
                        
                        else if (ncd_nr==1)
                        {
                            MyPointerToPerson->CKD=DateNCD;
                            //// --- Lets feed MI into the eventQ --- ////
                            int p=MyPointerToPerson->PersonID-1;
                            event * CKDEvent = new event;
                            Events.push_back(CKDEvent);
                            CKDEvent->time = MyPointerToPerson->CKD;
                            CKDEvent->p_fun = &EventMyCKDDate;
                            CKDEvent->person_ID = MyArrayOfPointersToPeople[p];
                            p_PQ->push(CKDEvent);
                        }
                        
                        
                    }
                    
                    ncd_nr++;                                                    // Lets do the next NCD
                    
                }
            }
        }
        
        E(cout << "We finished assigning NCDs for HIV+!" << endl;)
        
        
    }
    
    E(cout << "Somebody has just been infected with HIV!" << endl;)				// Error message - can be switched on/off
}


void EventCD4change(person *MyPointerToPerson){
    
    E(cout << "Somebody is about to experience a drop in CD4 count: " << endl;)	// Error message - can be switched on/off
    
    if(MyPointerToPerson->Alive == 1) {											// Add any additional things to progression of CD4 count
        
        //// --- Let's get the right index for all relevant arrays used here and update important info ---- ////
        int i=0;
        int a=25;
        
        while (MyPointerToPerson->Age>=a && a<46){a=a+10; i++;};				// To get the right age-specific row in the above sex-specific arrays
        MyPointerToPerson->Age= (*p_GT - MyPointerToPerson->DoB);				// Update age to get correct parameter below
        MyPointerToPerson->CD4_cat=MyPointerToPerson->CD4_cat+1;                // Update CD4 count
        MyPointerToPerson->CD4_change.at(MyPointerToPerson->CD4_cat)=*p_GT;
        
        
        //// --- When CD4 count hits the lowest possible value and no ART is started --- ////
        if (MyPointerToPerson->CD4_cat==6 && MyPointerToPerson->ART<0){
            
            double FindDeath_CD4_rate = Death_CD4_rates[MyPointerToPerson->Sex-1][i][MyPointerToPerson->CD4_cat];
            
            // Lets see when death would happen
            double death_test = 0;
            double dd = ((double)rand() / (RAND_MAX));
            death_test = (-1/FindDeath_CD4_rate) * log(dd);
            //cout <<  death_test << endl;
            
            double death_test_date = *p_GT +death_test;                      // Get the actual date, not just time until death
            
            if (death_test_date<MyPointerToPerson->DateOfDeath && death_test_date>*p_GT){            // Check HIV deaths happens before natural death
                
                MyPointerToPerson->DateOfDeath=death_test_date;
                
                event * DeathEvent = new event;								// In that case we need to add the new death date to the EventQ
                Events.push_back(DeathEvent);
                DeathEvent->time = MyPointerToPerson->DateOfDeath;
                DeathEvent->p_fun = &EventMyDeathDate;
                DeathEvent->person_ID = MyPointerToPerson;
                p_PQ->push(DeathEvent);
                
                MyPointerToPerson->CauseOfDeath=2;                          // and we will need to update Cause of Death
            }
        }
        
        //// --- In case CD4 count is higher than minimum possible category.  They can either die OR Progress AND they still have a 1 year risk of death if on ART --- ///
        if (MyPointerToPerson->CD4_cat<6){
            
            //// --- Let's see what will happen next (Death, CD4 count progression or ART initiation) ---- ////
            double FindART_CD4_rate = ART_CD4_rates[MyPointerToPerson->Sex-1][i][MyPointerToPerson->CD4_cat];
            double FindCD4_rate = CD4_prog_rates[MyPointerToPerson->Sex-1][MyPointerToPerson->CD4_cat];
            double FindDeath_CD4_rate = Death_CD4_rates[MyPointerToPerson->Sex-1][i][MyPointerToPerson->CD4_cat];
            
            // Lets see when CD4 count progression would start
            double CD4_test = 0;
            double cd4 = ((double)rand() / (RAND_MAX));
            CD4_test = (-1/FindCD4_rate) * log(cd4);
            
            
            // Lets see when death would happen
            double death_test = 0;
            double dd = ((double)rand() / (RAND_MAX));
            death_test = (-1/FindDeath_CD4_rate) * log(dd);
            
            
            // If patient hasn't started ART yet
            if (MyPointerToPerson->ART<0){
                
                if (CD4_test<death_test){
                    event * CD4change = new event;
                    Events.push_back(CD4change);
                    CD4change->time = *p_GT+CD4_test;
                    CD4change->p_fun = &EventCD4change;
                    CD4change->person_ID = MyPointerToPerson;
                    p_PQ->push(CD4change);
                }
                
                if (death_test<=CD4_test){
                    
                    double death_test_date = *p_GT +death_test;                      // Get the actual date, not just time until death
                    
                    if (death_test_date<MyPointerToPerson->DateOfDeath && death_test_date>*p_GT){            // Check HIV deaths happens before natural death
                        MyPointerToPerson->DateOfDeath=death_test_date;
                        event * DeathEvent = new event;								// In that case we need to add the new death date to the EventQ
                        Events.push_back(DeathEvent);
                        DeathEvent->time = MyPointerToPerson->DateOfDeath;
                        DeathEvent->p_fun = &EventMyDeathDate;
                        DeathEvent->person_ID = MyPointerToPerson;
                        p_PQ->push(DeathEvent);
                        
                        MyPointerToPerson->CauseOfDeath=2;                          // and we will need to update Cause of Death
                    }
                    
                }
                
            }
            
        }
        
    }
    
    E(cout << "Somebody has just experiences a drop in CD4 count!" << endl;)	// Error message - can be switched on/off
}


//// --- NCD EVENTS --- ////

void EventMyDepressionDate(person *MyPointerToPerson)		// Function executed when person develops depression
{
    E(cout << endl << endl << "This patient just developed depression!" << endl;)
    
    if(MyPointerToPerson->Alive == 1 && MyPointerToPerson->Depression_status==0) {
        
        MyPointerToPerson->Depression_status=1;
        
        // Lets see if we need to update death date
        int ncd_index=1;
        
        // Lets see if they die earlier
        int k=(MyPointerToPerson->DoB-1800);					// To find corresponding year of birth from mortality array
        int j=0;												// This will be matched to probability taken from random number generator
        double	d = ((double) rand() / (RAND_MAX)) ;			// get a random number to determine Life Expectancy
        double TestDeathDate;
        
        if (MyPointerToPerson->Sex==1){
            while(d>DeathArray_Men[k][j]*MortRisk[ncd_index] && j<121){j++;}
            TestDeathDate=(MyPointerToPerson->DoB+j);
        }
        
        if (MyPointerToPerson->Sex==2) {
            while(d>DeathArray_Women[k][j]*MortRisk[ncd_index] && j<121){j++;}
            TestDeathDate=(MyPointerToPerson->DoB+j);
        }
        
        if (TestDeathDate<MyPointerToPerson->DateOfDeath && TestDeathDate>*p_GT){
            
            MyPointerToPerson->DateOfDeath=TestDeathDate;
            
            // 2. Lets feed death into the eventQ
            if (MyPointerToPerson->DateOfDeath<EndYear){
                int p=MyPointerToPerson->PersonID-1;
                event * DeathEvent = new event;
                Events.push_back(DeathEvent);
                DeathEvent->time = MyPointerToPerson->DateOfDeath;
                DeathEvent->p_fun = &EventMyDeathDate;
                DeathEvent->person_ID = MyArrayOfPointersToPeople[p];
                p_PQ->push(DeathEvent);
                
                // Update cause of death
                MyPointerToPerson->CauseOfDeath=4;
            }
        }
    }
    
}

void EventMyAsthmaDate(person *MyPointerToPerson)			     // Function executed when person develops asthma
{
    E(cout << endl << endl << "This patient just developed asthma!" << endl;)
    
    if(MyPointerToPerson->Alive == 1 && MyPointerToPerson->Asthma_status==0) {
        MyPointerToPerson->Asthma_status=1;
        
        // Lets see if we need to update death date
        int ncd_index=2;
        
        // Lets see if they die earlier
        int k=(MyPointerToPerson->DoB-1800);					// To find corresponding year of birth from mortality array
        int j=0;												// This will be matched to probability taken from random number generator
        double	d = ((double) rand() / (RAND_MAX)) ;			// get a random number to determine Life Expectancy
        double TestDeathDate;
        
        if (MyPointerToPerson->Sex==1){
            while(d>DeathArray_Men[k][j]*MortRisk[ncd_index] && j<121){j++;}
            TestDeathDate=(MyPointerToPerson->DoB+j);
        }
        
        if (MyPointerToPerson->Sex==2) {
            while(d>DeathArray_Women[k][j]*MortRisk[ncd_index] && j<121){j++;}
            TestDeathDate=(MyPointerToPerson->DoB+j);
        }
        
        if (TestDeathDate<MyPointerToPerson->DateOfDeath && TestDeathDate>*p_GT){
            
            MyPointerToPerson->DateOfDeath=TestDeathDate;
            
            // 2. Lets feed death into the eventQ
            if (MyPointerToPerson->DateOfDeath<EndYear){
                int p=MyPointerToPerson->PersonID-1;
                event * DeathEvent = new event;
                Events.push_back(DeathEvent);
                DeathEvent->time = MyPointerToPerson->DateOfDeath;
                DeathEvent->p_fun = &EventMyDeathDate;
                DeathEvent->person_ID = MyArrayOfPointersToPeople[p];
                p_PQ->push(DeathEvent);
                
                // Update cause of death
                MyPointerToPerson->CauseOfDeath=5;
            }
        }
    }
}

void EventMyStrokeDate(person *MyPointerToPerson)			    // Function executed when person develops hypertension
{
    E(cout << endl << endl << "This patient just developed stroke!" << endl;)
    
    if(MyPointerToPerson->Alive == 1 && MyPointerToPerson->Stroke_status==0) {
        
        MyPointerToPerson->Stroke_status=1;
    
    
        // Lets see if we need to update death date
        int ncd_index=3;
        int error=0;
        int k=(MyPointerToPerson->DoB-1800);					// To find corresponding year of birth from mortality array
        int j=0;												// This will be matched to probability taken from random number generator
        double	d = ((double) rand() / (RAND_MAX)) ;			// get a random number to determine Life Expectancy
        double TestDeathDate;

        
        // Lets see if they die within the year                 REFERENCE: http://stroke.ahajournals.org/content/32/9/2131.full
        double p_mort=((double) rand () / (RAND_MAX));
        
        if (p_mort<0.6)
        {
            double YearFraction=(RandomMinMax_2(1,12))/12.1;     // See when during this year this person will die
            TestDeathDate=*p_GT + YearFraction;
        }

        // Lets see if they die earlier
        if (p_mort>=0.6)
        {
            if (MyPointerToPerson->Sex==1){
                while(d>DeathArray_Men[k][j]*MortRisk[ncd_index] && j<121){j++;}
                TestDeathDate=(MyPointerToPerson->DoB+j);
            }
        
            if (MyPointerToPerson->Sex==2) {
                while(d>DeathArray_Women[k][j]*MortRisk[ncd_index] && j<121){j++;}
                TestDeathDate=(MyPointerToPerson->DoB+j);
            }
        }
        
        
        if (TestDeathDate<MyPointerToPerson->DateOfDeath && TestDeathDate>*p_GT){
            
            MyPointerToPerson->DateOfDeath=TestDeathDate;
            
            // 2. Lets feed death into the eventQ
            if (MyPointerToPerson->DateOfDeath<EndYear){
                int p=MyPointerToPerson->PersonID-1;
                event * DeathEvent = new event;
                Events.push_back(DeathEvent);
                DeathEvent->time = MyPointerToPerson->DateOfDeath;
                DeathEvent->p_fun = &EventMyDeathDate;
                DeathEvent->person_ID = MyArrayOfPointersToPeople[p];
                p_PQ->push(DeathEvent);
                
                // Update cause of death
                MyPointerToPerson->CauseOfDeath=6;
            }
        }
    }
}


void EventMyDiabetesDate(person *MyPointerToPerson){
    
    E(cout << "I just developed Diabetes, lets see if I am at an increased risk of other NCDs!" << endl;)
    
    
    if (MyPointerToPerson->Diabetes_status==0 && MyPointerToPerson->Alive == 1)
    {
        // First lets update Diabetes status to make sure any over-written dates don't run the same cod again
        MyPointerToPerson->Diabetes_status=1;
        
        // Lets see if we need to update death date
        int ncd_index=4;
        
        // Lets see if they die earlier
        int k=(MyPointerToPerson->DoB-1800);					// To find corresponding year of birth from mortality array
        int j=0;												// This will be matched to probability taken from random number generator
        double	d = ((double) rand() / (RAND_MAX)) ;			// get a random number to determine Life Expectancy
        double TestDeathDate;
        
        if (MyPointerToPerson->Sex==1){
            while(d>DeathArray_Men[k][j]*MortRisk[ncd_index] && j<121){j++;}
            TestDeathDate=(MyPointerToPerson->DoB+j);
        }
        
        if (MyPointerToPerson->Sex==2) {
            while(d>DeathArray_Women[k][j]*MortRisk[ncd_index] && j<121){j++;}
            TestDeathDate=(MyPointerToPerson->DoB+j);
        }
        
        if (TestDeathDate<MyPointerToPerson->DateOfDeath && TestDeathDate>*p_GT){
            
            MyPointerToPerson->DateOfDeath=TestDeathDate;
            
            // 2. Lets feed death into the eventQ
            if (MyPointerToPerson->DateOfDeath<EndYear){
                int p=MyPointerToPerson->PersonID-1;
                event * DeathEvent = new event;
                Events.push_back(DeathEvent);
                DeathEvent->time = MyPointerToPerson->DateOfDeath;
                DeathEvent->p_fun = &EventMyDeathDate;
                DeathEvent->person_ID = MyArrayOfPointersToPeople[p];
                p_PQ->push(DeathEvent);
                
                // Update cause of death
                MyPointerToPerson->CauseOfDeath=7;
            }
        }
        
        
        
        // Some basic code and finding index for not getting NCDs
        int ncd_nr=0;
        double DateNCD=-997;                                                               // As with HIV, if they don't get NCDs set it to -998 to show code was executed
        
        
        // Re-evaluate NCD related to diabetes
        while (ncd_nr<nr_NCD_Diab)
        {
            
            // Get a random number for each NCD
            double r = ((double) rand() / (RAND_MAX));
            
            // If we are getting an NCD lets get the age and date of NCD
            if (r<NCDArray[relatedNCDs_Diab[ncd_nr]][120]*Risk_NCD_Diabetes[ncd_nr])
            {
                // Lets get the index for age at NCD
                int i=0;
                while (r>NCDArray[relatedNCDs_Diab[ncd_nr]][i]*Risk_NCD_Diabetes[ncd_nr]){i++;}
                
                
                // Lets get the age and date they will have the NCD
                double YearFraction=(RandomMinMax_2(1,12))/12.1;                          // This gets month of birth as a fraction of a year
                DateNCD=MyPointerToPerson->DoB+i+YearFraction;
                
            }
            
            
            // Lets see if this pushed forward the existing NCD date
            if (DateNCD>=*p_GT && DateNCD<MyPointerToPerson->NCD_DatesVector.at(relatedNCDs_Diab[ncd_nr]))
            {
                
                // Lets update the Date everywhere and add to queue
                MyPointerToPerson->NCD_DatesVector.at(relatedNCDs_Diab[ncd_nr])=DateNCD;
                
                if (ncd_nr==0)
                {
                    MyPointerToPerson->HT=DateNCD;
                    
                    //// --- Lets feed Hypertension into the eventQ --- ////
                    int p=MyPointerToPerson->PersonID-1;
                    event * HTEvent = new event;
                    Events.push_back(HTEvent);
                    HTEvent->time = MyPointerToPerson->HT;
                    HTEvent->p_fun = &EventMyHyptenDate;
                    HTEvent->person_ID = MyArrayOfPointersToPeople[p];
                    p_PQ->push(HTEvent);
                }
                
                
                if (ncd_nr==1)
                {
                    MyPointerToPerson->Stroke=DateNCD;
                    
                    //// --- Lets feed Stroke into the eventQ --- ////
                    int p=MyPointerToPerson->PersonID-1;
                    event * StrokeEvent = new event;
                    Events.push_back(StrokeEvent);
                    StrokeEvent->time = MyPointerToPerson->Stroke;
                    StrokeEvent->p_fun = &EventMyStrokeDate;
                    StrokeEvent->person_ID = MyArrayOfPointersToPeople[p];
                    p_PQ->push(StrokeEvent);
                }
                
                if (ncd_nr==2)
                {
                    MyPointerToPerson->CKD=DateNCD;
                    
                    //// --- Lets feed Stroke into the eventQ --- ////
                    int p=MyPointerToPerson->PersonID-1;
                    event * CKDEvent = new event;
                    Events.push_back(CKDEvent);
                    CKDEvent->time = MyPointerToPerson->CKD;
                    CKDEvent->p_fun = &EventMyCKDDate;
                    CKDEvent->person_ID = MyArrayOfPointersToPeople[p];
                    p_PQ->push(CKDEvent);
                }
                
            }
            
            ncd_nr++;
        }
    }
    E(cout << endl << endl << "Diabetes has developed and addition risks evaluated!" << endl;)
    
}

void EventMyHyptenDate(person *MyPointerToPerson)			// Function executed when person develops hypertension
{
    E(cout << "I just developed Hypertension, lets see if I am at an increased risk of other NCDs!" << endl;)
    
    
    if (MyPointerToPerson->HT_status==0 && MyPointerToPerson->Alive == 1)
    {
        // First lets update Diabetes status to make sure any over-written dates don't run the same cod again
        MyPointerToPerson->HT_status=1;
        
        // Lets see if we need to update death date
        int ncd_index=0;
        
        // Lets see if they die earlier
        int k=(MyPointerToPerson->DoB-1800);					// To find corresponding year of birth from mortality array
        int j=0;												// This will be matched to probability taken from random number generator
        double	d = ((double) rand() / (RAND_MAX)) ;			// get a random number to determine Life Expectancy
        double TestDeathDate;
        
        if (MyPointerToPerson->Sex==1){
            while(d>DeathArray_Men[k][j]*MortRisk[ncd_index] && j<121){j++;}
            TestDeathDate=(MyPointerToPerson->DoB+j);
        }
        
        if (MyPointerToPerson->Sex==2) {
            while(d>DeathArray_Women[k][j]*MortRisk[ncd_index] && j<121){j++;}
            TestDeathDate=(MyPointerToPerson->DoB+j);
        }
        
        if (TestDeathDate<MyPointerToPerson->DateOfDeath && TestDeathDate>*p_GT){
            
            MyPointerToPerson->DateOfDeath=TestDeathDate;
            
            // 2. Lets feed death into the eventQ
            if (MyPointerToPerson->DateOfDeath<EndYear){
                int p=MyPointerToPerson->PersonID-1;
                event * DeathEvent = new event;
                Events.push_back(DeathEvent);
                DeathEvent->time = MyPointerToPerson->DateOfDeath;
                DeathEvent->p_fun = &EventMyDeathDate;
                DeathEvent->person_ID = MyArrayOfPointersToPeople[p];
                p_PQ->push(DeathEvent);
                
                // Update cause of death
                MyPointerToPerson->CauseOfDeath=3;
            }
        }
        
        
        // Some basic code and finding index for not getting NCDs
        int ncd_nr=0;
        double DateNCD=-997;                                       // As with HIV, if they don't get NCDs set it to -998 to show code was executed
        
        
        // Re-evaluate HC/HT and Renal
        while (ncd_nr<nr_NCD_HT)
        {
            // Get a random number for each NCD
            double r = ((double) rand() / (RAND_MAX));
            
            
            // If we are getting an NCD lets get the age and date of NCD
            if (r<NCDArray[relatedNCDs_HT[ncd_nr]][120]*Risk_NCD_HT[ncd_nr])
            {
                // Lets get the age and date they will have the NCD
                int i=0;
                while (r>NCDArray[relatedNCDs_HT[ncd_nr]][i]*Risk_NCD_HT[ncd_nr]){i++;}
                
                
                // Lets get the age and the date they will have the NCD
                double YearFraction=(RandomMinMax_2(1,12))/12.1;                          // This gets month of birth as a fraction of a year
                DateNCD=MyPointerToPerson->DoB+i+YearFraction;
            }
            
            
            // Lets see if this pushed forward the existing NCD date
            if (DateNCD>=*p_GT && DateNCD<MyPointerToPerson->NCD_DatesVector.at(relatedNCDs_HT[ncd_nr]))
            {
                
                // Lets update the Date everywhere and add to queue
                MyPointerToPerson->NCD_DatesVector.at(relatedNCDs_HT[ncd_nr])=DateNCD;
                
                
                if (ncd_nr==0)
                {
                    MyPointerToPerson->Stroke=DateNCD;
                    
                    //// --- Lets feed Stroke into the eventQ --- ////
                    int p=MyPointerToPerson->PersonID-1;
                    event * StrokeEvent = new event;
                    Events.push_back(StrokeEvent);
                    StrokeEvent->time = MyPointerToPerson->Stroke;
                    StrokeEvent->p_fun = &EventMyStrokeDate;
                    StrokeEvent->person_ID = MyArrayOfPointersToPeople[p];
                    p_PQ->push(StrokeEvent);
                }
                
                if (ncd_nr==1)
                {
                    MyPointerToPerson->CKD=DateNCD;
                    
                    //// --- Lets feed Stroke into the eventQ --- ////
                    int p=MyPointerToPerson->PersonID-1;
                    event * CKDEvent = new event;
                    Events.push_back(CKDEvent);
                    CKDEvent->time = MyPointerToPerson->CKD;
                    CKDEvent->p_fun = &EventMyCKDDate;
                    CKDEvent->person_ID = MyArrayOfPointersToPeople[p];
                    p_PQ->push(CKDEvent);
                }
            }
            
            ncd_nr++;
        }
    }
    E(cout << endl << endl << "Hypercholesterolaemia has developed and addition risks evaluated!" << endl;)
}


void EventMyCKDDate (person *MyPointerToPerson)			// Function executed when person develops hypertension
{
    
    E(cout << endl << endl << "This patient just developed CKD!" << endl;)
    
    if (MyPointerToPerson->CKD_status==0 && MyPointerToPerson->Alive == 1){
        
        MyPointerToPerson->CKD_status=1;
        
        // Lets see if we need to update death date
        int ncd_index=5;
        
        // Lets see if they die earlier
        int k=(MyPointerToPerson->DoB-1800);					// To find corresponding year of birth from mortality array
        int j=0;												// This will be matched to probability taken from random number generator
        double	d = ((double) rand() / (RAND_MAX)) ;			// get a random number to determine Life Expectancy
        double TestDeathDate;
        
        if (MyPointerToPerson->Sex==1){
            while(d>DeathArray_Men[k][j]*MortRisk[ncd_index] && j<121){j++;}
            TestDeathDate=(MyPointerToPerson->DoB+j);
        }
        
        if (MyPointerToPerson->Sex==2) {
            while(d>DeathArray_Women[k][j]*MortRisk[ncd_index] && j<121){j++;}
            TestDeathDate=(MyPointerToPerson->DoB+j);
        }
        
        
        if (TestDeathDate<MyPointerToPerson->DateOfDeath && TestDeathDate>*p_GT) {
            
            MyPointerToPerson->DateOfDeath=TestDeathDate;
            
            // 2. Lets feed death into the eventQ
            if (MyPointerToPerson->DateOfDeath<EndYear){
                int p=MyPointerToPerson->PersonID-1;
                event * DeathEvent = new event;
                Events.push_back(DeathEvent);
                DeathEvent->time = MyPointerToPerson->DateOfDeath;
                DeathEvent->p_fun = &EventMyDeathDate;
                DeathEvent->person_ID = MyArrayOfPointersToPeople[p];
                p_PQ->push(DeathEvent);
                
                // Update cause of death
                MyPointerToPerson->CauseOfDeath=8;
            }
        }
    }
}


void EventMyBreastDate (person *MyPointerToPerson)			// Function executed when person develops hypertension
{
    
    E(cout << endl << endl << "This patient just developed breast cancer!" << endl;)
    
    if (MyPointerToPerson->Breast_status==0 && MyPointerToPerson->Alive == 1){
        
        MyPointerToPerson->Breast_status=1;
        
        // Lets see if we need to update death date
        int cancer_index=0;
        
        // Lets see if they die earlier
        int k=(MyPointerToPerson->DoB-1800);					// To find corresponding year of birth from mortality array
        int j=0;												// This will be matched to probability taken from random number generator
        double	d = ((double) rand() / (RAND_MAX)) ;			// get a random number to determine Life Expectancy
        double TestDeathDate;
        
        if (MyPointerToPerson->Sex==1){
            while(d>DeathArray_Men[k][j]*MortRisk_Cancer[cancer_index] && j<121){j++;}
            TestDeathDate=(MyPointerToPerson->DoB+j);
        }
        
        if (MyPointerToPerson->Sex==2) {
            while(d>DeathArray_Women[k][j]*MortRisk_Cancer[cancer_index] && j<121){j++;}
            TestDeathDate=(MyPointerToPerson->DoB+j);
        }
        
        if (TestDeathDate<MyPointerToPerson->DateOfDeath && TestDeathDate>*p_GT) {
            
            MyPointerToPerson->DateOfDeath=TestDeathDate;
            
            // 2. Lets feed death into the eventQ
            if (MyPointerToPerson->DateOfDeath<EndYear){
                int p=MyPointerToPerson->PersonID-1;
                event * DeathEvent = new event;
                Events.push_back(DeathEvent);
                DeathEvent->time = MyPointerToPerson->DateOfDeath;
                DeathEvent->p_fun = &EventMyDeathDate;
                DeathEvent->person_ID = MyArrayOfPointersToPeople[p];
                p_PQ->push(DeathEvent);
                
                // Update cause of death
                MyPointerToPerson->CauseOfDeath=9;
            }
        }
    }
}

void EventMyCervicalDate (person *MyPointerToPerson)			// Function executed when person develops hypertension
{
    
    E(cout << endl << endl << "This patient just developed cervical cancer!" << endl;)
    
    if (MyPointerToPerson->Cervical_status==0 && MyPointerToPerson->Alive == 1){
        
        MyPointerToPerson->Cervical_status=1;
        
        // Lets see if we need to update death date
        int cancer_index=1;
        
        // Lets see if they die earlier
        int k=(MyPointerToPerson->DoB-1800);					// To find corresponding year of birth from mortality array
        int j=0;												// This will be matched to probability taken from random number generator
        double	d = ((double) rand() / (RAND_MAX)) ;			// get a random number to determine Life Expectancy
        double TestDeathDate;
        
        if (MyPointerToPerson->Sex==1){
            while(d>DeathArray_Men[k][j]*MortRisk_Cancer[cancer_index] && j<121){j++;}
            TestDeathDate=(MyPointerToPerson->DoB+j);
        }
        
        if (MyPointerToPerson->Sex==2) {
            while(d>DeathArray_Women[k][j]*MortRisk_Cancer[cancer_index] && j<121){j++;}
            TestDeathDate=(MyPointerToPerson->DoB+j);
        }
        
        if (TestDeathDate<MyPointerToPerson->DateOfDeath && TestDeathDate>*p_GT) {
            
            MyPointerToPerson->DateOfDeath=TestDeathDate;
            
            // 2. Lets feed death into the eventQ
            if (MyPointerToPerson->DateOfDeath<EndYear){
                int p=MyPointerToPerson->PersonID-1;
                event * DeathEvent = new event;
                Events.push_back(DeathEvent);
                DeathEvent->time = MyPointerToPerson->DateOfDeath;
                DeathEvent->p_fun = &EventMyDeathDate;
                DeathEvent->person_ID = MyArrayOfPointersToPeople[p];
                p_PQ->push(DeathEvent);
                
                // Update cause of death
                MyPointerToPerson->CauseOfDeath=10;
            }
        }
    }
}

void EventMyColoDate (person *MyPointerToPerson)			// Function executed when person develops hypertension
{
    
    E(cout << endl << endl << "This patient just developed breast cancer!" << endl;)
    
    if (MyPointerToPerson->Colo_status==0 && MyPointerToPerson->Alive == 1){
        
        MyPointerToPerson->Colo_status=1;
        
        // Lets see if we need to update death date
        int cancer_index=2;
        
        // Lets see if they die earlier
        int k=(MyPointerToPerson->DoB-1800);					// To find corresponding year of birth from mortality array
        int j=0;												// This will be matched to probability taken from random number generator
        double	d = ((double) rand() / (RAND_MAX)) ;			// get a random number to determine Life Expectancy
        double TestDeathDate;
        
        if (MyPointerToPerson->Sex==1){
            while(d>DeathArray_Men[k][j]*MortRisk_Cancer[cancer_index] && j<121){j++;}
            TestDeathDate=(MyPointerToPerson->DoB+j);
        }
        
        if (MyPointerToPerson->Sex==2) {
            while(d>DeathArray_Women[k][j]*MortRisk_Cancer[cancer_index] && j<121){j++;}
            TestDeathDate=(MyPointerToPerson->DoB+j);
        }
        
        if (TestDeathDate<MyPointerToPerson->DateOfDeath && TestDeathDate>*p_GT) {
            
            MyPointerToPerson->DateOfDeath=TestDeathDate;
            
            // 2. Lets feed death into the eventQ
            if (MyPointerToPerson->DateOfDeath<EndYear){
                int p=MyPointerToPerson->PersonID-1;
                event * DeathEvent = new event;
                Events.push_back(DeathEvent);
                DeathEvent->time = MyPointerToPerson->DateOfDeath;
                DeathEvent->p_fun = &EventMyDeathDate;
                DeathEvent->person_ID = MyArrayOfPointersToPeople[p];
                p_PQ->push(DeathEvent);
                
                // Update cause of death
                MyPointerToPerson->CauseOfDeath=11;
            }
        }
    }
}



void EventMyLiverDate (person *MyPointerToPerson)			// Function executed when person develops hypertension
{
    
    E(cout << endl << endl << "This patient just developed Liver cancer!" << endl;)
    
    if (MyPointerToPerson->Liver_status==0 && MyPointerToPerson->Alive == 1){
        
        MyPointerToPerson->Liver_status=1;
        
        //cout << "We have Liver " << MyPointerToPerson->Liver << endl;
        
        // Lets see if we need to update death date
        int cancer_index=3;
        
        // Lets see if they die earlier
        int k=(MyPointerToPerson->DoB-1800);					// To find corresponding year of birth from mortality array
        int j=0;												// This will be matched to probability taken from random number generator
        double	d = ((double) rand() / (RAND_MAX)) ;			// get a random number to determine Life Expectancy
        double TestDeathDate;
        
        if (MyPointerToPerson->Sex==1){
            while(d>DeathArray_Men[k][j]*MortRisk_Cancer[cancer_index] && j<121){j++;}
            TestDeathDate=(MyPointerToPerson->DoB+j);
        }
        
        if (MyPointerToPerson->Sex==2) {
            while(d>DeathArray_Women[k][j]*MortRisk_Cancer[cancer_index] && j<121){j++;}
            TestDeathDate=(MyPointerToPerson->DoB+j);
        }
        
        
        if (TestDeathDate<MyPointerToPerson->DateOfDeath && TestDeathDate>*p_GT) {
            
            MyPointerToPerson->DateOfDeath=TestDeathDate;
            
            // 2. Lets feed death into the eventQ
            if (MyPointerToPerson->DateOfDeath<EndYear){
                int p=MyPointerToPerson->PersonID-1;
                event * DeathEvent = new event;
                Events.push_back(DeathEvent);
                DeathEvent->time = MyPointerToPerson->DateOfDeath;
                DeathEvent->p_fun = &EventMyDeathDate;
                DeathEvent->person_ID = MyArrayOfPointersToPeople[p];
                p_PQ->push(DeathEvent);
                
                // Update cause of death
                MyPointerToPerson->CauseOfDeath=12;
            }
        }
    }
}


void EventMyOesoDate (person *MyPointerToPerson)			// Function executed when person develops hypertension
{
    
    E(cout << endl << endl << "This patient just developed Oesophageal cancer!" << endl;)
    
    if (MyPointerToPerson->Oeso_status==0 && MyPointerToPerson->Alive == 1){
        
        MyPointerToPerson->Oeso_status=1;
        
        //cout << "We have Oeso " << MyPointerToPerson->Oeso << endl;
        
        // Lets see if we need to update death date
        int cancer_index=4;
        
        // Lets see if they die earlier
        int k=(MyPointerToPerson->DoB-1800);					// To find corresponding year of birth from mortality array
        int j=0;												// This will be matched to probability taken from random number generator
        double	d = ((double) rand() / (RAND_MAX)) ;			// get a random number to determine Life Expectancy
        double TestDeathDate;
        
        if (MyPointerToPerson->Sex==1){
            while(d>DeathArray_Men[k][j]*MortRisk_Cancer[cancer_index] && j<121){j++;}
            TestDeathDate=(MyPointerToPerson->DoB+j);
        }
        
        if (MyPointerToPerson->Sex==2) {
            while(d>DeathArray_Women[k][j]*MortRisk_Cancer[cancer_index] && j<121){j++;}
            TestDeathDate=(MyPointerToPerson->DoB+j);
        }
        
        if (TestDeathDate<MyPointerToPerson->DateOfDeath && TestDeathDate>*p_GT) {
            
            MyPointerToPerson->DateOfDeath=TestDeathDate;
            
            // 2. Lets feed death into the eventQ
            if (MyPointerToPerson->DateOfDeath<EndYear){
                int p=MyPointerToPerson->PersonID-1;
                event * DeathEvent = new event;
                Events.push_back(DeathEvent);
                DeathEvent->time = MyPointerToPerson->DateOfDeath;
                DeathEvent->p_fun = &EventMyDeathDate;
                DeathEvent->person_ID = MyArrayOfPointersToPeople[p];
                p_PQ->push(DeathEvent);
                
                // Update cause of death
                MyPointerToPerson->CauseOfDeath=13;
            }
        }
    }
}


void EventMyProstateDate (person *MyPointerToPerson)			// Function executed when person develops hypertension
{
    
    E(cout << endl << endl << "This patient just developed prostate cancer!" << endl;)
    
    if (MyPointerToPerson->Prostate==0 && MyPointerToPerson->Alive == 1){
        
        MyPointerToPerson->Prostate_status=1;
        
        // Lets see if we need to update death date
        int cancer_index=5;
        
        // Lets see if they die earlier
        int k=(MyPointerToPerson->DoB-1800);					// To find corresponding year of birth from mortality array
        int j=0;												// This will be matched to probability taken from random number generator
        double	d = ((double) rand() / (RAND_MAX)) ;			// get a random number to determine Life Expectancy
        double TestDeathDate;
        
        if (MyPointerToPerson->Sex==1){
            while(d>DeathArray_Men[k][j]*MortRisk_Cancer[cancer_index] && j<121){j++;}
            TestDeathDate=(MyPointerToPerson->DoB+j);
        }
        
        if (MyPointerToPerson->Sex==2) {
            while(d>DeathArray_Women[k][j]*MortRisk_Cancer[cancer_index] && j<121){j++;}
            TestDeathDate=(MyPointerToPerson->DoB+j);
        }
        
        if (TestDeathDate<MyPointerToPerson->DateOfDeath && TestDeathDate>*p_GT) {
            
            MyPointerToPerson->DateOfDeath=TestDeathDate;
            
            // 2. Lets feed death into the eventQ
            if (MyPointerToPerson->DateOfDeath<EndYear){
                int p=MyPointerToPerson->PersonID-1;
                event * DeathEvent = new event;
                Events.push_back(DeathEvent);
                DeathEvent->time = MyPointerToPerson->DateOfDeath;
                DeathEvent->p_fun = &EventMyDeathDate;
                DeathEvent->person_ID = MyArrayOfPointersToPeople[p];
                p_PQ->push(DeathEvent);
                
                // Update cause of death
                MyPointerToPerson->CauseOfDeath=14;
            }
        }
    }
}


void EventMyOtherCanDate (person *MyPointerToPerson)			// Function executed when person develops hypertension
{
    
    E(cout << endl << endl << "This patient just developed Other cancer!" << endl;)
    
    if (MyPointerToPerson->OtherCan_status==0 && MyPointerToPerson->Alive == 1){
        
        MyPointerToPerson->OtherCan_status=1;
        
        // Lets see if we need to update death date
        int cancer_index=6;
        
        // Lets see if they die earlier
        int k=(MyPointerToPerson->DoB-1800);					// To find corresponding year of birth from mortality array
        int j=0;												// This will be matched to probability taken from random number generator
        double	d = ((double) rand() / (RAND_MAX)) ;			// get a random number to determine Life Expectancy
        double TestDeathDate;
        
        if (MyPointerToPerson->Sex==1){
            while(d>DeathArray_Men[k][j]*MortRisk_Cancer[cancer_index] && j<121){j++;}
            TestDeathDate=(MyPointerToPerson->DoB+j);
        }
        
        if (MyPointerToPerson->Sex==2) {
            while(d>DeathArray_Women[k][j]*MortRisk_Cancer[cancer_index] && j<121){j++;}
            TestDeathDate=(MyPointerToPerson->DoB+j);
        }
        
        if (TestDeathDate<MyPointerToPerson->DateOfDeath && TestDeathDate>*p_GT) {
            
            MyPointerToPerson->DateOfDeath=TestDeathDate;
            
            // 2. Lets feed death into the eventQ
            if (MyPointerToPerson->DateOfDeath<EndYear){
                int p=MyPointerToPerson->PersonID-1;
                event * DeathEvent = new event;
                Events.push_back(DeathEvent);
                DeathEvent->time = MyPointerToPerson->DateOfDeath;
                DeathEvent->p_fun = &EventMyDeathDate;
                DeathEvent->person_ID = MyArrayOfPointersToPeople[p];
                p_PQ->push(DeathEvent);
                
                // Update cause of death
                MyPointerToPerson->CauseOfDeath=15;
            }
        }
    }
}



