#define MIST_NO_MAIN
#include "mist.cpp"

double mediana(vector<double> tempos){
    sort(tempos.begin(), tempos.end());

    int n = tempos.size();

    if (n%2) return tempos[n/2];
    return (tempos[n/2 - 1] + tempos[n/2])/2.0;
}

string leArquivo(string nome){
    ifstream entrada(nome, ios::binary);

    return string(
        (istreambuf_iterator<char>(entrada)),
        istreambuf_iterator<char>()
    );
}

int main(){
    vector<string> arquivos = {
        "livros/gettysburg.txt",
        "livros/raven.txt",
        "livros/alice.txt",
        "livros/war_and_peace.txt"
    };

    string chave = "MIST2026";

    cout << fixed << setprecision(3);

    cout << "arquivo,bytes,cifragem_ms,decifragem_ms,cifragem_MBps,decifragem_MBps\n";

    for (string arquivo : arquivos){
        string original = leArquivo(arquivo);

        vector<double> temposEncriptacao;
        vector<double> temposDecriptacao;

        for (int teste = 0; teste < 15; teste++){
            string s = original;

            auto inicio = chrono::high_resolution_clock::now();

            encripta_jovi(s, chave);

            auto fim = chrono::high_resolution_clock::now();

            double tempo = chrono::duration<double, milli>(fim - inicio).count();
            temposEncriptacao.push_back(tempo);
        }

        string cifrado = original;
        encripta_jovi(cifrado, chave);

        for (int teste = 0; teste < 15; teste++){
            string s = cifrado;

            auto inicio = chrono::high_resolution_clock::now();

            decripta_jovi(s, chave);

            auto fim = chrono::high_resolution_clock::now();

            double tempo = chrono::duration<double, milli>(fim - inicio).count();
            temposDecriptacao.push_back(tempo);

            if (s != original){
                cerr << "ERRO: decriptacao incorreta em " << arquivo << endl;
                return 1;
            }
        }

        double enc = mediana(temposEncriptacao);
        double dec = mediana(temposDecriptacao);

        double tamanhoMB = original.size()/(1024.0*1024.0);

        double encMBps = tamanhoMB/(enc/1000.0);
        double decMBps = tamanhoMB/(dec/1000.0);

        cout
            << arquivo << ","
            << original.size() << ","
            << enc << ","
            << dec << ","
            << encMBps << ","
            << decMBps << "\n";
    }

    return 0;
}