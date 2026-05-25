#include <iostream>
#include <cstdlib>
#include <cstdio>

struct edge {
	int u;	//pierwszy wierzcholek
	int v;	//drugi wierzcholek
	long long w;	//waga polaczenia
};

void test() {
	int n, m;
	scanf("%d %d", &n, &m);

	//przypadek w ktorym nie ma zadnych polaczen (krawedzi)
	if (m == 0) {
		printf("0 ");
		return;
	}


	int edgeCounter = 0;
	int* head = new int[n+1];
	int* to = new int[2 * m];
	int* nextEdge = new int[2 * m];
	long long* weight = new long long[2 * m];

	for (int i = 0; i <= n; i++) {
		head[i] = -1;
	}

	for (int i = 0; i < m; i++) {
		int u, v;
		long long w;
		scanf("%d %d %lld", &u, &v, &w);
		to[edgeCounter] = v;
		weight[edgeCounter] = w;
		nextEdge[edgeCounter] = head[u];
		head[u] = edgeCounter;
		edgeCounter++;
	}
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