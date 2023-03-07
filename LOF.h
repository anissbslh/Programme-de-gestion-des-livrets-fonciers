#ifndef LOF_H_INCLUDED
#define LOF_H_INCLUDED

#include "TOF.h"

#define ENRG_TAILLE_LOF 20
#define B_LOF 6
#define MAX_INDEX_TAILLE 200

//Declaration des strucures de types ==================================================================================>
//Type de l'enregistrement
typedef struct DLOF DLOF;
struct DLOF{
    char numero[10];
    char superficie[10];
};

//Type du bloc/buffer
typedef struct LOFBloc LOFBuffer;
struct LOFBloc{
    DLOF tab[B_LOF];
    int nb; //Nombre d'enregistrements insérés dans le bloc
    int suivant;
};

//Type de l'entete
typedef struct TypeEnteteLOF TypeEnteteLOF;
struct TypeEnteteLOF{
    int adr_premier; //Adresse du premier bloc
    int nb_inseres; //Nombre d'enregistrements insérés
    int nb_supprimes; //Nombre d'enregistrements supprimés (Supression logique)
    int nb_blocs; // Nombre total de blocs
    int adr_queue; // Adresse de la queue de la liste
    int pos_queue; // Dernière position libre dans la queue
};

//Type du fichier LOF
typedef struct FLOF FLOF;
struct FLOF{
    TypeEnteteLOF entete;
    FILE* fich;
};

//Decalration du type de la table d'index =======================================================================================
typedef struct DIndex DIndex;
struct DIndex{
    int cle; // Le champ clé
    int adr_i; // L'adresse du bloc
    int adr_j; // Le deplacement dans le bloc
};

typedef struct IndexTab IndexTab;
struct IndexTab{
    DIndex tab[MAX_INDEX_TAILLE];
    int taille;
};

//Implementation de la machine abstraite ========================================================================================
//Renvoie l'objet entete du fichier
int EnteteLOF(FLOF* fichier,int i)
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
void Aff_enteteLOF(FLOF* fichier,int i,int val)
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
void LireDirLOF(FLOF* fichier, int i, LOFBuffer* buf)
{
    fseek(fichier->fich,(sizeof(TypeEnteteLOF)+sizeof(LOFBuffer)*(i-1)),SEEK_SET); //Positionner le curseur à l'adresse i
    fread(buf,sizeof(LOFBuffer),1,fichier->fich); //Lire le bloc
}

//Ecrire le bloc à l'adresse i
void EcrireDirLOF(FLOF* fichier, int i, LOFBuffer* buf)
{
    fseek(fichier->fich,(sizeof(TypeEnteteLOF)+sizeof(LOFBuffer)*(i-1)),SEEK_SET); //Positionner le curseur à l'adresse i
    fwrite(buf,sizeof(LOFBuffer),1,fichier->fich); //Ecrire le bloc
}

//Ouvrir le fichier selon le mode
void OuvrirLOF(FLOF* fichier, char* nom_physique, char mode)
{
    if((mode == 'N') || (mode == 'n')) //Créer un nouveau fichier
    {
        fichier->fich = fopen(nom_physique,"wb+"); //Ouvrir le fichier en mode écriture (écrase le contenu du fichier s'il existe)
        //Initialiser tous les champs de l'entête
        Aff_enteteLOF(fichier,1,1);
        Aff_enteteLOF(fichier,2,0);
        Aff_enteteLOF(fichier,3,0);
        Aff_enteteLOF(fichier,4,0);
        Aff_enteteLOF(fichier,5,1);
        Aff_enteteLOF(fichier,6,0);
        fwrite(&(fichier->entete),sizeof(TypeEntete),1,fichier->fich); //Sauvegarder l'entête dans le fichier
    }
    else
    {
        if((mode == 'a') || (mode == 'A')) //Ouvrir un ancien fichier
        {
            fichier->fich = fopen(nom_physique,"rb+"); //Ouvrir le fichier en mode lecture/ecriture
            fread(&(fichier->entete),sizeof(TypeEnteteLOF),1,fichier->fich); //Lire l'entête et la charger dans le champ entête
        }
    }
}

