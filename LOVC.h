#ifndef LOVC_H_INCLUDED
#define LOVC_H_INCLUDED

#define B 1000 //Doit etre au moins 36 + NB_TAILLE
#define NB_TAILLE 10
//Declaration des strucures de types ==================================================================================>
//Type de l'enregistrement
typedef struct Donnee Donnee;
struct Donnee{
    char numero[10]; //Le num�ro du livret sur 10 carat�res
    char wilaya[14]; //La wilaya sur 14 caract�res
    char type; //Le type T/M/A/I
    char superficie[10]; //La superficie sur 10 caract�res
    char* observation; //Observation: Un champ de taille variable
    char taille[NB_TAILLE]; //Un champ pour sauvegarder la taille de l'enregistrement
    char eff; //Un champ qui � 0 signifie que l'enregistrement n'est pas supprim� et � 1 dans le cas contraire
};

//Type du bloc/buffer
typedef struct TBloc Buffer;
struct TBloc{
    char tab[B]; //Un tableau de taille B qui contient les enregistrements
    int suivant; //Un champ pour l'adresse du suivant bloc
};

//Type de l'entete
typedef struct TypeEntete TypeEntete;
struct TypeEntete{
    int adr_premier; //Adresse du premier bloc
    int nb_inseres; //Nombre de caract�res ins�r�s
    int nb_supprimes; //Nombre de caract�res supprim�s (Supression logique)
    int nb_blocs; // Nombre total de blocs
    int adr_queue; // Adresse de la queue de la liste
    int pos_queue; // Derni�re position libre dans la queue
};

//Type du fichier LOVC
typedef struct Fichier Fichier;
struct Fichier{
    TypeEntete entete; //Le champ de l'entete
    FILE* fich; //Le fichier
};

//D�finition des ensembles ======================================================================================================
char types[4] = {'T','M','A','I'}; //Ensemble des types possibles
char wilayas[58][14]={"Adrar","Chlef","Laghouat","OumElBouaghi","Batna","Bejaia","Biskra","Bechar","Blida","Bouira","Tamanrasset",
"Tebessa","Tlemcen","Tiaret","Tizi_Ouzou","Alger","Djelfa","Jijel","Setif","Saida","Skikda","Sidi_Bel_Abbes","Annaba","Guelma",
"Constantine","Medea","Mostaganem","M'Sila","Mascara","Ouargla","Oran","Bayadh","Illizi","BorjBouAreridj","Boumerdes","El_Taref",
"Tindouf","Tissemsilt","El_Oued","Khenchela","Souk_Ahras","Tipaza","Mila","Defla","Naama","Temouchent","Ghardaia","Relizane",
"Timimoun","Bordj_Badji","Ouled_Djellal","Beni_Abbes","In_Salah","In_Guezzam","Touggourt","Djanet","M'Ghair","Meniaa"}; //Ensemble des wilayas possibles

//Implementation de la machine abstraite ========================================================================================
//Renvoie pour chaque numero le champ de l'entete qui correspond
int Entete(Fichier* fichier,int i)
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

//Affecte � un champ de l'entete une certaine valeur
void Aff_entete(Fichier* fichier,int i,int val)
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
void LireDir(Fichier* fichier, int i, Buffer* buf)
{
    fseek(fichier->fich,(sizeof(TypeEntete)+sizeof(Buffer)*(i-1)),SEEK_SET); //Positionner le curseur � l'adresse i
    fread(buf,sizeof(Buffer),1,fichier->fich); //Lire le bloc
}

//Ecrire le bloc � l'adresse i
void EcrireDir(Fichier* fichier, int i, Buffer* buf)
{
    fseek(fichier->fich,(sizeof(TypeEntete)+sizeof(Buffer)*(i-1)),SEEK_SET); //Positionner le curseur � l'adresse i
    fwrite(buf,sizeof(Buffer),1,fichier->fich); //Ecrire le bloc
}

