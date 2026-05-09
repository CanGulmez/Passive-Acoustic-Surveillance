/**
 ******************************************************************************
 * @file 	dsp.h
 * @author 	Can GULMEZ
 * @brief 	Main header of Digital Signal Processing.
 * 
 ******************************************************************************
 * @attention
 * 
 * Copyright (c) 2026 Can GULMEZ.
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

/* Standard GNU Libraries */

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
#include <fcntl.h>
#include <sys/stat.h>

/* GNU Scientific Library */

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>

/* GUI and Plot Libraries */

#include <gtk/gtk.h>
#include <cairo/cairo.h>
 
/* User-defined Constants */

#define MAX_DATA        		4096
#define MAX_MICS           	24  
#define MAX_SOURCES        	8       
#define SOUND_SPEED        	343.0    // m/s

#define DEG(rad)           	(rad * 180.0 / M_PI)
#define RAD(deg)           	(deg * M_PI / 180.0)

#define PLOT_ID					"com.example.DSPPlot"
#define PLOT_MARGIN				40		/* pixel */
#define PLOT_GRID					20		/* pixel */
#define PLOT_WIDTH				800	/* pixel */
#define PLOT_HEIGHT				600	/* pixel */

/* User-defined Data Types */
 
typedef unsigned int len_t; 
typedef unsigned char mics_t;
typedef const char *str_t;

/* User-defined Enumerations */

