#! /bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

proj2out_check() {
    if [[ ! -f "proj2.out" ]]
    then
        echo -e "${RED}Error: File proj2.out not found!${NC}"
    exit 1
    fi

    if [[ ! -f "./kontrola-vystupu.sh" ]]
    then
        echo -e  "${RED}Error: File ./kontrola-vystupu.sh not found!${NC}"
    exit 1
    fi

    echo -e "${YELLOW}"
    cat ./proj2.out | ./kontrola-vystupu.sh
    echo -e "${NC}"

    rm ./proj2.out
}

if [[ $1 == "-h" ]]
then
    echo "IOS projekt 2 tester"
    echo "Tester vypisuje vsechny testovane vstupy, v pripade chyby vypise ocekavany navratovy kod programu a skutecny navratovy kod. Pokud se tester sekne, znamena to pravdepodobne deadlock ve vasek programu"
    echo "Prerekvizity: Funkcni Makefile a skript \"kontrola-vystupu.sh\" ve stejnem adresari jako je tento skript. test-vystupu.sh je dostupny na https://moodle.vut.cz/course/view.php?id=231005"
    echo "Spustenim skriptu s parametrem -e zapnete extremni testovani. Toto testuje vstupy daleko za hranicemi normalnich cisel, ktere pravdepodobne zpusobi pad programu, pripadne zaplneni maximalniho poctu procesu definovaneho v /proc/sys/kernel/pid_max. Nepredpoikladam, ze by cvici testovali neco z tohoto, takze to je spis pro fajnsmekry."
    echo "Skript predpoklada, ze soubor proj2 si vytvori proj2.out, pokud soubor neexistuje - vystupove soubory jsou ve skriptu premazavany (coz odpovida implementaci, protoze jestli ma make vytvorit spustitelny program nezavisly na souborech, musi si program v pripade neexistence proj2.out tento soubor vytvorit)."
    echo "Je normalni, ze beh skriptu chvili trva, vzhledem k extremnim paramnetrum, ktere programu zadava. Chvili (cca 30 s) pockejte nez situaci vyhodnotite jako deadlock/nekonecny cyklus."
    echo "V pripade problemu piste na dc: White Knight#8252"
    echo "PS: Berte to s rezervou, nemusi to fungovat spravne vsem, je to splacane na koleni v autobusu a jsem clovek co ze shell projektu dostal 6 bodu"
    exit 0
fi

if [[ ! -f "Makefile" ]]
then
    echo -e "${RED}Error: No makefile found!${NC}"
    exit 1
fi

make

if [[ ! -f "proj2" ]]
then
    echo -e "${RED}Error: Wrong file name! Correct is \"proj2\"${NC}"
exit 1
fi

#./proj2 NZ NU TZ TU F
#chybejici vstupni promenne:
echo "Testing missing arguments..."
    ./proj2 &>/dev/null
    if [[ $? != 1 ]]
    then
        echo -e "${RED}FAILED \"./proj2\" returned with: $?. Return code 1 expected!${NC}"
        rm ./proj2.out
        exit 1
    else 
        echo -e "${GREEN}OK ./proj2${NC}" 
    fi

    ./proj2 1 &>/dev/null
    if [[ $? != 1 ]]
    then
        echo -e "${RED}proj2 returned with: $?. Return code 1 expected!${NC}"
        rm ./proj2.out
        exit 1
    else 
        echo -e "${GREEN}OK ./proj2 1${NC}" 
    fi

    ./proj2 1 1 &>/dev/null
    if [[ $? != 1 ]]
    then
        echo -e "${RED}proj2 returned with: $?. Return code 1 expected!${NC}"
        rm ./proj2.out
        exit 1
    else 
        echo -e "${GREEN}OK ./proj2 1 1${NC}" 
    fi
    
    ./proj2 1 1 1 &>/dev/null
    if [[ $? != 1 ]]
    then
        echo -e "${RED}proj2 returned with: $?. Return code 1 expected!${NC}"
        rm ./proj2.out
        exit 1
    else 
        echo -e "${GREEN}OK ./proj2 1 1 1${NC}" 
    fi

    ./proj2 1 1 1 1 &>/dev/null
    if [[ $? != 1 ]]
    then
        echo -e "${RED}proj2 returned with: $?. Return code 1 expected!${NC}"
        rm ./proj2.out
        exit 1
    else 
        echo -e "${GREEN}OK ./proj2 1 1 1 1${NC}" 
    fi