//Ferme le fichier
void FermerLOF(FLOF* fichier)
{
    fseek(fichier->fich,0,SEEK_SET); //Positionne le curseur au début du fichier
    fwrite(&(fichier->entete),sizeof(TypeEnteteLOF),1,fichier->fich); //Sauvegarde l'entête
    fclose(fichier->fich); //Ferme le fichier
}

//Alloue un nouveau bloc et le chaine avec le reste
void AllocBlocLOF(FLOF* fichier)
{
    LOFBuffer* buf = malloc(sizeof(LOFBuffer)); //Allouer de l'espace mémoire pour le buffer
    LireDirLOF(fichier,EnteteLOF(fichier,5),buf); //Lire le dernier bloc de la liste
    buf->suivant = EnteteLOF(fichier,4) + 1; //Mettre à jour le champ suivant avec le nombre de blocs total de la liste + 1 (une nouvelle adresse)
    EcrireDirLOF(fichier,EnteteLOF(fichier,5),buf); //Ecrire le dernier bloc
    buf->suivant = -1; //Initialiser le champ suivant à -1 (NIL)
    buf->nb = 0; //Initialiser le nombre d'enregistrements dans le bloc à 0
    EcrireDirLOF(fichier,EnteteLOF(fichier,4) + 1,buf); //Ecrire le buffer à l'adresse (nombre de blocs total de la liste + 1)
    Aff_enteteLOF(fichier,4,EnteteLOF(fichier,4)+1); //Incrémenter le nombre total de blocs de la liste
    Aff_enteteLOF(fichier,5,EnteteLOF(fichier,4)); //Mettre à jour le champ qui correspond à l'adresse du dernier bloc de la liste
}

//Procédure pour afficher tous les blocs du fichier
void AfficherFichierLOF(FLOF* fichier,char* nom_physique)
{
    int i,j;
    int stop = 0;
    LOFBuffer buf;
    OuvrirLOF(fichier,nom_physique,'A'); //Ouvrir le fichier en mode ancien
    i = EnteteLOF(fichier,1);
    j = 0;
    while(stop == 0) //Tant qu'on n'est pas arrivé à la fin du fichier
    {
        LireDirLOF(fichier,i,&buf); //Lire le bloc
        if(j<buf.nb) //Si la posirtion est infèrieure au nombre d'enregistrements dans le bloc
        {
            //Afficher les champs de l'enregistrement
            printf("Numero : %s\n",buf.tab[j].numero);
            printf("Superficie : %s\n",buf.tab[j].superficie);
            j++;
        }
        else //Si l'enregistrement se situe dans le prochain bloc
        {
            i = buf.suivant; //Passer au prochain bloc
            if(i!=-1) //S'il existe
            {
                LireDirLOF(fichier,i,&buf); //Lire ce bloc
                j = 0;
            }
            else //Sinon on arrete l'affichage
                stop = 1;
        }
        if((i==EnteteLOF(fichier,5))&&(j>=EnteteLOF(fichier,6))) //Si on arrive à la fin du fichier on arrete l'affichage
            stop = 1;
    }
    FermerLOF(fichier); //Fermer le fichier
}

