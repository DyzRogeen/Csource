#define _CRT_SECURE_NO_WARNINGS 1
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <mmsystem.h>
#include "utils_SDL.h"

#define WHITE 255 << 16 |255 << 8 | 255
#define RED 255 << 16

typedef struct sPoint{
	int m_x;
	int m_y;
}point;

#define SAMPLE_RATE 44100      // Taux d'échantillonnage en Hz
#define BITS_PER_SAMPLE 16     // Profondeur de bits
#define CHANNELS 1             // Mono
#define DURATION 2            // Durée en secondes
#define FREQUENCY 500          // Fréquence de l'onde sinusoïdale en Hz
#define READTIME 2
#define READ_OFFSET 0.4

#define AMPLITUDE 32767        // Amplitude maximale pour 16 bits
#define SIG_LENGTH 22050
#define SECTION_TIME 0.03
#define NB_COEF 150
#define WIDTH 1280
#define HEIGHT 480

#define PI	3.14159265359
#define FILE_NAME "output.raw"
#define BUFFER_SIZE (int)(SAMPLE_RATE * READTIME * BITS_PER_SAMPLE / 8)
#define BUFF_TEST (int)(SAMPLE_RATE * (READTIME + READ_OFFSET) * BITS_PER_SAMPLE / 8)
#define NB_SECTIONS (int)((float)READTIME / SECTION_TIME + 1)

void CALLBACK waveInCallback(HWAVEIN hWaveIn, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
	if (uMsg == WIM_DATA) {
		WAVEHDR* waveHeader = (WAVEHDR*)dwParam1;

		/* Sauvegarder ou traiter les données ici */
		FILE* outputFile = fopen("output.raw", "ab");
		fseek(outputFile, 0L, SEEK_END);
		int sz = ftell(outputFile);

		int bytes_to_cut = 0;
		if (sz == 0) bytes_to_cut = READ_OFFSET * SAMPLE_RATE * BITS_PER_SAMPLE / 8;

		printf("Données capturées : taille = %d octets\n", waveHeader->dwBytesRecorded);

		//fseek(outputFile, 0L, SEEK_SET);
		if (outputFile) {
			if (waveHeader->dwBytesRecorded > bytes_to_cut)
				fwrite(waveHeader->lpData + bytes_to_cut, 1, waveHeader->dwBytesRecorded - bytes_to_cut, outputFile);
			fclose(outputFile);
		}
		return;

		/* Réutiliser le tampon pour continuer la capture */
		waveInAddBuffer(hWaveIn, waveHeader, sizeof(WAVEHDR));
	}
}