#neciselne argumenty
echo "Testing non-numeric arguments..."
    ./proj2 k 1 1 1 1 &>/dev/null
    if [[ $? != 1 ]]
    then
        echo -e "${RED}FAILED \"./proj2 k 1 1 1 1\" returned with: $?. Return code 1 expected!${NC}"
    else
        echo -e "${GREEN}OK ./proj2 k 1 1 1 1${NC}" 
    fi

    ./proj2 1 o 1 1 1 &>/dev/null
    if [[ $? != 1 ]]
    then
        echo -e "${RED}FAILED \"./proj2 1 o 1 1 1\" returned with: $?. Return code 1 expected!${NC}"
    else
        echo -e "${GREEN}OK ./proj2 1 o 1 1 1${NC}" 
    fi

    ./proj2 1 1 k 1 1 &>/dev/null
    if [[ $? != 1 ]]
    then
        echo -e "${RED}FAILED \"./proj2 1 1 k 1 1\" returned with: $?. Return code 1 expected!${NC}"
    else
        echo -e "${GREEN}OK ./proj2 1 1 k 1 1${NC}" 
    fi
    
    ./proj2 1 1 1 o 1 &>/dev/null
    if [[ $? != 1 ]]
    then
        echo -e "${RED}FAILED \"./proj2 1 1 1 o 1\" returned with: $?. Return code 1 expected!${NC}"
    else
        echo -e "${GREEN}OK ./proj2 1 1 1 o 1${NC}" 
    fi

    ./proj2 1 1 1 1 t &>/dev/null
    if [[ $? != 1 ]]
    then
        echo -e "${RED}FAILED \"./proj2 1 1 1 1 t\" returned with: $?. Return code 1 expected!${NC}"
    else
        echo -e "${GREEN}OK ./proj2 1 1 1 1 t${NC}" 
    fi
    
    if [[ ! -f "./proj2" ]]
    then
        rm ./proj2.out
    fi

#promenne mimo zadany rozsah - nema projit!:
echo "Testing invalid arguments..."

    ./proj2 0 1 1 1 1 &>/dev/null
    if [[ $? == 1 ]]
    then
        echo -e "${GREEN}OK ./proj2 0 1 1 1 1${NC}"
    else
        echo -e "${RED}FAILED \"./proj2 0 1 1 1 1\" returned with: $?. Return code 1 expected!${NC}"
        exit 1
    fi

    ./proj2 1 1 -1 1 1 &>/dev/null
    if [[ $? == 1 ]]
    then
        echo -e "${GREEN}OK ./proj2 1 1 -1 1 1${NC}"
    else
        echo -e "${RED}FAILED \"./proj2 1 1 -1 1 1\" returned with: $?. Return code 1 expected!${NC}"
        exit 1
    fi

    ./proj2 1 1 10001 1 1 &>/dev/null
    if [[ $? == 1 ]]
    then
        echo -e "${GREEN}OK ./proj2 1 1 10001 1 1${NC}"
    else
        echo -e "${RED}FAILED \"./proj2 1 1 10001 1 1\" returned with: $?. Return code 1 expected!${NC}"
        exit 1
    fi

    ./proj2 1 1 1 -1 1 &>/dev/null
    if [[ $? == 1 ]]
    then
        echo -e "${GREEN}OK ./proj2 1 1 1 -1 1${NC}"
    else
        echo -e "${RED}FAILED \"./proj2 1 1 1 -1 1\" returned with: $?. Return code 1 expected!${NC}"
        exit 1
    fi

    ./proj2 1 1 1 101 1 &>/dev/null
    if [[ $? == 1 ]]
    then
        echo -e "${GREEN}OK ./proj2 1 1 1 101 1${NC}"
    else
        echo -e "${RED}FAILED \"./proj2 1 1 1 101 1\" returned with: $?. Return code 1 expected!${NC}"
        exit 1
    fi

    ./proj2 1 1 1 1 -1 &>/dev/null
    if [[ $? == 1 ]]
    then
        echo -e "${GREEN}OK ./proj2 1 1 1 1 -1${NC}"
    else
        echo -e "${RED}FAILED \"./proj2 1 1 1 1 -1\" returned with: $?. Return code 1 expected!${NC}"
        exit 1
    fi

    ./proj2 1 1 1 1 10001 &>/dev/null
    if [[ $? == 1 ]]
    then
        echo -e "${GREEN}OK ./proj2 1 1 1 1 10001${NC}"
    else
        echo -e "${RED}FAILED \"./proj2 1 1 1 1 10001\" returned with: $?. Return code 1 expected!${NC}"
        exit 1
    fi

    if [[ ! -f "./proj2" ]]
    then
        rm ./proj2.out
    fi

