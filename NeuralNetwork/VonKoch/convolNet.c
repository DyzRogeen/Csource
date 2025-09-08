#include "convolNet.h"

sData applyFilter(sData D, sFilter F, float(*activation)(float)) {
	float* filter, bias = F.bias;
	int filter_size = F.filter_size, stride = F.stride;
	int filter_length = filter_size * filter_size;

	int height = D.h, width = D.w;
	int filtered_data_height = (height - filter_size + 1) / stride;
	int filtered_data_width = (width - filter_size + 1) / stride;
	if (filtered_data_height <= 0 || filtered_data_width <= 0) return;
	int filtered_data_length = filtered_data_height * filtered_data_width;

	float sum, mean = 0;
	float* data = D.data;
	float* filtered_data = (float*)calloc(filtered_data_length, sizeof(float));
	for (int y = 0; y < filtered_data_height; y++)
		for (int x = 0; x < filtered_data_width; x++) {

			int slot = (x + y * width) * stride;

			sum = 0;
			filter = F.filter;
			for (int j = 0; j < filter_size; j++)
				for (int k = 0; k < filter_size; k++)
					sum += data[slot + k + j * width] * *(filter++) + bias;
			mean += filtered_data[x + y * filtered_data_width] = activation(sum);

		}

	normalize(filtered_data, filtered_data_length, mean / filtered_data_length);

	return createCNNData(filtered_data, filtered_data_height, filtered_data_width);
}

sData* applyConvolution(sData D, CNNLayer* l) {
	sFilter* filters = l->filters;
	float (*activation)(float) = l->activation;

	sData* results = (sData*)calloc(l->nbFilters, sizeof(sData));

	int i = 0;
	while (filters) {
		results[i++] = applyFilter(D, *filters, activation);
		filters = filters->next;
	}

	return results;

}

sData maxPool(sData D, int pooling_size) {
	
	int height = D.h, width = D.w;
	int pooled_data_height = height / pooling_size;
	int pooled_data_width = width / pooling_size;
	int pooled_data_length = pooled_data_height * pooled_data_width;

	int* maxIndexes = malloc(pooled_data_length * sizeof(int)), maxIndex = 0;

	float* data = D.data, maxi, dataTmp;

	float* pooled_data = (float*)calloc(pooled_data_length, sizeof(float));
	for (int x = 0; x < pooled_data_width; x++)
		for (int y = 0; y < pooled_data_height; y++) {

			int slot = (x + y * width) * pooling_size;

			maxi = -10000000000000;
			for (int j = 0; j < pooling_size; j++)
				for (int k = 0; k < pooling_size; k++) {
					dataTmp = data[slot + k + j * width];
					if (maxi < dataTmp) {
						maxi = dataTmp;
						maxIndex = k + j * pooling_size;
					}
				}
			pooled_data[x + y * pooled_data_width] = maxi;
			maxIndexes[x + y * pooled_data_width] = maxIndex;

		}

	sData result = createCNNData(pooled_data, pooled_data_height, pooled_data_width);
	result.maxIndexes = maxIndexes;
	return result;
}

void flatten(CNNLayer* l, NeuralNetwork* n) {
	l = l->last;

	int nbData = l->nbDatas, data_length;
	sData* D = l->datas;
	float* flat = (float*)calloc(n->layerList->layer->size, sizeof(float)), *tmp, *data;
	tmp = flat;

	sData d;
	for (int i = 0; i < nbData; i++) {
		d = D[i];
		data = d.data;
		data_length = d.h * d.w;
		for (int j = 0; j < data_length; j++) *(flat++) = data[j];
	}

	setInputValues(n, tmp);

	free(tmp);

}

void mapLoss(CNNLayer* L, NeuralNetwork* n) {
	L = L->last;

	layer* l = n->layerList->layer;
	float* e = l->e;
	int idx = 0;

	sData* D = L->datas, d;
	float* loss;
	int nbDatas = L->nbDatas, data_length;

	for (int i = 0; i < nbDatas; i++) {
		d = D[i];
		loss = d.L;
		data_length = d.w * d.h;

		for (int j = 0; j < data_length; j++)
			loss[j] = e[idx++];

	}

}

void retroPropagateCNN(CNNLayer* l) {
	l = l->last;

	while (l->prec) {
		if (l->type == CONVOLUTIONAL)
			retroPropagateConvolution(l);
		else if (l->type == POOLING)
			retroPropagatePooling(l);
		l = l->prec;
	}

}