//Procédure pour créer le fichier LOF voulu
void SauvLOF(FTOF* fichier,FLOF* nouveau_fichier,char* nom_physique,char* nouveau_nom,float u,IndexTab* index_tab)
{
    int i,j,k,index_i,index_j;
    DLOF nouvelle_donnee;
    DIndex nouveau_index;
    TOFBuffer buf;
    LOFBuffer buf2;
    i = 1;
    j = 0;
    k = 0;
    OuvrirTOF(fichier,nom_physique,'A');//Ouvrir le fichier TOF en mode ancien
    OuvrirLOF(nouveau_fichier,nouveau_nom,'N');//Ouvrir le fichier LOF en mode nouveau
    AllocBlocLOF(nouveau_fichier);//Allouer le premier bloc du nouveau fichier
    index_tab->taille = 1;//Initialiser la taille de la table d'index à 1
    index_i = EnteteLOF(nouveau_fichier,5);//Charger l'adresse du nouveau bloc qui a été créer
    index_j = EnteteLOF(nouveau_fichier,6);//Chager la première position du nouveau bloc
    LireDirLOF(nouveau_fichier,index_i,&buf2);//Lire le nouveau bloc
    FermerLOF(nouveau_fichier);//Fermer le nouveau fichier
    while((i!=EnteteTOF(fichier,5))||(j<EnteteTOF(fichier,6)))//Tant qu'on n'est pas arrivé à la fin de l'ancien bloc
    {
        LireDirTOF(fichier,i,&buf); //Lire la bloc
        if(j<B_TOF) //Si la position est inférieure à la taille du bloc
        {
            if(buf.tab[j].type == 'T') //Si le type est égal à Terrain
            {
                OuvrirLOF(nouveau_fichier,nouveau_nom,'A'); //Ouvrir le nouveau fichier en mode ancien
                //Copier les champs dans la nouvelle donnée
                strcpy(nouvelle_donnee.numero,buf.tab[j].numero);
                strcpy(nouvelle_donnee.superficie,buf.tab[j].superficie);

                if(index_j<(int)(B_LOF*u)) //Si la position ne dépasse pas la limite du bloc
                {
                    buf2.tab[index_j] = nouvelle_donnee; //Insérer la nouvelle donnée
                    buf2.nb += 1; //Incrémenter le nombre de données dans le bloc

                    //Charger les champs dans la table d'index
                    nouveau_index.cle = atoi(buf2.tab[index_j].numero);
                    nouveau_index.adr_i = index_i;
                    nouveau_index.adr_j = index_j;

                    index_tab->tab[k] = nouveau_index;
                    index_j++;
                }
                else //Si la position dépasse la limite du bloc
                {
                    k++;
                    index_tab->taille += 1; //On incrémente la taille de la table d'index
                    EcrireDirLOF(nouveau_fichier,index_i,&buf2); //On écrit le bloc
                    AllocBlocLOF(nouveau_fichier); //On alloue un nouveau bloc
                    index_i = EnteteLOF(nouveau_fichier,5); //On charge l'adresse du dernier bloc
                    index_j = 0; //Réinitialiser la position à 0
                    LireDirLOF(nouveau_fichier,index_i,&buf2); //On lit le nouveau bloc
                }
                EcrireDirLOF(nouveau_fichier,index_i,&buf2); //On écrit le bloc
                FermerLOF(nouveau_fichier); //On ferme le nouveau fichier
            }
            j++;
        }
        else //Si la position est située dans le prochain bloc
        {
            if(i!=EnteteTOF(fichier,5)) //Si le courant n'est pas le dernier bloc de l'ancien fichier
            {
                i++; //On passe au prochain bloc
                LireDirTOF(fichier,i,&buf); //On lit ce bloc
                j = 0;//On réinitialise la position à 0
            }
        }
    }
    OuvrirLOF(nouveau_fichier,nouveau_nom,'A'); //Ouvrir le nouveau fichier en mode ancien
    Aff_enteteLOF(nouveau_fichier,6,index_j); //On met à jour la dernière position libre dans le dernier bloc dans l'entête
    FermerLOF(nouveau_fichier); //On ferme le nouveau fichier
    FermerTOF(fichier); //On ferme l'ancien fichier
}

