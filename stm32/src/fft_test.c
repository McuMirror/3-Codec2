/* ---------------------------------------------------------------------- 
* Copyright (C) 2010 ARM Limited. All rights reserved.   
*  
* $Date:        29. November 2010  
* $Revision: 	V1.0.3  
*  
* Project: 	    CMSIS DSP Library  
* Title:	    arm_fft_bin_example_f32.c		  
*  
* Description:	Example code demonstrating calculation of Max energy bin of  
*				frequency domain of input signal. 
* 
* Target Processor: Cortex-M4/Cortex-M3  
*
*
* Version 1.0.3 2010/11/29 
*    Re-organized the CMSIS folders and updated documentation. 
* 
* Version 1.0.1 2010/10/05 KK 
*    Production release and review comments incorporated.  
*
* Version 1.0.0 2010/09/20 KK
*    Production release and review comments incorporated.
* ------------------------------------------------------------------- */ 
 
/** 
 * @ingroup groupExamples 
 */ 
 
/**    
 * @defgroup FrequencyBin Frequency Bin Example    
 * 
 * \par Description
 * \par
 * Demonstrates the calculation of the maximum energy bin in the frequency 
 * domain of the input signal with the use of Complex FFT, Complex 
 * Magnitude, and Maximum functions. 
 * 
 * \par Algorithm:
 * \par
 * The input test signal contains a 10 kHz signal with uniformly distributed white noise.  
 * Calculating the FFT of the input signal will give us the maximum energy of the 
 * bin corresponding to the input frequency of 10 kHz.  
 * 
 * \par Block Diagram:
 * \image html FFTBin.gif "Block Diagram"
 * \par
 * The figure below shows the time domain signal of 10 kHz signal with 
 * uniformly distributed white noise, and the next figure shows the input
 * in the frequency domain. The bin with maximum energy corresponds to 10 kHz signal. 
 * \par
 * \image html FFTBinInput.gif "Input signal in Time domain" 
 * \image html FFTBinOutput.gif "Input signal in Frequency domain"
 *
 * \par Variables Description:
 * \par
 * \li \c testInput_f32_10khz points to the input data
 * \li \c testOutput points to the output data
 * \li \c fftSize length of FFT
 * \li \c ifftFlag flag for the selection of CFFT/CIFFT
 * \li \c doBitReverse Flag for selection of normal order or bit reversed order
 * \li \c refIndex reference index value at which maximum energy of bin ocuurs
 * \li \c testIndex calculated index value at which maximum energy of bin ocuurs
 * 
 * \par CMSIS DSP Software Library Functions Used:
 * \par
 * - arm_cfft_radix4_init_f32()
 * - arm_cfft_radix4_f32()
 * - arm_cmplx_mag_f32()
 * - arm_max_f32()
 *
 * <b> Refer  </b> 
 * \link arm_fft_bin_example_f32.c \endlink
 * 
 */ 
 
 
/** \example arm_fft_bin_example_f32.c 
  */  

     
#include "arm_math.h" 
#include "gdb_stdio.h"
#include "machdep.h"
#include "kiss_fft.h"
 
#define TEST_LENGTH_SAMPLES 1024
 
/* ------------------------------------------------------------------- 
* External Input and Output buffer Declarations for FFT Bin Example 
* ------------------------------------------------------------------- */ 
extern float32_t testInput_f32_10khz[TEST_LENGTH_SAMPLES]; 
static float32_t testOutput[TEST_LENGTH_SAMPLES/2]; 
static float32_t kiss_complex_out[TEST_LENGTH_SAMPLES]; 
static float32_t arm_fft_tmp[TEST_LENGTH_SAMPLES];
 
/* ------------------------------------------------------------------ 
* Global variables for FFT Bin Example 
* ------------------------------------------------------------------- */ 
uint32_t fftSize = TEST_LENGTH_SAMPLES/2; 
uint32_t ifftFlag = 0; 
uint32_t doBitReverse = 1; 
 
/* Reference index at which max energy of bin ocuurs */ 
uint32_t refIndex = 213, testIndex = 0; 
 
/* ---------------------------------------------------------------------- 
* Max magnitude FFT Bin test 
* ------------------------------------------------------------------- */ 
 
void SystemInit(void);

int main(void) 
{ 
   
	arm_status status; 
	arm_cfft_radix2_instance_f32 S; 
	float32_t maxValue; 
        unsigned int fft_start, kiss_fft_start;
        kiss_fft_cfg fft_fwd_cfg;

        machdep_timer_init();

        fft_fwd_cfg = kiss_fft_alloc(fftSize, 0, NULL, NULL);

	/* Initialize the CFFT/CIFFT module */  
	status = arm_cfft_radix2_init_f32(&S, fftSize, ifftFlag, doBitReverse); 	 


	for (;;) {
		kiss_fft_start = machdep_timer_sample();	
		kiss_fft(fft_fwd_cfg, (kiss_fft_cpx *)testInput_f32_10khz, 
				(kiss_fft_cpx *)kiss_complex_out);
		machdep_timer_sample_and_log(kiss_fft_start, "  kiss_fft");

		arm_cmplx_mag_f32(kiss_complex_out, testOutput, fftSize);
		arm_max_f32(testOutput, fftSize, &maxValue,
				&testIndex); 

		if (testIndex != refIndex) {
			printf("testIndex: %u (maxValue: %f), refIndex: %u\n", (unsigned)testIndex, maxValue, (unsigned)refIndex);
			status = ARM_MATH_TEST_FAILURE; 
		}
		status = ARM_MATH_SUCCESS; 

		if (status != ARM_MATH_SUCCESS) { 
			printf("kiss_fft test failed\n");
			for (;;) {
				;
			}
		} 

		/* Process the data through the CFFT/CIFFT module */ 
		fft_start = machdep_timer_sample();	
		memcpy(arm_fft_tmp, testInput_f32_10khz,
				sizeof(arm_fft_tmp));
		/* arm_fft_tmp used as In/Out parameter,
		 * modifid inplace */
		arm_cfft_radix2_f32(&S, arm_fft_tmp); 
		machdep_timer_sample_and_log(fft_start, "  cmsis-dsp.fft");     
		machdep_timer_print_logged_samples();

		/* Process the data through the Complex Magnitude Module for 
		   calculating the magnitude at each bin */ 
		arm_cmplx_mag_f32(arm_fft_tmp, testOutput, fftSize);

		/* Calculates maxValue and returns corresponding BIN value */
		arm_max_f32(testOutput, fftSize, &maxValue,
				&testIndex); 
		if (testIndex != refIndex) {
			printf("testIndex: %u (maxValue: %f), refIndex: %u\n", (unsigned)testIndex, maxValue, (unsigned)refIndex);
			status = ARM_MATH_TEST_FAILURE; 
		}
		if (status != ARM_MATH_SUCCESS) { 
			printf("cmsis-dsp.fft test failed\n");
			for (;;) {
				;
			}
		} 
	}

    return 0;
} 
 
 /** \endlink */ 
 
 
#if 0
/*
 * Dummy function to avoid compiler error
 */
void _init() { }
#endif
