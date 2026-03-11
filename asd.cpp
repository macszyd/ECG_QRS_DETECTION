#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <cctype>
#include <cmath>

using namespace std;

int range = 650000;
int n = 80;
int Nd = 7;
int Ng = 8;
//Czy cyfra
bool isDigit(string& x) {
    if (x.empty())
       return false;
    for (int i = 0; i < x.length(); i++) {
       if (x[i] != '1' && x[i] != '2' && x[i] != '3' && x[i] != '4' && x[i] != '5' && x[i] != '6' && x[i] != '7' && x[i] != '8' && x[i] != '9' && x[i] != '0')
          return false;
    }
    return true;
}
//Rysuj
void Draw(vector<float> sig, int start)
{
   // po deklaracji następuje znalezienie max i min. Potem nastepuje rysowanie
    float max = sig[0];
    float min = sig[0];
    int size = sig.size();
    for(int i = 0;i<size;i++)
    {
       if(max<sig[i]) max = sig[i];
       if(min>sig[i]) min = sig[i];
    }
    float q = (max - min)/n;
    for(int i = start; i<size;i++)
    {
       float bufor = min;
       float sample = sig[i];
       while(bufor<sample)
       {
          cout<<" ";
          bufor+=q;
       }
       cout<<"X"<<endl;
    }
}
// 1. filtracja N - ty element - N-Nd element
void firstFilter(vector<float>& sigin, vector<float>& sigout) {
    for(int j = 0; j < range - Nd; j++)
    {
       sigout.push_back(sigin[j+Nd] - sigin[j]);
    }
}
// 2. Filtracja średnia arytmetyczna z Ng poprzednich próbek
void secondFilter(vector<float>& sigin, vector<float>& sigout) {
    for(int k = Ng; k < sigin.size(); k++)
    {
       float sum = 0;
       for(int i = Ng; i>=0; i--)
          sum+=sigin[k-i];

       sigout.push_back(sum/Ng);
    }
}
// 3. Filtracja element podniesiony do kwadratu
void thirdFilter(vector<float>& sigin, vector<float>& sigout) {
    for(int m = 0; m < sigin.size(); m++)
    {
       sigout.push_back(sigin[m]*sigin[m]);
    }
}
//szukanie progu przez 1080 próbek, przy spełnieniu warunku przemnożenie przez współczynnik, w przeciwnym razie zmiana max, następnie odbywa się zliczanie i skip 72 próbek
void Prog(const vector<float>& sigin, vector<int>& sigout) {
   float thr = 0;
   int id = 0;

   for (int i = 1; i<1080 && i < sigin.size(); i++)
   {
      if (sigin[i]>thr)
      {
         thr = sigin[i];
         id = i;
      }
   }
   sigout.push_back(id);

   float suma = thr;
   float licznik = 1;
   id +=72;

   if(id < sigin.size()) thr = sigin[id];

   for (int i = 1080; i<range && i < sigin.size(); i++)
   {
      if (sigin[i]<=thr)
      {
         thr *=0.98165;
         continue;
      }
      else {
         float max = sigin[i];
         for (int j = 0; j<94 && (i+j) < sigin.size(); j++)
         {
            if (max < sigin[i+j])
            {
               max = sigin[i+j];
               id =i+j;
            }
         }
         sigout.push_back(id);
         licznik+=1;
         suma+=max;
         thr = suma/licznik;
         i+=72;
      }
   }
}
//Detekcja TP FP FN
void macierzPomylek(const vector<int>& wykryte, const vector<int>& annotacje, int margin, int& TP, int& FP, int& FN) {
    TP = 0;
    FP = 0;

    vector<bool> matched(annotacje.size(), false);

    for (int j = 0; j < wykryte.size(); j++) {
       bool found_match = false;

       for (int k = 0; k < annotacje.size(); k++) {
           if (abs(wykryte[j] - annotacje[k]) < margin && !matched[k]) {
               TP++;
               matched[k] = true;
               found_match = true;
               break;
           }
       }

       if (!found_match) {
           FP++;
       }
    }
    FN = annotacje.size() - TP;
}

int main() {
    string temp;
    float sample;
    vector<float> syg;
    vector<float> sygfilt;
    vector<float> sygfilt2;
    vector<float> sygfinal;

    ifstream reader("samples_111.csv");
    if (!reader.is_open())
    {
       cout<<"FILE ERROR"<<endl;
       return 0;
    }

     getline(reader,temp);
     getline(reader,temp);

     for(int i = 0;reader.good() && i<range;i++) {
       getline(reader, temp, ',');
       getline(reader, temp);
       sample = 200 * stof(temp);
       syg.push_back(sample);
    }
    reader.close();

    // Filtracja
    firstFilter(syg, sygfilt);
    secondFilter(sygfilt, sygfilt2);
    thirdFilter(sygfilt2, sygfinal);

    // Progowanie
    vector<int> indeksy;
    Prog(sygfinal, indeksy);

    // Wczytanie etykiet
    ifstream reader1("annotations_111m.txt");

    if (!reader1.is_open())
    {
       cout<<"FILE1 ERROR"<<endl;
       return 0;
    }
    string peak;
    string name;
    char R_SYMBOLS[] = {'N', 'V', 'L', 'R', '/', 'f', 'A', 'E', 'Q', 'F', 'j', 'J', 'a', 'S', 'e', 'r', 'F', 'n', '?'};
    vector<int> annotations;
    for(int j = 0;reader1.good() && j<range;j++) {
       getline(reader1, peak, ',');
       getline(reader1, name);
       if(name.empty()) continue;
       char namec = name[0];
       int count = 0;
       for (int k = 0; k < 18; k++) {
          if (namec == R_SYMBOLS[k])
             count++;
       }
          if (count == 1 && isDigit(peak) == true) {
             int pik = stoi(peak);
             annotations.push_back(pik);
          }
    }

    cout << "Annotacje: " << annotations.size() << " Wykryte: " << indeksy.size() << endl;

    int TP, FP, FN;
    int margin = 27; // odległość próbek od siebie

    macierzPomylek(indeksy, annotations, margin, TP, FP, FN);

    cout << "TP: " << TP << endl;
    cout << "FP: " << FP << endl;
    cout << "FN: " << FN << endl;

    return 0;
}