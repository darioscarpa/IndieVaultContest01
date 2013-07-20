#include <stdio.h>
#include <stdlib.h>
#include <string.h>



//#define PROFILING

#ifdef PROFILING
#include <Windows.h>
#endif

const int BUFSIZE = 65536;
char stdiobuf[BUFSIZE];

const long OUTBUFSIZE = BUFSIZE;//1024*1024; //20*65535; 

const int VALID_CHARS = 13;

char keyMap[] = {
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
 /*#*/ 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
 /*D*/ 1, -1, -1, -1, -1, -1, -1, 
 /*K*/ 2, 
 /*L*/ 3, -1, -1, -1, 
 /*P*/ 4, -1, 
 /*R*/ 5, -1, -1, 
 /*U*/ 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
 /*d*/ 7, -1, -1, -1, -1, -1, -1, 
 /*k*/ 8, 
 /*l*/ 9, -1, -1, -1, 
 /*p*/ 10, -1, 
 /*r*/ 11, -1, -1, 
 /*u*/ 12, }; // size= 118

//char revKeyMap[] = { '#', 'D', 'K', 'L', 'P', 'R', 'U', 'd', 'k', 'l', 'p', 'r', 'u'};

typedef struct Node {
	int   outDeg;
	char  cache;
	char* moveName;
	int   moveNameLen;
	Node* outEdges[VALID_CHARS];	
} Node;

typedef struct CompactNode {
	int   outDegree;
	//CompactNode  **outEdges;
	CompactNode  *outEdges[VALID_CHARS];
	char* text;
	int   textLen;
	char* moveName;
	int   moveNameLen;
} CompactNode;


CompactNode *makeCompactGraph(Node *n, char *tmp);

void makeCompactGraph_intNode(Node *n, CompactNode *ncn, char *tmp) {
	//ncn->outEdges = (CompactNode**) calloc(1, VALID_CHARS * sizeof(CompactNode*));
	ncn->outDegree = n->outDeg;
	// assign outgoing edges
	for (int i = 0; i < VALID_CHARS; i++) {
		if (Node *next = n->outEdges[i]) {
			ncn->outEdges[i] = makeCompactGraph(next, tmp);
		}
	}	
}

CompactNode *makeCompactGraph(Node *base, char *tmp) {	
	//static int contcpcn =0; 
	CompactNode *ncn = (CompactNode*) calloc(1, sizeof CompactNode);
	
	//CompactNode *ncn = *pool;
	//*pool = (*pool)+1;
	//printf("poolptr %08x\n", *pool);

	if ( base->outDeg < 2 ) { 		
		int pos = 0;
		Node *next = base;
		while (next->outDeg == 1) { // internal node with single outgoing edge			
			tmp[pos++] = next->cache;				
			next       = next->outEdges[keyMap[next->cache]];		
		}
		ncn->text = (char*) malloc(pos);
		memcpy(ncn->text, tmp, pos);
		ncn->textLen = pos;
		
		if (next->outDeg == 0) { // end node with associated supermove			
			ncn->moveName    = next->moveName;
			ncn->moveNameLen = next->moveNameLen;
		} else { // internal node with multiple outgoing edges
			makeCompactGraph_intNode(next, ncn, tmp);
		}
	} else { // internal node: 2 or more outgoing edges: if text matched, go to node outEdges[nextchar]
		makeCompactGraph_intNode(base, ncn, tmp);
	}		
	return ncn;
}