//Procédure de recherche d'un livret dans le fichier LOF en utilisant la table d'index
void RechercheLOF(FLOF* fichier,char* nom_physique,char* numero,int* index_i,int* index_j,IndexTab* index_tab,int* index,int* trouv)
{
    int i,j;
    i = 1;
    j = 0;
    LOFBuffer buf;
    int bi,bs,milieu;
    int nb_numero,nb_buf_numero;
    *trouv = 0;
    nb_numero = atoi(numero); //Convertir le numero en entier
    bi = 0;
    bs = index_tab->taille - 1;
    int stop = 0;
    //Recherche dichotomique dans la table d'index
    while((bi<=bs)&&(stop == 0))
    {
        milieu = (bi+bs)/2;
        if(index_tab->tab[milieu].cle == nb_numero)
        {
            stop = 1;
            *trouv = 1;
        }
        else
        {
            if(index_tab->tab[milieu].cle < nb_numero)
            {
                bi = milieu + 1;
            }
            else
            {
                bs = milieu - 1;
            }
        }
    }
    i = index_tab->tab[milieu].adr_i;
    j = index_tab->tab[milieu].adr_j;

    if(nb_numero>index_tab->tab[milieu].cle) //Si le numéro qu'on recherche est supérieure à la valeur qui se trouve dans la table d'index à la position milieu
    {
        if(milieu+1<index_tab->taille) //On cherchera dans le bloc qui suit le bloc courant si il existe
        {
            i = index_tab->tab[milieu + 1].adr_i;
            j = index_tab->tab[milieu + 1].adr_j;
            milieu+=1;
        }
    }

    //Recherche dans le bloc du fichier LOF
    *index = milieu;
    *index_i = i;
    *index_j = 0;
    if(stop == 0)
    {
        OuvrirLOF(fichier,nom_physique,'A'); //On ouvre le fichier en mode ancien
        LireDirLOF(fichier,i,&buf); //Lire le bloc
        while((*index_j < buf.nb)&&(stop==0)) //Tant qu'on a pas fini la recherche
        {
            nb_buf_numero = atoi(buf.tab[*index_j].numero); //On convertit chaque numero lu en entier
            if(nb_buf_numero == nb_numero) //Si la valeur est trouvée
            {
                *trouv = 1; //On pose trouv à Vrai
                stop = 1; //Et stop à Vrai
            }
            else
            {
                if(nb_buf_numero < nb_numero) //Si la valeur recherchée est supérieure
                {
                    *index_j += 1; //On incrémente
                }
                else //Si la valeur recherchée est inférieure on stoppe la recherche
                {
                    stop = 1;
                }
            }
        }
        FermerLOF(fichier); //Fermer le fichier
    }
}

//Procédure pour insérer un nouveau livret en utilisant la table d'index
void InsertionLOF(FLOF* fichier,char* nom_physique,char* numero,char* superficie,IndexTab* index_tab)
{
    int i,j;
    int index;
    int sauv_der,sauvi,sauv_suiv;
    int trouv;
    DLOF donnee;
    DLOF temp_donnee;
    DIndex index_donnee;
    DIndex temp_index_donnee;
    LOFBuffer buf;
    RechercheLOF(fichier,nom_physique,numero,&i,&j,index_tab,&index,&trouv); //On procède à une recherche du numéro

    if(trouv == 0) //Si la valeur n'a pas été trouvée
    {
        OuvrirLOF(fichier,nom_physique,'A'); //Ouvrir le fichier en mode ancien
        //Charger les champs de la donnée
        strcpy(donnee.numero,numero);
        strcpy(donnee.superficie,superficie);
        LireDirLOF(fichier,i,&buf); //Lire le bloc

        //On insére la nouvelle donnée et on décale les anciennes
        while(j<buf.nb) //Tant que la position est inférieure au nombre d'enregistrements dans le bloc
        {
            temp_donnee = buf.tab[j]; //Sauvegarder l'enregistrement dans cette position
            buf.tab[j] = donnee; //Insérer la nouvelle donnée
            donnee = temp_donnee; //Copier la donnée sauvegardée dans la donnée courante
            j++; //Incrémenter la position
        }

        //Il reste la dernière donnée qui a été sauvegardée mais pas encore reinsérée(décalée)
        if(buf.nb+1<=B_LOF) //Si le bloc n'est pas plein
        {
            buf.tab[buf.nb] = donnee; //On insère la dernière donnée dans la dernière place libre
            j++; //Incrémente la position
            buf.nb += 1; //On incrémente le nombre d'enregistrements insérés dans le bloc
            //On met à jour la table d'index
            index_donnee.cle = atoi(buf.tab[buf.nb-1].numero);
            index_donnee.adr_i = i;
            index_donnee.adr_j = buf.nb-1;
            index_tab->tab[index] = index_donnee;
            EcrireDirLOF(fichier,i,&buf);//On écrit le bloc
        }
        else //Si le bloc est plein
        {
            //On met à jour la table d'index
            index_donnee.cle = atoi(buf.tab[buf.nb-1].numero);
            index_donnee.adr_i = i;
            index_donnee.adr_j = buf.nb-1;
            index_tab->tab[index] = index_donnee;
            index++;

            sauv_der = EnteteLOF(fichier,5); //On récupère l'adresse du dernier bloc
            sauvi = i; //On sauvegarde l'adresse courante
            sauv_suiv = buf.suivant; //On sauvegarde l'adresse du bloc suivant le bloc courant
            AllocBlocLOF(fichier); //On alloue un nouveau bloc à la fin du fichier
            i = EnteteLOF(fichier,5); //On récupère l'adresse du nouveau bloc
            buf.suivant = i; //On met à jour le champ suivant du bloc courant à la valeur de l'adresse du nouveau bloc alloué
            EcrireDirLOF(fichier,sauvi,&buf); //On écrit le bloc à l'adresse précedemment sauvegarder (l'adresse du bloc courant)
            LireDirLOF(fichier,i,&buf); //Lire le nouveau bloc
            j = 0; //Initialiser la position à 0
            buf.tab[j] = donnee; //On insère la dernière donnée dans cette position
            buf.suivant = sauv_suiv; //On affecte au champ suivant du bloc l'adresse suivant précedemment sauvegardé comme cela on a chainé correctemment les blocs
            buf.nb = 1; //On pose le nombre d'enregistrement dans ce bloc est à 1
            if(buf.suivant != -1) //Si le nouveau bloc n'est pas le dernier bloc de la liste
            {
                Aff_enteteLOF(fichier,5,sauv_der); //On reaffecte l'adresse du dernier bloc qu'on a sauvegarder
            }
            EcrireDirLOF(fichier,i,&buf);//On écrit le bloc
            //On charge une nouvelle case dans la table d'index
            index_donnee.cle = atoi(buf.tab[j].numero);
            index_donnee.adr_i = i;
            index_donnee.adr_j = j;
            //On insère la nouvelle case tout en décalant les anciennes
            while(index<index_tab->taille)
            {
                temp_index_donnee = index_tab->tab[index];
                index_tab->tab[index] = index_donnee;
                index_donnee = temp_index_donnee;
                index++;
            }
            if(index<MAX_INDEX_TAILLE) //Si on n'est pas arrivé à la limite de la table
            {
                index_tab->tab[index] = index_donnee; //On insére la dernière case qui reste
                index_tab->taille += 1; //On incrémente la taille de la table d'index
            }
            else
            {
                printf("\t\t\tERREUR: Debordement du tableau d'index\n");
            }
            j++; //On incrémente la position
        }

        if(buf.suivant == -1) //Si le dernier bloc inséré est le dernier de la liste
        {
            Aff_enteteLOF(fichier,6,j); //On met à jour le dernier caractère du dernier bloc de la liste
        }
        FermerLOF(fichier); //Fermer le fichier
    }
}

