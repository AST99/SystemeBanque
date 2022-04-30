#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAXNOM 20
/* notre programme permet de gerer un compte bancaire*/
/* creation des structures */

typedef struct date {
  int jour;
  int mois;
  int annee;
} DATE;

typedef struct client {
  int numero_cpt;
  char nom[MAXNOM];
  char der_operation;
  double anc_solde;
  double nouv_solde;
  DATE date;
} CLIENT;
  /*  fonction qui gere les inforamtions */
void getDate(DATE *d) {
  time_t nsec;
  struct tm *temps;
  nsec = time(NULL);
  temps = localtime(&nsec);
  d->jour = temps-> tm_mday;
  d->mois = temps-> tm_mon + 1;
  d->annee = temps-> tm_year + 1900;
}
/*ouverture de notre fichier*/
void ouvrir(FILE **f, char nomfich[20]) {
  *f = fopen(nomfich, "r+");
  if (*f == NULL) {
    *f = fopen(nomfich, "w+");
    if (*f == NULL) {
      perror("Erreur d'ouverture du fichier");
      exit(EXIT_FAILURE);
    }
  }
}
/* fermerture de notre fichier*/
void fermer(FILE *fich) {
  if (fich != NULL) {
    fclose(fich);
  }
}
 /* chercher notre clients alpha*/
int chercher_nom(FILE *fich, char *nom) {
  CLIENT client;
  int trouve = 0, ret;
  rewind(fich);
  while (! trouve) {
    ret = fread(&client, sizeof(CLIENT), 1, fich);
    if (ret == 0) break;
    if (strcmp(client.nom, nom) == 0) {
      fseek(fich, -1l * sizeof(CLIENT), SEEK_CUR);
      return 1;
    }
  }
  return 0;
}
 /* fonction pour chercher des comptes*/
int chercher_compte(FILE *fich, int cpt) {
  CLIENT client;
  int trouve = 0, ret;
  rewind(fich);
  while (! trouve) {
    ret = fread(&client, sizeof(CLIENT), 1, fich);
    if (ret == 0) break;
    if (client.numero_cpt == cpt) {
	fseek(fich, -1l * sizeof(CLIENT), SEEK_CUR);
	return 1;
    }
  }
  return 0;
}
   /* afficher sur ecran les differentes informations que l utisateur devra inscrire dans le fichier */
int ajout(FILE *fich) {

  int ret;
  char *pc;
  CLIENT client;
  printf("Ajout d'un client\n");
  printf("\tNumero de compte : ");
  scanf("%d", &client.numero_cpt);
  if (chercher_compte(fich, client.numero_cpt)) {
    fprintf(stderr, "Compte existant\n");
    return 0;
  }
  getchar();
  printf("\tNom : ");
  fgets(client.nom, MAXNOM, stdin); pc = strchr(client.nom, '\n'); *pc = 0;
  if (chercher_nom(fich, client.nom)) {
    fprintf(stderr, "Nom existant\n");
    return 0;
  }
  client.der_operation = 'V';
  client.anc_solde = 0.0;
  printf("\tSolde initial : ");
  scanf("%lf", &client.nouv_solde);
  getDate(&client.date);
  fseek(fich, 0, SEEK_END);
  ret = fwrite(&client, sizeof(CLIENT), 1, fich);
  return ret;
}
/* verification des conditions */
void affiche(FILE *fich) {
  CLIENT cli;
  char nom[MAXNOM];
  int cpt, ret;
  char choix;
 // printf("----------------------------------------------------------\n");
  printf("      quel moyen de recherche vous conviendrais ?\n      ");
  printf("                    nom ou compte\n                   ");
  do {
    printf("votre nom (n) ou votre compte (c) ? \n ");
                   printf("               votre choix   :                         ");
    scanf(" %c", &choix);
  } while (choix != 'n' && choix != 'c');
  if (choix == 'c') {
    printf(" votre Numero du compte : ");
    scanf("%d", &cpt);
    ret = chercher_compte(fich, cpt);
  }
  else {
    printf(" votre Nom du compte : ");
    scanf("%s", nom);
    ret = chercher_nom(fich, nom);
  }
  if (ret == 0) {
    printf("Compte invalide  ou nom inexistant !!!!!!!!!!!!!!\n");
  }
  else {
    fread(&cli, sizeof(CLIENT), 1, fich);
    printf("Compte %d\nNom %s\nDerniere operation %c\nAncien solde %.2f\nNouveau solde %.2f\nDate %d/%d/%d\n", cli.numero_cpt,cli.nom,
	   cli.der_operation, cli.anc_solde, cli.nouv_solde, cli.date.jour,
	   cli.date.mois, cli.date.annee);
  }
}
 /* fonctions permettant de lister les differentes informations des  comptes */