//Ouvrir le fichier selon le mode
void Ouvrir(Fichier* fichier, char* nom_physique, char mode)
{
    if((mode == 'N') || (mode == 'n')) //Cr�er un nouveau fichier
    {
        fichier->fich = fopen(nom_physique,"wb+"); //Ouvrir le fichier en mode �criture (�crase le contenu du fichier s'il existe)
        //Initialiser tous les champs de l'ent�te
        Aff_entete(fichier,1,1);
        Aff_entete(fichier,2,0);
        Aff_entete(fichier,3,0);
        Aff_entete(fichier,4,0);
        Aff_entete(fichier,5,1);
        Aff_entete(fichier,6,0);
        fwrite(&(fichier->entete),sizeof(TypeEntete),1,fichier->fich); //Sauvegarder l'ent�te dans le fichier
    }
    else
    {
        if((mode == 'a') || (mode == 'A')) //Ouvrir un ancien fichier
        {
            fichier->fich = fopen(nom_physique,"rb+"); //Ouvrir le fichier en mode lecture/ecriture
            fread(&(fichier->entete),sizeof(TypeEntete),1,fichier->fich); //Lire l'ent�te et la charger dans le champ ent�te
        }
    }
}

//Ferme le fichier
void Fermer(Fichier* fichier)
{
    fseek(fichier->fich,0,SEEK_SET); //Positionne le curseur au d�but du fichier
    fwrite(&(fichier->entete),sizeof(TypeEntete),1,fichier->fich); //Sauvegarde l'ent�te
    fclose(fichier->fich); //Ferme le fichier
}

//Alloue un nouveau bloc et le chaine avec le reste
void AllocBloc(Fichier* fichier)
{
    Buffer* buf = malloc(sizeof(Buffer)); //Allouer de l'espace m�moire pour le buffer
    LireDir(fichier,Entete(fichier,5),buf); //Lire le dernier bloc de la liste
    buf->suivant = Entete(fichier,4) + 1; //Mettre � jour le champ suivant avec le nombre de blocs total de la liste + 1 (une nouvelle adresse)
    EcrireDir(fichier,Entete(fichier,5),buf); //Ecrire le dernier bloc
    strcpy(buf->tab,""); //Initialiser le buffer � une chaine de caract�res vide
    buf->suivant = -1; //Initialiser le champ suivant � -1 (NIL)
    EcrireDir(fichier,Entete(fichier,4) + 1,buf); //Ecrire le buffer � l'adresse (nombre de blocs total de la liste + 1)
    Aff_entete(fichier,4,Entete(fichier,4)+1); //Incr�menter le nombre total de blocs de la liste
    Aff_entete(fichier,5,Entete(fichier,4)); //Mettre � jour le champ qui correspond � l'adresse du dernier bloc de la liste
}

//Outils utilis�s
char* ChaineAlea(int taille)
{
    char* alpha = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!"; //Une chaine qui contient tous les caract�res possibles
    char* str = NULL; //La chaine al�atoire qu'on va g�n�rer
    str = (char*)malloc(sizeof(char)*(taille+2)); //Allouer de l'espace m�moire pour cette chaine
    for(int i=0;i<taille;i++)
    {
        str[i] = alpha[rand()%69]; //Choisir un caract�re al�atoirement
    }
    str[taille] = '|'; //Ajouter un s�p�rateur
    str[taille+1] = '\0'; //Un caract�re vide pour d�terminer la fin de la chaine de caract�res
    return str; //Retouner la chaine al�atoire
}

//Initialiser les enregistrements avec des s�parateurs
void InitialiserDonnee(Donnee* d)
{
    strcpy(d->numero,"||||||||||");
    strcpy(d->wilaya,"||||||||||||||");
    strcpy(d->superficie,"||||||||||");
    d->taille[0] = '0';
    for(int i=1;i<NB_TAILLE;i++)
        d->taille[i] = '|';
    d->eff = '0';
}

