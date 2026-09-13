#!/bin/bash

# faz processos zumbi para testar o programa
for i in {1..5}
do
    (sleep 1 & exec /bin/sleep 60) &
done
