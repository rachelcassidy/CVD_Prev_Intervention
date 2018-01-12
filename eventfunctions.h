//////////////////////////////////////////////////////////
//    eventfunction.h                                  //
//    Created by Mikaela Smit on 07/11/2014.       	   //
//    Copyright (c) 2014 Mikaela Smit. All rights reserved.    //
//    These event are added to the EventQ.			   //
/////////////////////////////////////////////////////////

#include <stdio.h> 
#include "person.h"

using namespace std;


//// ---- NOTE: Think about having different types of events ----

	//// --- Demographic Events ---
	void EventMyDeathDate(person *MyPointerToPerson);			// Function to be executed upon death
	void EventBirth(person *MyPointerToPerson);					// Function to be executed when someone gives birth
	
	//// --- Calendar Events ---
	void EventTellNewYear(person *MyPointerToPerson);			// Tell us when a new year start an update calendar-related items
	
	//// --- HIV Events ---
	void EventMyHIVInfection(person *MyPointerToPerson);        // Function executed once person gets infecte with HIV
	void EventCD4change(person *MyPointerToPerson);			   // Function executed when CD4 count is due to change (go down)
    void EventARTCatSwitch(person *MyPointerToPerson);         // Function to switch people from Kids ART cat to Adult ART cat

    //// --- HPV Events ---
    void EventMyHPVInfection(person *MyPointerToPerson);        // Function executed once person gets infected with HPV
    void EventMyHPVRecovery(person *MyPointerToPerson);         // Function executed once person recovers from HPV
    void EventMyCIN1Status(person *MyPointerToPerson);          // Function executed once person progresses from HPV to CIN1
    void EventMyCIN2_3Status(person *MyPointerToPerson);        // Function executed once person progresses from CIN1 to CIN2_3
    void EventMyCISStatus(person *MyPointerToPerson);           // Function executed once person progresses from CIN2_3 to CIS
    void EventMyICCStatus(person *MyPointerToPerson);           // Function executed once person progresses from CIS to ICC

    //// --- Cervical cancer interventions ---
    void EventMyFirst_VIA_Screening(person *MyPointerToPerson);
    void EventMy_VIA_FollowUp(person *MyPointerToPerson);
    void EventMyFirst_Pap_Screening(person *MyPointerToPerson);
    void EventMyCIN1_FollowUp(person *MyPointerToPerson);
    void EventMyCIN2_Cryo_Intervention(person *MyPointerToPerson);
    void EventMyCIN2_LEEP_Intervention(person *MyPointerToPerson);

    //// --- NCD Events ---
    void EventMyHyptenDate(person *MyPointerToPerson);			// Function executed when person develops hypertension
    void EventMyHypcholDate(person *MyPointerToPerson);			// Function executed when person develops high-cholesterol
    void EventMyDepressionDate(person *MyPointerToPerson);		// Function executed when person develops depression
    void EventMyAsthmaDate(person *MyPointerToPerson);			// Function executed when person develops asthma
    void EventMyStrokeDate(person *MyPointerToPerson);			// Function executed when person develops stroke
    void EventMyMIDate(person *MyPointerToPerson);              // Function executed when person develops MI
    void EventMyDiabetesDate(person *MyPointerToPerson);        // Function executed when person develops diabetes

    /// --- Cancer events --- 
    void EventMyBreastDate(person *MyPointerToPerson);          // Function executed when person develops breast cancer
    void EventMyCervicalDate(person *MyPointerToPerson);        // Function executed when person develops Cervical cancer
    void EventMyColoDate(person *MyPointerToPerson);            // Function executed when person develops Colorectum cancer
    void EventMyLiverDate(person *MyPointerToPerson);			// Function executed when person develops Liver cancer
    void EventMyOesoDate(person *MyPointerToPerson);            // Function executed when person develops Oesophageal cancer
    void EventMyProstateDate(person *MyPointerToPerson);		// Function executed when person develops prostate cancer
    void EventMyOtherCanDate(person *MyPointerToPerson);        // Function executed when person develops other cancers

    void EventMyCKDDate(person *MyPointerToPerson);             // Function executed when person develops CKD


  
