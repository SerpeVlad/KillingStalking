#include <iostream>
#include <string.h>
/**
        INITIAL PLAN
        
extract from dictionary

insert the sentence (ex"Ana nu are mere")///if it's negation N=1
api searches the google "Ana are mere"
then, it enters the sites and it:
	-takes the sentences that it found on the sites
	-seeks for key-words("Ana" si "mere")
	-it searches for negations, and if negatie==true && N==1/Daca negatie==false && N==0

the algorithm should search for if there are <x sites it won't take it into account

inserting the sites into the database, and if it's wrong multiple times, it loses credibility

if site==gasit_stire
	S=S+Puncte_Site;

what percentile is a credibilty site(it gives us an x)

Daca (site[i].negatie==true && N=1) || (site[i].negatie==false && N=0)
**/
using namespace std;

char V[200],T[500],Titlu[50][200],CuvinteC[60][200],Propozitii[200],P[50][200];
int N1=0,N2=0,j=0,c=0/**Cate cuvinte are titlul**/,y2=0/**Cate cuvinte cheie sunt in propozitia din site**/,nrp/**Numar de prop**/;
int nr_cp/**Numar cuvinte propozitie**/,nr_a;

struct site{
    char nume[50],continut[20000];
    int puncte,ADV;
}Siteuri[200];

void PlanB()
{
    /**simulates an API until we get around this**/
    strcpy(Siteuri[0].continut,"A Las Vegas man faces prison time after he traveled to Dubai in the United Arab Emirates and marijuana was detected in his urine, his lawyer said. Peter Clark, 51, traveled to Dubai for a business trip on Feb. 24 but was hospitalized for pancreatitis, Clark’s attorney Radha Stirling wrote on her website Detained in Dubai. The hospital took a urine test, found traces of marijuana and reported it to the police, who charged Peter and took him to the Al Barsha police station on March 3, Stirling said. Recreational marijuana is legal in Nevada, but consumption or possession of the drug is illegal in the UAE, even if presented with a doctor’s medical card, according to the U.S. State Department. People can still be charged and convicted in the UAE even if substances were taken outside the country, as long as traces are still present in the bloodstream upon arrival in the UAE. Clark was taken to the Anti-Narcotics unit and detained before being released on March 6 to his hotel, according to Stirling.");
    strcpy(Siteuri[0].nume,"www.sacbee.com");
    Siteuri[0].puncte=9;

    strcpy(Siteuri[1].continut,"Hardik Pandya on being asked if his direct-hits have come years and years of practice -- Not really. How's that for swagger! Hope you enjoyed reading this and until next time, ta ta!! Just to put things into context, SRH's collapse has been 70 runs | 10 wickets. The difference between the two teams though was 13 runs, which to be fair has to be credited to those two sixes that Pollard hit to close out Mumbai's innings. But that's what this Mumbai side has become, a brutal steamrolling machine, winning from anywhere, anytime. SRH Choke. Again.It's not as bad as what they've done over the last two games, but still, a choke is a choke. Samad's gone. Rashid's gone. And the last hope that remained, Shankar's gone too. Who's the star you ask? Jazzy B -- 4-0-14-1. If only there was a Man of the Match for non-stat-based performances. He's bowled the absolute terror overs, two in the powerplay, two at the death.");
    strcpy(Siteuri[1].nume,"cricbuzz.com");
    Siteuri[1].puncte=8;

    strcpy(Siteuri[2].continut,"A man who legally enjoyed some cannabis in his home of Las Vegas is now facing three years in prison in the United Arab Emirates after being caught with traces of it in his urine, according to his attorney’s website Detained in Dubai. Peter Clark, 51, flew to Dubai on Feb. 24 on a business trip looking for a recording studio. The retired game designer then went to a hospital following a pancreatitis attack. He was reported to authorites when doctors found hashish traces in his urine, which is considered possesion in the UAE. Clark was arrested on March 3 despite cannabis being legal in Nevada where he consumed it, kept in a “flea-ridden cell,” and has been waiting in his hotel since getting out of jail. Lost a ton of weight, no shower, no food, nothing to drink since I got here, no sleep, he said in a video after getting out. Clark adds that he did not bring drugs to Dubai, nor purchased or consumed them there. ");
    strcpy(Siteuri[2].nume,"complex.com");
    Siteuri[2].puncte=10;

}