void recSound() {

	HWAVEIN hWaveIn;
	WAVEFORMATEX waveFormat;
	WAVEHDR waveHeader;
	char buffer[BUFF_TEST];

	/* Configurer le format audio */
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nChannels = CHANNELS;
	waveFormat.nSamplesPerSec = SAMPLE_RATE;
	waveFormat.wBitsPerSample = BITS_PER_SAMPLE;
	waveFormat.nBlockAlign = (CHANNELS * BITS_PER_SAMPLE) / 8;
	waveFormat.nAvgBytesPerSec = SAMPLE_RATE * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	fclose(fopen("output.raw", "w"));

	/* Ouvrir le périphérique d'entrée audio */
	if (waveInOpen(&hWaveIn, WAVE_MAPPER, &waveFormat, (DWORD_PTR)waveInCallback, 0, CALLBACK_FUNCTION) != MMSYSERR_NOERROR) {
		fprintf(stderr, "Erreur : Impossible d'ouvrir le périphérique audio\n");
		return;
	}

	/* Configurer le tampon */
	waveHeader.lpData = buffer;
	waveHeader.dwBufferLength = BUFF_TEST;
	waveHeader.dwBytesRecorded = 0;
	waveHeader.dwUser = 0;
	waveHeader.dwFlags = 0;
	waveHeader.dwLoops = 0;

	/* Préparer le tampon */
	if (waveInPrepareHeader(hWaveIn, &waveHeader, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
		fprintf(stderr, "Erreur : Impossible de préparer le tampon\n");
		waveInClose(hWaveIn);
		return;
	}

	/* Associer le tampon au périphérique audio */
	if (waveInAddBuffer(hWaveIn, &waveHeader, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
		fprintf(stderr, "Erreur : Impossible d'ajouter le tampon\n");
		waveInClose(hWaveIn);
		return;
	}

	/* Démarrer l'enregistrement */
	if (waveInStart(hWaveIn) != MMSYSERR_NOERROR) {
		fprintf(stderr, "Erreur : Impossible de démarrer l'enregistrement\n");
		waveInClose(hWaveIn);
		return;
	}

	Sleep((DURATION + READ_OFFSET) * 1000);

	/* Arrêter l'enregistrement */
	waveInStop(hWaveIn);
	waveInReset(hWaveIn);
	waveInUnprepareHeader(hWaveIn, &waveHeader, sizeof(WAVEHDR));
	waveInClose(hWaveIn);

	printf("Enregistrement terminé. Données enregistrées dans 'output.raw'.\n");
}

void playSound() {

	WAVEFORMATEX wFormat;
	WAVEHDR waveHeader;
	HWAVEOUT hWaveOut;
	size_t bytesRead;
	char buffer[BUFFER_SIZE];

	wFormat.wFormatTag = WAVE_FORMAT_PCM;
	wFormat.nSamplesPerSec = SAMPLE_RATE;
	wFormat.nChannels = CHANNELS;
	wFormat.wBitsPerSample = BITS_PER_SAMPLE;
	wFormat.nBlockAlign = (CHANNELS * BITS_PER_SAMPLE) / 8;
	wFormat.nAvgBytesPerSec = SAMPLE_RATE * wFormat.nBlockAlign;
	wFormat.cbSize = 0;

	FILE* file = fopen(FILE_NAME, "rb");

	/* Ouvrir le périphérique de sortie audio */
	if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &wFormat, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) {
		fprintf(stderr, "Erreur : Impossible d'ouvrir le périphérique audio\n");
		fclose(file);
		return -1;
	}

	while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
		/* Configurer le tampon audio */
		waveHeader.lpData = buffer;
		waveHeader.dwBufferLength = bytesRead;
		waveHeader.dwFlags = 0;
		waveHeader.dwLoops = 0;

		/* Préparer le tampon */
		if (waveOutPrepareHeader(hWaveOut, &waveHeader, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
			fprintf(stderr, "Erreur : Impossible de préparer le tampon audio\n");
			break;
		}
		/* Envoyer le tampon au périphérique audio */
		if (waveOutWrite(hWaveOut, &waveHeader, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
			fprintf(stderr, "Erreur : Impossible d'écrire dans le périphérique audio\n");
			break;
		}
		/* Attendre que le tampon soit terminé */
		while (!(waveHeader.dwFlags & WHDR_DONE)) Sleep(10);

		/* Libérer le tampon */
		waveOutUnprepareHeader(hWaveOut, &waveHeader, sizeof(WAVEHDR));

	}

	/* Nettoyer et fermer */
	fclose(file);
	waveOutClose(hWaveOut);

	printf("Lecture terminée.\n");
}

void getWave(float* wave) {
	FILE* f = fopen(FILE_NAME, "rb");

	//fseek(f, READ_OFFSET * SAMPLE_RATE * BITS_PER_SAMPLE / 8, SEEK_SET);

	int sampleNum = 0;
	float nbSample = SAMPLE_RATE * READTIME, pxl = 0, pxlVal = 0;
	float samplePerPxl = nbSample / SIG_LENGTH;

	int16_t buffer;
	while (fread(&buffer, sizeof(int16_t), 1, f) && pxl < SIG_LENGTH && sampleNum < nbSample) {
		
		if (samplePerPxl > 1) {
			pxlVal += buffer;
			if (sampleNum % (int)samplePerPxl == (int)samplePerPxl - 1) {
				wave[(int)pxl++] = pxlVal / ((int)samplePerPxl * AMPLITUDE);
				pxlVal = 0;
			}
		}
		else {
			for (int i = 0; i < 1 / samplePerPxl; i++) {
				wave[(int)pxl + i] = (float)buffer / AMPLITUDE;
				if (pxl + i >= SIG_LENGTH) break;
			}
		}

		sampleNum++;
		pxl = sampleNum / samplePerPxl;

	}

	fclose(f);
}

void playWave(float* wave) {

	float nbSample = SAMPLE_RATE * READTIME;
	int samplePerPxl = nbSample / SIG_LENGTH, buff_idx = 0;
	char buffer[BUFFER_SIZE];
	int16_t sample;
	
	WAVEFORMATEX wFormat;
	WAVEHDR waveHeader;
	HWAVEOUT hWaveOut;

	wFormat.wFormatTag = WAVE_FORMAT_PCM;
	wFormat.nSamplesPerSec = SAMPLE_RATE;
	wFormat.nChannels = CHANNELS;
	wFormat.wBitsPerSample = BITS_PER_SAMPLE;
	wFormat.nBlockAlign = (CHANNELS * BITS_PER_SAMPLE) / 8;
	wFormat.nAvgBytesPerSec = SAMPLE_RATE * wFormat.nBlockAlign;
	wFormat.cbSize = 0;

	/* Ouvrir le périphérique de sortie audio */
	if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &wFormat, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) {
		fprintf(stderr, "Erreur : Impossible d'ouvrir le périphérique audio\n");
		return -1;
	}

	for (int i = 0; i < SIG_LENGTH; i++) {
		/* Configurer le tampon audio */
		sample = (int16_t)round(wave[i] * AMPLITUDE);

		for (int j = 0; j < samplePerPxl; j++) {
			buffer[buff_idx++] = (char)(sample);
			buffer[buff_idx++] = (char)(sample >> 8);
		}

	}

	waveHeader.lpData = buffer;
	waveHeader.dwBufferLength = BUFFER_SIZE;
	waveHeader.dwFlags = 0;
	waveHeader.dwLoops = 0;

	/* Préparer le tampon */
	if (waveOutPrepareHeader(hWaveOut, &waveHeader, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
		fprintf(stderr, "Erreur : Impossible de préparer le tampon audio\n");
	}
	/* Envoyer le tampon au périphérique audio */
	if (waveOutWrite(hWaveOut, &waveHeader, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
		fprintf(stderr, "Erreur : Impossible d'écrire dans le périphérique audio\n");
	}
	/* Attendre que le tampon soit terminé */
	while (!(waveHeader.dwFlags & WHDR_DONE)) Sleep(10);

	/* Libérer le tampon */
	waveOutUnprepareHeader(hWaveOut, &waveHeader, sizeof(WAVEHDR));

	/* Nettoyer et fermer */
	waveOutClose(hWaveOut);

	printf("Lecture terminée.\n");
}

void updateWindow(SDL_Surface* window, float* wave, Uint32 color) {

	Uint32* pxls = window->pixels;
	float vals_per_pxls = (float)SIG_LENGTH / WIDTH;
	float sum = 0.0, val_prec;
	int x = 0;

	if (vals_per_pxls >= 1) {

		for (int i = 0; i < SIG_LENGTH && x < WIDTH - 1; i++) {
			sum += wave[i];
			if (i % (int)vals_per_pxls == (int)vals_per_pxls - 1) {
				if (i > vals_per_pxls) _SDL_DrawLine(window, x, (val_prec + 1) * HEIGHT / 2, x + 1, (sum / vals_per_pxls + 1) * HEIGHT / 2, color);
				val_prec = sum / vals_per_pxls;
				sum = 0.0;
				x++;
			}
		}

	}
	else {
		// TODO ou flemme
	}

	SDL_Flip(window);
}

void fenetrage(float* wave, float* waveF) {

	float T = (float)(SAMPLE_RATE * SECTION_TIME) / 2;

	for (int i = 0; i < NB_SECTIONS; i++) {
		for (int x = i * T; x < (i + 1) * T && x < SIG_LENGTH; x++) {
			waveF[x] = wave[x] * (0.54 - 0.56 * cos(2 * PI * x / T));
		}
	}

}

void fourierTransform(float* wave, float** A, float** B) {

	float *a, *b;

	float T = (float)(SAMPLE_RATE * SECTION_TIME) / 2;

	for (int i = 0; i < NB_SECTIONS; i++) {

		a = (float*)calloc(NB_COEF, sizeof(float));
		b = (float*)calloc(NB_COEF, sizeof(float));
		printf("Section %d:\n", i);

		for (int n = 0; n < NB_COEF; n++) {
			a[n] = b[n] = 0;

			for (int x = i * T; x < (i + 1) * T && x < SIG_LENGTH; x++) {
				a[n] += cos(2 * PI * n * x / T) * wave[x];
				b[n] += sin(2 * PI * n * x / T) * wave[x];
			}

			a[n] *= 2 / T;
			b[n] *= 2 / T;

			//printf("%f cos(2*PI*%d) + %f sin(2*PI*%d)\n", a[n], n, b[n], n);

		}

		A[i] = a;
		B[i] = b;

	}

	printf("Traitement terminé.\n");

}

void fourierDisplay(SDL_Surface* window, float* wave, float** A, float** B) {
	for (int i = 0; i < SIG_LENGTH; i++) wave[i] = 0;

	float* a, * b;
	float T = (float)(SAMPLE_RATE * SECTION_TIME) / 2;

	for (int i = 0; i < NB_SECTIONS; i++) {
		a = A[i];
		b = B[i];

		for (int n = 0; n < NB_COEF; n++)
			for (int x = i * T; x < (i + 1) * T && x < SIG_LENGTH; x++)
				wave[x] += a[n] * cos(2 * PI * n * x / T) + b[n] * sin(2 * PI * n * x / T);

	}

	updateWindow(window, wave, RED);
}

void jsp() {
	int16_t sample;            // Échantillon audio (16 bits signé)
	double time;               // Temps courant
	double increment;          // Incrément du temps entre chaque échantillon
	int total_samples;         // Nombre total d'échantillons

	/* Calcul du nombre total d'échantillons */
	total_samples = SAMPLE_RATE * DURATION;

	/* Calcul de l'incrément temporel */
	increment = 1.0 / SAMPLE_RATE;

	/* Ouvrir le fichier RAW pour écriture binaire */
	FILE* file = fopen(FILE_NAME, "wb");
	if (!file) {
		fprintf(stderr, "Erreur : Impossible de créer le fichier %s\n", FILE_NAME);
		return -1;
	}
	/* Générer et écrire les échantillons sinusoïdaux */
	for (int i = 0; i < total_samples; i++) {
		time = i * increment;
		sample = (int16_t)(AMPLITUDE * (sin(2.0 * PI * 2.0 * FREQUENCY * time) * cos(2.0 * PI * 5.0 * FREQUENCY * time)));
		fwrite(&sample, sizeof(int16_t), 1, file);
	}

	/* Fermer le fichier */
	fclose(file);
}

int main(int argc, char** argv) {

	SDL_Surface * window;
	int quit = 0;
	SDL_Event e;
	point mousePos;
	int toggleFourrier = 0;

	float* wave = (float*)calloc(SIG_LENGTH, sizeof(float));
	float* waveF = (float*)calloc(SIG_LENGTH, sizeof(float));
	for (int i = 0; i < SIG_LENGTH; i++) wave[i] = 0;

	atexit(SDL_Quit);
	window = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

	if (window == NULL) {
		fprintf(stderr, "ERREUR - impossible de passer en : %dx%dx%d\n>>> %s\n", 640, 480, 32, SDL_GetError());
		exit(EXIT_FAILURE);
	}

	SDL_WM_SetCaption("VonKoch", NULL);
	SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));

	getWave(wave);
	updateWindow(window, wave, WHITE);

	
	//fenetrage(wave, waveF);
	
	float* A[NB_SECTIONS];
	float* B[NB_SECTIONS];

	fourierTransform(wave, A, B);

	fourierDisplay(window, waveF, A, B);

	while (!quit)
	{
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				quit = 1;
				break;
			case SDL_KEYDOWN:
				if (e.key.keysym.sym == SDLK_ESCAPE)
					quit = 1;
				if (e.key.keysym.sym == SDLK_p) {
					playSound();
					SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));

					//getWave(wave);
					//updateWindow(window, wave, WHITE);

					//fourierTransform(wave, a, b, nbCoeff);
					//fourierDisplay(window, waveF, a, b, nbCoeff);
				}
				if (e.key.keysym.sym == SDLK_d) {
					SDL_FillRect(window, &(window->clip_rect), SDL_MapRGB(window->format, 0, 0, 0));
					updateWindow(window, wave, WHITE);

					if (toggleFourrier) updateWindow(window, waveF, RED);

					toggleFourrier = !toggleFourrier;
				}
				if (e.key.keysym.sym == SDLK_r)
					recSound();
				if (e.key.keysym.sym == SDLK_o)
					playWave(waveF);
				break;
			case SDL_MOUSEBUTTONDOWN:
				SDL_GetMouseState(&mousePos.m_x, &mousePos.m_y);
				break;
			}
		}

	}

	for (int i = 0; i < NB_SECTIONS; i++) {
		free(A[i]);
		free(B[i]);
	}

	free(wave);
	free(waveF);

	return EXIT_SUCCESS;
}