//Concat�ne tous les champs de l'enregistrement en une chaine de caract�res
char* ConcatDonnee(Donnee d)
{
    char* str;
    str = (char*)malloc(sizeof(char)*(37+NB_TAILLE+strlen(d.observation))); //Alloue l'espace m�moire pour la chaine de caract�res
    int j=0;
    for(int i=0;i<NB_TAILLE;i++) //Le champ taille
    {
        if(d.taille[i]!='\0')
            str[j]=d.taille[i];
        else
            str[j]='|';
        j+=1;
    }
    str[j]=d.eff; //Le champ effac�
    j+=1;
    for(int i=0;i<10;i++) //Le champ numero
    {
        if(d.numero[i]!='\0')
            str[j]=d.numero[i];
        else
            str[j]='|';
        j+=1;
    }
    for(int i=0;i<14;i++) //Le champ wilaya
    {
        if(d.wilaya[i]!='\0')
            str[j]=d.wilaya[i];
        else
            str[j]='|';
        j+=1;
    }
    str[j]=d.type; //Le champ type
    j+=1;
    for(int i=0;i<10;i++) //Le champ superficie
    {
        if(d.superficie[i]!='\0')
            str[j]=d.superficie[i];
        else
            str[j]='|';
        j+=1;
    }
    for(int i=0;i<strlen(d.observation);i++) //Le champ observation
    {
        str[j]=d.observation[i];
        j+=1;
    }
    str[j] = '\0'; //Le dernier carat�re de la chaine
    return str; //Retourner la chaine conact�n�e
}

//Recup�re le champ de l'enregistrement � partir du fichier
void RecupChamp(Fichier* fichier,int n,Buffer* buf,int* i,int* j,char* donnee)
{
    int k = 0;
    while(k<n)
    {
        if(*j>=B) //En cas o� le champ est divis� sur plus d'un bloc
        {
            *i = buf->suivant;
            *j = 0; //R�nitiliser le j � 0 (au d�but du nouveau bloc)
            LireDir(fichier,*i,buf); //Lire le bloc suivant
        }

        if(buf->tab[*j]!='|') //Si le caract�re n'est pas un s�parateur
        {
            donnee[k] = buf->tab[*j];
        }
        else //Sinon mettre des caract�res vides
        {
            donnee[k] = '\0';
        }
        (*j)+=1;
        k+=1;
    }
}

//R�cup�re la chaine du fichier comme elle est sans enlever les s�parateurs
void RecupChaine(Fichier* fichier,int n,Buffer* buf,int* i,int* j,char donnee[])
{
    int k = 0;
    while(k<n)
    {
        if(*j>=B) //En cas o� la chaine est divis�e sur plus d'un bloc
        {
            *i = buf->suivant;
            *j = 0; //R�nitiliser le j � 0 (au d�but du nouveau bloc)
            LireDir(fichier,*i,buf);
        }

        donnee[k] = buf->tab[*j];
        (*j)+=1;
        k+=1;
    }
    donnee[k] = '\0';
}

//Ecrie la chaine de caract�res de taille n � la fin d'un fichier � partir de l'adresse i et la position j
void EcrireChaine(Fichier* fichier,char* nom_physique,int n,Buffer* buf,int* i,int* j,char donnee[])
{
    int k = 0;
    Ouvrir(fichier,nom_physique,'A'); //Ouvrir le fichier en mode ancien
    LireDir(fichier,*i,buf); //Lire le bloc d'adresse i
    while(k<n) //Tant que la chaine n'est pas finie
    {
        if(*j>=B) //Si la chaine d�passe la capacit� du bloc
        {
            EcrireDir(fichier,*i,buf); //Ecrire le bloc � l'adresse i
            AllocBloc(fichier); //Allouer un nouveau bloc � la fin du fichier et le chainer
            *i = Entete(fichier,5); //Mettre � jour i � l'adresse de la queue de la liste
            *j = 0; //R�initialiser j � 0
            LireDir(fichier,*i,buf); //Lire le nouveau bloc
        }
        buf->tab[*j] = donnee[k]; //Mettre � jour un caract�re du buffer avec un caract�re de la chaine qu'on veut �crire
        (*j)+=1;
        k+=1;
    }
    donnee[k] = '\0';
    EcrireDir(fichier,*i,buf); //Ecrire le bloc � l'adresse i
    Aff_entete(fichier,6,*j); //Mettre � jour la derni�re position libre dans la queue
    Aff_entete(fichier,2,Entete(fichier,2)+n); //Mettre � jour le nombre de caract�res ins�r�s
    Fermer(fichier); //Fermer le fichier
}

