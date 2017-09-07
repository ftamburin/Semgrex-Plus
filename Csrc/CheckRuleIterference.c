#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

typedef struct {
	char	attr[10];
	char	attr_value[10];
	char	id[10];
	int		idx;
	char	match_id[10];
	int		mod;
} node_t;

typedef struct {
	char	dir;
	char	name[20];
	char	id[10];
	int		mod;
} rel_t;

typedef struct {
	node_t	head;
	rel_t	rel;
	node_t	dep;
	int		mod;
} edge_t;

typedef struct {
	char	rule[500];
	char	ruleFrom[500];
	edge_t	ptf[15];
	edge_t	ptt[15];
	int		nptf,nptt;
} rule_t;



void ParseRel(char *s, rel_t *r)
{
	char	*dum;

fprintf(stderr,"Processing %s\n",s);
	r->dir = s[0];
	r->mod = 0;
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
	n->mod = 0;
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




void ReadPatternEdges(edge_t p[], int *ip)
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
		p[*ip].mod = 0;
		(*ip)++;

		fgets(line,500,stdin);
	}
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


void PrintNode(node_t *n)
{
	fprintf(stderr,"{%s:%s}=%s.%d",n->attr,n->attr_value,n->id,n->mod);
}


void PrintRel(rel_t *r)
{
	fprintf(stderr,"%c%s=%s.%d",r->dir,r->name,r->id,r->mod);
}


void PrintEdge(edge_t *e)
{
	PrintNode(&(e->head));fprintf(stderr," ");
	PrintRel(&(e->rel));fprintf(stderr," ");
	PrintNode(&(e->dep));
	fprintf(stderr,": edge_mod = %d\n",e->mod);
}


void PrintPatternEdges(edge_t p[], int ip)
{
	int		j,i;

	for (j = 0; j < ip; j++) {
		PrintEdge(&(p[j]));
	}
}


void MarkModParts(rule_t *r)
{
	int		j,i;

	assert (r->nptf == r->nptt);

	for (j = 0; j < r->nptf; j++) {
		// CHECK REL
		i = 0;
		while ((i < r->nptt) && 
			   (strcmp(r->ptf[j].rel.id,r->ptt[i].rel.id) != 0))
			i++;
		if (i < r->nptt)
			if ((strcmp(r->ptf[j].rel.name,r->ptt[i].rel.name) != 0) ||
				(strcmp(r->ptf[j].head.id,r->ptt[i].head.id) != 0) ||
				(strcmp(r->ptf[j].dep.id,r->ptt[i].dep.id) != 0))
				r->ptf[j].rel.mod = r->ptf[j].mod = 1;

		// CHECK HEAD
		i = 0;
		while ((i < r->nptt) && 
			   (strcmp(r->ptf[j].head.id,r->ptt[i].head.id) != 0))
			i++;
		if (i < r->nptt)
			if ((strcmp(r->ptf[j].head.attr,r->ptt[i].head.attr) == 0) &&
				(strcmp(r->ptf[j].head.attr_value,r->ptt[i].head.attr_value)!=0))
				r->ptf[j].head.mod = r->ptf[j].mod = 1;

		// CHECK DEP
		i = 0;
		while ((i < r->nptt) && 
			   (strcmp(r->ptf[j].dep.id,r->ptt[i].dep.id) != 0))
			i++;
		if (i < r->nptt)
			if ((strcmp(r->ptf[j].dep.attr,r->ptt[i].dep.attr) == 0) &&
				(strcmp(r->ptf[j].dep.attr_value,r->ptt[i].dep.attr_value)!=0))
				r->ptf[j].dep.mod = r->ptf[j].mod = 1;
	}
}


int MatchNodes(node_t *n1, node_t *n2)
{
	if (((strcmp(n1->attr, n2->attr) == 0) &&
		 (strcmp(n1->attr_value, n2->attr_value) == 0)) ||
		(strcmp(n1->attr, "") == 0) || 
		(strcmp(n2->attr, "") == 0))
		return (1);
	else
		return (0);
}


int MatchRels(rel_t *r1, rel_t *r2)
{
	if (r1->dir == r2->dir)
		if ((strcmp(r1->name,r2->name) == 0) ||
			(strcmp(r1->name, "") == 0) ||
			(strcmp(r2->name, "") == 0))
			return (1);
		else
			return (0);
	else
		return (0);
}


int RuleSamePTF(rule_t *r1, rule_t *r2)
{
	if (r1->nptf == r2->nptf) {
		int j = 0, mtch = 1;
		while (mtch && (j < r1->nptf)) {
			if (!(MatchNodes(&(r1->ptf[j].head), &(r2->ptf[j].head)) &&
				  MatchRels(&(r1->ptf[j].rel), &(r2->ptf[j].rel)) &&
				  MatchNodes(&(r1->ptf[j].dep), &(r2->ptf[j].dep))))
				mtch = 0;
			j++;
		}
		return (mtch);
	}
	else
		return (0);
}


