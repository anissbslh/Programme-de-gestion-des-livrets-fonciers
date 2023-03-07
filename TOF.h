#ifndef TOF_H_INCLUDED
#define TOF_H_INCLUDED
#include "LOVC.h"

#define ENRG_TAILLE_TOF 21
#define B_TOF 20
//Declaration des strucures de types ==================================================================================>
//Type de l'enregistrement
typedef struct DTOF DTOF;
struct DTOF{
    char numero[10];
    char type;
    char superficie[10];
};

//Type du bloc/buffer
typedef struct TOFBloc TOFBuffer;
struct TOFBloc{
    DTOF tab[B_TOF];
};

//Type de l'entete
typedef struct TypeEnteteTOF TypeEnteteTOF;
struct TypeEnteteTOF{
    int adr_premier; //Adresse du premier bloc
    int nb_inseres; //Nombre d'enregistrement insérés
    int nb_supprimes; //Nombre d'enregistrements supprimés (Supression logique)
    int nb_blocs; // Nombre total de blocs
    int adr_queue; // Adresse du dernier bloc du tableau
    int pos_queue; // Dernière position libre dans le dernier bloc du tableau
};

//Type du fichier TOF
typedef struct FTOF FTOF;
struct FTOF{
    TypeEntete entete;
    FILE* fich;
};

//Implementation de la machine abstraite ========================================================================================
//Renvoie l'objet entete du fichier
int EnteteTOF(FTOF* fichier,int i)
{
    switch(i)
    {
        case 1: return fichier->entete.adr_premier;
        case 2: return fichier->entete.nb_inseres;
        case 3: return fichier->entete.nb_supprimes;
        case 4: return fichier->entete.nb_blocs;
        case 5: return fichier->entete.adr_queue;
        case 6: return fichier->entete.pos_queue;
        default:{
            printf("\t\t\tERREUR: Cette position dans l'entete n'existe pas\n");
            return 0;
        }
    }
}

//Affecte à un champ de l'entete une certaine valeur
void Aff_enteteTOF(FTOF* fichier,int i,int val)
{
    switch(i)
    {
        case 1: {
            fichier->entete.adr_premier = val;
            break;
        }
        case 2: {
            fichier->entete.nb_inseres = val;
            break;
        }
        case 3: {
            fichier->entete.nb_supprimes = val;
            break;
        }
        case 4: {
            fichier->entete.nb_blocs = val;
            break;
        }
        case 5: {
            fichier->entete.adr_queue = val;
            break;
        }
        case 6: {
            fichier->entete.pos_queue = val;
            break;
        }
        default: {
            printf("\t\t\tERREUR: Cette position dans l'entete n'existe pas\n");
            break;
        }
    }
}

//Lire le bloc d'adresse i
void LireDirTOF(FTOF* fichier, int i, TOFBuffer* buf)
{
    fseek(fichier->fich,(sizeof(TypeEnteteTOF)+sizeof(TOFBuffer)*(i-1)),SEEK_SET); //Positionner le curseur à l'adresse i
    fread(buf,sizeof(TOFBuffer),1,fichier->fich); //Lire le bloc
}

//Ecrire le bloc à l'adresse i
void EcrireDirTOF(FTOF* fichier, int i, TOFBuffer* buf)
{
    fseek(fichier->fich,(sizeof(TypeEnteteTOF)+sizeof(TOFBuffer)*(i-1)),SEEK_SET); //Positionner le curseur à l'adresse i
    fwrite(buf,sizeof(TOFBuffer),1,fichier->fich); //Ecrire le bloc
}

//Ouvrir le fichier selon le mode
void OuvrirTOF(FTOF* fichier, char* nom_physique, char mode)
{
    if((mode == 'N') || (mode == 'n')) //Créer un nouveau fichier
    {
        fichier->fich = fopen(nom_physique,"wb+"); //Ouvrir le fichier en mode écriture (écrase le contenu du fichier s'il existe)
        //Initialiser tous les champs de l'entête
        Aff_enteteTOF(fichier,1,1);
        Aff_enteteTOF(fichier,2,0);
        Aff_enteteTOF(fichier,3,0);
        Aff_enteteTOF(fichier,4,0);
        Aff_enteteTOF(fichier,5,1);
        Aff_enteteTOF(fichier,6,0);
        fwrite(&(fichier->entete),sizeof(TypeEnteteTOF),1,fichier->fich); //Sauvegarder l'entête dans le fichier
    }
    else
    {
        if((mode == 'a') || (mode == 'A')) //Ouvrir un ancien fichier
        {
            fichier->fich = fopen(nom_physique,"rb+"); //Ouvrir le fichier en mode lecture/ecriture
            fread(&(fichier->entete),sizeof(TypeEnteteTOF),1,fichier->fich); //Lire l'entête et la charger dans le champ entête
        }
    }
}

//Ferme le fichier
void FermerTOF(FTOF* fichier)
{
    fseek(fichier->fich,0,SEEK_SET); //Positionne le curseur au début du fichier
    fwrite(&(fichier->entete),sizeof(TypeEnteteTOF),1,fichier->fich); //Sauvegarde l'entête
    fclose(fichier->fich); //Ferme le fichier
}

//Alloue un nouveau bloc et le chaine avec le reste
void AllocBlocTOF(FTOF* fichier)
{
    TOFBuffer* buf = malloc(sizeof(TOFBuffer)); //Allouer de l'espace mémoire pour le buffer
    EcrireDirTOF(fichier,EnteteTOF(fichier,4) + 1,buf);  //Ecrire le nouveau bloc
    Aff_enteteTOF(fichier,4,EnteteTOF(fichier,4)+1); //Incrémenter le nombre total de blocs du tableau
    Aff_enteteTOF(fichier,5,EnteteTOF(fichier,4)); //Mettre à jour le champ qui correspond à l'adresse du dernier bloc du tableau
}