//Une fonction pour demander � l'utilisateur d'entrer chaque champ manuellement et les concat�ner en une chaine de caract�res
char* EntrerDonnee(int numero)
{
    Donnee d; //D�clarer d comme enregistrement
    InitialiserDonnee(&d); //Initialiser les champs de d
    sprintf(d.numero,"%d",numero); //Affecter numero(entier) � d.numero(chaine de caract�res)
    printf("\t\t\tEntrez la wilaya : ");
    scanf("%s",d.wilaya); //Lire le champ wilaya
    printf("\t\t\tEntrez le type : ");
    scanf(" %c",&(d.type)); //Lire le champ type
    printf("\t\t\tEntrez la superficie : ");
    scanf("%s",d.superficie); //Lire le champ superficie
    printf("\t\t\tEntrez l'observation (sans espaces): ");
    d.observation = (char*)malloc(sizeof(char)*250);
    scanf("%s",d.observation); //Lire le champ observation
    sprintf(d.taille,"%d",NB_TAILLE+1+35+strlen(d.observation));
    return ConcatDonnee(d); //Retouner les champs concat�n�s
}


//Operations sur LOVC ==============================================================================================================================
//Cette proc�dure recherche un livret dans le fichier d'apr�s le num�ro
//i: l'adresse du bloc et j: la position dans le bloc
//(i,j) repr�sentent l'emplacement du livret s'il existe sinon o� il est suppos� plac�
void RechercheLOVC(Fichier* fichier,char* nom_physique,int val,int* i,int* j,int* trouv)
{
    Buffer buf; //Pour lire et �crire les blocs
    int stop; //Pour stopper la boucle dans certaines conditions
    int sauvi; //Pour sauvegarder i
    int sauvj; //Pour sauvegarder j
    char taille[NB_TAILLE];
    char numero[10];
    char eff;
    char* d; //La chaine de carat�res d'un enregistrement apr�s le champ taille, eff et numero
    int nb_taille,nb_numero; //Les champs taille et numero en entiers

    Ouvrir(fichier,nom_physique,'A'); //Ouvrir le fichier en mode ancien
    *trouv = 0; //Positionner trouv � Faux
    stop = 0; //Positionner stop � Faux
    *i = Entete(fichier,1);
    *j = 0;
    LireDir(fichier,*i,&buf); //Lire le premier bloc

    //Le cas d'un fichier vide
    if((*j>=Entete(fichier,6))&&(*i>=Entete(fichier,5)))
    {
        stop = 1; //stop � Vrai
    }

    while((*trouv == 0)&&(stop == 0)) //Tant qu'on a pas trouv� le livret et que aucune condition n'a �t� v�rifi�e pour stopper la boucle
    {
        sauvi = *i; //Sauvegarder i
        sauvj = *j; //Sauvegarder j
        RecupChamp(fichier,NB_TAILLE,&buf,i,j,taille); //Recup�rer le champ taille (le i et j sont mis � jour apr�s le champ)
        nb_taille = atoi(taille); //R�cup�rer le champ en tant qu'entier
        RecupChamp(fichier,1,&buf,i,j,&eff); //R�cup�rer le champ eff
        RecupChamp(fichier,10,&buf,i,j,numero); //R�cup�rer le champ num�ro
        nb_numero = atoi(numero); //R�cup�rer le champ en tant qu'entier
        d = malloc(sizeof(char)*(nb_taille-NB_TAILLE-10));
        RecupChaine(fichier,nb_taille-NB_TAILLE-11,&buf,i,j,d); //R�cup�rer le reste de l'enregistrement

        if(nb_numero == val) //Le meme num�ro a �t� trouv�
        {
            if(eff == '0') //Le livret n'est pas effec� donc il a �t� trouv�
                *trouv = 1; //trouv � Vrai
            else
                stop = 1; //Le livret a �t� supprim� donc on met stop � Vrai
            *i = sauvi; //R�cup�rer i avant d'avoir lu cet enregistrement
            *j = sauvj; //R�cup�rer j avant d'avoir lu cet enregistrement
        }
        else
        {
            if(nb_numero>val) //Si la valeur recherch�e est inf�rieure � la valeur lue il ne sert � rien de continuer la recherche donc il faut la stopper
            {
                stop = 1; //stop � Vrai
                *i = sauvi; //R�cup�rer i avant d'avoir lu cet enregistrement
                *j = sauvj; //R�cup�rer j avant d'avoir lu cet enregistrement
            }
        }
        if((*j>=Entete(fichier,6))&&(*i>=Entete(fichier,5))) //Si on arrive � la fin du fichier
        {
            stop = 1; //stop � Vrai
        }

        free(d); //On lib�re l'espace
    }
    Fermer(fichier); //Fermer le fichier
}

