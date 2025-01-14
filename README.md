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

# Montagem do arquivo .pre
```console
foo@bar:~$ ./montador file.pre
```
O arquivo de pre-processamento é gerado com o mesmo nome do arquivo ```.asm``` utilizado.

# Ligação dos arquivos
```console
foo@bar:~$ .ligador file1.obj file2.obj
```

# Versão G++
- gcc version 13.3.0 (Ubuntu 13.3.0-6ubuntu2~24.04) 