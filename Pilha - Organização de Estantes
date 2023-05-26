#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct bookStack {
    char title[50];
    struct bookStack *next;
} bookStack;

typedef struct shelf {
    int numBooks;
    struct bookStack *top;
    struct shelf *next;
} shelf;

shelf *initShelf () {
    shelf *newShelf = malloc(sizeof(shelf));

    newShelf->numBooks = 0;
    newShelf->top = NULL;
    newShelf->next = NULL;

    return newShelf;
}

bookStack *newBook (char *title) {
    bookStack *book = malloc(sizeof(bookStack));

    strcpy(book->title, title);
    book->next = NULL;

    return book;
}

void getTitle (char *title, int i) {
    char c = getchar();

    if (i > 50) {
        fflush(stdin);
        getTitle(title, 0);
        return;
    }

    if (c == '\n') {
        title[i] = '\0';
        return;
    }

    title[i] = c;
    getTitle(title, i + 1);
}

shelf *getBooks (shelf *shelfList, int maxBooks) {
    char title[50] = "";
    shelf *auxShelfPointer = shelfList;
    bool write = false;

    while (title[0] != '0') {
        getTitle(title, 0);

        if (title[0] == '\0')
            continue;

        if (auxShelfPointer)
            if (auxShelfPointer->numBooks == maxBooks)
                if (title[0] != '\\' && write) {
                    printf("\nvoce atingiu o limite de livros, finalize a prateleira atual, antes de continuar a inserir livros\n");
                    continue;
                }

        if (title[0] == '/') {
            if (write) {
                printf("\nvoce nao pode iniciar uma prateleira enquanto nao terminar a atual\n");
            } else {
                if (shelfList) {
                    while (auxShelfPointer->next) 
                        auxShelfPointer = auxShelfPointer->next;
                    
                    auxShelfPointer->next = initShelf();
                    auxShelfPointer = auxShelfPointer->next;
                } else {
                    shelfList = initShelf();
                    auxShelfPointer = shelfList;
                }

                write = true;
            }
        } else if (title[0] == '\\') {
            if (!write)
                printf("\nvoce ja finalizou a ultima prateleira iniciada\n");
            else {
                auxShelfPointer = shelfList;
                write = false;
            }
        } else {
            if (write) {
                if (title[0] != '0') { 
                    bookStack *top = auxShelfPointer->top;
                    bookStack *aux = top;

                    if (!top) {
                        top = newBook(title);
                    } else {
                        while (aux->next)
                            aux = aux->next;

                        aux->next = newBook(title);
                    }

                    auxShelfPointer->numBooks += 1;

                    auxShelfPointer->top = top;
                }
            } else {
                if (title[0] != '0')
                    printf("\nNao foi iniciada uma prateleira\n");
            }
        }
    }

    return shelfList;
}

void show (shelf *shelfList, int i) {
    if (!shelfList)
        return;

    if (i % 6 == 0)
        printf("\nEstante %d:", (i / 6) + 1);
    
    bookStack *aux = shelfList->top;
    
    printf("\n/");
    if (!aux)
        printf(" Prateleira vazia");
    while (aux) {
        printf(" %s", aux->title);
        aux = aux->next;
        if (aux) 
            printf(",");
    }
    printf(" \\\n");

    show(shelfList->next, i + 1);
}

void freeShelfs (shelf *shelfList) {
    if (!shelfList)
        return;

    bookStack *aux = shelfList->top, *next = NULL;

    while (aux) {
        next = aux->next;
        free(aux);
        aux = next;
    }

    freeShelfs(shelfList->next);
    free(shelfList);
}

bookStack *push (bookStack *stack, bookStack *book) {
    if (!stack)
        book->next = NULL;
    else
        book->next = stack;
    
    return book;
}

bookStack *pop (bookStack *stack, bookStack **book) {
    bookStack *aux = stack;
    (*book) = aux;
    stack = stack->next;
    aux->next = NULL;

    return stack;
}

int precedence (char *book1, char *book2, int i) {
    if (book2[i] > book1[i]) {
        return 0;
    } else if (book2[i] < book1[i]) {
        return 1;
    } else if (book2[i] == book2[i]) {
        return precedence(book1, book2, i + 1);
    }
}

bookStack *orderStack (bookStack *stack, bookStack *book) {
    bookStack *auxStack = NULL, *auxBook;
    
    while (stack && precedence(book->title, stack->title, 0)) {
        stack = pop(stack, &auxBook);
        auxStack = push(auxStack, auxBook);
    }

    stack = push(stack, book);

    while(auxStack) {
        auxStack = pop(auxStack, &auxBook);
        stack = push(stack, auxBook);
    }

    return stack;
}

shelf *orderShelfs (shelf *bookShelf, int maxBooks) {
    shelf *auxShelfPointer = bookShelf;
    bookStack *newStack = NULL;
    
    while (auxShelfPointer) {
        bookStack *auxStackPointer = auxShelfPointer->top;

        while (auxStackPointer) {
            bookStack *auxBook;

            auxStackPointer = pop(auxStackPointer, &auxBook);
            newStack = orderStack(newStack, auxBook);

            auxShelfPointer->numBooks -= 1;
        }

        auxShelfPointer->top = NULL;

        auxShelfPointer = auxShelfPointer->next;
    }

    auxShelfPointer = bookShelf;

    bookStack *stackPointer = auxShelfPointer->top;
    while (newStack) {
        bookStack *auxBook;

        newStack = pop(newStack, &auxBook);
        auxBook->next = NULL;

        if (auxShelfPointer->numBooks == maxBooks) {
            auxShelfPointer->top = stackPointer;
            auxShelfPointer = auxShelfPointer->next;
            stackPointer = auxShelfPointer->top;
        }

        if (!stackPointer) {
            stackPointer = auxBook;
        } else {
            bookStack *auxStackPointer = stackPointer;
            while (auxStackPointer->next) {
                auxStackPointer = auxStackPointer->next;
            }

            auxStackPointer->next = auxBook;
        }

        auxShelfPointer->numBooks += 1;
    }

    auxShelfPointer->top = stackPointer;

    return bookShelf;
}

int main (void) {
    shelf *shelfList = NULL;
    int maxBooks;

    printf ("\nAo digitar '/' eh inicializada uma prateleira"
            "\nAo digitar '\\' eh finalizada a prateleira"
            "\nAo digitar '0' as estantes irao ser reorganizadas");

    printf("\n\nDigite o numero maximo de livros por estante: ");
    scanf("%d", &maxBooks);

    fflush(stdin);

    shelfList = getBooks(shelfList, maxBooks);
    printf("\n--------------------------------------------");
    printf("\nEstantes antes de reorganizar:\n");
    show(shelfList, 0);

    shelfList = orderShelfs(shelfList, maxBooks);
    printf("--------------------------------------------");
    printf("\nEstantes apos reorganizar:\n");
    show(shelfList, 0);

    freeShelfs(shelfList);
    return 0;
}