//Cette proc�dure ins�re un nouveau livret d'apr�s le num�ro dans la bonne position
void InsertionLOVC(Fichier* fichier,char* nom_physique,int numero,char* donnee)
{
    Buffer buf;
    int trouv;
    int stop = 0;
    int i = 1;
    int j = 0;
    int l = 0;
    int rest;
    RechercheLOVC(fichier,nom_physique,numero,&i,&j,&trouv); //On effectue une recherche pour avoir l'adresse i et la position j o� ins�rer
    if(trouv == 0) //Si le num�ro n'existe pas d�j� dans le fichier
    {
        Ouvrir(fichier,nom_physique,'A'); //Ouvrir le fichier en mode ancien
        int sauvtaille = strlen(donnee); //On sauvegarde la taille du nouveau enregistrement
        Aff_entete(fichier,2,Entete(fichier,2)+sauvtaille); //Mettre � jour le champ (nombre de caract�res ins�r�s) de l'ent�te
        Aff_entete(fichier,6,(Entete(fichier,6)+sauvtaille)%B); //Mettre � jour le champ (derni�re position libre de la queue) de l'ent�te
        char* temp_donnee = (char*)malloc((sauvtaille)*sizeof(char)); //Allouer un espace m�moire de la m�me taille que le nouveau enregistrement
        while(stop == 0) //Tant que l'insertion n'est pas finie
        {
            LireDir(fichier,i,&buf); //Lire le bloc � l'adresse i
            if(j+sauvtaille<=B) //Si il y a assez de place pour l'enregistrement dans le bloc courant
            {
                for(int k=0;k<sauvtaille;k++)
                {
                    temp_donnee[k] = buf.tab[j+k]; //Sauvegarder les caract�res qui se trouvaient d�j� dans la position o� on veut ins�rer
                    buf.tab[j+k] = donnee[k]; //Ins�rer un carat�re du nouveau enregistrement
                }
                temp_donnee[sauvtaille] = '\0';
                EcrireDir(fichier,i,&buf); //Ecrire le bloc
                j+=sauvtaille; //Faire avancer la position
                strcpy(donnee,temp_donnee); //Copier les caract�res sauvegard�s pr�c�demment dans la donn�e courante qu'on veut ins�rer
            }
            else //Si il n'y a pas assez de place pour l'enregistrement dans le bloc courant
            {
                rest = j+sauvtaille - B; //Nombre de caract�res qu'on ins�rera dans le bloc suivant
                l = 0;
                while(j<B) //Ins�rer les caract�res jusqu'� la fin du bloc
                {
                    temp_donnee[l] = buf.tab[j];
                    buf.tab[j] = donnee[l];
                    j++;
                    l++;
                }
                EcrireDir(fichier,i,&buf); //Ecrire le bloc
                i = buf.suivant; //Passer au prochain bloc
                j=0;
                if(i == -1) //Si le bloc courant �tait le dernier bloc
                {
                    AllocBloc(fichier); //On alloue un nouveau bloc
                    i = Entete(fichier,5); //Mettre � jour le champ (adresse de la queue) de l'ent�te
                    stop = 1; //On stop l'insertion car on est arriv� au dernier bloc
                }
                LireDir(fichier,i,&buf); //Lire le nouveau bloc
                for(int k=0;k<rest;k++) //Ins�rer les caract�res dans ce nouveau bloc
                {
                    temp_donnee[l] = buf.tab[j];
                    buf.tab[j] = donnee[l];
                    j++;
                    l++;
                }
                temp_donnee[strlen(donnee)] = '\0';
                EcrireDir(fichier,i,&buf); //Ecrire le bloc
                strcpy(donnee,temp_donnee); //Copier les caract�res sauvegard�s pr�c�demment dans la donn�e courante qu'on veut ins�rer
            }
            if((i == Entete(fichier,5))&&(j>Entete(fichier,6))) //Si on arrive au dernier bloc et � la derni�re position
            {
                stop = 1; //On stoppe l'insertion
            }
        }
        free(temp_donnee);
        Fermer(fichier);
    }
    else //Si le num�ro existe d�j� dans le fichier
    {
        printf("\t\t\tUn livret avec le meme numero existe deja\n");
    }
}

