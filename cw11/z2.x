typedef unsigned int sum_argument<100>;

union sum_wynik switch (bool wynik_jest_ok) {
    case TRUE:
        unsigned int suma;
    case FALSE:
        void;
};

program SUM_PROGRAM {
    version SUM_WERSJA_1 {
        sum_wynik SUM_OBLICZ(sum_argument) = 1;
    } = 1;
} = 0x20002019;
