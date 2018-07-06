#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <wiringPi.h>
#include "parser.h"
#include "haiku.h"
#include "grammar.h"

#define ledPin    0  	//define the ledPin
#define buttonPin 1     //define the buttonPin

int main(void) {

    FILE *file = fopen("dictionary.txt", "r");
    FILE *haikus = fopen("stanzaformat.txt", "r");

    if (file == NULL) {
        perror("Failed to open dictionary file!");
        exit(EXIT_FAILURE);
    }

    if (haikus == NULL) {
        perror("Failed to open stanza format file!");
        exit(EXIT_FAILURE);
    }

    Dictionary dict;
    init_dict(&dict);
    parse_dict(file, &dict);
    fclose(file);
    Haiku haiku;
    parse_haikus(haikus, &haiku);
    fclose(haikus);

    //-------------------------------------------------------------------------
    // comment below for running with Raspberry Pi.
    //-------------------------------------------------------------------------
    /*
    char *result = make_haiku(&haiku,&dict);
    FILE *out = fopen("out.txt", "w");
    printf("%s\n",result);
    fprintf(out, "%s", result);
    fclose(out);
    */
    //-------------------------------------------------------------------------
    // comment above for running with Raspberry Pi.
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
           // uncomment for running with Raspberry Pi.
    //-------------------------------------------------------------------------
    
    if(wiringPiSetup() == -1){ //when initialize wiring failed,print messageto screen
        printf("setup wiringPi failed !");
        return 1;
    }

    pinMode(ledPin, OUTPUT); //Set ledPin output
    pinMode(buttonPin, INPUT);//Set buttonPin input

    pullUpDnControl(buttonPin, PUD_UP);  //pull up to high level
    while(1) {
        if(digitalRead(buttonPin) == LOW){ //button has pressed down
            digitalWrite(ledPin, HIGH);   //led on
            char *result = make_haiku(&haiku,&dict);
            printf("%s\n",result);
            printf("\n");
            printf("\n");
            printf("\n");
            FILE *out = fopen("out.txt", "w");
            fprintf(out, "%s", result);
            fclose(out);
            digitalWrite(ledPin, LOW);   //led off
            break;
        }
        else {				//button has released
            digitalWrite(ledPin, LOW);   //led off
            //printf("...led off\n");
        }
    }

    FILE *pp;
    pp = popen("festival --tts out.txt", "r");
    if (pp != NULL) {
        while (1) {
            char *line;
            char buf[1000];
            line = fgets(buf, sizeof buf, pp);
            if (line == NULL) break;
            if (line[0] == 'd') printf("%s", line);
        }
        pclose(pp);
    }

    
    //-------------------------------------------------------------------------
    // uncomment for running with Raspberry Pi.
    //-------------------------------------------------------------------------

    free_dict(&dict);
    free(haiku.haikus);
    free(haiku.source);
    
    main();

}
