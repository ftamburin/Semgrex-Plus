#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
	char	attr[10];
	char	attr_value[10];
	char	id[10];
	int		idx;
	char	match_id[10];
} node_t;

typedef struct {
	char	dir;
	char	name[20];
	char	id[10];
} rel_t;

typedef struct {
	node_t	head;
	rel_t	rel;
	node_t	dep;
} edge_t;

typedef struct {
    char	id[10];
    char    ref[100];
    char    tag[30];
} match_t;



void ParseRel(char *s, rel_t *r)
{
	char	*dum;

fprintf(stderr,"Processing %s\n",s);
	r->dir = s[0];
	if (strstr(s,"=") != NULL) {
		if (s[1] == '=') {
			strcpy(r->name, "");
			strcpy(r->id, strtok(s+1, "= \t\n"));
		}
		else {
			strcpy(r->name, strtok(s+1, "="));
			strcpy(r->id, strtok(NULL, "= \t\n"));
		}
	}
	else {
		dum = strtok(s+1, "= \t\n");
		if (dum != NULL)
			strcpy(r->name, dum);
		else
			strcpy(r->name, "");
		strcpy(r->id, "");
	}
fprintf(stderr,"ParseRel: dir='%c' name='%s' id='%s'\n",r->dir,r->name,r->id);
}


void ParseNode(char *s, node_t *n)
{
	if ((s[0] == '{') && (s[1] != '}')) {
		strcpy(n->attr, strtok(s,"{:"));
		strcpy(n->attr_value, strtok(NULL,"}"));
		strcpy(n->id, strtok(NULL,"="));
		n->idx = -1;
		strcpy(n->match_id, "");
	}
	else {
		strcpy(n->attr, "");
		strcpy(n->attr_value, "");
		strcpy(n->id, strtok(s,"{}="));
		n->idx = -1;
		strcpy(n->match_id, "");
	}
fprintf(stderr,"ParseNode: attr='%s' attr_value='%s' id='%s'\n",n->attr,n->attr_value,n->id);
}




void ReadpatternEdges(edge_t p[], int *ip)
{
	char	line[500],first[500], rel[500], last[500], *head, *dep;

	*ip = 0;
	fgets(line,500,stdin);
	while (strcmp(line,"\n") != 0) {
fprintf(stderr,"> %s",line);
		strcpy(first, strtok(line," \t\n"));
		strcpy(rel  , strtok(NULL," \t\n"));
		strcpy(last , strtok(NULL," \t\n"));

		ParseRel(rel, &(p[*ip].rel));
		if (p[*ip].rel.dir == '<') {
			dep = first;
			head = last;
		}
		else {		
			dep = last;
			head = first;
		}
		ParseNode(head, &(p[*ip].head));
		ParseNode(dep , &(p[*ip].dep));
		(*ip)++;

		fgets(line,500,stdin);
	}
}

	
char *getRef(char *id, match_t m[], int nm)
{
	int	j = 0;
	while ((j < nm) && (strcmp(id,m[j].id) != 0))
		j++;
	if (j == nm) {
		fprintf(stderr,"ERROR: Id %s not found!!!\n",id);
		exit(1);
	}
	else
		return (m[j].ref);
}


void GetTokenID(char *s, int *id)
{
	char	dummy[50];
	int		j;
	strcpy(dummy, s);
	j = strlen(dummy)-1;
	while ((j >= 0) && (dummy[j] != '-'))
		j--;
	*id = atoi(&(dummy[j+1]));
}


void CopyEdge(edge_t *b, edge_t *a)
{
	// COPY HEAD
	strcpy(b->head.attr, a->head.attr);
	strcpy(b->head.attr_value, a->head.attr_value);
	strcpy(b->head.id, a->head.id);
	b->head.idx = a->head.idx;
	strcpy(b->head.match_id, a->head.match_id);
	// COPY REL
	b->rel.dir = a->rel.dir;
	strcpy(b->rel.name, a->rel.name);
	strcpy(b->rel.id, a->rel.id);
	// COPY HEAD
	strcpy(b->dep.attr, a->dep.attr);
	strcpy(b->dep.attr_value, a->dep.attr_value);
	strcpy(b->dep.id, a->dep.id);
	b->dep.idx = a->dep.idx;
	strcpy(b->dep.match_id, a->dep.match_id);
}


