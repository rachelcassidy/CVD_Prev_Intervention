

///////////////////////////////////////////////////////////
//  Intervention.cpp                                     //
//  hivmodelzimbabwe                                     //
//                                                       //
//  Created by Mikaela Smit on 12/01/2018.               //
//  Copyright © 2018 Mikaela Smit. All rights reserved.  //
//  File for executing interventions                     //
//                                                       //
///////////////////////////////////////////////////////////

#include "Intervention.hpp"
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
#include "Intervention.hpp"

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
extern int int_HPVvaccination;
extern int int_CVDIntervention;

// Function that contains all interventions to be rolled out - adjust as needed
void EventStartIntervention(person *MyPointerToPerson){
    
    
    // Rolling out HPV vaccination
    if (int_HPVvaccination==1)
    {
        cout << "We are rolling out vaccination for HPV and it is the year: " << *p_GT << endl;
    }
    
}

// Function that contains all interventions to be rolled out - adjust as needed
void EventStartCVDIntervention(person *MyPointerToPerson){
    
    
    // Rolling out HPV vaccination
    if (int_CVDIntervention==1)
    {
        cout << "We are rolling out vaccination for HPV and it is the year: " << *p_GT << endl;
    }
    
}

void EventMyHPVVaccination(person *MyPointerToPerson){
    
    if (MyPointerToPerson->Alive == 1){
        
        MyPointerToPerson->HPVvaccination_status=1;
        MyPointerToPerson->HPVvaccination_date=*p_GT;
        
    }
}

