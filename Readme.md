# Encrypted Text Generator using Parent-Child Processes and Sezar Cipher Algorithm

This project demonstrates the usage of parent-child processes and Sezar Cipher algorithm to encrypt text using C language. The parent process sends an ordinary pipe to the child process, which encrypts the received text using the Sezar Cipher algorithm with 4 threads in parallel. The encrypted text is stored in a shared memory and is finally read by the parent process and displayed on the screen.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

What things you need to install the software and how to install them:

- A compiler that supports C11 or later
- GCC (GNU Compiler Collection)

### Installing

A step by step series of examples that tell you how to get a development env running:

1. Clone the repository to your local machine using the following command:

```bash
git clone https://github.com/salihozkara/Encrypted-Text-Generator-using-Parent-Child-Processes-and-Sezar-Cipher-Algorithm.git
```

2. Compile the code using GCC:

```bash
gcc -pthread -o <executable_file_name> main.c
```

3. Run the executable file:

```bash
./<executable_file_name>
```

## Built With

- [C](https://en.wikipedia.org/wiki/C_(programming_language)) - The programming language used
- [GCC](https://gcc.gnu.org/) - The compiler used

## Author

- **Salih Özkara** - [salihozkara](https://github.com/salihozkara)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details
