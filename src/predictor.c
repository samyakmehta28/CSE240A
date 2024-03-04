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
unsigned int ghistory_truncate;
unsigned int lhistory_truncate;
unsigned int pc_truncate;

unsigned int *BHT_g;
unsigned int *BHT_l;
unsigned int *PHT;
unsigned int *chooser;




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
  BHT_g = (unsigned int *)malloc((1 << ghistoryBits) * sizeof(unsigned int));
  for (int i = 0; i < (1 << ghistoryBits); i++)
  {
    BHT_g[i] = WN;
  }
}

void init_tournament(){
  ghistory = 0;
  ghistory_truncate = (1<<ghistoryBits) - 1;
  BHT_g = (unsigned int *)malloc((1 << ghistoryBits) * sizeof(unsigned int));
  lhistory_truncate = (1<<lhistoryBits)-1;
  BHT_l = (unsigned int *)malloc((1 << lhistoryBits) * sizeof(unsigned int));
  PHT = (unsigned int *)malloc((1 << pcIndexBits) * sizeof(unsigned int));
  chooser = (unsigned int *)malloc((1 << ghistoryBits) * sizeof(unsigned int));

  for (int i = 0; i < (1 << ghistoryBits); i++)
  {
    BHT_g[i] = WN;
  }
  for (int i = 0; i < (1 << lhistoryBits); i++)
  {
    BHT_l[i] = WN;
  }

  for (int i = 0; i < (1 << pcIndexBits); i++)
  {
    PHT[i] = 0;
  }

  for (int i = 0; i < (1 << ghistoryBits); i++)
  {
    //chooser[i]=0 means Strong local
    //chooser[i]=1 means weak local
    //chooser[i]=2 means weak global
    //chooser[i]=3 means Strong global
    chooser[i] = 2;
  }

  pc_truncate = (1<<pcIndexBits)-1;

}
void init_custom(){
  ghistory = 0;
  ghistory_truncate = (1<<ghistoryBits) - 1;
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
  unsigned int prediction = BHT_g[ind];
  uint8_t pred_outcome = prediction/2;
  //printf("%d",pred_outcome);
  return pred_outcome;
}

uint8_t prediction_tournament(uint32_t pc)
{
  //gshare
  unsigned int pred = BHT_g[ghistory];
  if(chooser[ghistory]<2){
    //local
    unsigned int PHT_ind = pc & pc_truncate;
    unsigned int lhistory = PHT[PHT_ind];
    pred = BHT_l[lhistory];
  }
  uint8_t pred_out = pred/2;
  //printf("%d",pred_out);
  return pred_out;
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
  unsigned int prediction = BHT_g[ind];
  if (outcome == TAKEN)
  {
    if(BHT_g[ind]<3){
      BHT_g[ind]=BHT_g[ind]+1;
    }
  }
  else
  {
    if(BHT_g[ind]>0){
      BHT_g[ind]=BHT_g[ind]-1;
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
  //printf(" Training ");
  unsigned int pred_g = BHT_g[ghistory];
  unsigned int PHT_ind = pc & pc_truncate;
  unsigned int lhistory = PHT[PHT_ind];
  unsigned int pred_l = BHT_l[lhistory];
  uint8_t pred_g_out = pred_g/2;
  uint8_t pred_l_out = pred_l/2;
  //printf(" Training ");
  if (pred_g_out != pred_l_out){
    if(outcome == pred_g_out && chooser[ghistory]<3){
      chooser[ghistory] = chooser[ghistory]+1;
    }
    else if(outcome == pred_l_out && chooser[ghistory]>0){
      chooser[ghistory] = chooser[ghistory]-1;
    }
  }

  if(outcome == NOTTAKEN){
    if(pred_l>0){
      BHT_l[lhistory] = pred_l - 1;
    }
    if(pred_g>0){
      BHT_g[ghistory] = pred_g - 1;
    }
  }
  else{
    if(pred_l<3){
      BHT_l[lhistory] = pred_l + 1;
    }
    if(pred_g<3){
      BHT_g[ghistory] = pred_g + 1;
    }
  }
  ghistory = (ghistory << 1) | outcome;
  ghistory = ghistory & ghistory_truncate;

  lhistory = (lhistory << 1) | outcome;
  lhistory = lhistory & lhistory_truncate;
  PHT[PHT_ind] = lhistory;
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