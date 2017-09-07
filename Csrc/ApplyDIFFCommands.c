#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	int		graphid;
	int		id;
	char	word[50];
	char	lemma[50];
	char	posFG[50];
	char	pos[50];
	char	morpho[100];
	int		idrel;
	char	rel[50];
} conllu_t;

typedef struct {
	int		graphid;
	char	oper[30];
	char	rel_tag[50];
	char	dep[50];
	int		iddep;
	char	head[50];
	int		idhead;
} cmd_t;


void ParseCoNLLULine(char *line, conllu_t c[], int *nc, int gid)
{
//fprintf(stderr,"Parsing: %s",line);
	char *dum = strtok(line," \t\n");
	if (dum != NULL) {
		c[*nc].graphid = gid;
		c[*nc].id = atoi(dum);
		strcpy(c[*nc].word, strtok(NULL," \t\n"));
		strcpy(c[*nc].lemma, strtok(NULL," \t\n"));
		strcpy(c[*nc].posFG, strtok(NULL," \t\n"));
		strcpy(c[*nc].pos, strtok(NULL," \t\n"));
		strcpy(c[*nc].morpho, strtok(NULL," \t\n"));
		c[*nc].idrel = atoi(strtok(NULL," \t\n"));
		strcpy(c[*nc].rel, strtok(NULL," \t\n"));
		(*nc)++;
	}
}

char *ParseElement(char *p, char *s, int *sid)
{
	char	dum[100],*q;
	int		j;

//fprintf(stderr,"%s\n",p);
	strcpy(dum,strtok_r(p," \t\n",&q));
	j = strlen(dum)-1; 
	while ((j >= 0) && (dum[j] != '-'))
		j--;
	if (j > 0) {
		*sid = atoi(&(dum[j+1]));
		dum[j] = '\0';
		strcpy(s,dum);
	}
	return (q);
}

void ParseCMDLine(char *line, cmd_t *cmd)
{
	char	*p;
	cmd->graphid = atoi(strtok_r(line," \t\n",&p));
	strcpy(cmd->oper, strtok_r(NULL," \t\n",&p));
	strcpy(cmd->rel_tag, strtok_r(NULL," \t\n",&p));
	p = ParseElement(p,cmd->dep,&(cmd->iddep));
/*
	strcpy(cmd->dep, strtok_r(NULL,"- \t\n",&p));
	cmd->iddep = atoi(strtok_r(NULL," \t\n",&p));
*/
	if (strcmp(cmd->oper, "REN_TAG") != 0) {
		ParseElement(p,cmd->head,&(cmd->idhead));
/*
		strcpy(cmd->head, strtok_r(NULL,"- \t\n",&p));
		cmd->idhead = atoi(strtok_r(NULL," \t\n",&p));
*/
	}
	else {
		strcpy(cmd->head, "_DUMMY_");
		cmd->idhead = 1;
	}
}

int main(int argc, char **argv)
{
	int			j,i,nconllu,graphid;
	conllu_t	*conllu;
	cmd_t		cmd;
	char		line[2000];
	FILE		*fd;

	if (argc < 2) {
		fprintf(stderr,"USAGE: %s <IN treebank CoNLL-U file>  <  <Command List>\n",argv[0]);
		exit(1);
	}

	conllu = (conllu_t*)malloc(350000*sizeof(conllu_t));

	fd = fopen(argv[1],"r");
	nconllu = 0;
	while (fgets(line,2000,fd) != NULL) {
		if (line[0] == '#') {
			graphid = atoi(strtok(line,"# \t\n"));
			fprintf(stderr,"Reading Graph %d\n",graphid);
		}
		else
			ParseCoNLLULine(line ,conllu, &nconllu, graphid);
	}
	fclose(fd);

/*
for(j=0;j<nconllu;j++)
 fprintf(stderr,"%d %d %s %d %s\n",conllu[j].graphid,conllu[j].id,conllu[j].word,conllu[j].idrel,conllu[j].rel);
*/

	while (fgets(line,2000,stdin) != NULL) {
		ParseCMDLine(line, &cmd);
		fprintf(stderr,"Applying CMD: %d %s %s %s %d %s %d\n",cmd.graphid,cmd.oper,cmd.rel_tag,cmd.dep,cmd.iddep,cmd.head,cmd.idhead);

		j = 0;
		while ((j < nconllu) && 
				((conllu[j].graphid != cmd.graphid) ||
				(conllu[j].id != cmd.iddep)))
			j++;
		if (j < nconllu) {
			if (strcmp(cmd.oper, "DEL_REL") == 0) {
				conllu[j].idrel = 1;
				strcpy(conllu[j].rel, "_DUMMY_");
			}
			else
			if (strcmp(cmd.oper, "INS_REL") == 0) {
				conllu[j].idrel = cmd.idhead;
				strcpy(conllu[j].rel, cmd.rel_tag);
			}
			else
			if (strcmp(cmd.oper, "REN_TAG") == 0) {
				strcpy(conllu[j].posFG, cmd.rel_tag);
				strcpy(conllu[j].pos, cmd.rel_tag);
			}
			else {
				fprintf(stderr,"ERROR: Wrong command!!!\n");
				exit(1);
			}
		}
		else {
			fprintf(stderr,"ERROR: %s %d_%d not found!",cmd.oper,cmd.graphid,cmd.iddep);
			exit(1);
		}
	}
	// PRINTOUT FINAL DB
	int pr = 0;
	for(j = 0; j < nconllu; j++) {
		if (conllu[j].graphid != pr) {
			if (pr > 0)
				printf("\n");
			printf("# %d\n",conllu[j].graphid);
			pr = conllu[j].graphid;
		}
 		printf("%d\t%s\t%s\t%s\t%s\t%s\t%d\t%s\t_\t_\n",
				conllu[j].id, conllu[j].word, conllu[j].lemma, conllu[j].posFG,
				conllu[j].pos, conllu[j].morpho, conllu[j].idrel,
				conllu[j].rel);
	}
	printf("\n");
}
