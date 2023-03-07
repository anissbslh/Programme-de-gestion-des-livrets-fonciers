#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "LOVC.h"
#include "TOF.h"
#include "LOF.h"

int main()
{
    int choix;
    int fin = 0;
    int reo_automatique = 0;
    float seuil = 0.6;
    Fichier LOVC_fichier;
    IndexTab index_tab;
    FILE* test_file;
    index_tab.taille = 0;
    while(fin == 0)
    {
        system("cls");
        printf("\n\n\t\t\t\tBienvenue au programme de gestion de Livrets Fonciers\n\n\n");
        printf("\t\t\t1-Generation de donnees aleatoires\n");
        printf("\t\t\t2-Recherche d'un livret\n");
        printf("\t\t\t3-Insertion d'un nouveau livret\n");
        printf("\t\t\t4-Supprimer un livret (Suppresion logique)\n");
        printf("\t\t\t5-Reorganiser le fichier (manuellement)\n");
        printf("\t\t\t6-Activer/Deactiver la reorganisation automatique\n");
        printf("\t\t\t7-Decentraliser les livrets d'apres la wilaya\n");
        printf("\t\t\t8-Inserer un livret dans le fichier LOF \"Livrets_Wilaya_Terrain.bin\"\n");
        printf("\t\t\t9-Sauvegarder la table d'index\n");
        printf("\t\t\t10-Charger la table d'index\n");
        printf("\t\t\t11-Afficher la table d'index\n");
        printf("\t\t\t12-Quitter\n\n");
        printf("\t\t\tVotre choix : ");
        scanf("%d",&choix);

        switch(choix)
        {
            case 1:
                {
                    system("cls");
                    int nb_livrets;
                    printf("\n\n\t\t\tVeuillez entrer le nombre de livrets que vous voulez generer : ");
                    scanf("%d",&nb_livrets);
                    GenererContenuAlea(&LOVC_fichier,"Livrets_National.bin",nb_livrets);
                    printf("\n\t\t\tContenu de l'entete : ");
                    printf("\n\t\t\tAdresse du premier bloc : %d",Entete(&LOVC_fichier,1));
                    printf("\n\t\t\tNombre de caracteres inseres : %d",Entete(&LOVC_fichier,2));
                    printf("\n\t\t\tNombre de caracteres supprimes : %d",Entete(&LOVC_fichier,3));
                    printf("\n\t\t\tNombre total de blocs : %d",Entete(&LOVC_fichier,4));
                    printf("\n\t\t\tAdresse de la queue de la liste : %d",Entete(&LOVC_fichier,5));
                    printf("\n\t\t\tDerniere position libre dans la queue : %d",Entete(&LOVC_fichier,6));
                    printf("\n\t\t\tOperation effectuee avec succes\n");
                    break;
                }
            case 2:
                {
                    system("cls");

                    //Tester si le fichier existe
                    if(test_file = fopen("Livrets_National.bin","rb"))
                    {
                        fclose(test_file);

                        int numero,i,j,trouv;
                        int nb_taille;
                        Buffer buf;
                        char taille[NB_TAILLE];
                        char eff;
                        char ch_numero[10];
                        char wilaya[14];
                        char type;
                        char superficie[10];
                        char* observation;
                        printf("\n\n\t\t\tVeuillez entrer le numero du livret que vous recherchiez : ");
                        scanf("%d",&numero);
                        RechercheLOVC(&LOVC_fichier,"Livrets_National.bin",numero,&i,&j,&trouv);
                        if(trouv == 0)
                        {
                            printf("\n\t\t\tLe numero que vouz avez entre n'existe pas\n");
                            printf("\n\t\t\tLe livret doit se trouve dans le bloc %d a la position %d si on veut l'inserer\n",i,j);
                        }
                        else
                        {
                            printf("\n\t\t\tLe livret se trouve dans le bloc %d a la position %d\n",i,j);
                            Ouvrir(&LOVC_fichier,"Livrets_National.bin",'A');
                            LireDir(&LOVC_fichier,i,&buf);
                            RecupChamp(&LOVC_fichier,NB_TAILLE,&buf,&i,&j,taille);
                            nb_taille = atoi(taille);
                            observation = (char*)malloc(sizeof(char)*(nb_taille-NB_TAILLE-36+1));
                            RecupChamp(&LOVC_fichier,1,&buf,&i,&j,&eff);
                            RecupChamp(&LOVC_fichier,10,&buf,&i,&j,ch_numero);
                            RecupChamp(&LOVC_fichier,14,&buf,&i,&j,wilaya);
                            RecupChamp(&LOVC_fichier,1,&buf,&i,&j,&type);
                            RecupChamp(&LOVC_fichier,10,&buf,&i,&j,superficie);
                            RecupChaine(&LOVC_fichier,nb_taille-NB_TAILLE-36,&buf,&i,&j,observation);
                            printf("\n\t\t\tContenu du livret : ");
                            printf("\n\t\t\tTaille : %s",taille);
                            printf("\n\t\t\tEfface : %c",eff);
                            printf("\n\t\t\tNumero : %s",ch_numero);
                            printf("\n\t\t\tWilaya : %s",wilaya);
                            printf("\n\t\t\tType : %c",type);
                            printf("\n\t\t\tSuperficie : %s",superficie);
                            printf("\n\t\t\tObservation : %s\n",observation);
                        }
                    }
                    else
                    {
                        printf("\n\n\t\t\tLe fichier n'existe pas, veuillez d'abord generer les livrets aléatoires au choix 1\n");
                    }

                    break;
                }
            case 3:
                {
                    system("cls");
                    int numero;
                    if(test_file = fopen("Livrets_National.bin","rb"))
                    {
                        fclose(test_file);
                    }
                    else
                    {
                        Ouvrir(&LOVC_fichier,"Livrets_National.bin",'N');
                        Fermer(&LOVC_fichier);
                    }
                    printf("\n\n\t\t\tVeuillez entrer le numero du livret que vous voulez inserer : ");
                    scanf("%d",&numero);
                    InsertionLOVC(&LOVC_fichier,"Livrets_National.bin",numero,EntrerDonnee(numero));
                    printf("\n\t\t\tContenu de l'entete : ");
                    printf("\n\t\t\tAdresse du premier bloc : %d",Entete(&LOVC_fichier,1));
                    printf("\n\t\t\tNombre de caracteres inseres : %d",Entete(&LOVC_fichier,2));
                    printf("\n\t\t\tNombre de caracteres supprimes : %d",Entete(&LOVC_fichier,3));
                    printf("\n\t\t\tNombre total de blocs : %d",Entete(&LOVC_fichier,4));
                    printf("\n\t\t\tAdresse de la queue de la liste : %d",Entete(&LOVC_fichier,5));
                    printf("\n\t\t\tDerniere position libre dans la queue : %d",Entete(&LOVC_fichier,6));
                    printf("\n\t\t\tOperation effectuee avec succes\n");
                    break;
                }
            case 4:
                {
                    system("cls");
                    int numero;
                    if(test_file = fopen("Livrets_National.bin","rb"))
                    {
                        fclose(test_file);
                        printf("\n\n\t\t\tVeuillez entrer le numero du livret que vous voulez supprimer : ");
                        scanf("%d",&numero);
                        SuppressionLogique(&LOVC_fichier,"Livrets_National.bin",numero,reo_automatique,seuil);
                        printf("\n\t\t\tContenu de l'entete : ");
                        printf("\n\t\t\tAdresse du premier bloc : %d",Entete(&LOVC_fichier,1));
                        printf("\n\t\t\tNombre de caracteres inseres : %d",Entete(&LOVC_fichier,2));
                        printf("\n\t\t\tNombre de caracteres supprimes : %d",Entete(&LOVC_fichier,3));
                        printf("\n\t\t\tNombre total de blocs : %d",Entete(&LOVC_fichier,4));
                        printf("\n\t\t\tAdresse de la queue de la liste : %d",Entete(&LOVC_fichier,5));
                        printf("\n\t\t\tDerniere position libre dans la queue : %d",Entete(&LOVC_fichier,6));
                        printf("\n\t\t\tOperation effectuee avec succes\n");
                    }
                    else
                    {
                        printf("\n\n\t\t\tLe fichier n'existe pas, veuillez d'abord generer les livrets aléatoires au choix 1\n");
                    }

                    break;
                }
            case 5:
                {
                    system("cls");
                    Fichier f;
                    if(test_file = fopen("Livrets_National.bin","rb"))
                    {
                        fclose(test_file);

                        Reorganisation(&LOVC_fichier,&f,"Livrets_National.bin","Reo_Livrets_National.bin");
                        printf("\n\t\t\tContenu de l'entete du fichier reorganise: ");
                        printf("\n\t\t\tAdresse du premier bloc : %d",Entete(&f,1));
                        printf("\n\t\t\tNombre de caracteres inseres : %d",Entete(&f,2));
                        printf("\n\t\t\tNombre de caracteres supprimes : %d",Entete(&f,3));
                        printf("\n\t\t\tNombre total de blocs : %d",Entete(&f,4));
                        printf("\n\t\t\tAdresse de la queue de la liste : %d",Entete(&f,5));
                        printf("\n\t\t\tDerniere position libre dans la queue : %d",Entete(&f,6));
                        printf("\n\t\t\tOperation effectuee avec succes\n");
                    }
                    else
                    {
                        printf("\n\n\t\t\tLe fichier n'existe pas, veuillez d'abord generer les livrets aléatoires au choix 1\n");
                    }

                    break;
                }
            case 6:
                {
                    system("cls");
                    int choix2;
                    printf("\n\n\t\t\t1-Activer\t2-Deactiver\n\n");
                    printf("\t\t\tVotre choix : ");
                    scanf("%d",&choix2);
                    if(choix2 == 1)
                    {
                        reo_automatique = 1;
                        printf("\t\t\tVeuillez entrer le seuil (de 0 a 1): ");
                        scanf("%f",&seuil);
                        printf("\t\t\tLa reorganisation automatique a ete active\n");
                    }
                    else
                    {
                        reo_automatique = 0;
                        seuil = 0.6;
                        printf("\t\t\tLa reorganisation automatique a ete deactive\n");
                    }
                    break;
                }
            case 7:
                {
                    system("cls");
                    if(test_file = fopen("Livrets_National.bin","rb"))
                    {
                        fclose(test_file);

                        FTOF fichier2;
                        FLOF fichier3;
                        int choix2;
                        float u;
                        char wilaya[14];
                        char* nouveau_nom;
                        char* ancien_nom;
                        nouveau_nom = (char*)malloc(sizeof(char)*38);
                        ancien_nom = (char*)malloc(sizeof(char)*38);
                        strcpy(nouveau_nom,"Livrets_");
                        printf("\t\t\tVeuillez choisir une wilaya\n\n");
                        for(int i=0;i<58;i++)
                        {
                            printf("\t\t\t%d- %s\n",i+1,wilayas[i]);
                        }
                        printf("\n\n\t\t\tChoix : ");
                        scanf("%d",&choix2);
                        choix2 -= 1;
                        if(choix2<0)
                            choix2 = 0;
                        if(choix2>57)
                            choix2 = 57;
                        strcpy(wilaya,wilayas[choix2]);
                        strcat(nouveau_nom,wilaya);
                        strcat(nouveau_nom,".bin");
                        SauvTOF(&LOVC_fichier,&fichier2,"Livrets_National.bin",nouveau_nom,wilaya);
                        printf("\t\t\tLe fichier %s a ete cree!\n\n",nouveau_nom);

                        strcpy(ancien_nom,nouveau_nom);
                        strcpy(nouveau_nom,"Livrets_");
                        strcat(nouveau_nom,wilaya);
                        strcat(nouveau_nom,"_Terrain.bin");
                        printf("\t\t\tVeuillez entrer le pourcentage de chargement u (de 0 a 1) : ");
                        scanf("%f",&u);
                        SauvLOF(&fichier2,&fichier3,ancien_nom,nouveau_nom,u,&index_tab);
                        printf("\t\t\tLe fichier %s a ete cree!\n",nouveau_nom);
                        SauvegardeIndex("Index.bin",&index_tab);
                        printf("\n\t\t\tOperation effectuee avec succes\n");
                    }
                    else
                    {
                        printf("\n\n\t\t\tLe fichier n'existe pas, veuillez d'abord generer les livrets aléatoires au choix 1\n");
                    }

                    break;
                }
            case 8:
                {
                    system("cls");
                    FLOF fichier2;
                    char* nom;
                    char* superficie;
                    nom = (char*)malloc(sizeof(char)*38);
                    superficie = (char*)malloc(sizeof(char)*10);
                    printf("\n\n\t\t\tEntrez le nom complet du fichier : ");
                    scanf("%s",nom);
                    //Tester si le fichier existe
                    if(test_file = fopen(nom,"rb"))
                    {
                        fclose(test_file);

                        OuvrirLOF(&fichier2,nom,'A');
                        ChargerIndex("Index.bin",&index_tab);

                        char* numero;
                        numero = (char*)malloc(sizeof(char)*10);
                        printf("\t\t\tEntrez le numero du livret : ");
                        scanf("%s",numero);
                        printf("\t\t\tEntrez la superficie : ");
                        scanf("%s",superficie);
                        InsertionLOF(&fichier2,nom,numero,superficie,&index_tab);
                        printf("\n");

                        SauvegardeIndex("Index.bin",&index_tab);
                        printf("\n\t\t\tOperation effectuee avec succes\n");
                        FermerLOF(&fichier2);
                    }
                    else
                    {
                        printf("\n\t\t\tERREUR : Ce fichier n'existe pas\n");
                    }

                    break;
                }
            case 9:
                {
                    system("cls");
                    SauvegardeIndex("Index.bin",&index_tab);
                    printf("\n\t\t\tOperation effectuee avec succes\n");
                    break;
                }
            case 10:
                {
                    system("cls");
                    ChargerIndex("Index.bin",&index_tab);
                    printf("\n\t\t\tOperation effectuee avec succes\n");
                    break;
                }
            case 11:
                {
                    system("cls");
                    AfficherIndex(&index_tab);
                    break;
                }
            default:
                {
                    fin = 1;
                    break;
                }
        }
        if(fin == 0)
        {
            printf("\n\t\t\tVoulez vous quitter?\n\t\t\t1-Oui 2-Non\n\t\t\tChoix : ");
            scanf("%d",&choix);
            if(choix == 1)
                fin = 1;
        }
    }
    return 0;
}
