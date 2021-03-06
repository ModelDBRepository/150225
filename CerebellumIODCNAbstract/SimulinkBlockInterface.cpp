/***************************************************************************
 *                           SimulinkBlockInterface.cpp                    *
 *                           -------------------                           *
 * copyright            : (C) 2011 by Jesus Garrido                        *
 *                                                                         *
 * email                : jgarrido@atc.ugr.es                              *
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <cstdlib>

#include "./SimulinkBlockInterface.h"

using namespace std;

// Define input parameters
#define PARAMCONFIG ssGetSFcnParam(S,0) 	// Learning setting file
#define PARAMJOINTS ssGetSFcnParam(S,1) 	// Number of joints
#define PARAMSTATES ssGetSFcnParam(S,2) 	// Number of input states
#define PARAMWFILE ssGetSFcnParam(S,3)	// pfs-PC weight file

#define IODCNWEIGHT 0

#define RANDINTERVAL(A,B) (rand()/double(RAND_MAX))*(B-A)+A

#define E 2.71828182845904523536028747135266249775724709369995

SimulinkBlockInterface::SimulinkBlockInterface(): PCTable(0), PCActivity(0), MFActivity(0),IOActivity(0), DCNActivity(0), PCWeights(0),MFWeights(0),KPWeights(0),IOWeights(0), PFPCLearning(0), MFDCNLearning(0),KPLearning(0),IODCNLearning(0), PCDCNLearning(0), NumberOfJoints(0), NumberOfStates(0), WeightNumber(0) {

}

SimulinkBlockInterface::~SimulinkBlockInterface() {
	if (this->PCTable!=0){
		for (unsigned int i=0; i<this->NumberOfStates; ++i){
			delete [] this->PCTable[i];
		}
		delete [] this->PCTable;
		this->PCTable = 0;
	}

	if (this->PCActivity!=0){
		delete [] this->PCActivity;
		this->PCActivity=0;
	}

	if (this->MFActivity!=0){
		delete [] this->MFActivity;
		this->MFActivity=0;
	}

	if (this->DCNActivity!=0){
		delete [] this->DCNActivity;
		this->DCNActivity=0;
	}
	
	if (this->KPWeights!=0){
		delete [] this->KPWeights;
		this->KPWeights=0;
	}

	if (this->IOActivity!=0){
		delete [] this->IOActivity;
		this->IOActivity=0;
	}

	if (this->MFWeights!=0){
		delete [] this->MFWeights;
		this->MFWeights=0;
	}
	
	if (this->PCWeights!=0){
		delete [] this->PCWeights;
		this->PCWeights=0;
	}
	if (this->IOWeights!=0){
		delete [] this->IOWeights;
		this->IOWeights=0;
	}

	if (this->PFPCLearning!=0){
		delete [] this->PFPCLearning;
		this->PFPCLearning = 0;
	}

	if (this->MFDCNLearning!=0){
		delete [] this->MFDCNLearning;
		this->MFDCNLearning = 0;
	}
	
	if (this->KPLearning!=0){
		delete [] this->KPLearning;
		this->KPLearning = 0;
	}
	if (this->IODCNLearning!=0){
		delete [] this->IODCNLearning;
		this->IODCNLearning = 0;
	}
	if (this->PCDCNLearning!=0){
		delete [] this->PCDCNLearning;
		this->PCDCNLearning = 0;
	}

}

void SimulinkBlockInterface::InitializeSimulation(SimStruct *S){
	// Initialize the Simulation Object
	// Get the inputs.
	char ConfigurationFile[128];
	mxGetString(PARAMCONFIG, ConfigurationFile, 128);

	char WFile[128];
	mxGetString(PARAMWFILE, WFile, 128);

	const double InitialMFDCNWeights [] = {20, 20, 20, 20, 20, 20};
	const double InitialPCDCNWeights [] = {20, 20, 20, 20, 20, 20};
	const double InitialIODCNWeights [] = {1, 1, 1, 1, 1, 1};
	const double InitialKPWeights [] = {1, 1, 1, 1, 1, 1};


	DTypeId  dtype4 = ssGetDTypeIdFromMxArray(PARAMJOINTS);
	if (!(dtype4 == SS_UINT8 || dtype4 == SS_UINT16 || dtype4 == SS_UINT32)){
		real_T * InputJoints = (real_T *)mxGetData(PARAMJOINTS);
		unsigned int NumberOfElements = (unsigned int) mxGetNumberOfElements(PARAMJOINTS);

		if (NumberOfElements>1){
			ssSetErrorStatus(S, "Invalid joint number - Non-scalar value");
			return;
		}
		
		if ((int)InputJoints[0] != InputJoints[0] || InputJoints[0]<0){
			ssSetErrorStatus(S, "Invalid joint number - Non-integer value");
			return;
		}

		this->NumberOfJoints = (int) InputJoints[0];
		
	}
    
    if (this->NumberOfJoints<=0){
		ssSetErrorStatus(S, "Number of joints must be greater than 0");
		return;
	}
	
	// 3nd parameter -> State number.
    dtype4 = ssGetDTypeIdFromMxArray(PARAMSTATES);
	if (!(dtype4 == SS_UINT8 || dtype4 == SS_UINT16 || dtype4 == SS_UINT32)){
		real_T * InputStates = (real_T *)mxGetData(PARAMSTATES);
		unsigned int NumberOfElements = (unsigned int) mxGetNumberOfElements(PARAMSTATES);

		if (NumberOfElements>1){
			ssSetErrorStatus(S, "Invalid state number - Non-scalar value");
			return;
		}
		
		if ((int)InputStates[0] != InputStates[0] || InputStates[0]<0){
			ssSetErrorStatus(S, "Invalid state number - Non-integer value");
			return;
		}

		this->NumberOfStates = (int) InputStates[0];
	}
	
	if (this->NumberOfStates<=0){
		ssSetErrorStatus(S, "Number of states must be greater than 0");
		return;
	}

	// Read learning rule configuration file
	this->WeightFile = WFile;

	this->PFPCLearning = (LearningRule *) new LearningRule [this->NumberOfJoints];
	if (!this->PFPCLearning){
		ssSetErrorStatus(S, "Error allocating PF-PC Learning");
		return;
	}

	this->MFDCNLearning = (LearningRule *) new LearningRule [this->NumberOfJoints];
	if (!this->MFDCNLearning){
		ssSetErrorStatus(S, "Error allocating MF-DCN Learning");
		return;
	}
	this->KPLearning = (LearningRule *) new LearningRule [this->NumberOfJoints];
	if (!this->KPLearning){
		ssSetErrorStatus(S, "Error allocating KP Learning");
		return;
	}
	this->IODCNLearning = (LearningRule *) new LearningRule [this->NumberOfJoints];
	if (!this->IODCNLearning){
		ssSetErrorStatus(S, "Error allocating IO-DCN Learning");
		return;
	}
	this->PCDCNLearning = (LearningRule *) new LearningRule [this->NumberOfJoints];
	if (!this->PCDCNLearning){
		ssSetErrorStatus(S, "Error allocating PC-DCN Learning");
		return;
	}

	if (!this->ReadLearningConfig(ConfigurationFile)){
		ssSetErrorStatus(S, "File error in loading configuration file");
		return;
	}


	// Initialize table of states and joints
	this->PCTable = (double **) new double* [this->NumberOfStates];
	if (!this->PCTable){
		ssSetErrorStatus(S, "Error allocating PC Table memory");
		return;
	}

	for (unsigned int i=0; i<NumberOfStates; ++i){
		this->PCTable[i] = (double *) new double [this->NumberOfJoints];
		if (!this->PCTable[i]){
			ssSetErrorStatus(S, "Error allocating PC Table memory");
			return;
		}

		for (unsigned int j=0; j<this->NumberOfJoints; ++j){
			this->PCTable[i][j] = 1;
			//this->PCTable[i][j] = RANDINTERVAL(0,1);
		}
	}

	this->PCActivity = (double *) new double [this->NumberOfJoints];
	if (!this->PCActivity){
		ssSetErrorStatus(S, "Error allocating PC activity vector");
		return;
	}

	for (unsigned int i=0; i<this->NumberOfJoints; ++i){
		this->PCActivity[i] = 0;
	}

	this->MFActivity = (double *) new double [this->NumberOfJoints];
	if (!this->MFActivity){
		ssSetErrorStatus(S, "Error allocating MF activity vector");
		return;
	}

	for (unsigned int i=0; i<this->NumberOfJoints; ++i){
		this->MFActivity[i] = 0;
	}
	
	this->IOActivity = (double *) new double [this->NumberOfJoints];
	if (!this->IOActivity){
		ssSetErrorStatus(S, "Error allocating IO activity vector");
		return;
	}

	for (unsigned int i=0; i<this->NumberOfJoints; ++i){
		this->IOActivity[i] = 0;
	}

	this->DCNActivity = (double *) new double [this->NumberOfJoints];
	if (!this->DCNActivity){
		ssSetErrorStatus(S, "Error allocating DCN activity vector");
		return;
	}

	for (unsigned int i=0; i<this->NumberOfJoints; ++i){
		this->DCNActivity[i] = 0;
	}

	this->MFWeights = (double *) new double [this->NumberOfJoints];
	if (!this->MFWeights){
		ssSetErrorStatus(S, "Error allocating MF weight vector");
		return;
	}

	for (unsigned int i=0; i<this->NumberOfJoints; ++i){
		this->MFWeights[i] = InitialMFDCNWeights[i];
		//this->MFWeights[i] = RANDINTERVAL(0,20);
	}
	
	this->KPWeights = (double *) new double [this->NumberOfJoints];
	if (!this->KPWeights){
		ssSetErrorStatus(S, "Error allocating KP weight vector");
		return;
	}

	for (unsigned int i=0; i<this->NumberOfJoints; ++i){
		this->KPWeights[i] = InitialKPWeights[i];
		//this->MFWeights[i] = RANDINTERVAL(0,20);
	}

	this->IOWeights = (double *) new double [this->NumberOfJoints];
	if (!this->IOWeights){
		ssSetErrorStatus(S, "Error allocating IO weight vector");
		return;
	}

	for (unsigned int i=0; i<this->NumberOfJoints; ++i){
		this->IOWeights[i] = InitialIODCNWeights[i];
		//this->IOWeights[i] = RANDINTERVAL(0,20);
	}

	this->PCWeights = (double *) new double [this->NumberOfJoints];
	if (!this->PCWeights){
		ssSetErrorStatus(S, "Error allocating PC weight vector");
		return;
	}

	for (unsigned int i=0; i<this->NumberOfJoints; ++i){
		this->PCWeights[i] = InitialPCDCNWeights[i];
		//this->PCWeights[i] = RANDINTERVAL(0,20);
	}
}

void SimulinkBlockInterface::SimulateStep(SimStruct *S, int_T tid){

	if (ssIsSampleHit(S,0,tid)){

		// Get MF activity
		InputPtrsType	u = ssGetInputPortSignalPtrs(S,0);
		InputRealPtrsType uPtrs = (InputRealPtrsType) u;

		double * MFActiv = (double *) new double [this->NumberOfJoints];
		
		for (unsigned int i=0; i<this->NumberOfJoints; ++i){
			real_T value = *uPtrs[i];
			MFActiv[i] = value;
			//ssPrintf("Input signal %i: Value %i\n",i,value);
		}

		// Get IO activity
		u = ssGetInputPortSignalPtrs(S,1);
		uPtrs = (InputRealPtrsType) u;

		double * IOActiv = (double *) new double [this->NumberOfJoints];
		
		for (unsigned int i=0; i<this->NumberOfJoints; ++i){
			real_T value = *uPtrs[i];
			IOActiv[i] = value;
			//ssPrintf("Input signal %i: Value %i\n",i,value);
		}
		
		// Get current state
		u = ssGetInputPortSignalPtrs(S,2);
		InputInt16PtrsType  uPtrs2 = (InputInt16PtrsType)u;

		
		int CurrentState = (int) *uPtrs2[0];

		if (CurrentState == 0){
			if (!this->SaveWeights(this->WeightFile,this->WeightNumber)){
				ssSetErrorStatus(S, "Error saving GR-PC weight matrix");
			}
			this->WeightNumber++;
		} else {
			// Learning at PCTable
			for (unsigned int i=0; i<this->NumberOfJoints; ++i){
				this->PCTable[(CurrentState-1)%this->NumberOfStates][i] += 
					this->PFPCLearning[i].LTPStep/pow((IOActiv[i] + 1),1000)-this->PFPCLearning[i].LTDStep*IOActiv[i];

				if (this->PCTable[(CurrentState-1)%this->NumberOfStates][i] < 0){
					this->PCTable[(CurrentState-1)%this->NumberOfStates][i] = 0;
				} else if (this->PCTable[(CurrentState-1)%this->NumberOfStates][i] > 1){
					this->PCTable[(CurrentState-1)%this->NumberOfStates][i] = 1;
				}

				// Meta-learning at parallel fibers
				
				this->PFPCLearning[i].LTDStep = this->PFPCLearning[i].MaxLTD;
				
				this->PFPCLearning[i].LTPStep = this->PFPCLearning[i].MaxLTP;
				

			}
		}

		// Learning at MF-DCN
		for (unsigned int i=0; i<this->NumberOfJoints; ++i){

			this->MFWeights[i] += this->MFDCNLearning[i].LTPStep/pow((PCActivity[i] + 1),1000)-this->MFDCNLearning[i].LTDStep*PCActivity[i];

			if (this->MFWeights[i] < 0){
				this->MFWeights[i] = 0;
			}
		}

		// Learning at IO-DCN
		
		for (unsigned int i=0; i<this->NumberOfJoints; ++i){

			this->IOWeights[i] += -this-> IODCNLearning[i].LTPStep/pow((PCActivity[i] + 1),1000)+this->IODCNLearning[i].LTDStep*PCActivity[i];

			if (this->IOWeights[i] < 0){
				this->IOWeights[i] = 0;
			}
		}
		
		// Learning at KP-IO (autoscale the error)
		
		for (unsigned int i=0; i<this->NumberOfJoints; ++i){

			
			this->KPWeights[i] +=-this->KPLearning[i].LTPStep/pow((IOActiv[i] + 1),1000)+this->KPLearning[i].LTDStep*IOActiv[i];

			if (this->KPWeights[i] > 1500){
				this->KPWeights[i] = 1500;
			}
			
			if (this->KPWeights[i] < 0){
				this->KPWeights[i] = 0;
			}
		}

		// Learning at PC-DCN
		for (unsigned int i=0; i<this->NumberOfJoints; ++i){
				this->PCWeights[i] += this->PCDCNLearning[i].LTPStep*pow(PCActivity[i],1000)*
				(1.-1./(pow(this->DCNActivity[i]+1.,1000)))
				- this->PCDCNLearning[i].LTDStep*(1-PCActivity[i]);

			if (this->PCWeights[i] < 0){
				this->PCWeights[i] = 0;
			}			
		}


		delete [] MFActiv;
		delete [] IOActiv;
	
	}
}

void SimulinkBlockInterface::AssignOutputs(SimStruct *S){
	
	// Get MF activity
	InputPtrsType	u = ssGetInputPortSignalPtrs(S,0);
	InputRealPtrsType uPtrs = (InputRealPtrsType) u;

	double * MFActiv = (double *) new double [this->NumberOfJoints];
	
	for (unsigned int i=0; i<this->NumberOfJoints; ++i){
		real_T value = *uPtrs[i];
		MFActiv[i] = value;
		//ssPrintf("Input signal %i: Value %i\n",i,value);
	}

	// Get IO activity
	InputPtrsType u11 = ssGetInputPortSignalPtrs(S,1);
	InputRealPtrsType uPtrs11 = (InputRealPtrsType) u11;

	double * IOActiv = (double *) new double [this->NumberOfJoints];
	
	for (unsigned int i=0; i<this->NumberOfJoints; ++i){
		real_T value1 = *uPtrs11[i];
		IOActiv[i] = value1;
		//ssPrintf("Input signal %i: Value %i\n",i,value);
	}
	
	// Get current state
	u = ssGetInputPortSignalPtrs(S,2);
	InputInt16PtrsType  uPtrs2 = (InputInt16PtrsType)u;

	int CurrentState = (int) *uPtrs2[0];

	for (unsigned int i=0; i<this->NumberOfJoints; ++i){
		this->PCActivity[i] = this->PCTable[CurrentState][i];
		this->MFActivity[i] = MFActiv[i];
		this->IOActivity[i] = IOActiv[i];
		
		
		
			this->DCNActivity[i] = this->MFActivity[i] * this->MFWeights[i] - this->PCActivity[i] * this->PCWeights[i] + this->IOActivity[i]*this->KPWeights[i]*this->IOWeights[i];//*1/pow(E,this->MFWeights[i]/pow(InitialIODCNWeights[i],2));//IOActivity[i]*IODCNWEIGHT
			
				
		
		if (this->DCNActivity[i]<0){
			this->DCNActivity[i]=0;
			
		}
	}
	
	// Get DCN Outputs
	real_T * u1 = (real_T *) ssGetOutputPortSignal(S,0);
	real_T * u2 = (real_T *) ssGetOutputPortSignal(S,1);
	real_T * u3 = (real_T *) ssGetOutputPortSignal(S,2);
	real_T * u4 = (real_T *) ssGetOutputPortSignal(S,3);
	real_T * u5 = (real_T *) ssGetOutputPortSignal(S,4);
	real_T * u6 = (real_T *) ssGetOutputPortSignal(S,5);


	for (unsigned int i=0; i<NumberOfJoints; ++i){
		u1[i] = (real_T) this->DCNActivity[i];
		u2[i] = (real_T) this->PCActivity[i];
		u3[i] = (real_T) this->MFWeights[i];
		u4[i] = (real_T) this->PCWeights[i];
		u5[i] = (real_T) this->IOWeights[i];
		u6[i] = (real_T) this->KPWeights[i];
	}

	delete [] MFActiv;
	delete [] IOActiv;
	
}

bool SimulinkBlockInterface::ReadLearningConfig(string ConfigFile){
	
	string line;

	ifstream myfile (ConfigFile.c_str());
  
	if (myfile.is_open()){

		// Load PF-PC meta-learning rule configuration
		if (myfile.good()){
			double MaxLTD, MinLTD, MaxLTP, MinLTP, MinThreshold;
			myfile >> MaxLTD >> MinLTD >> MaxLTP >> MinLTP >> MinThreshold;
			for (unsigned int i=0; i<this->NumberOfJoints; ++i){
				this->PFPCLearning[i].MaxLTD = MaxLTD;
				this->PFPCLearning[i].MinLTD = MinLTD;
				this->PFPCLearning[i].MaxLTP = MaxLTP;
				this->PFPCLearning[i].MinLTP = MinLTP;
				this->PFPCLearning[i].MinThreshold = MinThreshold;
			}
		} else {
			myfile.close();
			return false;
		}

		// Load MF-DCN learning rule configuration
		if (myfile.good()){
			double LTDStep, LTPStep, Threshold;
			myfile >> LTDStep >> LTPStep >> Threshold;
			for (unsigned int i=0; i<this->NumberOfJoints; ++i){
				this->MFDCNLearning[i].LTDStep = LTDStep;
				this->MFDCNLearning[i].LTPStep = LTPStep;
				this->MFDCNLearning[i].Threshold = Threshold;
			}
		}  else {
			myfile.close();
			return false;
		}
		
	// Load KP learning rule configuration
		if (myfile.good()){
			double LTDStep, LTPStep, Threshold;
			myfile >> LTDStep >> LTPStep >> Threshold;
			for (unsigned int i=0; i<this->NumberOfJoints; ++i){
				this->KPLearning[i].LTDStep = LTDStep;
				this->KPLearning[i].LTPStep = LTPStep;
				this->KPLearning[i].Threshold = Threshold;
			}
		}  else {
			myfile.close();
			return false;
		}
		
		// Load IO-DCN learning rule configuration
		if (myfile.good()){
			double LTDStep, LTPStep, Threshold;
			myfile >> LTDStep >> LTPStep >> Threshold;
			for (unsigned int i=0; i<this->NumberOfJoints; ++i){
				this->IODCNLearning[i].LTDStep = LTDStep;
				this->IODCNLearning[i].LTPStep = LTPStep;
				this->IODCNLearning[i].Threshold = Threshold;
			}
		}  else {
			myfile.close();
			return false;
		}

		// Load PC-DCN learning rule configuration
		if (myfile.good()){
			double LTDStep, LTPStep, Threshold;
			myfile >> LTDStep >> LTPStep >> Threshold;
			for (unsigned int i=0; i<this->NumberOfJoints; ++i){
				this->PCDCNLearning[i].LTDStep = LTDStep;
				this->PCDCNLearning[i].LTPStep = LTPStep;
				this->PCDCNLearning[i].Threshold = Threshold;
			}
		} else {
			myfile.close();
			return false;
		}
	} else {
		return false;
	}
	myfile.close();
    return true;
}

bool SimulinkBlockInterface::SaveWeights(string ConfigFile, unsigned int iteration){
	
	string Name = ConfigFile;

	char* str = new char[30];
    sprintf(str, "%.4u", iteration );    

	Name = Name.insert(Name.find_last_of('.'),string(str));		
	
	ofstream myfile (Name.c_str());
  
	if (myfile.is_open()){

		unsigned int i=0;

		while (myfile.good() && i<this->NumberOfStates){
			for (unsigned int j=0; j<this->NumberOfJoints && myfile.good(); ++j){
				myfile << this->PCTable[i][j] << "\t";
			}

			if (myfile.good()){
				myfile << endl;
			}

			i++;
		}
	}

	myfile.close();

	delete [] str;
    
	return myfile.good();
}

