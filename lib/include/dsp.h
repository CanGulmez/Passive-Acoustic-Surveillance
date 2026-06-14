/**
 ******************************************************************************
 * @file 	dsp.h
 * @author 	Can Gulmez
 * @brief 	Main header of Digital Signal Processing.
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

#ifndef DSP_H
#define DSP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Standard libraries */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <time.h>

/* GNU scientific library */

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>

/* GUI and plot libraries */

#include <gtk/gtk.h>
#include <cairo/cairo.h>
 
/* User-defined constants */

#define DATA_SIZE					1024
#define MAX_MICS					24 
#define MAX_SOURCES        	8  
#define SOUND_SPEED        	343.0    // m/s

#define DEG(rad)           	((rad) * 180.0 / M_PI)
#define RAD(deg)           	((deg) * M_PI / 180.0)

#define PLOT_ID					"com.example.dsp"
#define PLOT_MARGIN				40		/* pixel */
#define PLOT_GRID					20		/* pixel */

/*****************************************************************************/
/*****************************************************************************/

/* User-defined data types */

typedef unsigned int len_t;
typedef int index_t;
typedef unsigned char mic_t;
typedef unsigned char src_t;
typedef const char *str_t;

/*****************************************************************************/
/*****************************************************************************/

/* User-defined enumerations */

typedef enum _DspStatus
{
	DSP_SUCCESS = 1,			/* success */
	DSP_ERR_NULL_PTR,			/* invalid pointer  */
	DSP_ERR_LENGTH,			/* invalid length */
	DSP_ERR_SAMPLE,			/* invalid pointer || invalid length */
	DSP_ERR_MISMATCH,			/* sample length mismatch */
	DSP_ERR_INDEX,				/* invalid index */
	DSP_ERR_FIR_FILTER,		/* invalid fir filter type */
	DSP_ERR_IIR_FILTER,		/* invalid iir filter type */
	DSP_ERR_PARAM				/* invalid function parameter */
} DspStatus;

typedef enum _DspColor
{
	DSP_COLOR_BLACK = 1,
	DSP_COLOR_RED,
	DSP_COLOR_GREEN,
	DSP_COLOR_BLUE,
	DSP_COLOR_YELLOW,
	DSP_COLOR_MAGENTA,
	DSP_COLOR_CYAN,
	DSP_COLOR_ORANGE,
	DSP_COLOR_PINK,
	DSP_COLOR_BROWN,
	DSP_COLOR_PURPLE,
	DSP_COLOR_SILVER,
	DSP_COLOR_GOLD
} DspColor;

typedef enum _DspFirFilter
{
	DSP_FIR_FILTER_LOW_PASS = 1,
	DSP_FIR_FILTER_HIGH_PASS
} DspFirFilter;

typedef enum _DspIirFilter
{
	DSP_IIR_FILTER_LOW_PASS = 1,
	DSP_IIR_FILTER_HIGH_PASS,
	DSP_IIR_FILTER_BAND_PASS,
	DSP_IIR_FILTER_BAND_STOP
} DspIirFilter;

/*****************************************************************************/
/*****************************************************************************/

/* User-defined structures */

typedef struct _DspTime
{
	len_t length;
	double data[DATA_SIZE];
} DspTime;

typedef struct _DspFreq
{
	len_t length;
	double data[DATA_SIZE][2];
} DspFreq;

typedef struct _DspArrival
{
	mic_t mics;						/* mics in phased array */
	double radius;                  /* radius in meters */
	double freq;                    /* signal frequency in Hz */
	src_t sources;					/* sound sources */
	DspTime *samples[MAX_MICS];		
} DspArrival;

typedef struct _DspBeamform
{
	mic_t mics;						/* mics in phased array */
	double fs;                     	/* sampling frequency in Hz */
	double radius;                  /* radius in meter */
	double theta;                   /* arrival angle in degrees */
	DspTime *samples[MAX_MICS];
} DspBeamform;

typedef struct _DspPlot
{
	str_t title;					/* title of plot */
	double width;					/* sample data line width */
	DspColor color;					/* sample data line color */
	DspTime *sample;				/* sample to be plotted */
} DspPlot;

/*****************************************************************************/
/*****************************************************************************/

#define IS_LENGTH(length) ((length) >= 1 && (length) <= DATA_SIZE)
#define IS_MISMATCH(fsample, ssample) ((fsample)->length != (ssample)->length)
#define IS_INDEX(sample, index) ((index) >= 0 && (index) < (sample)->length)

#define IS_SAMPLE(sample) ((sample) != NULL && IS_LENGTH((sample)->length))

#define IS_FIR_FILTER(filter) ((filter) == DSP_FIR_FILTER_LOW_PASS || 	\
							   (filter) == DSP_FIR_FILTER_HIGH_PASS)
