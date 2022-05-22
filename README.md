# MI-FAP-I
Progetto per la tesi di laurea

30/03/2022
Per il momento è caricato solo del codice test per studiare la libreria ed eseguire alcuni test sul file "Tiny.json"

02/04/2022
Ho eseguito due commit, il primo è un prototipo non funzionante (causa bug) in cui il costruttore di una classe genera in tutti i dati necessari in una volta sola, il secondo prototipo funziona e utilizza invece più metodi.
Note: 
- nel secondo prototipo si potrebbe mettere alcuni metodi privati e usare un solo metodo unico che chiami li e completi da solo tutte le operazioni di inizializzazione.
- il test necessita del file tiny.json, può essere usato con altri file basta correggere i percorsi nel main.

22/05/2022
Questa versione ha una prima implementazione dell largest degree con un test su "Tiny.json": determina a chi verrà assegnata la frequenza dal numero di frequenze bloccate (segnate con un numero negativo), se non ci sono frequenze libere la assegna alla frequenza bloccata con modulo minore
Presenta i principali problemi:
- non sono considerati nella creazione dei file le frequenze bloccate globalmente per una svista (sarà corretta in futuro).
- nel aggiornamento dei costi viene tenuto solo conto dei vincoli (con la seguente priorità) co-cella > co-sito > handover (se due celle hanno relazione di co sito e handover sono appliati i vincoli di cosito ma non quelli di handover poichè meno restrittivi) ma non essendomi chiara l'interpretazione del problema per quando riguarda l'inteferenza da come costo a tutti -1.
- le relazioni tra le cella descritte sono considerate nel seguente modo: (from 5 to 1, assegnate frequenze su trasmettitori della cella 5 causano dei vincoli su quelli della cella 1 ma non il viceversa se 1 non ha una relazione from 1 to 5) 