//Procédure pour sauvegarder la table d'index dans un fichier index
void SauvegardeIndex(char* nom_physique,IndexTab* index_tab)
{
    FILE* fichier;
    fichier = fopen(nom_physique,"wb+"); //Ouvrir le fichier en mode nouveau (écriture)
    fseek(fichier,0,SEEK_SET); //Positionner le cursor au début du fichier
    fwrite(&(index_tab->taille),sizeof(int),1,fichier); //Ecrire la taille de la table
    for(int i=0;i<index_tab->taille;i++) //Pour chaque élément du tableau écrire cet élément
    {
        fwrite(&(index_tab->tab[i]),sizeof(DIndex),1,fichier);
    }
    fclose(fichier); //Fermer le fichier
}

//Procédure pour chager la table d'index depuis un fichier index
void ChargerIndex(char* nom_physique,IndexTab* index_tab)
{
    FILE* fichier;
    fichier = fopen(nom_physique,"rb+"); //Ouvrir le fichier en mode ancien (lecture)
    fseek(fichier,0,SEEK_SET); //Positionner le cursor au début du fichier
    fread(&(index_tab->taille),sizeof(int),1,fichier); //Lire la taille de la table
    for(int i=0;i<index_tab->taille;i++) //Pour chaque élément du tableau lire cet élément
    {
        fread(&(index_tab->tab[i]),sizeof(DIndex),1,fichier);
    }
    fclose(fichier); //Fermer le fichier
}

//Procédure pour afficher la table d'index
void AfficherIndex(IndexTab* index_tab)
{
    printf("\n\n\t\t\tContenu de la table d'index : \n");
    for(int i=0;i<index_tab->taille;i++) //Pour chaque élément de la table afficher ses champs
    {
        printf("\t\t\tCle: %d | Adr_i: %d | Adr_j: %d\n",index_tab->tab[i].cle,index_tab->tab[i].adr_i,index_tab->tab[i].adr_j);
    }
}

#endif // LOF_H_INCLUDED