void retroPropagateConvolution(CNNLayer* l) {
	CNNLayer* l_prec = l->prec;
	if (l_prec == NULL) return;

	sData* D_in = l_prec->datas, *D_out = l->datas;
	sFilter* filter, *ftmp = l->filters;
	int nbDatas_prec = l_prec->nbDatas, j;
	float (*deriv)(float) = l->deriv;

	if (l_prec->type != INITIAL)
		for (int i = 0; i < nbDatas_prec; i++) {
			filter = ftmp;
			j = 0;
			while (filter) {
				updateLoss(filter, D_in[i], D_out[j++], deriv, i == 0);
				filter = filter->next;
			}	
		}

	for (int i = 0; i < nbDatas_prec; i++) {
		filter = ftmp;
		j = 0;
		while (filter) {
			updateFilter(filter, D_in[i], D_out[j++]);
			filter = filter->next;
		}
	}

}

void retroPropagatePooling(CNNLayer* l) {
	CNNLayer* l_prec = l->prec;
	sData* D_prec = l_prec->datas, * D = l->datas, Dtmp;
	float* L_prec, *L;
	int* maxIndexes, w, h, x, y, maxIndex, pooling_size = l->pooling_size, w_prec, slot;

	for (int i = 0; i < l->nbDatas; i++) {
		Dtmp = D[i];
		L_prec = D_prec[i].L;
		L = Dtmp.L;
		maxIndexes = Dtmp.maxIndexes;
		w = Dtmp.w;
		h = Dtmp.h;
		w_prec = w * pooling_size;

		slot = 0;
		for (int j = 0; j < h; j++)
			for (int k = 0; k < w; k++) {
				maxIndex = maxIndexes[slot];
				x = maxIndex % pooling_size;
				y = maxIndex / pooling_size;
				L_prec[(k + j * w_prec) * pooling_size + x + y * w_prec] = L[slot++];
			}

	}
}

void updateFilter(sFilter* f, sData D_in, sData D_out) {
	int filter_size = f->filter_size, filter_length = filter_size * filter_size, stride = f->stride;
	float* filter = f->filter;
	float* data_in = D_in.data, *L_out = D_out.L;
	int w_in = D_in.w, h_in = D_in.h, length_out = D_out.w * D_out.h, w_bound, h_bound, slot_out;

	float a = 0.2; // Taux d'apprentissage

	float sum;
	h_bound = D_out.h; // Tenir compte de la taille du filtre
	w_bound = D_out.w;

	for (int y = 0; y < filter_size; y++) {

		for (int x = 0; x < filter_size; x++) {

			// dL/dw = somme[dz/dw * dL/dz]
			sum = 0;
			slot_out = 0;
			for (int j = y; j < h_bound; j+=stride)
				for (int i = x; i < w_bound; i+=stride)
					sum += data_in[i + j * w_in] * L_out[slot_out++];

			// w = w - a * dL/dw
			*(filter) -= a * sum;

			filter++;
		}
	}

	// Maj du biais
	sum = 0;
	for (int i = 0; i < length_out; i++) sum += *(L_out++);
	f->bias -= a * sum;
		
}

void updateLoss(sFilter* f, sData D_in, sData D_out, float(*deriv)(float), int isFirstUpdate) {
	int w_prec = D_in.w, h_prec = D_in.h, w = D_out.w, h = D_out.h, filter_size = f->filter_size, stride = f->stride, x, y, i0, j0;
	float* L_prec = D_in.L, * L = D_out.L, *filter = f->filter, *data = D_out.data, sum;

	// Pour chaque valeur de la data de la couche précédente...
	for (int y_prec = 0; y_prec < h_prec; y_prec++) {

		j0 = y_prec < filter_size ? y_prec : filter_size - 1; // Pour pas que le filtre dépasse de la data prec

		for (int x_prec = 0; x_prec < w_prec; x_prec++) {

			i0 = x_prec < filter_size ? x_prec : filter_size - 1;
			y = max(0, (y_prec - filter_size + 1)) / stride; // Coordonnées du premier y à traîter

			// On cherche le couple (y; w) dans lequel elle est impliquée
			// dL/dx = somme[dL/dz * dz/dy * dy/dx] avec
			// dL/dz => loss de la couche actuelle
			// dz/dy => dérivée de la fonction d'activation de la donnée actuelle
			// dy/dx => poids concerné du filtre (w)
			sum = 0;
			for (int j = j0; j >= 0 && y < h; j -= stride) {
				x = max(0, (x_prec - filter_size + 1)) / stride;
				for (int i = i0; i >= 0 && x < w; i -= stride) {
					sum += L[x + y * w] * deriv(data[x + y * w]) * filter[i + j * filter_size];
					x++;
				}
				y++;
			}

			if (isFirstUpdate)
				L_prec[x_prec + y_prec * w_prec] = sum;
			else
				L_prec[x_prec + y_prec * w_prec] += sum;

		}
	}

}

