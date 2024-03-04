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

// Data Structures for Perceptron (Custom Predictor)
int **perceptron;
int *global_history_feat;
int perceptron_output;
unsigned int perceptron_idx_truncate;

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
  // Implement Perceptron based predictor
  ghistory = 0;
  ghistory_truncate = (1<<ghistoryBits) - 1;
  perceptron_idx_truncate = (1<<pcIndexBits) - 1;

  // Initialize One Perceptron per PC Index
  perceptron = (int **) malloc((1 << pcIndexBits)*sizeof(int *));
  for(int i = 0; i <  (1 << pcIndexBits); i++){
    perceptron[i] = (int *) malloc((ghistoryBits+1)*sizeof(int));
    for(int j = 0; j < ghistoryBits + 1; j++){
      perceptron[i][j] = 0;
    }
  }

  // Initializing Global History i.e Input Features to Perceptron
  global_history_feat = (int *) malloc(ghistoryBits*sizeof(int));
  for(int i = 0; i < ghistoryBits; i++){
    global_history_feat[i] = -1;
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
  unsigned int perceptron_idx = (pc ^ ghistory) & perceptron_idx_truncate;
  perceptron_output = 0;

  // Computing Activation of the Perceptron
  for (int i = 0; i < ghistoryBits; i++){
    perceptron_output += perceptron[perceptron_idx][i] * global_history_feat[i];
  }
  perceptron_output += perceptron[perceptron_idx][ghistoryBits]; // Bias
  
  // Making Prediction based on Activation
  if (perceptron_output >= 0){
    return TAKEN;
  }
  else return NOTTAKEN;
}

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
  unsigned int perceptron_idx = (pc ^ ghistory) & perceptron_idx_truncate;
  uint8_t pred = perceptron_output >= 0 ? TAKEN : NOTTAKEN;
  unsigned int threshold = 1.93 * ghistoryBits + 14;

  // Update Rule Presented in the Paper
  if(outcome != pred || abs(perceptron_output) <= threshold){
    for (int i = 0; i < ghistoryBits; i++){
      if (abs(perceptron[perceptron_idx][i]) < threshold){
          perceptron[perceptron_idx][i] += ((outcome==TAKEN)? 1 :-1)*global_history_feat[i];
        }
    }
      if (abs(perceptron[perceptron_idx][ghistoryBits]) < threshold)
          perceptron[perceptron_idx][ghistoryBits] += (outcome==TAKEN)? 1 :-1;
  }

  // Update the global history
  // Shift the global history to the left by 1 and add the outcome
  // And truncate the global history to the number of bits
  for (int j = ghistoryBits-1; j > 0; j--){
    global_history_feat[j] = global_history_feat[j-1];
  }
  global_history_feat[0] = (outcome==TAKEN)? 1 : -1;

  ghistory = (ghistory << 1) | outcome;
  ghistory = ghistory & ghistory_truncate;
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