#define IS_IIR_FILTER(filter) ((filter) == DSP_IIR_FILTER_LOW_PASS ||  	\
							   (filter) == DSP_IIR_FILTER_HIGH_PASS || 	\
							   (filter) == DSP_IIR_FILTER_BAND_PASS || 	\
							   (filter) == DSP_IIR_FILTER_BAND_STOP)

/*****************************************************************************/
/*****************************************************************************/

/* Time-domain signal processing methods  */

DspStatus dsp_time_add(const DspTime *fsample, const DspTime *ssample, DspTime *res);
DspStatus dsp_time_sub(const DspTime *fsample, const DspTime *ssample, DspTime *res);
DspStatus dsp_time_scalar_mul(const DspTime *sample, double scalar, DspTime *res);
DspStatus dsp_time_dot_mul(const DspTime *fsample, const DspTime *ssample, DspTime *res);
DspStatus dsp_time_scalar_div(const DspTime *sample, double scalar, DspTime *res);
DspStatus dsp_time_resize(const DspTime *sample, len_t length, DspTime *res);
DspStatus dsp_time_insert(const DspTime *sample, index_t index, double data, DspTime *res);
DspStatus dsp_time_remove(const DspTime *sample, index_t index, DspTime *res);
DspStatus dsp_time_append(const DspTime *sample, double data, DspTime *res);
DspStatus dsp_time_pop(const DspTime *sample, DspTime *res);
DspStatus dsp_time_shift(const DspTime *sample, int shift, DspTime *res);
DspStatus dsp_time_delay_linear(const DspTime *sample, double delay, DspTime *res);
DspStatus dsp_time_delay_lagrange(const DspTime *sample, double delay, DspTime *res);
DspStatus dsp_time_concat(const DspTime *fsample, const DspTime *ssample, DspTime * rsample);
DspStatus dsp_time_abs(const DspTime *sample, DspTime *res);
DspStatus dsp_time_rand(len_t length, DspTime *res);
DspStatus dsp_time_randn(len_t length, DspTime *res); 
index_t dsp_time_index(const DspTime *sample, double data);
double dsp_time_max(const DspTime *sample);
double dsp_time_min(const DspTime *sample);
double dsp_time_abs_max(const DspTime *sample);
double dsp_time_abs_min(const DspTime *sample);
index_t dsp_time_argmax(const DspTime *sample);
index_t dsp_time_argmin(const DspTime *sample);
double dsp_time_mean(const DspTime *sample);
double dsp_time_stddev(const DspTime *sample);
double dsp_time_energy(const DspTime *sample);
double dsp_time_power(const DspTime *sample);
double dsp_time_rms(const DspTime *sample);
double dsp_time_crest_factor(const DspTime *sample);
double dsp_time_skewness(const DspTime *sample);
double dsp_time_kurtosis(const DspTime *sample);
double dsp_time_variance(const DspTime *sample);
double dsp_time_snr(const DspTime *sample, const DspTime *noise);
double dsp_time_entropy(const DspTime *sample, int bins);
double dsp_time_sum(const DspTime *sample);
double dsp_time_product(const DspTime *sample);
DspStatus dsp_time_scale(const DspTime *sample, double scale, DspTime *res);
DspStatus dsp_time_downsample(const DspTime *sample, int factor, DspTime *res);
DspStatus dsp_time_upsample(const DspTime *sample, int factor, DspTime *res);
DspStatus dsp_time_peaks(const DspTime *sample, double threshold, DspTime *res);
DspStatus dsp_time_clip(const DspTime *sample, double lowest, double highest, DspTime *res);
DspStatus dsp_time_convolve(const DspTime *fsample, const DspTime *ssample, DspTime *res);
DspStatus dsp_time_auto_corr(const DspTime *sample, DspTime *res);
DspStatus dsp_time_auto_corr_neg(const DspTime *sample, DspTime *res);
DspStatus dsp_time_auto_corr_pos(const DspTime *sample, DspTime *res);
DspStatus dsp_time_cross_corr(const DspTime *fsample, const DspTime *ssample, DspTime *res);
DspStatus dsp_time_cross_corr_neg(const DspTime *fsample, const DspTime *ssample, DspTime *res);
DspStatus dsp_time_cross_corr_pos(const DspTime *fsample, const DspTime *ssample, DspTime *res); 

/* frequency-domain signal processing methods */

