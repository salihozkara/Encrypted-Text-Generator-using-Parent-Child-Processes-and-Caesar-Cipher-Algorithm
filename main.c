// Aralarında Parent-Child ilişkisi olan iki proses yaratılacak.
// Parent proses Ordinary Pipe kullanarak şifrelemek istediği veriyi Child prosese gönderecek. (En az 150 karakterden oluşan istediğiniz bir cümle olabilir.)
// Child proses gelen metini Sezar Şifreleme yöntemi kullanarak değiştirecek.
// Child proses şifreleme işlemini 4 thread kullanarak (paralel bir şekilde) yapacak.
// Kullanılan 4 thread, metini eşit parçalara bölecek ve şifreleme işlemi gerçekleştirilecek
// Threadlerde ki şifreleme işlemi bittikten sonra 4’e bölünen şifreli metin birleştirilecek ve Shared Memory kullanılarak ortak bir alana yazılacak.
// Son olarak Parent proses Shared Memory de ki şifrelenmiş veriyi okuyacak ve ekrana basacak.

// Komutlar
// gcc -pthread -o out main.c
// ./out "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam sagittis, ipsum at gravida suscipit, tortor lectus tincidunt mauris, iaculis semper magna turpis."

// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <pthread.h>

// Shared memory oluşturma
void *shared_memory_olustur(size_t size)
{
    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_SHARED | MAP_ANONYMOUS;
    return mmap(NULL, size, protection, visibility, -1, 0);
}
// Threadlerde kullanılan parametre tipi
typedef struct Thread_verisi
{
    int thread_id;
    char *metin;
} Thread_verisi;
// Sezar şifreleme fonksiyonu
void shift(char *kelime, int anahtar);
// Sezar şifreleme işlemini gerçekleştiren fonksiyon
void sifrele(char *kelime);
void coz(char *kelime);

// Tharedları çalıştıran fonksiyon
void child_process_fonksiyon(char *);
// Threadın iş fonksiyonu
void *thread_fonksiyon(void *);
// Shared memory pointerı
void *shmem;

int main(int argc, char *argv[])
{
    // Argüman gönderilmediyse hata verir
    if (argc == 1)
    {
        printf("Usage: %s <string>\n", argv[0]);
        return EXIT_FAILURE;
    }
    // Shared memorynin oluşturulması
    shmem = shared_memory_olustur(strlen(argv[1]));

    // Ebeveyn tarafından çocuğa yazmak için kullanılan parent_fds
    int parent_fds[2];
    // read:[0] - write:[1]
    if (pipe(parent_fds) != 0)
    {
        printf("pipes failed!\n");
        return EXIT_FAILURE;
    }
    
    // fork() child process
    int child = fork();
    if (child == -1)
    {
        printf("fork failed!\n");
        return EXIT_FAILURE;
    }
    // child process
    if (child == 0)
    {
        // Child tarafından kullanılmmayan pipe uçlarını kapatır
        close(parent_fds[1]);

        // Parent tarafından gelen metni oku
        char fromParent[161];
        int n = read(parent_fds[0], fromParent, sizeof(fromParent) - 1);
        fromParent[n] = '\0';
        //Okuduğumuz ucu artık kullanmayacağımız için kapatıyoruz
        close(parent_fds[0]);
        //Gelen metni threadlara bölmek için gönder
        child_process_fonksiyon(fromParent);

        return EXIT_SUCCESS;
    }
    // parent process
    else
    {
        // Parent tarafından kullanılmmayan pipe uçlarını kapat
        close(parent_fds[0]);
        // Child'a gönderilen metni yaz
        write(parent_fds[1], argv[1], strlen(argv[1]));
        // Yazdığımız ucu artık kullanmayacağımız için kapat
        close(parent_fds[1]);
        //Çocuğu bekliyoruz
        wait(NULL);
        //Shared memoryden okunan şifreli metni ekrana yazdır
        printf("shared memory: %s\n", (char *)shmem);
        return EXIT_SUCCESS;
    }

    return 0;
}
void child_process_fonksiyon(char *metin)
{
    // Child proses şifreleme işlemini 4 thread kullanarak (paralel bir şekilde) yapar
    for (int i = 0; i < 4; i++)
    {
        pthread_t thread;
        //Thread verisi oluşturma
        Thread_verisi *thread_verisi = (Thread_verisi *)malloc(sizeof(Thread_verisi));
        thread_verisi->thread_id = i;
        thread_verisi->metin = metin;
        //Thread oluşturma
        pthread_create(&thread, NULL, thread_fonksiyon, (void *)thread_verisi);
        //Thread başlatma
        pthread_join(thread, NULL);
    }
    //Metni shared memory'e kopyalıyor
    memcpy(shmem, metin, strlen(metin));
}
// Thread fonksiyonu
void *thread_fonksiyon(void *arg)
{
    Thread_verisi *thread_verisi = (Thread_verisi *)arg;
    int len = strlen(thread_verisi->metin);
    char sifrelenecekKisim[len / 4];
    for (int i = 0; i < len / 4; i++)
        sifrelenecekKisim[i] = thread_verisi->metin[(len / 4) * thread_verisi->thread_id + i];

    sifrele(sifrelenecekKisim);
    for (int i = 0; i < len / 4; i++)
        thread_verisi->metin[(len / 4) * thread_verisi->thread_id + i] = sifrelenecekKisim[i];

    pthread_exit(NULL);
}
// sezar şifreleme
void shift(char *kelime, int anahtar)
{
    char c = kelime[0];
    int i = 0;
    while (c != '\0')
    {
        if (c >= 65 && c <= 90)
        {
            kelime[i] = (kelime[i] + anahtar);
            if (kelime[i] > 90)
                kelime[i] = (kelime[i] % 90) + 65;
        }
        if (c >= 97 && c <= 122)
        {
            kelime[i] = (kelime[i] + anahtar);
            if (kelime[i] > 122)
                kelime[i] = (kelime[i] % 122) + 97;
        }
        i++;
        c = kelime[i];
    }
}
void sifrele(char *kelime)
{
    int anahtar = 2;
    shift(kelime, anahtar);
}
void coz(char *kelime)
{
    shift(kelime, -2);
}