//Cette proc�dure r�alise la r�organisation du fichier dans un nouveau fichier
void Reorganisation(Fichier* fichier,Fichier* nouveau_fichier,char* nom_physique,char* nouveau_nom)
{
    Buffer buf;
    int i,j,index_i,index_j,index;
    int nb_taille;
    char taille[NB_TAILLE];
    char eff;
    char* d;
    char* donnee;
    int stop = 0;
    Ouvrir(fichier,nom_physique,'A'); //Ouvrir l'ancien fichier en mode ancien
    i = Entete(fichier,1);
    j = 0;
    Ouvrir(nouveau_fichier,nouveau_nom,'N'); //Ouvrir un nouveau fichier en mode nouveau
    AllocBloc(nouveau_fichier); //Allouer le premier bloc du nouveau fichier
    index_i = Entete(nouveau_fichier,5); //Recup�rer l'adresse indexi du nouveau fichier
    index_j = Entete(nouveau_fichier,6); //Recup�rer la position indexj du nouveau fichier
    Fermer(nouveau_fichier);
    while(stop == 0) //Tant que la r�organisation n'est pas finie
    {
        LireDir(fichier,i,&buf); //Lire le premier bloc de l'ancien fichier
        RecupChamp(fichier,NB_TAILLE,&buf,&i,&j,taille); //Recup�rer le champ taille
        RecupChamp(fichier,1,&buf,&i,&j,&eff); //R�cup�rer le champ effac�
        nb_taille = atoi(taille); //Convertir taille en entier
        d = malloc(sizeof(char)*(nb_taille-NB_TAILLE));
        RecupChaine(fichier,nb_taille-1-NB_TAILLE,&buf,&i,&j,d); //R�cup�rer le reste de l'enregistrement
        donnee = malloc(sizeof(char)*(nb_taille+1));
        index = 0;
        //Concat�ner tous les champs en une chaine de caract�res (donnee)
        for(int k=0;k<NB_TAILLE;k++)
        {
            if(taille[k] != '\0')
            {
                donnee[index] = taille[k];
            }
            else
            {
                donnee[index] = '|';
            }
            index++;
        }
        donnee[index] = eff;
        index++;
        for(int k=0;k<strlen(d);k++)
        {
            donnee[index] = d[k];
            index++;
        }
        donnee[nb_taille] = '\0';
        if(eff == '0') //Si la donnee n'�tait pas �ffac� logiquement
        {
            EcrireChaine(nouveau_fichier,nouveau_nom,nb_taille,&buf,&index_i,&index_j,donnee); //Ecrire la chaine de carac�res (donnee) � la fin du nouveau fichier
        }
        free(d);

        if((i == Entete(fichier,5))&&(j >= Entete(fichier,6))) //Si on arrive � la fin de l'ancien fichier
            stop = 1; //On arrete la r�organisation
    }
    Fermer(fichier);
}

