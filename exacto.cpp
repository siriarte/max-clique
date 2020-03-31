#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <queue>
#include <algorithm>

using namespace std;

#include "clique.h"

bool done;
vint bestSol;

// Precondición: used[v] == false
bool tieneCiclos(graph &g, int v, vbool &used) {
	// vec<id, precedencia> q;
	queue<pii> q;
	q.push(make_pair(v, -1));
	used[v] = true;
	while(!q.empty()) {
		pii pCurr = q.front();
		int w = pCurr.first;
		int prec = pCurr.second;
		q.pop();
		forn(i,g[w].size()) {
			int ady = g[w][i];
			if (!used[ady]) {
				q.push(make_pair(ady, w));
				used[ady] = true;
			} else if (ady != prec) return true;
		}
	}
	return false;
}

bool esBosque(graph &g) {
	vbool used(g.size(), false);
	forn(i,g.size())
		if (!used[i] && tieneCiclos(g, i, used))
			return false;
	return true;
}

int cotaSup(graph &g) {
	int n = g.size();
	vint dcount(n, 0);
	vint mayoresIgA(n, 0); // Cant. de nodos de grado mayor o igual a [d]
	forn(i,n) dcount[g[i].size()]++;
	
	// Programación dinámica
	mayoresIgA[n-1] = dcount[n-1];
	if (mayoresIgA[n-1] > n-1) return n;
	for(int d=n-2; d >= 0; d--) {
		mayoresIgA[d] = mayoresIgA[d+1] + dcount[d];
		if (mayoresIgA[d] > d) return d+1;
	}
	return 1;
}

void maxClique(graph &g, vint &clique, int pnew, int imin, int cSup) {
	if (clique.size() == 0) imin = pnew;
	else {
		if (pnew <= clique[clique.size() - 1]) return; // Exigimos un orden
		if (!isAdy(g, pnew, clique)) return;
	}
	clique.push_back(pnew);
	if (clique.size() > bestSol.size()) bestSol = clique;
	if (clique.size() == cSup) {
		done = true;
		return;
	}
	if (g[pnew].size() < g[imin].size()) imin = pnew;
	if (g[imin].size() + 1 <= bestSol.size()) return; // A lo sumo se podrá maximizar hasta g[imin].size() + 1
	forn(i,g[imin].size()) {
		maxClique(g, clique, g[imin][i], imin, cSup);
		if (done) return;
	}
	clique.pop_back();
}

vint exacto(graph &g) {
	vint sol;
	if (g.empty()) return sol;
	if (esBosque(g)) {
		forn(i,g.size()) if (g[i].size() >= 1) {
			sol.push_back(i);
			sol.push_back(g[i][0]);
			return sol;
		}
		sol.push_back(0);
		return sol;
	}
	
	int cSup = cotaSup(g);
	
	done = false;
	bestSol.clear();
	
	forn(i, g.size()) {
		vint clique;
		maxClique(g, clique, i, -1, cSup);
		if (done) return bestSol;
	}
	return bestSol;
}

unsigned long long time(graph &g) {
	unsigned int t, t2, tk, tk2;
	unsigned long long mint = -1, T, TK;
	for (int i=0;i<100;i++) {
		__asm__ __volatile__ ("rdtsc;mov %%eax,%0" : : "g" (t2));
		__asm__ __volatile__ ("mov %%edx,%0" : : "g" (t));
		exacto(g);
		__asm__ __volatile__ ("rdtsc;mov %%eax,%0" : : "g" (tk2));
		__asm__ __volatile__ ("mov %%edx,%0" : : "g" (tk));
		T = t;
		T = (T << 32) + t2;
		TK = tk;
		TK = (TK << 32) + tk2;
		TK = TK - T;
		if (TK < mint) mint = TK;
	}
	return mint;
}

int main() {
	ifstream fin("Tp3.in");
	ofstream fout("Tp3exacto.out");
	
	int n;
	while(true) {
		fin >> n;
		if (n == -1) return 0;
		
		graph g(n);
		forn(i,n) {
			int k;
			fin >> k;
			vint adys(k);
			forn(j,k) fin >> adys[j];
			forn(j,k) adys[j]--;
			g[i] = adys;
		}
		
		cout << n << "\t" << time(g) << endl;
		
		vint sol = exacto(g);
		fout << sol.size() << endl << "N";
		forn(i,sol.size()) fout << " " << (sol[i]+1);
		fout << endl;
	}
	return 0;
}