void convolFromIMG(SDL_Surface* win, CNNLayer* l, int RGBmode) {

	sData* D, * Dres, * Dtmp, * Dtmp2;
	int nbData, init = 1;

	if (RGBmode) {
		D = getRGBDataFromPxls(win);
		nbData = 3;
	}
	else {
		D = getDataFromPxls(win);
		nbData = 1;
	}

	l->datas = D;
	l->nbDatas = nbData;

	convol(l, RGBmode);

}

void convol(CNNLayer* l, int RGBmode) {

	sData* D = l->datas, * Dres, * Dtmp, * Dtmp2;

	// Nombre de canaux de la couche initiale
	int nbData = l->nbDatas;
	l = l->next;

	while (l) {

		switch (l->type) {
		case CONVOLUTIONAL:

			Dres = (sData*)malloc(nbData * l->nbFilters * sizeof(sData));
			Dtmp = Dtmp2 = Dres;

			for (int i = 0; i < nbData; i++) {
				Dtmp = applyConvolution(D[i], l);
				for (int j = 0; j < l->nbFilters; j++) Dres[j] = Dtmp[j];
				Dres += l->nbFilters;
			}
			nbData *= l->nbFilters;

			// Nettoyage des données si existantes
			if (l->datas != NULL) {
				Dtmp = l->datas;
				for (int i = 0; i < l->nbDatas; i++) free(Dtmp[i].data);
				free(Dtmp);
			}

			l->datas = Dtmp2;
			l->nbDatas = nbData;

			D = Dtmp2;
			//displayDatas(D, nbData, RGBmode);


			break;
		case POOLING:
			Dres = (sData*)malloc(nbData * sizeof(sData));
			for (int i = 0; i < nbData; i++) Dres[i] = maxPool(D[i], l->pooling_size);

			// Nettoyage des données si existantes
			if (l->datas != NULL) {
				for (int i = 0; i < l->nbDatas; i++) {
					free(l->datas[i].data);
					free(l->datas[i].maxIndexes);
				}
				free(l->datas);
			}

			l->datas = D = Dres;
			l->nbDatas = nbData;
			displayDatas(D, nbData, RGBmode);
			break;
		default:
			break;
		}

		l = l->next;
	}

}

void CNNStepTab(float* tab, int height, int width, CNNLayer* l, NeuralNetwork* N, int nbIterations, float* y) {

	sData* D = malloc(sizeof(sData));
	D[0] = createCNNData(tab, height, width);
	l->datas = D;
	l->nbDatas = 1;

	CNNStep(NULL, l, N, nbIterations, 0, y);
}

void CNNStep(SDL_Surface* win, CNNLayer* CNN, NeuralNetwork* N, int nbIterations, int RGBmode, float* y) {

	// TODO : prendre une autre image à chaque fois

	for (int i = 0; i < nbIterations; i++) {
		// Propagation CNN
		if (win == NULL) convol(CNN, 0);
		else convolFromIMG(win, CNN, RGBmode);
		// Applatissage vers partie fully-connected
		flatten(CNN, N);
		// Propagation Réseau Dense
		propagate(N);

		// RétroPropagation Réseau Dense
		retroPropagate(N, y);
		// Rétropropagation des pertes Réseau Dense vers CNN
		mapLoss(CNN, N);
		// Rétropropagation CNN
		retroPropagateCNN(CNN);

	}
	
	free(y);

}

void guessTab(float* tab, CNNLayer* CNN, NeuralNetwork* N) {
	convol(CNN, 0);
	flatten(CNN, N);
	propagate(N);
	maxIndex(N->lastLayer->layer->nodeVals, 1);
}

