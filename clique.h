#define forsn(i,s,n) for(int i=(s);i<(n);i++)
#define forall(it,X) for(typeof((X).begin()) it = (X).begin(); it != (X).end(); it++)
#define forn(i,n) forsn((i),0,(n))

typedef vector<int> vint;
typedef vector<vint> vvint;
typedef vector<bool> vbool;
typedef pair<int, int> pii;
typedef list<int> lint;
typedef vvint graph;

void ltAdd(lint &tl, int tlmaxSize, int v) {
	tl.push_back(v);
	if (tl.size() > tlmaxSize) tl.pop_front();
}

vint relation(int n, vint &A, vint &B, bool va, bool vb) {
	vint s;
	vbool a(n, false), b(n, false);
	forn(i,A.size()) a[A[i]] = true;
	forn(i,B.size()) b[B[i]] = true;
	forn(i,n) if ((va==a[i]) && (vb==b[i])) s.push_back(i);
	return s;
}

vint intersection(int n, vint &A, vint &B) {
	return relation(n, A, B, true, true);
}

vint subtraction(int n, vint &A, vint &B) {
	return relation(n, A, B, true, false);
}

vint getAdys(graph &g, int v, vint &ns) {
	vint adys;
	vbool ady(g.size(), false);
	forn(i,g[v].size()) ady[g[v][i]] = true;
	forn(i,ns.size()) if (ady[ns[i]]) adys.push_back(ns[i]);
	return adys;
}

bool isAdy(graph &g, int v, vint &ns) {
	if (g[v].size() < ns.size()) return false;
	vint adys = getAdys(g,v,ns);
	return adys.size() == ns.size();
}

void maximizar(graph &g, vint &clique) {
	if (g.empty()) return;
	if (clique.empty()) clique.push_back(0);
	int imind = clique[0], imaxd;
	forsn(i,1,clique.size())
		if (g[clique[i]].size() < g[imind].size())
			imind = clique[i];
	do {
		imaxd = -1;
		forn(i,g[imind].size()) {
			int pnew = g[imind][i];
			if (imaxd == -1 || g[pnew].size() > g[imaxd].size())
				if (isAdy(g, pnew, clique)) imaxd = pnew;
		}
		if (imaxd != -1) {
			clique.push_back(imaxd);
			if (g[imaxd].size() < g[imind].size()) imind = imaxd;
		}
	} while(imaxd != -1);
}

vint hconstr(graph &g) {
	vint clique;
	if (g.empty()) return clique;
	int imaxd = 0;
	forsn(i,1,g.size()) if (g[i].size() > g[imaxd].size()) imaxd = i;
	clique.push_back(imaxd);
	maximizar(g, clique);
	return clique;
}


void hblocal(graph &g, vint &clique) {
	if (g.empty()) return;
	int n = g.size();
	while (true) {
		maximizar(g, clique);
		
		vint toadd, noady(n, -1);
		forn(i,n) {
			vint adys = getAdys(g, i, clique);
			if (adys.size() == clique.size() - 1) {
				int nady = subtraction(n, clique, adys)[0];
				if (i != nady) {
					toadd.push_back(i);
					noady[i] = nady;
				}
			}
		}
		
		int imaxdOld = -1, imaxdNew1 = -1, imaxdNew2 = -1, maxd = -1;
		forn(i,toadd.size()) {
			int a = toadd[i];
			vint inter = intersection(n, toadd, g[a]);
			forn(j,inter.size()) {
				int b = inter[j];
				if (b >= a) break; // Probamos sólo (b, a) con b < a
				if (noady[a] == noady[b]) {
					int d = g[a].size() + g[b].size() - g[noady[a]].size();
					if (imaxdOld == -1 || d > maxd) {
						imaxdOld = noady[a];
						imaxdNew1 = a;
						imaxdNew2 = b;
						maxd = d;
					}
				}
			}		
		}
		
		if (imaxdOld != -1) {
			vint::iterator old = find(clique.begin(), clique.end(), imaxdOld);
			clique.erase(old);
			clique.push_back(imaxdNew1);
			clique.push_back(imaxdNew2);
		} else return;
	}
}

// Precondición: clique.size() > 0
void maximizarTabu(graph &g, vint &clique, lint &tl, int tlMaxSize, vint &freq) {
	if (clique.empty()) {
		tl.clear();
		int iminf = 0;
		forsn(i,1,g.size()) if (freq[i] < freq[iminf]) iminf = i;
		ltAdd(tl, tlMaxSize, iminf);
		clique.push_back(iminf);
	}
	int imind = clique[0], imaxd;
	forsn(i,1,clique.size())
		if (g[clique[i]].size() < g[imind].size())
			imind = clique[i];
	do {
		imaxd = -1;
		forn(i,g[imind].size()) {
			int pnew = g[imind][i];
			if (find(tl.begin(), tl.end(), pnew) != tl.end()) continue;
			if (isAdy(g, pnew, clique)) {
				if (imaxd == -1 || g[pnew].size() > g[imaxd].size())
					imaxd = pnew;
			}
		}
		if (imaxd != -1) {
			clique.push_back(imaxd);
			ltAdd(tl, tlMaxSize, imaxd);
			if (g[imaxd].size() < g[imind].size()) imind = imaxd;
		}
	} while(imaxd != -1);
}


void hbtabu(graph &g, vint &clique, int tlMaxSize, int maxFailedIterations) {
	if (g.empty()) return;
	int n = g.size();
	
	lint tl;
	vint freq(n, 0);
	
	vint bestSol = clique;
	
	int failedIterations = 0;
	
	// VER PARA QUE SE PASE COMO PARAMETRO!!!
	while (failedIterations < maxFailedIterations) {
		// Exploramos adyacentes en la vecindad con más nodos (hasta que no se pueda más)
		maximizarTabu(g, clique, tl, tlMaxSize, freq);
		forn(i,clique.size()) freq[clique[i]]++;
		
		if (clique.size() > bestSol.size())
			bestSol = clique;
		else failedIterations++;
		
		// Exploramos adyacentes en la vecindad con menos nodos
		int imind = -1;
		while (imind == -1) {
			vbool isTabu(g.size(), false);
			forall(it,tl) isTabu[*it] = true;
			forn(i,clique.size()) if (!isTabu[clique[i]])
				if (imind == -1 || g[clique[i]].size() < g[imind].size())
					imind = clique[i];
			
			// Aspiración selectiva
			if (imind == -1) {
				vbool inClique(n, false);
				forn(i,clique.size()) inClique[clique[i]] = true;
				forall(it,tl) {
					if (inClique[*it]) {
						imind = *it;
						break;
					}
				}
				lint::iterator toRem = find(tl.begin(), tl.end(), imind);
				tl.erase(toRem);
			}
			
			vint::iterator toRem = find(clique.begin(), clique.end(), imind);
			clique.erase(toRem);
			ltAdd(tl, tlMaxSize, imind);
		}
	}
	clique = bestSol;
}

