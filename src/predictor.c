//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "Kunind Sahu && Samyak Mehta";
const char *studentID   = "A59023804 && A59024704";
const char *email       = "kusahu@ucsd.edu && sam012@ucsd.edu";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//
unsigned int ghistory;
unsigned int *lhistory;
unsigned int *BHT;
unsigned int ghistory_truncate;





//
//TODO: Add your own Branch Predictor data structures here
//


//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void init_gshare(){
  ghistory = 0;
  ghistory_truncate = (1<<ghistoryBits) - 1;
  BHT = (unsigned int *)malloc((1 << ghistoryBits) * sizeof(unsigned int));
  for (int i = 0; i < (1 << ghistoryBits); i++)
  {
    BHT[i] = WN;
  }
}

void init_tournament(){
  ghistory = 0;
  ghistory_truncate = (1<<ghistoryBits) - 1;
  BHT = (unsigned int *)malloc((1 << ghistoryBits) * sizeof(unsigned int));
  for (int i = 0; i < (1 << ghistoryBits); i++)
  {
    BHT[i] = WN;
  }
}
void init_custom(){
  ghistory = 0;
  ghistory_truncate = (1<<ghistoryBits) - 1;
  BHT = (unsigned int *)malloc((1 << ghistoryBits) * sizeof(unsigned int));
  for (int i = 0; i < (1 << ghistoryBits); i++)
  {
    BHT[i] = WN;
  }
}

void init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //
  switch (bpType)
  {
  case STATIC:
    break;
  case GSHARE:
    init_gshare();
    break;
  case TOURNAMENT:
    init_tournament();
    break;
  case CUSTOM:
    init_custom();
    break;
  default:
    break;
  }
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken

uint8_t prediction_gshare(uint32_t pc)
{
  unsigned int ind = (pc ^ ghistory) & ghistory_truncate;
  unsigned int prediction = BHT[ind];
  if (prediction == WN || prediction == SN)
  {
    return NOTTAKEN;
  }
  else
  {
    return TAKEN;
  }
}

uint8_t prediction_tournament(uint32_t pc)
{
  return TAKEN;
}

uint8_t prediction_custom(uint32_t pc)
{
  return TAKEN;
}

//
uint8_t make_prediction(uint32_t pc)
{
  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case GSHARE:
      return prediction_gshare(pc);
      break;
    case TOURNAMENT:
      return prediction_tournament(pc);
      break;
    case CUSTOM:
      return prediction_custom(pc);
      break;
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
void train_gshare(uint32_t pc, uint8_t outcome)
{
  unsigned int ind = (pc ^ ghistory) & ghistory_truncate;
  unsigned int prediction = BHT[ind];
  if (outcome == TAKEN)
  {
    if (prediction == WN)
    {
      BHT[ind] = WT;
    }
    else if (prediction == SN)
    {
      BHT[ind] = WN;
    }
    else if (prediction == WT)
    {
      BHT[ind] = ST;
    }
  }
  else
  {
    if (prediction == WN)
    {
      BHT[ind] = SN;
    }
    else if (prediction == WT)
    {
      BHT[ind] = WN;
    }
    else if (prediction == ST)
    {
      BHT[ind] = WT;
    }
  }
  // Update the global history
  // Shift the global history to the left by 1 and add the outcome
  // And truncate the global history to the number of bits
  ghistory = (ghistory << 1) | outcome;
  ghistory = ghistory & ghistory_truncate;
}

void train_tournament(uint32_t pc, uint8_t outcome)
{
  int x =  3;
}

void train_custom(uint32_t pc, uint8_t outcome)
{
  int x =  3;
}

void train_predictor(uint32_t pc, uint8_t outcome)
{
  switch (bpType){
    case STATIC:
      break;
    case GSHARE:
      train_gshare(pc, outcome);
      break;
    case TOURNAMENT:
      train_tournament(pc, outcome);
      break;
    case CUSTOM:
      train_custom(pc, outcome);
      break;
    default:
      break;
  }
}