#promenne tesne v zadanem rozsahu - ma projit!:
echo "Testing max/min valid arguments..."
    ./proj2 1 1 10000 1 1
    if [[ $? == 1 ]]
    then
        echo -e "${RED}FAILED \"./proj2 1 1 10000 1 1\" returned with 1 even though arguments were valid!${NC}"
        exit 1
    else
        echo -e "${GREEN}OK \"./proj2 1 1 10000 1 1\" ... running ./kontrola-vystupu.sh${NC}"
        proj2out_check
    fi

    ./proj2 1 1 0 1 1
    if [[ $? == 1 ]]
    then
        echo -e "${RED}FAILED \"./proj2 1 1 0 1 1\" returned with 1 even though arguments were valid!${NC}"
        exit 1
    else
        echo -e "${GREEN}OK \"./proj2 1 1 0 1 1\" ... running ./kontrola-vystupu.sh${NC}"
        proj2out_check
    fi

    ./proj2 1 1 100 1 1
    if [[ $? == 1 ]]
    then
        echo -e "${RED}FAILED \"./proj2 1 1 100 1 1\" returned with 1 even though arguments were valid!${NC}"
        exit 1
    else
        echo -e "${GREEN}OK \"./proj2 1 1 100 1 1\" ... running ./kontrola-vystupu.sh${NC}"
        proj2out_check
    fi
    
    ./proj2 1 1 0 1 1
    if [[ $? == 1 ]]
    then
        echo -e "${RED}FAILED \"./proj2 1 1 0 1 1\" returned with 1 even though arguments were valid!${NC}"
        exit 1
    else
        echo -e "${GREEN}OK \"./proj2 1 1 0 1 1\" ... running ./kontrola-vystupu.sh${NC}"
        proj2out_check
    fi

    ./proj2 1 1 1 1 10000
    if [[ $? == 1 ]]
    then
        echo -e "${RED}FAILED \"./proj2 1 1 1 1 10000\" returned with 1 even though arguments were valid!${NC}"
        exit 1
    else
        echo -e "${GREEN}OK \"./proj2 1 1 1 1 10000\" ... running ./kontrola-vystupu.sh${NC}"
        proj2out_check
    fi

    ./proj2 1 1 1 1 0
    if [[ $? == 1 ]]
    then
        echo -e "${RED}FAILED \"./proj2 1 1 1 1 0\" returned with 1 even though arguments were valid!${NC}"
        exit 1
    else
        echo -e "${GREEN}OK \"./proj2 1 1 1 1 0\" ... running ./kontrola-vystupu.sh${NC}"
        proj2out_check
    fi

    ./proj2 100 1 1 1 10000
    if [[ $? == 1 ]]
    then
        echo -e "${RED}FAILED \"./proj2 100 1 1 1 10000\" returned with 1 even though arguments were valid!${NC}"
        exit 1
    else
        echo -e "${GREEN}OK \"./proj2 100 1 1 1 10000\" ... running ./kontrola-vystupu.sh${NC}"
        proj2out_check
    fi

    ./proj2 1 100 1 1 10000
    if [[ $? == 1 ]]
    then
        echo -e "${RED}FAILED \"./proj2 1 100 1 1 10000\" returned with 1 even though arguments were valid!${NC}"
        exit 1
    else
        echo -e "${GREEN}OK \"./proj2 1 100 1 1 10000\" ... running ./kontrola-vystupu.sh${NC}"
        proj2out_check
    fi

if [[ $1 == "-e" ]]
then
#test extremnich hodnot NZ a NU
echo "Ruining your day by tring arguments that are far above normally used numbers..."
    echo "Running \"./proj2 1000000000000000000000000000000000 1 1 1 1...\""
    ./proj2 1000000000000000000000000000000000 1 1 1 1
    echo "Running \"./proj2 1 1000000000000000000000000000000000 1 1 1...\""
    ./proj2 1 1000000000000000000000000000000000 1 1 1
    echo "Running \"10000000000 1 1 1 1...\""
    ./proj2 10000000000 1 1 1 1
    echo "Running \"./proj2 1 10000000000 1 1 1...\""
    ./proj2 1 10000000000 1 1 1
    echo "Running \"./proj2 5000000000 1 1 1 1...\""
    ./proj2 5000000000 1 1 1 1
    echo "Running \"./proj2 1 5000000000 1 1 1...\""
    ./proj2 1 5000000000 1 1 1
    echo "Running \"./proj2 1 0.5 1 1 1...\""
    ./proj2 1 0.5 1 1 1
    echo "Running \"./proj2 0.5 1 1 1 1...\""
    ./proj2 0.5 1 1 1 1
    echo "Running \"./proj2 1 helloworld 1 1 1...\""
    ./proj2 1 helloworld 1 1 1
    echo "Running \"./proj2 helloworld 1 1 1 1...\""
    ./proj2 helloworld 1 1 1 1
fi

echo -e "Named semaphores sentenced by you to wander across the vast plains of /dev/shm for eternity:${RED}"
ls /dev/shm
echo -e "${NC}Unnamed semaphores left in memory:${RED}"
ipcs -s
echo -e "${NC}"

exit 0