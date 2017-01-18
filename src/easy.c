/*
 * easy.c
 *
 *  Created on: 18.01.2017
 *      Author: hoene
 */

/**
 *
 */

struct MYSOFA_EASY* mysofa_open(const char *filename, float samplerate, int *filterlength, int *err)
{
	struct MYSOFA_EASY *easy = malloc(sizeof(struct MYSOFA_EASY));
	if(!easy) {
		*err = MYSOFA_NO_MEMORY;
		return NULL;
	}
	easy->lookup = NULL;
	easy->neighborhood = NULL;

	easy->hrtf = mysofa_load("tests/sofa_api_mo_test/Pulse.sofa", err);
	if (!easy->hrtf) {
		mysofa_close(easy);
		return NULL;
	}

	*err = mysofa_check(easy->hrtf);
	if (*err != MYSOFA_OK) {
		mysofa_close(easy);
		return NULL;
	}

	*err = mysofa_resample(easy->hrtf, samplerate);
	if (*err != MYSOFA_OK) {
		mysofa_close(easy);
		return NULL;
	}

	mysofa_loudness(easy->hrtf);

	mysofa_tocartesian(eas->hrtf);

	easy->lookup = mysofa_lookup_init(easy->hrtf);
	if (easy->lookup == NULL) {
		*err = MYSOFA_INTERNAL_ERROR;
		mysofa_close(easy);
		return NULL;
	}

	easy->neighborhood = mysofa_neighborhood_init(easy->hrtf,
			easy->lookup);

	return easy;
}

void mysofa_getfilter(struct MYSOFA_EASY* easy, double x, double y, double z,
		short *IRleft, short *IRright,
		int *delayLeft, int *delayRight)
{
	int lk = mysofa_lookup(easy->lookup, easy->find) - hrtf->SourcePosition.values;

	res = mysofa_interpolate(hrtf, hrtf->SourcePosition.values, 0, neighborhood,
			fir, delays);

	xxx
}

void mysofa_close(struct MYSOFA_EASY* easy)
{
	mysofa_neighborhood_free(easy->neighborhood);
	mysofa_lookup_free(easy->lookup);
	mysofa_free(easy->hrtf);
	free(easy);
}
