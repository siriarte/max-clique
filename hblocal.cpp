#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <algorithm>

using namespace std;

#include "clique.h"

unsigned long long time(graph &g, vint &sol) {
	unsigned int t, t2, tk, tk2;
	unsigned long long mint = -1, T, TK;
	for (int i=0;i<100;i++) {
		__asm__ __volatile__ ("rdtsc;mov %%eax,%0" : : "g" (t2));
		__asm__ __volatile__ ("mov %%edx,%0" : : "g" (t));
		hblocal(g, sol);
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
	ofstream fout("Tp3hblocal.out");
	
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
		
		vint sol = hconstr(g);
		
		cout << n << "\t" << time(g, sol) << endl;
		
		hblocal(g, sol);
		fout << sol.size() << endl << "N";
		forn(i,sol.size()) fout << " " << (sol[i]+1);
		fout << endl;
	}
	return 0;
}
