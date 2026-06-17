/**
 ******************************************************************************
 * @file 	model.c
 * @author 	Can Gulmez
 * @brief 	AI model integration headers of passive acoustic surveillance.
 * 
 ******************************************************************************
 * @attention
 * 
 * Copyright (c) 2026 Can Gulmez.
 * All rights reserved.
 * 
 * This software is licensed under the MIT License.
 * 
 ******************************************************************************
 */

#include "__ui.h"

/* Global macro definitions */

#define MODEL_DATASET_PATH					"./datasets/"
#define MODEL_DATASET_SUFFIX				".csv"
#define MODEL_FIT_SCRIPT					"./scripts/acoustic_model.py"
#define MODEL_LOG_PATH						"./logs/keras-output.log"
#define MODEL_MAX_DATASET					128
#define MODEL_MAX_UNIT						1024
#define MODEL_MAX_EPOCH						100
#define MODEL_MAX_LAYER						48

/* AI model enumerations */

typedef enum _ModelLayerType
{
	MODEL_LAYER_TYPE_LSTM = 1,
	MODEL_LAYER_TYPE_GRU
} ModelLayerType;

typedef enum _ModelBatchSize
{
	MODEL_BATCH_SIZE_16 = 1,
	MODEL_BATCH_SIZE_32,
	MODEL_BATCH_SIZE_64,
	MODEL_BATCH_SIZE_128,
	MODEL_BATCH_SIZE_256,
	MODEL_BATCH_SIZE_512
} ModelBatchSize;

typedef enum _ModelEarlyStop
{
	MODEL_EARLY_STOP_TRUE = 1,
	MODEL_EARLY_STOP_FALSE
} ModelEarlyStop;

typedef enum _ModelButton
{
	MODEL_BUTTON_FIT = 1,
	MODEL_BUTTON_ABORT,
	MODEL_BUTTON_EVALUATE,
	MODEL_BUTTON_PREDICT
} ModelButton;

/* AI model shared widgets and variables */

extern char *modelDatasets[MODEL_MAX_DATASET];
extern guint modelFitPid;
extern char *modelDataset;
extern char *modelKerasLog;
extern guint modelTimeout;
extern GtkWidget *modelTextView;
extern GtkTextBuffer *modelTextBuffer;
extern ModelLayerType modelLayerType;
extern guint modelLayerNumber;
extern guint modelUnits;
extern guint modelEpochs;
extern gdouble modelDropout;
extern ModelBatchSize modelBatchSize;
extern ModelEarlyStop modelEarlyStop;
extern char *modelOutputName;
extern ModelButton modelButton;

/* AI Model function prototypes */

void model(GtkBox *modelBox, gpointer data);
void model_group_dataset(GtkWidget *datasetGroup);
void model_group_model(GtkWidget *modelGroup);

/* AI model signal handler prototypes */

void on_dataset_selected(GObject *gobject, GParamSpec *pspec, gpointer data);
void on_layer_type_selected(GObject *gobject, GParamSpec *pspec, gpointer data);
void on_recurrent_dropout_changed(GObject *gobject, GParamSpec *pspec, gpointer data);
void on_units_changed(GObject *gobject, GParamSpec *pspec, gpointer data);
void on_batch_size_selected(GObject *gobject, GParamSpec *pspec, gpointer data);
void on_epochs_changed(GObject *gobject, GParamSpec *pspec, gpointer data);
void on_layer_number_changed(GObject *gobject, GParamSpec *pspec, gpointer data);
void on_early_stop_switched(GObject *gobject, GParamSpec *pspec, gpointer data);
void on_output_model_texted(GObject *gobject, GParamSpec *pspec, gpointer data);
void on_model_button_clicked(GtkButton *button, gpointer data);