DspStatus dsp_freq_add(const DspFreq *fsample, const DspFreq *ssample, DspFreq *res);
DspStatus dsp_freq_sub(const DspFreq *fsample, const DspFreq *ssample, DspFreq *res);
DspStatus dsp_freq_scalar_mul(const DspFreq *sample, double scalar, DspFreq *res);
DspStatus dsp_freq_dot_mul(const DspFreq *fsample, const DspFreq *ssample, DspFreq *res);
DspStatus dsp_freq_scalar_div(const DspFreq *sample, double scalar, DspFreq *res);
DspStatus dsp_freq_resize(const DspFreq *sample, len_t length, DspFreq *res);
DspStatus dsp_freq_insert(const DspFreq *sample, index_t index, double real, double imag, DspFreq *res);
DspStatus dsp_freq_remove(const DspFreq *sample, index_t index, DspFreq *res);
DspStatus dsp_freq_append(const DspFreq *sample, double real, double imag, DspFreq *res);
DspStatus dsp_freq_pop(const DspFreq *sample, DspFreq *res);
DspStatus dsp_freq_concat(const DspFreq *fsample, const DspFreq *ssample, DspFreq *res);
DspStatus dsp_freq_real(const DspFreq *sample, DspTime *res);
DspStatus dsp_freq_imag(const DspFreq *sample, DspTime *res);
DspStatus dsp_freq_magnitude(const DspFreq *sample, DspTime *res);
DspStatus dsp_freq_phase(const DspFreq *sample, DspTime *res);
double dsp_freq_psd(const DspFreq *sample, index_t index);
double dsp_freq_power(const DspFreq *sample);
double dsp_freq_spectral_centroid(const DspFreq *sample, double fs);
double dsp_freq_spectral_flatness(const DspFreq *sample);
double dsp_freq_spectral_rolloff(const DspFreq *sample, double threshold, double fs);
double dsp_freq_thd(const DspFreq *sample, double fs);

/* Plotting methods */

int dsp_plot_sample(const DspPlot *plot);

/* Signal generation methods */

DspStatus dsp_signal_normal(double mean, double stddev, len_t length, DspTime *res);
DspStatus dsp_signal_awgn(const DspTime *sample, double snr, DspTime *res);
DspStatus dsp_signal_sin(double a, double fc, double fs, double theta, len_t length, DspTime *res);
DspStatus dsp_signal_sinc(double a, double fc, double fs, len_t length, DspTime *res);
DspStatus dsp_signal_cos(double a, double fc, double fs, double theta, len_t length, DspTime *res);
DspStatus dsp_signal_impulse(double a, index_t index, len_t length, DspTime *res);
DspStatus dsp_signal_step(double a, int fsample, int ssample, len_t length, DspTime *res);
DspStatus dsp_signal_square(double a, double fc, double fs, len_t length, DspTime *res);
DspStatus dsp_signal_sawtooth(double a, double fc, double fs, len_t length, DspTime *res);
DspStatus dsp_signal_triangle(double a, double fc, double fs, len_t length, DspTime *res);

/* Windowing methods */

DspStatus dsp_window_hamming(const DspTime *sample, DspTime *res);
DspStatus dsp_window_hanning(const DspTime *sample, DspTime *res);
DspStatus dsp_window_blackman(const DspTime *sample, DspTime *res);
DspStatus dsp_window_chebyshev(const DspTime *sample, int factor, DspTime *res);
DspStatus dsp_window_kaiser(const DspTime *sample, int factor, DspTime *res);

/* Time-frequency transformation methods */

DspStatus dsp_transform_dft(const DspTime *sample, DspFreq *res);
DspStatus dsp_transform_dft_real(const DspTime *sample, DspFreq *res);
DspStatus dsp_transform_idft(const DspFreq *sample, DspTime *res);
DspStatus dsp_transform_fft(const DspTime *sample, DspFreq *res);
DspStatus dsp_transform_fft_real(const DspTime *sample, DspFreq *res);
DspStatus dsp_transform_ifft(const DspFreq *sample, DspTime *res);

/* FIR- and IIR-based filter methods */

DspStatus dsp_filter_fir(const DspTime *sample, DspFirFilter filter, double fc, double fs, int taps, DspTime *res);
DspStatus dsp_filter_fir_low_pass(const DspTime *sample, double fc, double fs, int taps, DspTime *res);
DspStatus dsp_filter_fir_high_pass(const DspTime *sample, double fc, double fs, int taps, DspTime *res);
DspStatus dsp_filter_fir_band_pass(const DspTime *sample, double fc1, double fc2, double fs, int taps, DspTime *res);
DspStatus dsp_filter_fir_band_stop(const DspTime *sample, double fc1, double fc2, double fs, int taps, DspTime *res);
DspStatus dsp_filter_iir(const DspTime *sample, DspIirFilter filter, double fc, double fs, double Q, DspTime *res);
DspStatus dsp_filter_iir_low_pass(const DspTime *sample, double fc, double fs, DspTime *res);
DspStatus dsp_filter_iir_high_pass(const DspTime *sample, double fc, double fs, DspTime *res);
DspStatus dsp_filter_iir_band_pass(const DspTime *sample, double fc, double fs, double Q, DspTime *res);
DspStatus dsp_filter_iir_band_stop(const DspTime *sample, double fc, double fs, double Q, DspTime *res);

/* Beamforming methods */

DspStatus dsp_beamform_delay_sum(const DspBeamform *beamform, DspTime *res);

/* DoA (Direction of Arrival) methods */

int dsp_arrival_music(const DspArrival *arrival);

#ifdef __cplusplus
}
#endif

#endif /* DSP */