void Cate_p()
{
    int x=strlen(Siteuri[nr_a].continut);
    char test[20000];

    strcpy(test,Siteuri[nr_a].continut);

    nrp=0;
    for(int i=0;i<x;i++)
        if(test[i]=='.' || test[i]=='?' || test[i]=='!')
            nrp++;
}

void Separe_propozitii()
{
    int x=strlen(Siteuri[nr_a].continut);
    char test[20000];

    strcpy(test,Siteuri[nr_a].continut);

    int g=0;
    for(int i=0;i<x;i++)
    {
        if((test[i]!='.' || test[i]!='?' || test[i]!='!') && test[i]!=' ' && test[i]!=',')
            Propozitii[g]=test[i];
        if(test[i]=='.' || test[i]=='?' || test[i]=='!')
        {
            Propozitii[g-1]=NULL;
            strcpy(test,test+i+1);
            g=0;
            break;
        }
    }

    strcpy(Siteuri[nr_a].continut,test);
}

void Compara_Adevarat()
{
    Cate_p();
   for(int p=0;p<nrp;p++)
    {
        Separe_propozitii();
        for(int i=0;i<c;i++)
        /**equal number of key-words**/
        if(strstr(Propozitii,Titlu[i])==0)
            y2++;

        if(y2>=c*(75/100))
        {
            j=0;
            nr_cp=0;
            for(int i=0;i<strlen(Propozitii);i++)
            {
               if(Propozitii[i]!=' ' || Propozitii[i+1]!=NULL)
               {
                   V[j]=Propozitii[i];
                   j++;
               }
               if(Propozitii[i]==' ')
               {
                   V[j-1]=NULL;
                   strcpy(P[nr_cp],V);
                   j=0;
                   nr_cp++;
               }
               if(Propozitii[i+1]==NULL)
               {
                   V[j]=NULL;
                   strcpy(P[nr_cp],V);
                   j=0;
                   nr_cp++;
               }
            }

            /**NEGATION PREPOSITION**/
            for(int i=0;i<nr_cp;i++)
            {
            if(strcmp(P[i],"not")==0)
                N2++;
            if(strcmp(P[i],"no")==0)
                N2++;
            if(strcmp(P[i],"Not")==0 || strcmp(P[i],"No")==0)
                N2++;
            if(strcmp(P[i],"None")==0 || strcmp(P[i],"Nobody")==0 || strcmp("Nothing",P[i])==0 || strcmp("Neither",P[i])==0 || strcmp("Nowhere",P[i])==0 || strcmp("Never",P[i])==0)
                N2++;
             if(strcmp(P[i],"none")==0 || strcmp(P[i],"nobody")==0 || strcmp("nothing",P[i])==0 || strcmp("neither",P[i])==0 || strcmp("nowhere",P[i])==0 || strcmp("never",P[i])==0)
                N2++;
            if(strstr("n't",P[i])==0)
                N2++;
            }

            if(N2%2==N1%2)
            {
                Siteuri[nr_a].ADV=1;
                break;
            }
            else
            {
                Siteuri[nr_a].ADV=0;
                break;
            }
            N2=0;
            nr_cp=0;
            /**Later on we could implement algorithms to find out if:
                        -it could be clickbait
                        -the news is racist
                        -there are wors used in wrong methods
                        -Sentimentul articolului

            **/
        }
        y2=0;
    }
    nrp=0;
}