int CheckMatch(match_t matches[], int nmt, char matchAT[][50], int nmAT, 
				int relax) 
{
	int	j,i,ret = 1,reason = 0;

	j = 0;
	while ((j < nmAT) && (strcmp(matchAT[j],matchAT[nmAT]) != 0))
		j++;
	if (!relax && (j < nmAT))
		// NO MATCH WITH THE SAME HEAD or RELAX!
		ret = reason = 0;
	else {
		// CHECK IF ALL THE NODES IN THE MATCH ARE DIFFERENT
		// BY CHECKING IF ALL CoNLL INDICES ARE REALLY DIFFERENT.
		// INTRODUCED TO CORRECT SEMGREXPATTERN OVERMATCHING.
		int	nd[nmt];
		for (j = 0; j < nmt; j++) 
			GetTokenID(matches[j].ref, &(nd[j]));
		for (j = 0; j < nmt-1; j++)
			for (i = j+1; i < nmt; i++)
				if (nd[j] == nd[i]) {
					ret = 0;
					reason = 1;
				}
	}
	fprintf(stderr,"CheckMatch_RET = %d   reason = %d\n",ret,reason);
	return (ret);
}



int CheckMiniGraph(edge_t ptf[], int nptf, match_t matches[], int nmt, 
								match_t mrels[], int nrl, int relax)
{
	edge_t	dum[100];
	int		j,i,ret = 1,reason = 0;

	fprintf(stderr,"- MINI GRAPH -\n");
	for (j = 0; j < nptf; j++)
		CopyEdge(&(dum[j]), &(ptf[j]));
	
	// REPLACE IN PATTERNFROM COPY "dum" ABSTRACT IDs WITH ACTUAL ref STRINGS
	for (j = 0; j < nptf; j++) {
		// REL_ID
		i = 0;
		while ((i < nrl) && (strcmp(dum[j].rel.id, mrels[i].id) != 0))
			i++;
		strcpy(dum[j].rel.id, mrels[i].ref);
		// HEAD ID
		i = 0;
		while ((i < nmt) && (strcmp(dum[j].head.id, matches[i].id) != 0))
			i++;
		strcpy(dum[j].head.id, matches[i].ref);
		// DEP ID
		i = 0;
		while ((i < nmt) && (strcmp(dum[j].dep.id, matches[i].id) != 0))
			i++;
		strcpy(dum[j].dep.match_id, matches[i].id);

		GetTokenID(matches[i].ref, &(dum[j].dep.idx));
		strcpy(dum[j].dep.attr, matches[i].tag);
		
		fprintf(stderr,"%s %s %s-%d-%s\n",dum[j].head.id,dum[j].rel.id,
										dum[j].dep.attr,dum[j].dep.idx,
										dum[j].dep.match_id);
	}

/*
	// ONLY APPLICABLE FOR RELATION DISTANCE AMONG DEPs = 1
	if (relax) 
		for (j = 0; j < nmt-1; j++)
			for (i = j+1; i < nmt; i++)
{
fprintf(stderr,"%s %s %d-%d=%d\n",dum[j].head.id,dum[i].head.id,dum[j].dep.idx,dum[i].dep.idx,abs(dum[j].dep.idx-dum[i].dep.idx));
				if ((strcmp(dum[j].head.id,dum[i].head.id) == 0) &&
					(abs(dum[j].dep.idx-dum[i].dep.idx) > 1))
					ret = reason = 0;
}
*/

	if (ret == 1) 
		// CHECK FOR DEPENDANTS WITH THE SAME ATTRIBUTES but SCRAMBLED.
		// INTRODUCED TO CORRECT SEMGREXPATTERN OVERMATCHING
		for (j = 0; j < nptf-1; j++)
			for (i = j+1; i < nptf; i++)
				if ((strcmp(dum[j].head.id,dum[i].head.id) == 0) &&
					(strcmp(dum[j].rel.id,dum[i].rel.id) == 0) &&
					(strcmp(dum[j].dep.attr,dum[i].dep.attr) == 0) &&
					(dum[j].dep.idx > dum[i].dep.idx)) {
					ret = 0;
					reason = 1;
		}

	if (ret == 1) 
		// MATCH_IDs (derived from rule order) MUST RESPECT SENTENCE IDs
		// ORDERS.
		// INTRODUCED TO CORRECT SEMGREXPATTERN OVERMATCHING
fprintf(stderr,"nptf = %d\n",nptf);
		for (j = 0; j < nptf-1; j++)
			for (i = j+1; i < nptf; i++) {
fprintf(stderr,"%s %s\n",dum[j].head.id,dum[i].head.id);
				if (strcmp(dum[j].head.id,dum[i].head.id) == 0) {
fprintf(stderr,"%s %s   %d %d\n",dum[j].dep.match_id,dum[i].dep.match_id,
									dum[j].dep.idx,dum[i].dep.idx);
					if (((dum[j].dep.match_id > dum[i].dep.match_id) &&
					 	(dum[j].dep.idx < dum[i].dep.idx)) ||
						((dum[j].dep.match_id < dum[i].dep.match_id) &&
					 	(dum[j].dep.idx > dum[i].dep.idx))) {
						ret = 0;
						reason = 2;
					}
				}
			}

	fprintf(stderr,"CheckMiniGraph_RET = %d   reason = %d\n",ret,reason);
	return (ret);
}


