//#include <iostream>
#include <fstream>

using namespace std;

static const int VALID_CHARS = 13;
const int BUFSIZE = 4096;
	
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

class Node {
public:
	char* moveName;
	int moveNameLen;
	Node* outEdges[VALID_CHARS];
	Node() : moveName(nullptr) {
		memset(outEdges, 0, sizeof(Node*)*VALID_CHARS);		
	}
	//~Node() { if (moveName) delete moveName; // recursively free children... or just don't care! }
};


void addMove(Node *n, const char *keyseq, const char *moveStr, int len) {				
	if (*keyseq == '\n') {
		n->moveNameLen = len;
		n->moveName = (char*)malloc(n->moveNameLen);
		n->moveName[0] = '\n';
		memcpy(n->moveName+1, moveStr, len-1);		
		return;
	}
	char c = *keyseq;
	Node *next = n->outEdges[keyMap[c]];
	if (next == nullptr) { // new node needed
		next = n->outEdges[keyMap[c]] = new Node();
	}
	keyseq++;
	addMove(next, keyseq, moveStr, len);			
}


int main(int argc, char* argv[]) {
	
	//genKeyMap();

	fstream fin("input.txt",   ios::in  | ios::binary);

	fin.seekg (0, fin.end);
	unsigned int length = fin.tellg();
	fin.seekg (0, fin.beg);
	char *fbuffer = new char [length+1];
	fin.read (fbuffer, length);
	fbuffer[length] = '\0';

	
	fstream fout("output.txt", ios::out | ios::binary);
	//fout.write(fbuffer, length);
	
	Node movesGraph;
	
	char *ptrKeySeq;
	char *ptrMoveName;

	char *ptrSplit = fbuffer;

	int maxMoveNameLen = 0;
	while (*ptrSplit != '\n') {				
		ptrMoveName = ptrSplit;
		
		while (*ptrSplit != ':') ptrSplit++;
		*ptrSplit = '\n';

		ptrSplit++;
		ptrKeySeq = ptrSplit;		
		while (*ptrSplit != '\n') ptrSplit++;
		//*ptrSplit = '\0';
				
		int len = ptrKeySeq - ptrMoveName + 1;
		if ( len > maxMoveNameLen ) maxMoveNameLen = len;

		addMove(&movesGraph, ptrKeySeq, ptrMoveName, len);

		ptrSplit++;		
	}

	//	printNode(movesGraph, 0);

	char *ptrUnmatched;
	char *ptrMatched;
	ptrMatched = ptrUnmatched = ++ptrSplit;

	Node *curr = &movesGraph;

	char outbuf[BUFSIZE];
	int  outbufpos = 0;
	
	while (*ptrUnmatched) {
		
		char currChar = *ptrMatched;	
		Node *next = curr->outEdges[keyMap[currChar]];
		if (next == nullptr) { // no edge for currchar, discard it
			//cout << "no edge for " <<  currChar << endl;
			outbuf[outbufpos++] = *ptrUnmatched;
			ptrMatched = ++ptrUnmatched;		
			curr = &movesGraph;				
		} else { // outgoing edge, go forward
			//cout << "edge found for " <<  currChar << endl;			
			curr = next;
			++ptrMatched;
			if (curr->moveName != nullptr) {				
				memcpy(outbuf + outbufpos,  curr->moveName, curr->moveNameLen);
				outbufpos += curr->moveNameLen;
				ptrUnmatched = ptrMatched;
				curr = &movesGraph;
			}			
		}	
		
		if (outbufpos + maxMoveNameLen >= BUFSIZE) {
			fout.write(outbuf, outbufpos);
			outbufpos = 0;
		}
	}		
	fout.write(outbuf, outbufpos);
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