int main()
{

/**READ AND SEPARATE THE SENTENCE IN WORDS**/
    cout<<"The news title that you wanna check: ";
    cin.get(T,500);
    cin.get();
    /**
   Later on we could implement:
        -if they search for a name, show news of that person
        -if they're looking for a country, to take them directly to that country's most credible site
    **/
    for(int i=0;i<strlen(T);i++)
    {
        if(T[i]!=' ' || T[i+1]!=NULL)
        {
            V[j]=T[i];
            j++;
        }
        if(T[i]==' ')
        {
            V[j-1]=NULL;
            strcpy(Titlu[c],V);
            j=0;
            c++;
        }
        if(T[i+1]==NULL)
        {
           V[j]=NULL;
            strcpy(Titlu[c],V);
            j=0;
            c++;
        }
    }

/**COUNTING NEGATIONS**/

    for(int i=0;i<c;i++)
    {
        if(strcmp(Titlu[i],"not")==0)
            N1++;
        if(strcmp(Titlu[i],"no")==0)
            N1++;
        if(strcmp(Titlu[i],"Not")==0 || strcmp(Titlu[i],"No")==0)
            N1++;
        if(strcmp(Titlu[i],"None")==0 || strcmp(Titlu[i],"Nobody")==0 || strcmp("Nothing",Titlu[i])==0 || strcmp("Neither",Titlu[i])==0 || strcmp("Nowhere",Titlu[i])==0 || strcmp("Never",Titlu[i])==0)
            N1++;
        if(strcmp(Titlu[i],"none")==0 || strcmp(Titlu[i],"nobody")==0 || strcmp("nothing",Titlu[i])==0 || strcmp("neither",Titlu[i])==0 || strcmp("nowhere",Titlu[i])==0 || strcmp("never",Titlu[i])==0)
            N1++;
        if(strstr("n't",Titlu[i])==0)
            N1++;
    }

PlanB();
int S=0;
for(nr_a=0;nr_a<3/**this is how many an example database had**/;nr_a++)
    {
    Compara_Adevarat();
    S=S+Siteuri[nr_a].puncte;
    }

int p/**percentile of a cred point**/,procent=0/**percentile of how credible that news is**/;
p=100/S;

/**calculates the percentile of credibility**/
for(nr_a=0;nr_a<3/**this is how many an example database had**/;nr_a++)
    if(Siteuri[nr_a].ADV==1)
     procent+=Siteuri[nr_a].puncte*p;


     cout<<"Siteuri cu stirea adevarata: "<<endl;
     if(procent>=50)
     {
        for(nr_a=0;nr_a<3/**this is how many an example database had**/;nr_a++)
        if(Siteuri[nr_a].ADV==1)
            cout<<"   -"<<Siteuri[nr_a].nume<<";"<<endl;
     }
     else
     {
        for(nr_a=0;nr_a<3/**this is how many an example database had**/;nr_a++)
        if(Siteuri[nr_a].ADV==0)
             cout<<"   -"<<Siteuri[nr_a].nume<<";"<<endl;
     }

     if(procent>=50)
        cout<<endl<<"Sansele sa fie reala stirea originala sunt: "<<procent<<"% deci siteurile de ti le-am prezentat o sustin"<<endl;
     else
        cout<<endl<<"Sansele sa fie reala stirea originala sunt: "<<procent<<"% deci siteurile de ti le-am prezentat nu o sustin"<<endl;

/**modifies the credibity points a site has**/

    if(procent>=50)
    {
        for(nr_a=0;nr_a<3/**this is how many an example database had**/;nr_a++)
        if(Siteuri[nr_a].ADV==1)
        Siteuri[nr_a].puncte+=2;
        else
        Siteuri[nr_a].puncte-=2;
    }
    else
    {
        for(nr_a=0;nr_a<3/**this is how many an example database had**/;nr_a++)
        if(Siteuri[nr_a].ADV==0)
        Siteuri[nr_a].puncte+=2;
        else
        Siteuri[nr_a].puncte-=2;
    }

    return 0;
}
