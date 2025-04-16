[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/H1vNwaly)
# Hello, World!

Protokoll Übung Systemprogrammierung:

Besprochene Themen in der LE: 
In der LE zur Systemprogrammierung haben wir uns mit der Interprozesskommunikation(IPC) via Message Queues auseinandergesetzt. Hauptfokus war hier zu lernen, wie Prozesse miteinander kommunizieren können, und wie man MAN-Pages aufrufen / verwenden kann.

Während der LE wurden mehrere neue Libraries verwendet und demonstriert, welche wir während des ersten Semesters in Grundlagen der Programmierung noch nicht eingesetzt haben. Beispiele für diese Libraries wären:

-sys/types.h
-sys/wait.h
-mqueue.h
-unistd.h

Im Detail wurde sich in der LE auf das Verständnis fokussiert, wie Message Queues mit Prozessen verwendet werden, wie man die MAN-Pages korrekt liest, versteht und die daraus abgeleiteten Informationen verwenden kann. Weiters wurde durch diverse gemeinsame Implementationen in der LE die Verwendung der neu erlernten Libraries geübt. Unter anderem:

-getopt um die Argumente aus der Command-Line zu übergeben (in der LE Übung -i , -s, -b)
-fork() - Um neue Prozesse, und neue Child-Prozesse zu erstellen
-sleep() - Damit zufällige Unterbrechungen simuliert werden können - via rand() Funktion
-wait() - Um den Parent-Prozess auf die Child-Prozesse warten zu lassen, bis diese beendet wurden
-srand(), rand() - Um Zufallszahlen zu generieren

Hintergedanke war hier als Erkenntnis mitzunehmen, wie Prozesse erstellt und verwaltet werden, und wie Child mit Parent Prozessen kommunizieren. Mitgegeben wurde, dass es wichtig ist die Message Queues korrekt zu verwenden, zu schließen und auch wieder zu löschen.

Während der Übung war es jedoch etwas anspruchsvoll, dem kompletten Coding Verlauf zu folgen, da dieser schon in relativ schneller Geschwindigkeit vorgezeigt wurde und das Vorwissen zu den neuen Libraries noch nicht vorhanden war. Somit kam es schnell dazu, dass eine Funktion oder neue Implementation während der LE nicht korrekt übernommen werden konnte, und somit das Ergebnis der Veranschaulichung nicht 1:1 gleich war. Dies konnte aber durch nachträgliche Recherche und Nachfragen bei Studienkollegen wieder ausgebessert werden. Für die Hausübung war diese LE relativ wichtig, da die vorgezeigte Implementation eine gute Grundlage für die Hausübung geboten hat.
