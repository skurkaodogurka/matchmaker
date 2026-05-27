#include <iostream>
#include <cstdlib>
#include <cstdio>
#define _CRT_SECURE_NO_WARNINGS

struct queue {
	int* elements;
	int head = 0;
	int tail = 0;

	void push(int value) {
		elements[tail] = value;
		tail++;
	}
	int pop() {
		int value = elements[head];
		head++;
		return value;
	}
	bool isEmpty() {
		if (head == tail) {
			return true;
		}
		else {
			return false;
		}
	}
};

bool bfsEdgeColoring(int n, int* head, int* to, int* nextEdge, int* colors) {

	queue q; //tworzenie lokalnej kolejki
	q.elements = new int[n + 1];
	for (int i = 1; i <= n; i++) {
		if (colors[i] == -1) {
			colors[i] = 0;
			q.push(i);

			while (!q.isEmpty()) {
				int u = q.pop();
				int currentEdge = head[u];
				while (currentEdge != -1) {
					int v = to[currentEdge];
					if (colors[v] == -1) {
						colors[v] = 1 - colors[u];
						q.push(v);
					}
					else if (colors[v] == colors[u]) {
						delete[] q.elements;
						return false;
					}
					currentEdge = nextEdge[currentEdge];
				}
			}
		}
	}

	delete[] q.elements;
	return true;
}

long long hungarian(int matrixSize, long long* matrix) {
	long long* leftLabel = new long long[matrixSize + 1](); //etykiety dla lewej strony
	long long* rightLabel = new long long[matrixSize + 1](); //etykiety dla prawej strony
	int* match = new int[matrixSize + 1]();
	int* traceBack = new int[matrixSize + 1](); //tablica do odtwarzania sciezki
	long long* slack = new long long[matrixSize + 1]();
	bool* met = new bool[matrixSize + 1]();

	for (int i = 0; i <= matrixSize; i++) {
		leftLabel[i] = 0;
		rightLabel[i] = 0;
		match[i] = 0;
		traceBack[i] = 0;
	}

	for (int i = 1; i <= matrixSize; i++) {
		long long maxVal = 0; //maksymalne wymagania dla lewej strony
		long long* currentRow = &matrix[(i - 1) * matrixSize];
		for (int j = 1; j <= matrixSize; j++) {
			if (currentRow[j - 1] > maxVal) {
				maxVal = currentRow[j - 1];
			}
		}
		leftLabel[i] = maxVal; //przypisanie najwiekszej wartosci (wymagania)
	}

	//glowna petla algorytmu
	for (int i = 1; i <= matrixSize; i++) {
		match[0] = i; //indeks zerowy i tak nie jest uzywany wiec posluzy jako bufor
		int rightPosition = 0; //zmienna do chodzenia po grafach

		for (int j = 0; j <= matrixSize; j++) {
			slack[j] = 2000000000000000000LL;
			met[j] = false;
		}

		do {
			met[rightPosition] = true;
			int currentLeftPerson = match[rightPosition];
			int nextRightPosition = 0;
			long long delta = 2000000000000000000LL;

			long long* currentRow = &matrix[(currentLeftPerson - 1) * matrixSize];
			long long currentLeftLabel = leftLabel[currentLeftPerson];

			for (int j = 1; j <= matrixSize; j++) {
				if (!met[j]) {
					long long gap = currentLeftLabel + rightLabel[j] - currentRow[j - 1]; //obliczanie roznicy pomiedzy dwoma osobami
					if (gap < slack[j]) {
						slack[j] = gap;
						traceBack[j] = rightPosition; //zostawienie sladu 
					}
					if (slack[j] < delta) {
						delta = slack[j];
						nextRightPosition = j; //przypisanie nastepnego kroku
					}
				}
			}
			for (int j = 0; j <= matrixSize; j++) { //petla obnizajaca wymagania
				if (met[j]) {
					leftLabel[match[j]] -= delta; //obnizanie wymagan lewej strony
					rightLabel[j] += delta; //podnoszenie wymagan praweje strony
				}
				else {
					slack[j] -= delta;
				}
			}
			rightPosition = nextRightPosition; //przejscie krok dalej
		} while (match[rightPosition] != 0);

		//wrocenie sie po krokach zeby zaaktualizowac tablice match (po znalezieniu pary)
		do {
			int previousRightPosition = traceBack[rightPosition]; //pobranie ostatniego kroku
			match[rightPosition] = match[previousRightPosition]; //zabranie partnera poprzedniej osobie
			rightPosition = previousRightPosition; //cofniecie sie o krok
		} while (rightPosition != 0);

	}

	long long totalWeight = 0;
	for (int j = 1; j <= matrixSize; j++) {
		if (match[j] != 0 && matrix[(match[j] - 1) * matrixSize + (j - 1)] > 0) {
			totalWeight += matrix[(match[j] - 1) * matrixSize + (j - 1)];
		}
	}

	delete[] leftLabel;
	delete[] rightLabel;
	delete[] match;
	delete[] traceBack;
	delete[] slack;
	delete[] met;
	return totalWeight;
}