//Cette proc�dure supprime logiquement un livret d'apr�s son num�ro
void SuppressionLogique(Fichier* fichier,char* nom_physique,int numero,int automatic,float seuil)
{
    int trouv;
    int i,j;
    int nb_taille;
    Buffer buf;
    char taille[NB_TAILLE];
    RechercheLOVC(fichier,nom_physique,numero,&i,&j,&trouv); //On effectue d'abord une recherche
    Ouvrir(fichier,nom_physique,'A'); //On ouvre le fichier en mode ancien
    LireDir(fichier,i,&buf); //On lie le bloc
    RecupChamp(fichier,NB_TAILLE,&buf,&i,&j,taille); //On r�cup�re le champ taille
    nb_taille = atoi(taille); //On le convertit en entier
    if(trouv == 1) //Si le livret a �t� trouv�
    {
        if(j<B) //la position du champ eff est dans le bloc courant
        {
            buf.tab[j] = '1'; //On met � jour le champ � 1 ce qui signifie que la donn�e a �t� effac�
            EcrireDir(fichier,i,&buf); //Ecrire le bloc
        }
        else //la position du champ eff est dans le bloc suivant
        {
            i = buf.suivant; //On passe au bloc suivant
            if(i == -1) //S'il n'existe pas c'est une erreur de conception
            {
                printf("\t\t\tERREUR: Le champ efface n'existe pas\n");
            }
            else
            {
                LireDir(fichier,i,&buf); //On lie ce bloc
                buf.tab[B-j] = '1'; //On met � jour le champ � 1 ce qui signifie que la donn�e a �t� effac�
                EcrireDir(fichier,i,&buf); //On �crit le bloc
            }
        }
        Aff_entete(fichier,3,Entete(fichier,3)+nb_taille); //On met � jour le champ (nombre de caract�res supprim�s) de l'ent�te
    }
    else //Si le livret n'a pas �t� trouv�
    {
        printf("\t\t\tLe livret que vous voulez supprimer n'existe pas\n");
    }

    if(automatic == 1) //Si la r�organisation automatique �tait activ�e
    {
        Fichier fichier2;
        if((float)Entete(fichier,3)/(float)Entete(fichier,2)>=seuil) //Si le seuil a �t� atteint ((nombre de caract�res supprim�s/nombre de caract�res ins�r�s)>=seuil)
        {
            Reorganisation(fichier,&fichier2,nom_physique,"Reo_Livrets_National.bin");
            printf("\t\t\tReorganisation effectuee !\n");
        }
    }
    Fermer(fichier);
}