// UTILS

sData createCNNData(float* data, int height, int width) {
	sData d;
	d.data = data;
	d.L = (float*)calloc(height * width, sizeof(float));
	d.h = height;
	d.w = width;
	return d;
}

sFilter* createfilter(float* filter, int filter_size, int stride) {
	sFilter* f = (sFilter*)calloc(1, sizeof(sFilter));

	f->filter = filter;
	f->bias = 0;
	f->filter_size = filter_size;
	f->stride = stride;
	f->next = NULL;

	return f;
}

sFilter* createDynamicFilter(int filter_size, int stride) {
	int filter_length = filter_size * filter_size;
	float* filter = (float*)calloc(filter_length, sizeof(float));

	for (int i = 0; i < filter_length; i++) filter[i] = (float)(rand() % 201 - 100) / 100;

	return createfilter(filter, filter_size, stride);
}

void addFilter(CNNLayer* l, sFilter* f) {
	if (!f) return;

	f->next = l->filters;
	l->filters = f;
	l->nbFilters++;
}

CNNLayer* createInitCNN() {
	CNNLayer* l = (CNNLayer*)calloc(1, sizeof(CNNLayer));
	l->type = INITIAL;
	l->next = NULL;
	l->prec = NULL;
	l->last = l;
}

CNNLayer* createConvolutionalLayer(sFilter* f, float (*activation)(float), float (*deriv)(float)) {
	CNNLayer* l = (CNNLayer*)calloc(1, sizeof(CNNLayer));
	l->type = CONVOLUTIONAL;
	l->filters = f;
	l->nbFilters = 1;
	l->datas = NULL;
	l->next = NULL;
	l->prec = NULL;
	l->last = l;
	l->activation = activation;
	l->deriv = deriv;
	return l;
}

CNNLayer* createPoolingLayer(poolingType type, int size) {
	CNNLayer* l = (CNNLayer*)calloc(1, sizeof(CNNLayer));
	l->type = POOLING;
	l->pool_type = type;
	l->pooling_size = size;
	l->next = NULL;
	l->prec = NULL;
	l->last = l;
	return l;
}

void addCNNLayer(CNNLayer* L, CNNLayer* l) {
	if (!L) {
		L = l;
		return;
	}

	while (L->next) {
		L->last = l;
		L = L->next;
	}
	L->next = l;
	l->prec = L;
	L->last = l;
}

void freeFilters(sFilter* F) {
	if (!F) return;
	freeFilters(F->next);
	free(F->filter);
	free(F);
}

void freeCNNLayer(CNNLayer* L) {
	if (!L) return;
	freeCNNLayer(L->next);

	sData* D = L->datas;
	if (D != NULL) {
		for (int i = 0; i < L->nbFilters; i++) {
			free(D[i].data);
			free(D[i].L);
			if (L->type == POOLING) free(D[i].maxIndexes);
		}
		free(D);
	}

	if (L->type == INITIAL) return;

	freeFilters(L->filters);
	free(L);
}

float mini(float a, float b) {
	return a < b ? a : b;
}

float maxi(float a, float b) {
	return a > b ? a : b;
}

// SDL UTILS

int countNbWindows(CNNLayer* l) {
	int cnt = l->nbDatas;
	while (l) {
		cnt *= l->nbFilters;
		l = l->next;
	}
	return cnt;
}

int countFlattenSize(CNNLayer* l, int w, int h) {
	if (!l) return w * h;
	if (l->type == INITIAL) return countFlattenSize(l->next, w, h);

	CNNLayer* l_next = l->next;
	int cnt = 0;
	if (l->type == CONVOLUTIONAL) {
		sFilter* filter = l->filters;
		int minus;
		while (filter) {
			minus = filter->filter_size - 1;
			cnt += countFlattenSize(l_next, (w - minus) / filter->stride, (h - minus) / filter->stride);
			filter = filter->next;
		}
	}
	else if (l->type == POOLING)
		cnt += countFlattenSize(l_next, w / l->pooling_size, h / l->pooling_size);
	return cnt;
}

void renderSurface(SDL_Renderer* renderer, SDL_Surface* surface) {
	SDL_RenderClear(renderer);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
	SDL_DestroyTexture(texture);
}

