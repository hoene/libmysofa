/*
 * cache.c
 *
 *  Created on: 08.02.2017
 *      Author: hoene
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include "mysofa.h"

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static struct MYSOFA_CACHE_ENTRY {
	struct MYSOFA_CACHE_ENTRY *next;
	struct MYSOFA_EASY *easy;
	char *filename;
	float samplerate;
	int count;
} *cache;

struct MYSOFA_EASY *mysofa_cache_lookup(const char *filename, float samplerate)
{
	assert(filename);

	pthread_mutex_lock(&mutex);

	struct MYSOFA_CACHE_ENTRY *p = cache;
	struct MYSOFA_EASY *res = NULL;

	while(p) {
		if(samplerate == p->samplerate && !strcmp(filename, p->filename)) {
			res = p->easy;
			p->count++;
			break;
		}
		p=p->next;
	}

	pthread_mutex_unlock(&mutex);
	return res;
}

struct MYSOFA_EASY *mysofa_cache_store(struct MYSOFA_EASY *easy, const char *filename, float samplerate)
{
	assert(easy);
	assert(filename);

	pthread_mutex_lock(&mutex);

	struct MYSOFA_CACHE_ENTRY *p = cache;

	while(p) {
		if(samplerate == p->samplerate && !strcmp(filename, p->filename)) {
			pthread_mutex_unlock(&mutex);
			mysofa_close(easy);
			return p->easy;
		}
		p=p->next;
	}

	p = malloc(sizeof(struct MYSOFA_CACHE_ENTRY));
	if(p == NULL) {
		pthread_mutex_unlock(&mutex);
		return NULL;
	}
	p->next = cache;
	p->samplerate = samplerate;
	p->filename = strdup(filename);
	if(p->filename == NULL) {
		free(p);
		pthread_mutex_unlock(&mutex);
		return NULL;
	}
	p->easy = easy;
	p->count = 1;
	cache = p;
	pthread_mutex_unlock(&mutex);
	return easy;
}

void mysofa_cache_release(struct MYSOFA_EASY *easy)
{
	assert(easy);
	assert(cache);

	pthread_mutex_lock(&mutex);
	struct MYSOFA_CACHE_ENTRY **p = &cache;
	int count;

	assert(cache);
	assert(easy);

	for(count=0;;count++) {
		if((*p)->easy == easy)
			break;
		p = &((*p)->next);
		assert(*p);
	}

	if((*p)->count==1 && (count>0 || (*p)->next != NULL)) {
		struct MYSOFA_CACHE_ENTRY *gone = *p;
		free(gone->filename);
		mysofa_close(easy);
		*p = (*p)->next;
		free(gone);
	}
	else {
		(*p)->count--;
	}

	pthread_mutex_unlock(&mutex);
}

void mysofa_cache_release_all()
{
	pthread_mutex_lock(&mutex);
	struct MYSOFA_CACHE_ENTRY *p = cache;
	while(p) {
		struct MYSOFA_CACHE_ENTRY *gone = p;
		fprintf(stderr,"ra %p %s %p\n",p,gone->filename,p->easy);
		p=p->next;
		free(gone->filename);
		free(gone->easy);
//		mysofa_close2(gone->easy);
		free(gone);
	}
	cache = NULL;
	pthread_mutex_unlock(&mutex);
}
