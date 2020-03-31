#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <queue>
#include <algorithm>

using namespace std;

#include "clique.h"

unsigned long long time(graph &g) {
	unsigned int t, t2, tk, tk2;
	unsigned long long mint = -1, T, TK;
	for (int i=0;i<100;i++) {
		__asm__ __volatile__ ("rdtsc;mov %%eax,%0" : : "g" (t2));
		__asm__ __volatile__ ("mov %%edx,%0" : : "g" (t));
		hconstr(g);
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
	ofstream fout("Tp3hconstr.out");
	
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
		
		vint sol = hconstr(g);
		fout << sol.size() << endl << "N";
		forn(i,sol.size()) fout << " " << (sol[i]+1);
		fout << endl;
	}
	return 0;
}
