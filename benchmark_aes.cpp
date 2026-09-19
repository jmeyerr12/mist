#include <bits/stdc++.h>
#include <openssl/evp.h>
using namespace std;

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

string encriptaAES(string texto, unsigned char *chave, unsigned char *iv){
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    vector<unsigned char> resultado(texto.size()+EVP_MAX_BLOCK_LENGTH);

    int tamanho1 = 0;
    int tamanho2 = 0;

    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, chave, iv);

    EVP_EncryptUpdate(
        ctx,
        resultado.data(),
        &tamanho1,
        (unsigned char*)texto.data(),
        texto.size()
    );

    EVP_EncryptFinal_ex(
        ctx,
        resultado.data()+tamanho1,
        &tamanho2
    );

    EVP_CIPHER_CTX_free(ctx);

    return string(
        (char*)resultado.data(),
        tamanho1+tamanho2
    );
}

string decriptaAES(string texto, unsigned char *chave, unsigned char *iv){
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    vector<unsigned char> resultado(texto.size()+EVP_MAX_BLOCK_LENGTH);

    int tamanho1 = 0;
    int tamanho2 = 0;

    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, chave, iv);

    EVP_DecryptUpdate(
        ctx,
        resultado.data(),
        &tamanho1,
        (unsigned char*)texto.data(),
        texto.size()
    );

    EVP_DecryptFinal_ex(
        ctx,
        resultado.data()+tamanho1,
        &tamanho2
    );

    EVP_CIPHER_CTX_free(ctx);

    return string(
        (char*)resultado.data(),
        tamanho1+tamanho2
    );
}

int main(){
    vector<string> arquivos = {
        "livros/gettysburg.txt",
        "livros/raven.txt",
        "livros/alice.txt",
        "livros/war_and_peace.txt"
    };

    unsigned char chave[32] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
        0x17, 0x18, 0x19, 0x20, 0x21, 0x22, 0x23, 0x24,
        0x25, 0x26, 0x27, 0x28, 0x29, 0x30, 0x31, 0x32
    };

    unsigned char iv[16] = {
        0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x07, 0x08,
        0x09, 0x10, 0x11, 0x12,
        0x13, 0x14, 0x15, 0x16
    };

    cout << fixed << setprecision(3);

    cout << "arquivo,bytes,cifragem_ms,decifragem_ms,cifragem_MBps,decifragem_MBps\n";

    for (string arquivo : arquivos){
        string original = leArquivo(arquivo);

        vector<double> temposEncriptacao;
        vector<double> temposDecriptacao;

        for (int teste = 0; teste < 15; teste++){
            auto inicio = chrono::high_resolution_clock::now();

            string cifrado = encriptaAES(original, chave, iv);

            auto fim = chrono::high_resolution_clock::now();

            double tempo =
                chrono::duration<double, milli>(fim-inicio).count();

            temposEncriptacao.push_back(tempo);
        }

        string cifrado = encriptaAES(original, chave, iv);

        for (int teste = 0; teste < 15; teste++){
            auto inicio = chrono::high_resolution_clock::now();

            string decriptado = decriptaAES(cifrado, chave, iv);

            auto fim = chrono::high_resolution_clock::now();

            double tempo =
                chrono::duration<double, milli>(fim-inicio).count();

            temposDecriptacao.push_back(tempo);

            if (decriptado != original){
                cerr << "ERRO: decriptacao incorreta em "
                     << arquivo << endl;
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

    return 0;
}