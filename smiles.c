#include "smiles.h"

/*
 * Mapping of xchat smile numbers to human readable smiles.
 */

const char *smiles[] = {
    [1] ":D",
    [2] ":))",
    [3] ":DD",
    [4] ";)",
    [5] "8)",
    [6] ":)",
    [7] ":|",
    [8] ":/",
    [9] ":o",
    [10] ":(",
    [11] ">:(",
    [12] ":'(",
    [13] "*andel/dabel*",
    [14] "*smoking*",
    [15] "*srdce*",
    [16] "*kytka*",
    [17] ":{",
    [18] "=)",
    [19] "*skaredy pohled*",
    [20] ":.)",
    [21] "*mimo*",
    [22] "*pivo*",
    [23] ":-*",
    [24] "*fialovej :)*", // fialovej toci ocima
    [25] "*zamilovane oci*",
    [26] "*mmnt*",
    [27] "*amor*",
    [28] "*zarofka*",
    [29] "*hodiny*",
    [30] "*nadava*", // rve a krici
    [31] "*slunicko*",
    [32] "*oblizovak*",
    [33] "*telefon*",
    [34] "*rozhledne se a pak :)*",
    [35] "*kafe*",
    [36] "*cwok*", // toci ocima
    [37] ":-E",
    [38] "*zmotanej xicht*",
    [39] ">E",
    [40] "*zubatej*",
    [41] "*odpurce*",
    [42] "*cervena se*",
    [43] "8",
    [44] "*:) rotuje*",
    [45] ":-*",
    [46] "*bezi*",
    [47] "*hihihi*",
    [48] "*rose*",
    [49] "*dabel :D*",
    [50] ":]",
    [51] "*nesouhlasi*",
    [52] "*souhlasi*",
    [53] "*chrrr*",
    [54] "*stopa*",
    [55] "*vozickar*",
    [56] "*KISS*",
    [57] "*beruska*",
    [58] "*hamburger*",
    [59] "*ziiv*",
    [60] "*ufoun*",
    [61] "*kocka*",
    [62] "*vozrala*",
    [63] "*d:*",
    [64] "*pichacka*",
    [65] "*exit*",
    [66] "*dabel :)*",
    [67] "*duhovej smile*",
    [68] "*bobr :)*",
    [69] "*ee*",
    [70] ":P",
    [71] "*omg*",
    [72] "*nesmelej :D*",
    [73] "*zaba*",
    [74] "*?*",
    [75] "*deska*",
    [76] ":)",
    [77] "*rit*",
    [78] "*bleje*",
    [79] ";)",
    [80] "*smrtka*",
    [81] "*baby*",
    [82] "*skakajici zelenej curak*",
    [83] ":((",
    [84] "*karatista*",
    [85] "*cervena se*",
    [86] "*mava*",
    [87] "*mava*",
    [88] "*oboci*",
    [89] "*pivo*",
    [90] "*!*",
    [91] "*upir*",
    [92] ":p", // toci ocima a plazi jazyk
    [93] "*mrak*",
    [94] "*zemekoule*",
    [95] "*runner*",
    [96] "*snehulak*",
    [97] "*mesic*",
    [98] "[:)]",
    [99] "*zelva ninja*",
    [100] "*srdce s voblicejem*",
    [101] "*pirat*",
    [102] "*bu bu bu*"
};

const int smiles_count = sizeof(smiles)/sizeof(*smiles);
