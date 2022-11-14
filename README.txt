 autor: Matúš Vráblik
 dátum vytvorenia: 4.11.2022
 popis: NetFlow Exportér (offline)
            - ide o NetFlow exportér ktorý spracuje zachytené záznamy zo súboru alebo STDIN a
                exportuje ich na adresu analyzátoru. Tento program nie je určený pre prácu so živými paketmi!
 príklad spustenia: flow -f test.pcap -m 512 -i 10 -a 60 -c localhost:2055
