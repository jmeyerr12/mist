#include <bits/stdc++.h>
using namespace std;
#define ll long long
#define ull unsigned long long
#define MOD 1000000007
#define MODCode 998244353
#define INF 987654321
#define NUMERO_RAILS 8
#define UTF 256
// NUNCA FLOAT SEMPRE DOUBLE
// NUNCA ll SEMPRE LL

string geraChave(int n, string chave){
    int ind = 0;
    string ret = "";
    while (ret.length() < n){
        ret += chave[ind];
        ind = (ind+1)%chave.length();
    }
    return ret;
}

string zigzag(string text, int rails) { //railfence transposition cypher
    vector<string> linhas(rails);
    int periodo = 2*(rails-1);

    for (int i = 0; i < text.size(); i++) {
        int p = i % periodo;
        int linha;
        if (p < rails) linha = p;
        else linha = periodo - p;

        linhas[linha] += text[i];
    }

    string resultado = "";
    for (string linha : linhas) resultado += linha;
    return resultado;
}

string dezigzag(string text, int rails) {
    int n = text.size();

    if (rails <= 1) 
        return text;

    int periodo = 2 * (rails - 1);

    vector<int> qtd(rails, 0);

    for (int i = 0; i < n; i++){ //conta chars de cada rail
        int p = i % periodo;
        int rail;

        if (p < rails) rail = p;
        else rail = periodo - p;

        qtd[rail]++;
    }

    vector<int> pos(rails);

    pos[0] = 0;

    for (int i = 1; i < rails; i++){ //descobre onde cada rail comeca
        pos[i] = pos[i - 1] + qtd[i - 1];
    }

    string resultado = "";

    for (int i = 0; i < n; i++){ // reconstroi o texto
        int p = i % periodo;
        int rail;

        if (p < rails)
            rail = p;
        else
            rail = periodo - p;

        resultado += text[pos[rail]++];
    }

    return resultado;
}

void encripta_jovi(string &s, string chave){
    // loop de subs vegenare
    string chaveNormalizada = geraChave(s.length(), chave);
    for (int i = 0; i < s.length(); i++){
        // cript-> ci = (ki + pi)%26
        unsigned char p = (unsigned char)s[i];
        unsigned char k = (unsigned char)chaveNormalizada[i];
        s[i] = (p + k) % UTF;
    }

    // transposicao
    s = zigzag(s, NUMERO_RAILS);
}

void decripta_jovi(string &s, string chave){
    s = dezigzag(s, NUMERO_RAILS);

    string chaveNormalizada = geraChave(s.length(), chave);
    // loop de subs vegenare
    for (int i = 0; i < s.length(); i++){
        // decript-> pi = (ci-ki+26)%26
        unsigned char c = (unsigned char)s[i];
        unsigned char k = (unsigned char)chaveNormalizada[i];

        s[i] = (c - k + UTF) % UTF;
    }
}
#ifndef MIST_NO_MAIN
int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(0);
    string nomeDoArquivo, chave;
    cout << "insira o nome do arquivo e a chave para a cifra (pode ser uma string qualquer)" << endl;
    cin >> nomeDoArquivo >> chave;

    int op;
    cout << "insira a operacao (1 para cifrar, 2 para decifrar)" << endl;
    cin >> op;

    ifstream entrada(nomeDoArquivo, ios::binary);
    if (!entrada.is_open()){
        cout << "erro ao abrir o arquivo" << endl;
        return 0;
    }

    string s((istreambuf_iterator<char>(entrada)), istreambuf_iterator<char>());

    entrada.close();

    if (op == 1) encripta_jovi(s, chave);
    else decripta_jovi(s, chave);

    string nomeDaResposta = (op == 1 ? "encriptado.txt" : "decriptado.txt");
    ofstream resposta(nomeDaResposta, ios::binary);

    resposta.write(s.data(), s.size());
    resposta.close();

    return 0;
}
#endif