void mapImg(SDL_Surface* win, SDL_Surface* img) {

	int w = win->w, h = win->h, bpp = img->format->BytesPerPixel;
	Uint32* pxls = win->pixels;
	Uint8* img_pxls = img->pixels;
	for (int i = 0; i < h * w; i++) {
		*(pxls++) = (0xFF000000 | img_pxls[0] << 16 | img_pxls[1] << 8 | img_pxls[2]);
		img_pxls += bpp;
	}

}

void mapDataOnSurface(SDL_Surface* s, sData dR, sData dG, sData dB) {
	int w = dR.w, h = dR.h, s_w = s->w, slot;
	int pxl_size = s_w / w;
	float* dataR = dR.data;
	float* dataG = dG.data;
	float* dataB = dB.data;
	Uint32* pxls = s->pixels, pxl;

	for (int i = 0; i < w * h; i++) {
		pxl = 0xFF000000 | (Uint8)(maxi(0, 255 * mini(1, dataR[i]))) << 16 | (Uint8)(maxi(0, 255 * mini(1, dataG[i]))) << 8 | (Uint8)(maxi(0, 255 * mini(1, dataB[i])));
		//pxl = 0xFF000000 | (Uint8)max(0, min(255, dataR[i])) << 16 | (Uint8)max(0, min(255, dataG[i])) << 8 | (Uint8)max(0, min(255, dataB[i]));
		slot = (i % w + (i / w) * s_w) * pxl_size;
		for (int y = 0; y < pxl_size; y++)
			for (int x = 0; x < pxl_size; x++)
				pxls[slot + x + y * s_w] = pxl;

	}


}

sData* getRGBDataFromPxls(SDL_Surface* win) {
	int w = win->w, h = win->h;
	int length = w * h;
	Uint8* pxl = win->pixels;

	float* dataR = (float*)calloc(length, sizeof(float));
	float* dataG = (float*)calloc(length, sizeof(float));
	float* dataB = (float*)calloc(length, sizeof(float));
	for (int i = 0; i < length; i++) {
		dataR[i] = pxl[2];
		dataG[i] = pxl[1];
		dataB[i] = pxl[0];
		pxl += 4;
	}

	sData* D = (sData*)malloc(3 * sizeof(sData));
	D[0] = createCNNData(dataR, h, w);
	D[1] = createCNNData(dataG, h, w);
	D[2] = createCNNData(dataB, h, w);

	return D;
}

sData* getDataFromPxls(SDL_Surface* win) {
	int w = win->w, h = win->h;
	int length = w * h;
	Uint8* pxl = win->pixels;

	float* data = (float*)calloc(length, sizeof(float));
	for (int i = 0; i < length; i++) {
		data[i] = (pxl[2] + pxl[1] + pxl[0]) / 3;
		pxl += 4;
	}

	sData* D = (sData*)malloc(sizeof(sData));
	*D = createCNNData(data, h, w);

	return D;
}

SDL_Surface** displayDatas(sData* D, int nbDataPerCanal, int RGBmode) {

	SDL_Surface** windows = (SDL_Surface**)calloc(nbDataPerCanal, sizeof(SDL_Surface*)), * window, * surface;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	sData dR, dG, dB;

	int forced_w = 100, forced_h;

	if (RGBmode) nbDataPerCanal /= 3;

	int dx = 0;
	for (int i = 0; i < nbDataPerCanal; i++) {
		dR = D[i];

		if (RGBmode) {
			dG = D[i + nbDataPerCanal];
			dB = D[i + 2 * nbDataPerCanal];
		}

		if (dR.w > 50) {
			forced_w = dR.w;
			forced_h = dR.h;
		}
		else {
			forced_w = 100;
			forced_h = forced_w * dR.h / dR.w;
		}

		window = SDL_CreateWindow("Neural Network", dx, 20, forced_w, forced_h, SDL_WINDOW_SHOWN);
		surface = SDL_CreateRGBSurface(0, forced_w, forced_h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
		renderer = SDL_CreateRenderer(window, 0, 0);

		dx += 30;

		if (RGBmode) mapDataOnSurface(surface, dR, dG, dB);
		else		 mapDataOnSurface(surface, dR, dR, dR);
		renderSurface(renderer, surface);

		SDL_FreeSurface(surface);
		SDL_DestroyRenderer(renderer);

		windows[i] = window;

	}

	//free(D);

	return windows;

}