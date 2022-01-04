Dwóch producentów (p1, p2) umieszcza liczby (p1 – parzyste, p2 - nieparzyste) do trzech 
buforów FIFO (buf1, buf2, buf3 o maksymalnym rozmiarze N) na zmianę (p1 do buf1 i buf2, p2 
do buf2 i buf3). Trzech konsumentów c1, c2, c3 odbiera odpowiednio buf1, buf2, buf3. Nie 
dopuścić do czytania z pustego bufora, zapisywania do pełnego, aktywnego oczekiwania –
producenci i konsumenci powinni zawieszać się na semaforach
