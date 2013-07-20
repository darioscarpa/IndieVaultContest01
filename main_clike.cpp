#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define PROFILING

#ifdef PROFILING
#include <Windows.h>
#endif

const int BUFSIZE = 65536;

const int OUTBUFSIZE = 65536;

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


typedef struct Node {		
	int   moveNameLen;
	char* moveName;	
	Node* outEdges[VALID_CHARS];
} Node;

int main(int argc, char* argv[]) {

#ifdef PROFILING
	long profiling[1000];
	int profcnt = 0;
	//genKeyMap();
			
	profiling[profcnt++] = GetTickCount();
#endif			

	char *stdiobuf = (char*) malloc(BUFSIZE);

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

	Node *movesGraph = (Node*) calloc(1, sizeof Node);
	//Node movesGraph;
	//memset(&movesGraph, 0, sizeof Node);
			
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

		//addMove(&movesGraph, ptrKeySeq, ptrMoveName, len);
		//////////////////////////////
		Node *n = movesGraph;
		char c;
		while ( (c = *ptrKeySeq++)  != '\n') {
			Node *next = n->outEdges[keyMap[c]];
			if (next == nullptr) {
				next = n->outEdges[keyMap[c]] = (Node*)calloc(1, sizeof(Node));	
			}
			n = next;		
		}
		n->moveNameLen = len;
				
		--ptrMoveName;
		*ptrMoveName  = '\n';
		n->moveName = ptrMoveName;
		//////////////////////////////	
	}
#ifdef PROFILING		
	profiling[profcnt++] = GetTickCount();
#endif

	//	printNode(movesGraph, 0);
		
	FILE *cfout = fopen("output.txt", "wb");
	setvbuf(cfout, stdiobuf, _IOFBF, BUFSIZE) ;
		
	//char *outbuf = (char*) malloc(OUTBUFSIZE);
	char outbuf[OUTBUFSIZE];
	int  outbufpos = 0;

	Node *curr = movesGraph;

#ifdef PROFILING		
	profiling[profcnt++] = GetTickCount();
#endif

	char *ptrUnmatched;
	char *ptrMatched;
	ptrMatched = ptrUnmatched = ++ptrSplit;
	
	while (*ptrUnmatched) {		
		Node *next = curr->outEdges[keyMap[*ptrMatched]];
		if (next == nullptr) { // no edge for currchar, discard it
			outbuf[outbufpos++] = *ptrUnmatched;
			ptrMatched = ++ptrUnmatched;		
			curr = movesGraph;				
		} else { // outgoing edge, go forward
			curr = next;
			++ptrMatched;
			if (curr->moveName != nullptr) {				
				memcpy(outbuf + outbufpos,  curr->moveName, curr->moveNameLen);
				outbufpos += curr->moveNameLen;
				ptrUnmatched = ptrMatched;
				curr = movesGraph;
			}			
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


/*
void printNode(Node &n, int level) {
	if (n.moveName != nullptr)  {
		for (int k = 0 ; k < level; k++) cout << " ";
		//cout << " -> " << n.moveName << endl;
		for (int k = 0 ; k < n.moveNameLen; k++) cout << n.moveName[k];
	} else {
		for (int i = 0; i < VALID_CHARS; i++) {			
			if (n.outEdges[i] != nullptr) {
				for (int k = 0 ; k < level; k++) cout << " ";
				cout << i << " -> " << endl;
				printNode( *n.outEdges[i], level+1 );
				//cout << "} ";
			}			
		}
	}
}*/
