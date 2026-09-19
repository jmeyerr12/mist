#include <bits/stdc++.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
using namespace std;

#define TAMANHO_BLOCO_RSA 190
#define REPETICOES 3

string leArquivo(string nome){
    ifstream entrada(nome, ios::binary);

    return string(
        (istreambuf_iterator<char>(entrada)),
        istreambuf_iterator<char>()
    );
}

double mediana(vector<double> tempos){
    sort(tempos.begin(), tempos.end());

    int n = tempos.size();

    if (n%2) return tempos[n/2];
    return (tempos[n/2-1] + tempos[n/2])/2.0;
}

EVP_PKEY *geraChaveRSA(){
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);

    EVP_PKEY_keygen_init(ctx);
    EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048);

    EVP_PKEY *chave = NULL;
    EVP_PKEY_keygen(ctx, &chave);

    EVP_PKEY_CTX_free(ctx);

    return chave;
}

string encriptaRSA(string texto, EVP_PKEY *chave){
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(chave, NULL);

    EVP_PKEY_encrypt_init(ctx);
    EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING);
    EVP_PKEY_CTX_set_rsa_oaep_md(ctx, EVP_sha256());
    EVP_PKEY_CTX_set_rsa_mgf1_md(ctx, EVP_sha256());

    int tamanhoSaida = EVP_PKEY_size(chave);

    string resultado;

    int numeroBlocos =
        (texto.size() + TAMANHO_BLOCO_RSA - 1)/TAMANHO_BLOCO_RSA;

    resultado.reserve(numeroBlocos*tamanhoSaida);

    for (int i = 0; i < texto.size(); i += TAMANHO_BLOCO_RSA){
        int tamanho =
            min(TAMANHO_BLOCO_RSA, (int)texto.size()-i);

        vector<unsigned char> saida(tamanhoSaida);
        size_t tamanhoGerado = saida.size();

        int ok = EVP_PKEY_encrypt(
            ctx,
            saida.data(),
            &tamanhoGerado,
            (unsigned char*)texto.data()+i,
            tamanho
        );

        if (ok <= 0){
            cerr << "erro na cifragem RSA" << endl;
            exit(1);
        }

        resultado.append(
            (char*)saida.data(),
            tamanhoGerado
        );
    }

    EVP_PKEY_CTX_free(ctx);

    return resultado;
}

string decriptaRSA(string texto, EVP_PKEY *chave){
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(chave, NULL);

    EVP_PKEY_decrypt_init(ctx);
    EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING);
    EVP_PKEY_CTX_set_rsa_oaep_md(ctx, EVP_sha256());
    EVP_PKEY_CTX_set_rsa_mgf1_md(ctx, EVP_sha256());

    int tamanhoBlocoCifrado = EVP_PKEY_size(chave);

    string resultado;

    for (int i = 0; i < texto.size(); i += tamanhoBlocoCifrado){
        vector<unsigned char> saida(tamanhoBlocoCifrado);
        size_t tamanhoGerado = saida.size();

        int ok = EVP_PKEY_decrypt(
            ctx,
            saida.data(),
            &tamanhoGerado,
            (unsigned char*)texto.data()+i,
            tamanhoBlocoCifrado
        );

        if (ok <= 0){
            cerr << "erro na decifragem RSA" << endl;
            exit(1);
        }

        resultado.append(
            (char*)saida.data(),
            tamanhoGerado
        );
    }

    EVP_PKEY_CTX_free(ctx);

    return resultado;
}

int main(){
    vector<string> arquivos = {
        "livros/gettysburg.txt",
        "livros/raven.txt",
        "livros/alice.txt",
        "livros/war_and_peace.txt"
    };

    // Geracao da chave nao faz parte do tempo medido
    EVP_PKEY *chave = geraChaveRSA();

    cout << fixed << setprecision(3);

    cout << "arquivo,bytes,cifragem_ms,decifragem_ms,cifragem_MBps,decifragem_MBps\n";

    for (string arquivo : arquivos){
        string original = leArquivo(arquivo);

        vector<double> temposEncriptacao;
        vector<double> temposDecriptacao;

        for (int teste = 0; teste < REPETICOES; teste++){
            auto inicio = chrono::high_resolution_clock::now();

            string cifrado = encriptaRSA(original, chave);

            auto fim = chrono::high_resolution_clock::now();

            double tempo =
                chrono::duration<double, milli>(fim-inicio).count();

            temposEncriptacao.push_back(tempo);
        }

        string cifrado = encriptaRSA(original, chave);

        for (int teste = 0; teste < REPETICOES; teste++){
            auto inicio = chrono::high_resolution_clock::now();

            string decriptado = decriptaRSA(cifrado, chave);

            auto fim = chrono::high_resolution_clock::now();

            double tempo =
                chrono::duration<double, milli>(fim-inicio).count();

            temposDecriptacao.push_back(tempo);

            if (decriptado != original){
                cerr << "ERRO: decriptacao incorreta em "
                     << arquivo << endl;

                EVP_PKEY_free(chave);
                return 1;
            }
        }

        double enc = mediana(temposEncriptacao);
        double dec = mediana(temposDecriptacao);

        double tamanhoMB =
            original.size()/(1024.0*1024.0);

        double encMBps =
            tamanhoMB/(enc/1000.0);

        double decMBps =
            tamanhoMB/(dec/1000.0);

        cout
            << arquivo << ","
            << original.size() << ","
            << enc << ","
            << dec << ","
            << encMBps << ","
            << decMBps << "\n";
    }

    EVP_PKEY_free(chave);

    return 0;
}