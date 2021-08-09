#include <stdio.h>
#include <string.h>
#include <stdbool.h>
void f1(){
    int x = 0;
    while (x<1000){
	x++;
        while(x < 100){
	    x++;
	}
	for (int i = 0; i< 100; i++){
	    if (x%2==0){
		    x =  x+3;
	    } else {
		    x = x+1;
	    }
	    x--;
	}
	x++;
    }
    return;
}

int main() {
    char str[5][50], temp[50];
    printf("Enter 5 words: ");
// Getting strings input
    for (int i = 0; i < 5; ++i) {
        fgets(str[i], sizeof(str[i]), stdin);
    }
// storing strings in the lexicographical order
    for (int i = 0; i < 5; ++i) {
        for (int j = i + 1; j < 5; ++j) {
            if (strcmp(str[i], str[j]) > 0) {
                strcpy(temp, str[i]);
                strcpy(str[i], str[j]);
                strcpy(str[j], temp);
            }
        }
    }
    printf("\nIn the lexicographical order: \n");
    for (int i = 0; i < 5; ++i) {
        fputs(str[i], stdout);
    }
    return 0;
}  