//Proc�dure pour g�n�rer des livrets al�atoires
void GenererContenuAlea(Fichier* fichier,char* nom_physique,int nb_livret)
{
    srand(time(NULL)); //Pour les fonctions al�atoires
    int i=1; //Se positionner au d�but du fichier
    int j=0;
    Buffer buf;
    Ouvrir(fichier,nom_physique,'N'); //Ouvrir le fichier en mode nouveau
    AllocBloc(fichier); //Allouer le premier bloc du fichier
    for(int k=0;k<nb_livret;k++) //Ins�rer les livrets un par un
    {
        LireDir(fichier,i,&buf); //Lire le bloc

        Donnee nouvelle_donnee; //Cr�er une nouvelle donn�e
        InitialiserDonnee(&nouvelle_donnee); //L'initialiser
        sprintf(nouvelle_donnee.numero,"%d",k); //G�n�rer le num�ro
        strcpy(nouvelle_donnee.wilaya,wilayas[rand()%58]); //G�n�rer la wilaya
        nouvelle_donnee.type = types[rand()%4]; //G�n�rer le type
        sprintf(nouvelle_donnee.superficie,"%d",rand()%10000000000); //G�n�rer la superficie
        nouvelle_donnee.observation = ChaineAlea(rand()%(B-36-NB_TAILLE)); //G�n�rer l'observation
        //nouvelle_donnee.observation = ChaineAlea(rand()%20); //G�n�rer l'observation

        sprintf(nouvelle_donnee.taille,"%d",NB_TAILLE+1+35+strlen(nouvelle_donnee.observation)); //Calculer la taille
        char* str = ConcatDonnee(nouvelle_donnee); //Concat�ner tous les champs
        int index = 0;
        while(index<strlen(str)) //Ins�rer la donn�e caract�re par caract�re
        {
            if(j<B) //Si la position est inf�rieure � la taille du bloc
            {
                buf.tab[j] = str[index]; //Ins�rer le caract�re
                index+=1;
                j+=1;
            }
            else //Si le caract�re doit s'ins�rer dans le bloc suivant
            {
                j=0;
                EcrireDir(fichier,i,&buf); //On �crit le bloc
                AllocBloc(fichier); //Alloue un nouveau bloc
                i=Entete(fichier,5); //Met � jour l'adresse i
                LireDir(fichier,i,&buf); //Lire le nouveau bloc
            }
        }
        EcrireDir(fichier,i,&buf); //Ecrit le dernier bloc
        Aff_entete(fichier,2,Entete(fichier,2)+strlen(str)); //Met � jour le nombre de caract�res ins�r�s dans l'ent�te
        Aff_entete(fichier,6,j); //Met � jour la derni�re position dans la queue dans l'ent�te
    }
    Fermer(fichier);
}

//Proc�dure pour afficher le fichier en entier
void AfficherFichier(Fichier* fichier,char* nom_physique)
{
    Buffer buf;
    int i,j,index;
    int nb_taille;
    char taille[NB_TAILLE];
    char eff;
    char* d;
    char* donnee;
    index = 0;
    int stop = 0;
    Ouvrir(fichier,nom_physique,'A'); //Ouvrir le fichier en mode ancien
    i = Entete(fichier,1);
    j = 0;
    while(stop == 0)
    {
        LireDir(fichier,i,&buf); //Lire le bloc
        RecupChamp(fichier,NB_TAILLE,&buf,&i,&j,taille); //R�cup�re le champ taille
        RecupChamp(fichier,1,&buf,&i,&j,&eff); //R�cup�re le champ effac�
        nb_taille = atoi(taille); //Convertie taille en entier
        d = malloc(sizeof(char)*(nb_taille-NB_TAILLE));
        donnee = malloc(sizeof(char)*(nb_taille+1));
        RecupChaine(fichier,nb_taille-1-NB_TAILLE,&buf,&i,&j,d); //Recup�re le reste de la donn�e
        index = 0;
        //Concat�ner tous les champs de l'enregistrement
        for(int k=0;k<NB_TAILLE;k++)
        {
            if(taille[k] != '\0')
            {
                donnee[index] = taille[k];
            }
            else
            {
                donnee[index] = '|';
            }
            index++;
        }
        donnee[index] = eff;
        index++;
        for(int k=0;k<strlen(d);k++)
        {
            donnee[index] = d[k];
            index++;
        }
        donnee[nb_taille] = '\0';
        printf("%s\n",donnee); //Afficher l'enregistrement en entier
        if((i == Entete(fichier,5))&&(j >= Entete(fichier,6))) //Si on arrive � la fin du fichier on stoppe l'affichage
            stop = 1;
    }
    Fermer(fichier);
}

#endif // LOVC_H_INCLUDED