//Cette procédure affiche tous les blocs du fichier TOF
void AfficherFichierTOF(FTOF* fichier,char* nom_physique)
{
    int i,j;
    TOFBuffer buf;
    i = 1;
    j = 0;
    OuvrirTOF(fichier,nom_physique,'A'); //Ouvrir le fichier en mode ancien
    while((i!=EnteteTOF(fichier,5))||(j<EnteteTOF(fichier,6)))
    {
        LireDirTOF(fichier,i,&buf); //Lire le bloc
        if(j<B_TOF) //Si la position est inférieure à la taille du bloc
        {
            //Afficher les données de l'enregistrement
            printf("Numero : %s\n",buf.tab[j].numero);
            printf("Type : %c\n",buf.tab[j].type);
            printf("Superficie : %s\n",buf.tab[j].superficie);
            j++;
        }
        else //Sinon passer au bloc suivant
        {
            if(i!=EnteteTOF(fichier,5)) //Si le bloc courant n'est pas le dernier bloc du tableau
            {
                i++; //Passer au bloc suivant
                LireDirTOF(fichier,i,&buf); //Lire le bloc
                j = 0; //Réinitialiser la position à 0
            }
        }
    }
    FermerTOF(fichier); //Fermer le fichier
}

//Cette procédure créée le fichier TOF voulu
void SauvTOF(Fichier* fichier,FTOF* nouveau_fichier,char* nom_physique,char* nouveau_nom,char* wilaya)
{
    Buffer buf;
    TOFBuffer buf2;
    DTOF nouvelle_donnee;
    int i,j,index_i,index_j;
    int nb_taille;
    char* d;
    char taille[NB_TAILLE];
    char numero[10];
    char wilaya_ch[14];
    char type;
    char superficie[10];
    char eff;
    index_i = 1;
    index_j = 0;
    int stop = 0;
    Ouvrir(fichier,nom_physique,'A'); //Ouvrir le fichier LOVC en mode ancien
    i = Entete(fichier,1);
    j = 0;
    OuvrirTOF(nouveau_fichier,nouveau_nom,'N'); //Ouvrir le nouveau fichier TOF en mode nouveau
    AllocBlocTOF(nouveau_fichier); //Allouer le premier bloc du nouveau fichier
    index_i = EnteteTOF(nouveau_fichier,5); //Mettre à jour le champ adresse du dernier bloc
    index_j = EnteteTOF(nouveau_fichier,6); //Mettre à jour le champ dernière position libre dans le dernier bloc
    FermerTOF(nouveau_fichier); //Fermer le nouveau fichier
    while(stop == 0)
    {
        LireDir(fichier,i,&buf); //Lire le bloc de l'ancien fichier
        //Récupérer les champs
        RecupChamp(fichier,NB_TAILLE,&buf,&i,&j,taille);
        RecupChamp(fichier,1,&buf,&i,&j,&eff);
        RecupChamp(fichier,10,&buf,&i,&j,numero);
        RecupChamp(fichier,14,&buf,&i,&j,wilaya_ch);
        RecupChamp(fichier,1,&buf,&i,&j,&type);
        RecupChamp(fichier,10,&buf,&i,&j,superficie);
        nb_taille = atoi(taille);
        d = (char*)malloc(sizeof(char)*(nb_taille-36-NB_TAILLE));
        RecupChaine(fichier,nb_taille-36-NB_TAILLE,&buf,&i,&j,d);
        if(eff == '0') //Si la donnée n'est pas effacée
        {
            //Copier les champs dans la nouvelle donnée pour l'insérer
            strcpy(nouvelle_donnee.numero,numero);
            nouvelle_donnee.type = type;
            strcpy(nouvelle_donnee.superficie,superficie);
            OuvrirTOF(nouveau_fichier,nouveau_nom,'A'); //Ouvrir le nouveau fichier en mode ancien
            LireDirTOF(nouveau_fichier,index_i,&buf2); //Lire le bloc
            if(strcmp(wilaya,wilaya_ch) == 0) //Si la wilaya qu'on recherche est la même que la donnée lue
            {
                //On insère la nouvelle donnée
                if(index_j>=B_TOF) //Si la position est supérieure à la taille du bloc
                {
                    EcrireDirTOF(nouveau_fichier,index_i,&buf2); //Ecrire le bloc
                    AllocBlocTOF(nouveau_fichier); //Allouer un nouveau bloc
                    index_i = EnteteTOF(nouveau_fichier,5); //Mettre à jour à l'adresse du dernier bloc
                    index_j = 0; //Réinitialiser la position à 0
                    LireDirTOF(nouveau_fichier,index_i,&buf2);//Lire le nouveau bloc
                }
                buf2.tab[index_j] = nouvelle_donnee;//Insérer la nouvelle donnée
                index_j++;
            }
            EcrireDirTOF(nouveau_fichier,index_i,&buf2); //Ecrire le bloc
            FermerTOF(nouveau_fichier); //Fermer le fichier
        }
        if((i == Entete(fichier,5))&&(j >= Entete(fichier,6))) //Si on arrive à la fin de l'ancien fichier
        {
            stop = 1; //On stoppe la création
            OuvrirTOF(nouveau_fichier,nouveau_nom,'A');
            Aff_enteteTOF(nouveau_fichier,6,index_j); //On met à jour la dernière position libre dans le dernier bloc dans l'entête
            FermerTOF(nouveau_fichier);
        }
    }
    Fermer(fichier); //On ferme l'ancien fichier
}
#endif // TOF_H_INCLUDED