int Overlaps(rule_t *r1, rule_t *r2)
{
	int		j,i;
	int		intModMatch, intMod;

	intModMatch = intMod = 0;
//fprintf(stderr,"%s %s\n",r1->ruleFrom,r2->ruleFrom);
	for (j = 0; j < r1->nptf; j++)
		for (i = 0; i < r2->nptf; i++) {
			if (r1->ptf[j].mod && r2->ptf[i].mod) {

				// SAME NODES SAME REL
				if (MatchNodes(&(r1->ptf[j].head), &(r2->ptf[i].head)) &&
					MatchRels(&(r1->ptf[j].rel), &(r2->ptf[i].rel)) &&
					MatchNodes(&(r1->ptf[j].dep), &(r2->ptf[i].dep)))
/*
				if ((strcmp(r1->ptf[j].head.attr,r2->ptf[i].head.attr) == 0) &&
					(strcmp(r1->ptf[j].head.attr_value,
							r2->ptf[i].head.attr_value) == 0) &&
					(r1->ptf[j].rel.dir == r2->ptf[i].rel.dir) &&
					(strcmp(r1->ptf[j].rel.name,r2->ptf[i].rel.name) == 0) &&
					(strcmp(r1->ptf[j].dep.attr,r2->ptf[i].dep.attr) == 0) &&
					(strcmp(r1->ptf[j].dep.attr_value,r2->ptf[i].dep.attr_value) == 0))
*/
					intModMatch++;
				intMod++;
			}
		}
/*
fprintf(stderr," r1F==r2F = %d   intModMatch = %d   [intMod = %d]\n",
				strcmp(r1->ruleFrom,r2->ruleFrom),intModMatch,intMod);
*/
	if (intModMatch > 0) {
		if (RuleSamePTF(r1, r2)) 
			return (2);
		else
			return (1);
	}
	else
			return (0);
}


int main(int argc, char **argv)
{
	char	line[500],rulestr[500];
	rule_t	*rules[10000];
	int		j,i,ret,nrules;

	nrules = 0;
	while (fgets(line,500,stdin) != NULL) {
		if (strstr(line,"Applying rule ") != NULL) {
			// NEW RULE: ALLOCATE RULE SPACE
			rules[nrules] = (rule_t*)malloc(sizeof(rule_t));
			strcpy(rules[nrules]->rule, strstr(line,"\""));
			strcpy(rules[nrules]->ruleFrom, rules[nrules]->rule);
			*(strstr(rules[nrules]->ruleFrom,"\t")) = '\0';
		}
		if (strstr(line,"PATTERN_FROM") != NULL) {
			// PARSE PATTERN_FROM
			ReadPatternEdges(rules[nrules]->ptf, &(rules[nrules]->nptf));
			fprintf(stderr,"Read %d edges from 'PATTERN_FROM'\n",
														rules[nrules]->nptf);
		}
		if (strstr(line,"PATTERN_TO") != NULL) {
			// PARSE PATTERN_TO
			ReadPatternEdges(rules[nrules]->ptt, &(rules[nrules]->nptt));
			fprintf(stderr,"Read %d edges from 'PATTERN_TO'\n",
														rules[nrules]->nptt);
			// FIND MODIFIED PARTS IN RULE
			MarkModParts(rules[nrules]);

			nrules++;
			fprintf(stderr,"---------------------------------\n%s",&line[15]);
		}
	}

	fprintf(stderr,"\n\n");
	for (j = 0; j < nrules; j++) {
		fprintf(stderr,"************* RULE %d ***************\n",j);
		fprintf(stderr,"%s",rules[j]->rule);
		fprintf(stderr,"PATTERN_FROM:\n");
		PrintPatternEdges(rules[j]->ptf, rules[j]->nptf);
	}

	// CHECK FOR OVERLAPS
	fprintf(stderr,"\n\n*************************************************\nOVERLAP CHECK:\n");
	int ncomp = 0;
	for (j = 0; j < nrules-1; j++) 
		for (i = j+1; i < nrules; i++) {
			if ((ret = Overlaps(rules[j], rules[i])) > 0) {
				if (ret == 2) 
					if (strcmp(rules[j]->rule, rules[i]->rule) == 0)
						fprintf(stderr,"\n----- DOUBLE RULE -----\n");
					else
						fprintf(stderr,"\n----- FULL OVERLAP ----\n");
				else
					fprintf(stderr,"\n--- PARTIAL OVERLAP ---\n");
				fprintf(stderr,"%d: %s",j,rules[j]->rule);
				PrintPatternEdges(rules[j]->ptf, rules[j]->nptf);
				fprintf(stderr,"%d: %s",i,rules[i]->rule);
				PrintPatternEdges(rules[i]->ptf, rules[i]->nptf);
				fprintf(stderr,"-----------------------\n\n");
			}
			ncomp++;
		}
	fprintf(stderr,"Checked %d rule combinations\n",ncomp);
}