int main(int argc, char **argv)
{
	char	line[500];
	edge_t	ptf[1000];
	edge_t	ptt[1000];
	int		j,i,nptf,nptt,relax;
	match_t	matches[50], mrels[50];
	char	matchAT[1000][50];
	int		nmt, nrl, nmAT;

	if ((argc > 1) && (strcmp(argv[1],"relax") == 0))
		relax = 1;
	else
		relax = 0;

// FOR NOW, LET'S RELAX! :-P
relax = 1;

	while (fgets(line,500,stdin) != NULL) {
		if (strstr(line,"PATTERN_FROM") != NULL) {
			ReadpatternEdges(ptf, &nptf);
			fprintf(stderr,"Read %d edges from 'PATTERN_FROM'\n",nptf);
		}
		if (strstr(line,"PATTERN_TO") != NULL) {
			ReadpatternEdges(ptt, &nptt);
			fprintf(stderr,"Read %d edges from 'PATTERN_TO'\n",nptt);
		}
		if (strstr(line,"-----") != NULL) {
			fprintf(stderr,"\n");
			fgets(line,500,stdin);
			strtok(line," \t\n");
			int graph = atoi(strtok(NULL," \t\n"));
			fprintf(stderr,"Processing matches for graph %d\n",graph);

			fgets(line,500,stdin);	// EMPTY LINE

			nmAT = 0;
			while (strcmp(line,"#####\n") != 0) {
				if (strstr(line,"Matches at:") == NULL) 
					fgets(line,500,stdin);	// MATCHES AT:
				fprintf(stderr,"%s",line);
				strtok(line," \t\n");
				strtok(NULL," \t\n");
				strcpy(matchAT[nmAT], strtok(NULL," \t\n"));

				fgets(line,500,stdin);
				nmt = nrl = 0;
				while (strcmp(line,"\n") != 0) {
					strcpy(matches[nmt].id, strtok(line,": \t\n"));
					strcpy(matches[nmt].ref, strtok(NULL," \t\n"));
					strcpy(matches[nmt].tag, strtok(NULL," \t\n"));
fprintf(stderr,"word: %s %s %s\n",matches[nmt].id,matches[nmt].ref,matches[nmt].tag);
					nmt++;
					fgets(line,500,stdin);
				}
				fprintf(stderr,"Read %d node matches\n",nmt);

				fgets(line,500,stdin);
				while (strcmp(line,"\n") != 0) {
					strcpy(mrels[nrl].id, strtok(line,": \t\n"));
					strcpy(mrels[nrl].ref, strtok(NULL," \t\n"));
fprintf(stderr,"rel : %s %s\n",mrels[nrl].id,mrels[nrl].ref);
					nrl++;
					fgets(line,500,stdin);
				}
				fprintf(stderr,"Read %d rel matches\n",nrl);


				if ((CheckMatch(matches,nmt,matchAT,nmAT,relax) == 1) &&
					(CheckMiniGraph(ptf,nptf,matches,nmt,mrels,nrl,relax)==1)){
					fprintf(stderr,"***** VALID MATCH *****\n\n");
					nmAT++;

					// PRODUCE DIFF COMMANDS
					// DEL_REL
					for (j = 0; j < nptf; j++) {
						printf("%d\tDEL_REL\t",graph);
						printf("%s\t",getRef(ptf[j].rel.id, mrels, nrl));
						printf("%s\t",getRef(ptf[j].dep.id, matches, nmt));
						printf("%s\n",getRef(ptf[j].head.id, matches, nmt));
					}
					// INS_REL
					for (j = 0; j < nptt; j++) {
						printf("%d\tINS_REL\t",graph);
						if (strcmp(ptt[j].rel.name,"") != 0)
							printf("%s\t",ptt[j].rel.name);
						else {
							i = 0;
							while (strcmp(mrels[i].id,ptt[j].rel.id) != 0)
								i++;
							printf("%s\t",mrels[i].ref);
						}
						printf("%s\t",getRef(ptt[j].dep.id, matches, nmt));
						printf("%s\n",getRef(ptt[j].head.id, matches, nmt));
					}
					// REN_TAG
					for (j = 0; j < nptt; j++) {
						if (strcmp(ptt[j].head.attr,"tag") == 0)
							printf("%d\tREN_TAG\t%s\t%s\n",graph,
										ptt[j].head.attr_value,
										getRef(ptt[j].head.id, matches, nmt));
						if (strcmp(ptt[j].dep.attr,"tag") == 0)
							printf("%d\tREN_TAG\t%s\t%s\n",graph,
										ptt[j].dep.attr_value,
										getRef(ptt[j].dep.id, matches, nmt));
					}
	
				}
				else 
					fprintf(stderr,"> INVALID MATCH <\n\n");

				fgets(line,500,stdin);
			}
		}
	}
}