void test() {
	int n, m;
	scanf("%d %d", &n, &m);

	//przypadek w ktorym nie ma zadnych polaczen (krawedzi)
	if (m == 0) {
		printf("0 ");
		return;
	}

	int edgeCounter = 0;
	int* head = new int[n + 1];
	int* to = new int[(long long)2 * m];
	int* nextEdge = new int[(long long)2 * m];
	long long* weight = new long long[(long long)2 * m];
	int* colors = new int[n + 1];

	for (int i = 0; i <= n; i++) {
		head[i] = -1;
		colors[i] = -1;
	}

	for (int i = 0; i < m; i++) {
		int u, v;
		long long w;
		scanf("%d %d %lld", &u, &v, &w);

		//dodanie relacji u->v
		to[edgeCounter] = v;
		weight[edgeCounter] = w;
		nextEdge[edgeCounter] = head[u];
		head[u] = edgeCounter;
		edgeCounter++;

		//dodanie relacji v->u
		to[edgeCounter] = u;
		weight[edgeCounter] = w;
		nextEdge[edgeCounter] = head[v];
		head[v] = edgeCounter;
		edgeCounter++;

	}

	bool isBipartite = bfsEdgeColoring(n, head, to, nextEdge, colors);

	if (isBipartite) {
		int leftCount = 0;
		int rightCount = 0;
		int* mappedId = new int[n + 1]; //tablica do przekladania wierzcholkow na macierz

		for (int i = 1; i <= n; i++) {
			if (colors[i] == 0) {
				mappedId[i] = leftCount;
				leftCount++;
			}
			else if (colors[i] == 1) {
				mappedId[i] = rightCount;
				rightCount++;
			}
		}

		int matrixSize = 0;
		if (leftCount > rightCount) {
			matrixSize = leftCount;
		}
		else {
			matrixSize = rightCount;
		}

		long long* matrix = new long long[(long long)matrixSize * matrixSize](); //macierz do ktorej beda zapisywane wagi relacji

		for (int i = 1; i <= n; i++) {
			if (colors[i] == 0) {
				int row = mappedId[i];
				int currentEdge = head[i];

				while (currentEdge != -1) {
					int v = to[currentEdge];
					int column = mappedId[v];
					if (weight[currentEdge] > matrix[row * matrixSize + column]) {
						matrix[row * matrixSize + column] = weight[currentEdge];
					}
					currentEdge = nextEdge[currentEdge];
				}
			}
		}

		long long result = hungarian(matrixSize, matrix);
		printf("%lld ", result);

		delete[] mappedId;
		delete[] matrix;
	}
	else {
		printf("0 ");
	}

	delete[] colors;
	delete[] head;
	delete[] weight;
	delete[] nextEdge;
	delete[] to;
}

int main() {
	int t;
	scanf("%d", &t);

	for (int i = 0; i < t; i++) {
		test();
	}
	printf("\n");

	return 0;
}