# Grupo
Felipe Costa de Sousa - 211055236
Cássio Vinicius Teixeira Borges - 211036141

# Sobre
O projeto foi desenvolvido na linguagem c++

# Compilando o montador/ligador

```console
foo@bar:~$ g++ -o montador montador.cpp
```

```console
foo@bar:~$ g++ -o ligador ligador.cpp
```

# Montagem do arquivo .asm
```console
foo@bar:~$ ./montador file.asm
```
O arquivo de pre-processamento é gerado com o mesmo nome do arquivo utilizado ```.asm``` utilizado.

# Montagem do arquivo .pre
```console
foo@bar:~$ ./montador file.pre
```
Ao término da montagem é gerado com o mesmo nome do arquivo utilizado ```file.obj```

# Ligação dos arquivos
```console
foo@bar:~$ .ligador file1.obj file2.obj
```

# Versão G++
- gcc version 13.3.0 (Ubuntu 13.3.0-6ubuntu2~24.04)
- gcc version 14.1.0 (Windows 11)

# GitHub
- https://github.com/Cassio-hue/sb-trabalho-1/tree/main

# Begin
- Nosso BEGIN está como os slides visto em aula, segue o exemplo:

```code
MOD_A: BEGIN
```