typedef enum _DspColor
{
	DSP_COLOR_BLACK,
	DSP_COLOR_WHITE,
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

typedef enum _DspFilter
{
	DSP_FILTER_LOW_PASS,
	DSP_FILTER_HIGH_PASS,
	DSP_FILTER_BAND_PASS,
	DSP_FILTER_BAND_STOP
} DspFilter;

/* User-defined Structures */
 
typedef struct _DspTime
{
   len_t length;
   double data[MAX_DATA];
} DspTime;

typedef struct _DspFreq
{
   len_t length;
   double data[MAX_DATA][2];
} DspFreq;
 
typedef struct _DspBeamform
{
   mics_t mics;							/* mics in phased array */
   double fs;                     	/* sampling frequency in Hz */
   double radius;                   /* radius in meter */
   double theta;                    /* arrival angle in degrees */
   DspTime *samples[MAX_MICS];
} DspBeamform;

typedef struct _DspArrival
{
   mics_t mics;							/* mics in phased array */
   double radius;                   /* radius in meters */
   double freq;                     /* signal frequency in Hz */
   uint8_t sources;						/* sound sources */
   DspTime *samples[MAX_MICS];		
} DspArrival; 

typedef struct _DspPlot
{
	str_t title;							/* title of plot */
	DspTime *sample;						/* sample to be plotted */
	double width;							/* sample data line width */
	DspColor color;						/* sample data line color */
	int factor;								/* factor to widen the x-axis */
} DspPlot;

/**
 * Validate the `length` object. It's passed by value to functions.
 */
#define assert_length(length)														\
{																							\
	assert (length > 0 && length <= MAX_DATA);								\
}

/**
 * Validate the `sample` object. It's passed by reference to functions.
 */
#define assert_sample(sample) 													\
{																							\
	assert (sample != NULL);														\
	assert_length(sample->length);												\
}		

/**
 * Validate the `beamform` object. It's passed by reference to functions.
 */
#define assert_beamform(beamform)												\
{                               													\
   assert (beamform->mics > 1 && beamform->mics <= MAX_MICS);        \
   for (int i = 0; i < beamform->mics; i++)									\
	{																						\
		assert_sample(beamform->samples[i]); 									\
	}                         														\
}

/**
 * Validate the `arrival` object. It's passed by reference to functions.
 */
#define assert_arrival(arrival)													\
{                                 												\
   assert (arrival->mics > 0 && arrival->mics <= MAX_MICS &&         \
      arrival->sources > 0 && arrival->sources <= MAX_SOURCES);      \
   for (int i = 0; i < arrival->mics; i++)									\
	{																						\
		assert_sample(arrival->samples[i]);										\
	}                          													\
} 

/**
 * Validate the `plot` object. It's passed by reference to functions.
 */
#define assert_plot(plot)															\
{																							\
	assert(plot->title != NULL);													\
	assert_sample(plot->sample);													\
	assert(plot->factor >= 1);														\
	assert((plot->color == DSP_COLOR_BLACK) 	|| 							\
			 (plot->color == DSP_COLOR_WHITE)	||								\
			 (plot->color == DSP_COLOR_RED)		||								\
			 (plot->color == DSP_COLOR_GREEN)	||								\
			 (plot->color == DSP_COLOR_BLUE)		||								\
			 (plot->color == DSP_COLOR_YELLOW)	||								\
			 (plot->color == DSP_COLOR_MAGENTA)	||								\
			 (plot->color == DSP_COLOR_CYAN)		||								\
			 (plot->color == DSP_COLOR_ORANGE)	||								\
			 (plot->color == DSP_COLOR_PINK)		||								\
			 (plot->color == DSP_COLOR_BROWN)	||								\
			 (plot->color == DSP_COLOR_PURPLE)	||								\
			 (plot->color == DSP_COLOR_SILVER)	||								\
			 (plot->color == DSP_COLOR_GOLD));									\
	assert(plot->width >= 0.0);													\
}

/* Time Domain Signal Processing Methods  */

extern void dsp_time_add(const DspTime *fsample, const DspTime *ssample, DspTime *result);
extern void dsp_time_subtract(const DspTime *fsample, const DspTime *ssample, DspTime *result);
extern void dsp_time_scalar_mul(const DspTime *sample, double scalar, DspTime *result);
extern void dsp_time_dot_mul(const DspTime *fsample, const DspTime *ssample, DspTime *result);
extern void dsp_time_scalar_div(const DspTime *sample, double scalar, DspTime *result);
extern void dsp_time_resize(const DspTime *sample, len_t length, DspTime *result);
extern void dsp_time_insert(const DspTime *sample, int index, double data, DspTime *result);
extern void dsp_time_remove(const DspTime *sample, int index, DspTime *result);
extern void dsp_time_append(const DspTime *sample, double data, DspTime *result);
extern void dsp_time_pop(const DspTime *sample, DspTime *result);
extern void dsp_time_shift(const DspTime *sample, int shift, DspTime *result);
extern void dsp_time_delay_linear(const DspTime *sample, double delay, DspTime *result);
extern void dsp_time_delay_lagrange(const DspTime *sample, double delay, DspTime *result);
extern void dsp_time_concat(const DspTime *fsample, const DspTime *ssample, DspTime * rsample);
extern void dsp_time_abs(const DspTime *sample, DspTime *result);
extern void dsp_time_rand(len_t length, DspTime *result);
extern void dsp_time_randn(len_t length, DspTime *result); 
extern len_t dsp_time_length(const DspTime *sample);
extern double dsp_time_data(const DspTime *sample, int index);
extern int dsp_time_index(const DspTime *sample, double data);
extern double dsp_time_max(const DspTime *sample);
extern double dsp_time_min(const DspTime *sample);
extern double dsp_time_abs_max(const DspTime *sample);
extern double dsp_time_abs_min(const DspTime *sample);
extern int dsp_time_argmax(const DspTime *sample);
extern int dsp_time_argmin(const DspTime *sample);
extern double dsp_time_mean(const DspTime *sample);
extern double dsp_time_stddev(const DspTime *sample);
extern double dsp_time_energy(const DspTime *sample);
extern double dsp_time_power(const DspTime *sample);
extern double dsp_time_rms(const DspTime *sample);
extern double dsp_time_crest_factor(const DspTime *sample);
extern double dsp_time_skewness(const DspTime *sample);
extern double dsp_time_kurtosis(const DspTime *sample);
extern double dsp_time_variance(const DspTime *sample);
extern double dsp_time_snr(const DspTime *sample, const DspTime *noise);
extern double dsp_time_entropy(const DspTime *sample, int bins);
extern void dsp_time_scale(const DspTime *sample, double scale, DspTime *result);
extern void dsp_time_downsample(const DspTime *sample, int factor, DspTime *result);
extern void dsp_time_upsample(const DspTime *sample, int factor, DspTime *result);
extern void dsp_time_peaks(const DspTime *sample, double threshold, DspTime *result);
extern void dsp_time_clip(const DspTime *sample, double min_val, double max_val, DspTime *result);
extern void dsp_time_convolve(const DspTime *fsample, const DspTime *ssample, DspTime *result);
extern void dsp_time_auto_corr(const DspTime *sample, DspTime *result);
extern void dsp_time_auto_corr_neg(const DspTime *sample, DspTime *result);
extern void dsp_time_auto_corr_pos(const DspTime *sample, DspTime *result);
extern void dsp_time_cross_corr(const DspTime *fsample, const DspTime *ssample, DspTime *result);
extern void dsp_time_cross_corr_neg(const DspTime *fsample, const DspTime *ssample, DspTime *result);
extern void dsp_time_cross_corr_pos(const DspTime *fsample, const DspTime *ssample, DspTime *result); 

/* Frequency Domain Signal Processing Methods */

extern void dsp_freq_add(const DspFreq *fsample, const DspFreq *ssample, DspFreq *result);
extern void dsp_freq_subtract(const DspFreq *fsample, const DspFreq *ssample, DspFreq *result);
extern void dsp_freq_scalar_mul(const DspFreq *sample, double scalar, DspFreq *result);
extern void dsp_freq_dot_mul(const DspFreq *fsample, const DspFreq *ssample, DspFreq *result);
extern void dsp_freq_scalar_div(const DspFreq *sample, double scalar, DspFreq *result);
extern void dsp_freq_resize(const DspFreq *sample, len_t length, DspFreq *result);
extern void dsp_freq_insert(const DspFreq *sample, double real, double imag, int index, DspFreq *result);
extern void dsp_freq_remove(const DspFreq *sample, int index, DspFreq *result);
extern void dsp_freq_append(const DspFreq *sample, double real, double imag, DspFreq *result);
extern void dsp_freq_pop(const DspFreq *sample, DspFreq *result);
extern void dsp_freq_concat(const DspFreq *fsample, const DspFreq *ssample, DspFreq *result);
extern len_t dsp_freq_length(const DspFreq *sample);
extern void dsp_freq_real(const DspFreq *sample, DspTime *result);
extern void dsp_freq_imag(const DspFreq *sample, DspTime *result);
extern void dsp_freq_magnitude(const DspFreq *sample, DspTime *result);
extern void dsp_freq_phase(const DspFreq *sample, DspTime *result);
extern double dsp_freq_psd(const DspFreq *sample, int index);
extern double dsp_freq_power(const DspFreq *sample);
extern double dsp_freq_spectral_centroid(const DspFreq *sample, double fs);
extern double dsp_freq_spectral_flatness(const DspFreq *sample);
extern double dsp_freq_spectral_rolloff(const DspFreq *sample, double threshold, double fs);
extern double dsp_freq_thd(const DspFreq *sample, double fs);

/* Plotting Methods */

extern int dsp_plot_sample(const DspPlot *plot);

/* Signal Generation Methods */

extern void dsp_signal_normal(double mean, double stddev, len_t length, DspTime *result);
extern void dsp_signal_awgn(const DspTime *sample, double snr, DspTime *result);
extern void dsp_signal_sin(double A, double fc, double fs, double theta, len_t length, DspTime *result);
extern void dsp_signal_sinc(double A, double fc, double fs, double theta, len_t length, DspTime *result);
extern void dsp_signal_cos(double A, double fc, double fs, double theta, len_t length, DspTime *result);
extern void dsp_signal_impulse(int index, len_t length, DspTime *result);
extern void dsp_signal_step(double A, int fsample, int ssample, len_t length, DspTime *result);
extern void dsp_signal_square(double A, double fc, double fs, len_t length, DspTime *result);
extern void dsp_signal_sawtooth(double A, double fc, double fs, len_t length, DspTime *result);
extern void dsp_signal_triangle(double A, double fc, double fs, len_t length, DspTime *result);

/* Windowing Methods */

extern void dsp_window_hamming(const DspTime *sample, DspTime *result);
extern void dsp_window_hanning(const DspTime *sample, DspTime *result);
extern void dsp_window_blackman(const DspTime *sample, DspTime *result);
extern void dsp_window_chebyshev(const DspTime *sample, int factor, DspTime *result);
extern void dsp_window_kaiser(const DspTime *sample, int factor, DspTime *result);

/* Time-Frequency Transformation Methods */

extern void dsp_transform_dft(const DspTime *sample, DspFreq *result);
extern void dsp_transform_dft_real(const DspTime *sample, DspFreq *result);
extern void dsp_transform_idft(const DspFreq *sample, DspTime *result);
// extern void dsp_transform_fft(const DspTime *sample, DspFreq *result);
// extern void dsp_transform_ifft(const DspFreq *sample, DspTime *result);
// extern void dsp_transform_dct(const DspTime *sample, DspTime *result);
// extern void dsp_transform_idct(const DspTime *sample, DspTime *result);
// extern void dsp_transform_hilbert(const DspTime *sample, DspTime *result);

/* FIR-based Filter Methods */

extern void dsp_filter_fir_low_pass(const DspTime *sample, double fc, double fs, int taps, DspTime *result);
extern void dsp_filter_fir_high_pass(const DspTime *sample, double fc, double fs, int taps, DspTime *result);
extern void dsp_filter_fir_band_pass(const DspTime *sample, double fc1, double fc2, double fs, int taps, DspTime *result);
extern void dsp_filter_fir_band_stop(const DspTime *sample, double fc1, double fc2, double fs, int taps, DspTime *result);
extern void dsp_filter_iir(const DspTime *sample, DspFilter filter, double fc, double fs, double Q, DspTime *result);
extern void dsp_filter_iir_low_pass(const DspTime *sample, double fc, double fs, DspTime *result);
extern void dsp_filter_iir_high_pass(const DspTime *sample, double fc, double fs, DspTime *result);
extern void dsp_filter_iir_band_pass(const DspTime *sample, double fc, double fs, double Q, DspTime *result);
extern void dsp_filter_iir_band_stop(const DspTime *sample, double fc, double fs, double Q, DspTime *result);
extern void dsp_filter_dc_block(const DspTime *sample, double fc, double fs, DspTime *result);

/* Beamforming Methods */

extern void dsp_beamform_delay_sum(const DspBeamform *beamform, DspTime *result); 
// extern void dsp_beamform_mvdr(const dsp_beamform_t *beamform, double tetha, DspTime *result);

/* DoA (Direction of Arrival) Methods */

extern int dsp_arrival_music(const DspArrival *arrival);

#ifdef __cplusplus
}
#endif

#endif /* DSP */