void lister(FILE *fich) {
  CLIENT cli;
  rewind(fich);
  printf("voici les differents comptes sur le systeme .\n");
  printf("Num\tNom\tOpe\tAnc\tNouv\tDate\n");
  while (fread(&cli, sizeof(CLIENT), 1, fich) == 1) {
    printf("%d\t%s\t%c\t%.2f\t%.2f\t%d/%d/%d\n", cli.numero_cpt,cli.nom,
	   cli.der_operation, cli.anc_solde, cli.nouv_solde, cli.date.jour,
	   cli.date.mois, cli.date.annee);
  }
}
 /* fonction  consultation des differents comptes */
void operation(FILE *fich) {
  CLIENT cli;
  char choix;
  double somme;
  printf(" votre Numero du compte : ");
  scanf("%d", &cli.numero_cpt);
  if (!chercher_compte(fich, cli.numero_cpt)) {
    printf(" Compte inexistant...\n");
    return;
  }
  fread(&cli, sizeof(CLIENT), 1, fich);
  printf("Compte %d\nNom %s\nAncien solde %.2f\nNouveau solde %.2f\nDate %d/%d/%d\n",
	 cli.numero_cpt,cli.nom, cli.anc_solde, cli.nouv_solde,
	 cli.date.jour, cli.date.mois, cli.date.annee);
  printf("Que voulez-vous faire ?\n");
  do {
    printf(" Versement : V\n Retrait   : R\n   Votre choix : ");
    scanf(" %c", &choix);
  } while (choix != 'V' && choix != 'R');
  cli.anc_solde = cli.nouv_solde;
  printf("   Somme : ");
  scanf("%lf", &somme);
  if (somme < 0.0) {
    somme = -somme;
  }
  if (choix == 'V') {
    cli.der_operation = 'V';
    cli.nouv_solde += somme;
  }
  else {
    cli.der_operation = 'R';
    cli.nouv_solde -= somme;
  }
  getDate(&cli.date);
  fseek(fich, -1l * sizeof(CLIENT), SEEK_CUR);
  fwrite(&cli, sizeof(CLIENT), 1, fich);
}
/* remplissage d'autre information relatif aux comptes*/
void menu(FILE *fic) {
  char choix;
  do {

        printf("--------------------- bonjour Monsieur/Madame entrez votre choix s'il vous plait !! ---------------------");


    printf("\n\nAjouter d'un nouveau client  ? ...........: A\n");
    printf("\nConsultation d'un compte client  ? .......: C\n");
    printf("\nLister tous les comptes des clients  ? ...: L\n");
    printf("\nOperation sur un compte client  ? ........: O\n");
    printf("\nQuitter  ? ...............................: Q\n\n");
                    printf("               votre choix : ");
    scanf(" %c", &choix);
    printf("\n");system("cls");
    /* teste des choix */
    switch(choix) {
    case 'a':
    case 'A':
      ajout(fic);
      break;
    case 'c':
    case 'C':
      affiche(fic);
      break;
    case 'l':
    case 'L':
      lister(fic);
    break;
    case 'o':
    case 'O':
      operation(fic);
    break;
    case 'q':
    case 'Q':
      break;
    default:
      printf("Oups !!!!! les informations entrez ne peuvent etre traitez veillez recommencer\n");
      break;
    }
  }
  while (choix != 'q' && choix != 'Q');
}
 /* sauvegarde les informations dans un fichier*/
int main() {
  FILE *fich;
  ouvrir(&fich, "repert.dat");
  menu(fich);
  fermer(fich);
  return 0;
}

