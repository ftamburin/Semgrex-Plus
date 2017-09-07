#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
	char	attr[10];
	char	attr_value[10];
	char	id[10];
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
	}
	else {
		strcpy(n->attr, "");
		strcpy(n->attr_value, "");
		strcpy(n->id, strtok(s,"{}="));
	}
fprintf(stderr,"ParseNode: attr='%s' attr_value='%s' id='%s'\n",n->attr,n->attr_value,n->id);
}




void ReadpatternNode(edge_t p[], int *ip)
{
	char	line[500],first[500], rel[500], last[500], *head, *dep;

	*ip = 0;
	fgets(line,500,stdin);
	while (strcmp(line,"\n") != 0) {
fprintf(stderr,"> %s",line);
		strcpy(first, strtok(line," \t\n"));
		head = first;
		ParseNode(head, &(p[*ip].head));
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


int main(int argc, char **argv)
{
	char	line[500];
	edge_t	ptf[1000];
	edge_t	ptt[1000];
	int		j,nptf,nptt;
	match_t	matches[50], mrels[50];
	int		nmt, nrl;

	while (fgets(line,500,stdin) != NULL) {
		if (strstr(line,"PATTERN_FROM") != NULL) {
			ReadpatternNode(ptf, &nptf);
		}
		if (strstr(line,"PATTERN_TO") != NULL) {
			ReadpatternNode(ptt, &nptt);
		}
		if (strstr(line,"-----") != NULL) {
			fprintf(stderr,"\n");
			fgets(line,500,stdin);
			strtok(line," \t\n");
			int graph = atoi(strtok(NULL," \t\n"));
			fprintf(stderr,"Processing matches for graph %d\n",graph);

			fgets(line,500,stdin);	// EMPTY LINE

			while (strcmp(line,"#####\n") != 0) {
				if (strstr(line,"Matches at:") == NULL)
					fgets(line,500,stdin);	// MATCHES AT:
				fgets(line,500,stdin);
				nmt = nrl = 0;
				while (strcmp(line,"\n") != 0) {
					strcpy(matches[nmt].id, strtok(line,": \t\n"));
					strcpy(matches[nmt].ref, strtok(NULL," \t\n"));
					nmt++;
					fgets(line,500,stdin);
				}
				fprintf(stderr,"Read %d node matches\n",nmt);

				// REN_TAG
				for (j = 0; j < nmt; j++) 
					printf("%d\tREN_TAG\t%s\t%s\n",graph,
										ptt[j].head.attr_value,
										getRef(ptt[j].head.id, matches, nmt));

				fgets(line,500,stdin);
				fgets(line,500,stdin);
			}
		}
	}
}