int main(int argc, char* argv[]) {

#ifdef PROFILING
	long profiling[1000];
	int profcnt = 0;
	//genKeyMap();
			
	profiling[profcnt++] = GetTickCount();
#endif			

	FILE *cfin = fopen("input.txt", "rb");
	setvbuf(cfin, stdiobuf, _IOFBF, BUFSIZE) ;
	
	fseek(cfin, 0, SEEK_END);
	long clength = ftell (cfin);
	//rewind(fin);
	fseek(cfin, 0, SEEK_SET);
    
	char *cfbuffer = (char*) malloc(clength+2);
	cfbuffer[0] = '\n';
	fread(cfbuffer+1, clength, 1, cfin);
	cfbuffer[clength+1] = '\0';

#ifdef PROFILING		
	profiling[profcnt++] = GetTickCount();
#endif
	int nodecont = 0;
	Node *movesGraph = (Node*)calloc(1, sizeof Node);
	//Node movesGraph;
		
	char *ptrKeySeq;
	char *ptrMoveName;

	char *ptrSplit = cfbuffer+1;

	int maxMoveNameLen = 0;
	while (*ptrSplit != '\n') {		
		//////////////////////////////
		ptrMoveName = ptrSplit;
		
		while (*ptrSplit != ':') ptrSplit++;
		*ptrSplit = '\n';

		ptrSplit++;
		ptrKeySeq = ptrSplit;
		while (*ptrSplit != '\n') ptrSplit++;
		//*ptrSplit = '\0';
				
		int len = ptrKeySeq - ptrMoveName + 1;
		if ( len > maxMoveNameLen ) maxMoveNameLen = len;
				
		ptrSplit++;

		//////////////////////////////
		Node *n = movesGraph;		
		char c;
		while ( (c = *ptrKeySeq++)  != '\n') {
			Node *next = n->outEdges[keyMap[c]];
			if (next == nullptr) {
				next = n->outEdges[keyMap[c]] = (Node*)calloc(1, sizeof(Node));		
				n->outDeg++;
				n->cache = c;

				nodecont++;
			} 
			n = next;		
		}
		n->moveNameLen = len;
		
		--ptrMoveName;
		*ptrMoveName  = '\n';
		n->moveName = ptrMoveName;
	}
#ifdef PROFILING		
	profiling[profcnt++] = GetTickCount();
#endif
	
	char outbuf[OUTBUFSIZE];
	int  outbufpos = 0;
	
	CompactNode *compactNodeGraph = makeCompactGraph(movesGraph, outbuf);	

#ifdef PROFILING		
	profiling[profcnt++] = GetTickCount();
#endif	
	
	FILE *cfout = fopen("output.txt", "wb");
	setvbuf(cfout, stdiobuf, _IOFBF, BUFSIZE) ;
	
	char *ptrUnmatched;
	char *ptrMatched;
	ptrMatched = ptrUnmatched = ++ptrSplit;
	
	
	
#ifdef PROFILING		
	profiling[profcnt++] = GetTickCount();
#endif
	//getchar();
	CompactNode *ccurr = compactNodeGraph;
	while (*ptrUnmatched) {
		CompactNode *next = ccurr->outEdges[keyMap[*ptrMatched]];					
		
		if ( next &&  ( next->textLen==0 || (memcmp(next->text, ptrMatched+1, next->textLen)==0) )) {
			ptrMatched += next->textLen + 1;
			if (next->outDegree == 0) { // supermove match
				memcpy(outbuf + outbufpos, next->moveName, next->moveNameLen);							
				outbufpos += next->moveNameLen;
				ptrUnmatched = ptrMatched;
				ccurr = compactNodeGraph;
			} else { // internal node
				ccurr = next;
			}
		} else { // discard char
			outbuf[outbufpos++] = *ptrUnmatched;
			ptrMatched = ++ptrUnmatched;			
			ccurr = compactNodeGraph;
		}		
		if (outbufpos + maxMoveNameLen >= OUTBUFSIZE) {
//#ifdef PROFILING		
//			profiling[profcnt++] = GetTickCount();
//#endif
			fwrite(outbuf, outbufpos, 1, cfout);
			outbufpos = 0;
		}
	}
	fwrite(outbuf, outbufpos, 1, cfout);
#ifdef PROFILING		
			profiling[profcnt++] = GetTickCount();
#endif

#ifdef PROFILING		
	for (int i = 1; i < profcnt; i++) {
		//if (profiling[i]-profiling[i-1] != 0) printf("%d %ld\n", i, profiling[i]-profiling[i-1]);
		 printf("%d %ld\n", i, profiling[i]-profiling[i-1]);
	}
#endif
	
	return 0;
}



//# 35

//D 68	
//K 75
//L 76
//P 80
//R 82
//U 85

//d 100
//k 107
//l 108
//p 112
//r 114
//u 117	

//void genKeyMap() {
//	char keyMap['u'+1];
//	string s("UuDdLlRrPpKk#");
//	int found = 0;
//	printf("char keyMap[] = {\n");
//	for (int c = 0; c <= 'u'; c++) {		
//     if (s.find(c)!=string::npos) {
//			keyMap[c] = found++;
//			printf("\n /*%c*/ ", c);
//		} else {
//			keyMap[c] = -1;
//		} 
//		printf("%d, ", keyMap[c]);
//	}
//	printf("}; // size= %d\n", 'u'+1);	
//}

//void printCompactNode(CompactNode &n, int level) {
//	for (int i=0; i<level; i++) printf(" ");
//	printf("deg: %d -  text[%d] ", n.outDegree, n.textLen);
//	for (int i = 0; i < n.textLen; i++) printf("%c", n.text[i]);
//	printf(" - moveName[%d] ", n.moveNameLen);
//	for (int i = 1; i < n.moveNameLen; i++) printf("%c", n.moveName[i]);
//	printf("\n");
//	if (n.outDegree>0)
//	for (int i=0; i < VALID_CHARS; i++) {
//		if (n.outEdges[i] != nullptr) {
//			for (int k=0; k<level; k++) printf(" ");
//			printf("%c -> ", revKeyMap[i]);
//			printCompactNode(*n.outEdges[i], level+1);
//		}
//	}
//}