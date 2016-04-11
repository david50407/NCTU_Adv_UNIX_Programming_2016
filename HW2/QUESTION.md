# Homework #2
## Advanced Programming in the UNIX Environment

Due: April 10, 2016

---

# Hi! Jack ... Hijack!?

In this homework, we are going to practice library injection and API hijacking. Please pick up one client program from the below candidate list, inject your implemented shared library (using `LD_PRELOAD`), and retrieve useful information from a running instance. Your injected library may consider retrieving the following information:

1. The IP address or host name that a client program connects to.
2. The involved file and directory operations: creation, open, read, write, rename, deletion, and so on. It would be better if the arguments of the involved operations are recorded as well.
3. The data exchanged between the client program and the servers. Please note that connections could be encrypted so you may have to hijack APIs for both encrypted and unencrypted connections.
4. Other information you think that's useful.

You have to compile your source codes and generate a shared object. You don't have to implement client programs by yourself. Instead, you must work with those binaries already installed in the system. To show that your implementation works properly, you may store the information you obtained into a single file or multiple files.

You don't have to implement all the aforementioned features. However, we will grade your homework based on the correctness, completeness, number of features, and difficulties. The evaluation of your homework will be performed on Linux workstations.

## List of Candidate Programs
You can choose your preferred client program from the following list. To simplify your work, we also provide links to the official sites of the packages. Sometimes reading the source codes of your target may help your injection.

* wget: [web](https://www.gnu.org/software/wget/); [source](http://ftp.gnu.org/gnu/wget/).
* lftp: [web](http://lftp.yar.ru/).
* links: [web](http://links.twibright.com/).
* curl: [web](https://curl.haxx.se/).
* Other networked client programs available on CS Linux workstations.

## Homework Submission
Please pack your files into a single ZIP archive and submit your homework via the E3 system. Please also provide a `Makefile` (used for compiling and linking your codes) and a `README` file (indicating which target your code hijacks and how to test your codes).

## Hints
Most of involved API functions are from the standard C library (e.g., libc.so.6 on Linux). For hijacking encrypted connections, a client program may work with OpenSSL (e.g., libcrypto.so.1.0.0 and libssl.so.1.0.0) or GNU TLS library (e.g., libgnutls.so.26). You may read the source codes of your target program first and check how your target program calls the mentioned libraries. You can then implement your homework based on your observations.
