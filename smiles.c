/**
 * \file smiles.c
 * This file contains the #smiles table.
 */
#include "smiles.h"

/**
 * \brief Mapping of xchat smile numbers to human readable ones.
 */
const char *smiles[] = {
    [1] ":D", /* onovy */ 
    [2] ":-PP", /* onovy */ 
    [3] ":DD", /* onovy */ 
    [4] ";)", /* onovy */ 
    [5] "8)", /* onovy */ 
    [6] ":)", /* onovy */ 
    [7] ":|", /* onovy */ 
    [8] ":/", /* onovy */ 
    [9] ":o", /* onovy */ 
    [10] ":(", /* onovy */ 
    [11] ">:(", /* onovy */ 
    [12] ":\'(", /* onovy */ 
    [13] "*andel/dabel*", /* onovy */ 
    [14] "*smoking*", /* onovy */ 
    [15] "*srdce*", /* onovy */ 
    [16] "*kytka*", /* onovy */ 
    [17] ":{", /* onovy */ 
    [18] "=)", /* andilek.c */ 
    [19] ":-|", /* onovy */ 
    [20] ":.)", /* onovy */ 
    [21] "*mimo*", /* onovy */ 
    [22] "*pivo*", /* onovy */ 
    [23] ":-**", /* onovy */ 
    [24] "*fialovej :)*", /* andilek.c */ 
    [25] "*zamilovane oci*", /* onovy */ 
    [26] "*mmnt*", /* onovy */ 
    [27] "*amor*", /* onovy */ 
    [28] "*zarofka*", /* onovy */ 
    [29] "*hodiny*", /* onovy */ 
    [30] "*nadava*", /* onovy */ 
    [31] "*slunicko*", /* onovy */ 
    [32] "*oblizovak*", /* onovy */ 
    [33] "*telefon*", /* onovy */ 
    [34] "*rozhledne se a pak :)*", /* onovy */ 
    [35] "*kafe*", /* onovy */ 
    [36] "*cwok*", /* onovy */ 
    [37] ":-E", /* onovy */ 
    [38] "*zmotanej xicht*", /* onovy */ 
    [39] ">E", /* onovy */ 
    [40] "*zubatej*", /* onovy */ 
    [41] "*odpurce*", /* onovy */ 
    [42] "*cervena se*", /* onovy */ 
    [43] "8", /* onovy */ 
    [44] "*:) rotuje*", /* onovy */ 
    [45] ":-*", /* onovy */ 
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
    [92] ":p", 
    [93] "*mrak*", 
    [94] "*zemekoule*", /* onovy */ 
    [95] "*runner*", 
    [96] "*snehulak*", 
    [97] "*mesic*", 
    [98] "[:)]", 
    [99] "*zelva ninja*", 
    [100] "*srdce s voblicejem*", 
    [101] "*pirat*", 
    [102] "*bu bu bu*", 
    [103] "*lahev*", 
    [104] "*disco*", 
    [105] "*nevim*", 
    [106] "*kamaradi*", 
    [107] "*duel*", 
    [108] "*domluveno*", 
    [109] "*rum*", 
    [110] "*vader*", 
    [111] ":PP", /* onovy */ 
    [112] ":D", 
    [113] ":>", 
    [114] "8)", 
    [115] ";)", 
    [116] ":)", 
    [117] "*pohoda*", 
    [118] ">:)", 
    [119] "8(", 
    [120] "*vyvoleni*", 
    [121] ":((", 
    [122] ":(((", 
    [123] ":-**", 
    [124] "*bond*", 
    [125] "*pusa*", 
    [126] "*smoking*", 
    [127] "*cartman*", 
    [128] ";)", 
    [129] "%-/", 
    [130] ":\'(", 
    [131] "*vyvoleni*", 
    [132] "*K.O.*", /* mathou */ 
    [133] "*fuck off*", /* mathou */ 
    [134] "*jujky*", 
    [135] ":D", 
    [136] "*had*", 
    [137] "*zamek*", 
    [138] "*hvezda*", 
    [139] "*nadava*", 
    [140] "*pubertak*", 
    [141] "*megafon*", 
    [142] "*exhibice*", /* andilek.c */ 
    [143] "*nuzky*", 
    [144] "*nevim*", 
    [145] "*zmlaceny*", 
    [146] "*mario*", 
    [147] "*ruzove srdce*", 
    [148] "*oki*", 
    [149] "*skaredy pohled*", 
    [150] "*banik*", 
    [151] "*kapky*", 
    [152] ":o", 
    [153] "*andel*", 
    [154] "*kachnicka*", 
    [155] "*lizatko*", 
    [156] "*ostrov*", 
    [157] "*blazne*", 
    [158] "*obalka*", 
    [159] "*kostka*", 
    [160] "*satan*", 
    [161] "*ROFL*", 
    [162] ":D", 
    [163] "*party*", 
    [164] "*nevim*", 
    [165] "*kytka*", 
    [166] "*prvni pomoc*", 
    [167] "*zavislak*", 
    [168] "*papir*", 
    [169] "*magor*", 
    [170] "*call of duty*", 
    [171] "*hovno*", 
    [172] "*tocime*", 
    [173] "*zalaskovany*", 
    [174] "*co?*", 
    [175] "*andel*", 
    [176] "*supeer*", 
    [177] "*zlomene srdce*", 
    [178] "*jing & jang*", 
    [179] "*prase*", 
    [180] "8o", 
    [181] "*zarizlej*", 
    [182] "*cr*", 
    [183] "*censored*", 
    [184] "*kiss*", 
    [185] "*mizim*", 
    [186] "*icq*", 
    [187] "*prd*", 
    [188] "*na zdravi*", 
    [189] "*stop*", 
    [190] "*ctyrlistek*", 
    [191] "*klaun*", 
    [192] "*ovce*", 
    [193] "*meda*", 
    [194] "*chlastam*", 
    [195] "*spermie*", 
    [196] "*kvetinka*", 
    [197] "*darek*", 
    [198] "*tux*", 
    [199] "*ham ham*", 
    [200] "*nevinny*", 
    [201] "*bloncka*", 
    [202] "*kyklop*", 
    [203] "*zeru te*", 
    [204] "*hadka*", 
    [205] "*pes*", 
    [206] "%-/", 
    [207] "*certik*", 
    [208] "*ufon*", 
    [209] "*letadlo*", 
    [210] "*help*", 
    [211] "*cyklista*", 
    [212] "*trabi*", 
    [213] "*nuda*", 
    [214] "*mys*", 
    [215] "*obejmuti*", 
    [216] "*hlad*", 
    [217] "*nota*", 
    [218] "*pila*", 
    [219] "*potlesk*", 
    [220] "*windows*", 
    [221] "*wrong way*", 
    [222] "*spirala*", 
    [223] "*cerv*", 
    [224] "*$*", 
    [225] "*smoula*", 
    [226] "*srdicka*", 
    [227] "*nova*", 
    [228] "*pocitace*", 
    [229] "*skejtak*", 
    [230] "*(?)*", 
    [231] "*krava*", 
    [232] "*kenny*", 
    [233] "*quake3*", 
    [234] "*kytka*", 
    [235] "*prosim*", 
    [236] "*chameleon*", 
    [237] "*slovensko*", 
    [238] "*worm*", 
    [239] "*jahoda*", 
    [240] "*music*", 
    [241] "*jablko*", 
    [242] "*tresne*", 
    [243] "*cumacek*", 
    [244] "*ryba*", 
    [245] "*pes*", 
    [246] "*lebka*", 
    [247] "*polib mi*", 
    [248] "*playboy*", 
    [249] "*pepsi*", 
    [250] "*fish*", 
    [251] "*firefox*", 
    [252] "*muzika*", 
    [253] "*vino*", 
    [254] "*meda*", 
    [255] "*ufff*", 
    [256] "*myc*", 
    [257] "*hlavou do zdi*", 
    [258] "*pizza*", 
    [259] "*zaba*", 
    [260] "*hotovej*", 
    [261] "*hehehe*", 
    [262] "*stopar*", 
    [263] "*msn*", 
    [264] "*jedi*", 
    [265] "*oci z dulku*", 
    [266] "*cd*", 
    [267] "*no smile*", 
    [268] "*boze!*", 
    [269] "*grrr*", 
    [270] "*ne ne*", 
    [271] "*zurim*", 
    [272] "*darek*", 
    [273] "*kuk*", 
    [274] ":-}", 
    [275] ":((", 
    [276] ":-D", 
    [277] "*help*", 
    [278] "*strelec*", 
    [279] "*snoopy*", 
    [280] "*skype*", 
    [281] "*nike*", 
    [282] "*duch*", 
    [283] "*blizenec*", 
    [284] "*dlouhy jazyk*", 
    [285] "*pich*", 
    [286] "*prosim*", 
    [287] "*plavec*", 
    [288] "*drink*", 
    [289] "*tasmansky cert*", 
    [290] "*ooooo*", 
    [291] "*rozkaz*", 
    [292] "*kytara*", 
    [293] "*dj*", 
    [294] "*basketak*", 
    [295] "*robot*", 
    [296] "*britanie*", 
    [297] "*usa*", 
    [298] "*papam*", 
    [299] "*Shrek*", /* onovy */ 
    [300] "*zelenac*", 
    [301] "*malir*", 
    [302] "*znak*", 
    [303] "*loveletter*", 
    [304] "*vanoce*", 
    [305] "*cekam*", 
    [306] "*lebka*", 
    [307] "*pistole*", 
    [308] "*kure*", 
    [309] "*shorty\'s*", 
    [310] "*hory*", 
    [311] "*motorka*", 
    [312] "*mobil*", 
    [313] "*cigareta*", 
    [314] "*snek*", 
    [315] "*TV*", 
    [316] "*winamp*", 
    [317] "*prosiim*", 
    [318] "*ufo*", 
    [319] "*evil*", 
    [320] "*->*", 
    [321] "*slon*", 
    [322] "*miranda*", 
    [323] "*houslista*", 
    [324] "*tancujici*", 
    [325] "*malo mista*", 
    [326] "*oko*", 
    [327] "*spirala*", 
    [328] "*ryby*", 
    [329] "*hadka*", 
    [330] "*german*", 
    [331] "*kuk*", 
    [332] "*telefon*", 
    [333] "*CZ srdce*", 
    [334] "*zzzz...*", 
    [335] "*evileyes*", 
    [336] "*kenny*", 
    [337] "*maul*", 
    [338] "*silak*", 
    [339] "*zamilovane oci*", 
    [340] "*beran*", 
    [341] "*usmev*", 
    [342] "*miluju te*", 
    [343] "*pusu*", 
    [344] "*kulikuli*", 
    [345] ":-D", /* mathou */ 
    [346] "*zamilovany*", 
    [347] "*krychle*", 
    [348] "*laska*", 
    [349] "*skoda*", 
    [350] "*kozoroh*", 
    [351] "%-/", 
    [352] "*srdicka*", 
    [353] "*domov*", 
    [354] "*toust*", 
    [355] "*zadek*", 
    [356] "*lebka*", 
    [357] "*opice*", 
    [358] "*cd*", 
    [359] "*yes*", 
    [360] "*krychle*", 
    [361] "*zamilovane oci*", 
    [362] "*kukuc*", 
    [363] "*no*", 
    [364] "*laska*", 
    [365] "*radioaktivita*", 
    [366] "*elvis*", 
    [367] "*andilek*", 
    [368] "*cert.zena*", 
    [369] "*omdlim*", 
    [370] "*cert.muz*", 
    [371] "*YS*", 
    [372] "*kniha*", 
    [373] "*RS*", 
    [374] "*kun*", 
    [375] "*nova*", 
    [376] "*maul*", 
    [377] "*bleee*", 
    [378] "*cvoku*", 
    [379] "*alien*", 
    [380] "*majacek*", 
    [381] "*red dwarf*", 
    [382] "*drink*", 
    [383] "*certik*", 
    [384] "*?*", 
    [385] "*cvok*", 
    [386] "0-)", 
    [387] "*stir*", 
    [388] "*casper*", 
    [389] "*velke oci*", 
    [390] "*joint*", 
    [391] "*BS*", 
    [392] "*stydlin*", 
    [393] "*vahy*", 
    [394] "%-O", /* mathou */ 
    [395] "*lev*", 
    [396] "*mlcim*", 
    [397] "*brucoun*", 
    [398] "*GS*", 
    [399] "*pohodicka*", 
    [400] "*slon*", 
    [401] "*VIP*", 
    [402] "*PC crash*", 
    [403] "*haha*", 
    [404] "*amerika*", 
    [405] "*andilek*", /* onovy */ 
    [406] "*slint*", 
    [407] "*tabletka*", 
    [408] "*zamerovac*", 
    [409] "*bublina*", 
    [410] "*strom*", 
    [411] "*bublina*", 
    [412] "8-)", 
    [413] ";-D", 
    [414] "*vetrak*", 
    [415] "*lol*", 
    [416] ">-(", 
    [417] "*vodnar*", 
    [418] "X-|", 
    [419] ":-)", 
    [420] "*pes*", 
    [421] "*jejky*", 
    [422] "*playboy*", 
    [423] "^-)", 
    [424] "*akta x*", 
    [425] "*apu*", 
    [426] "*jednovokej*", 
    [427] "0-D", 
    [428] ":-<", 
    [429] "*IE*", 
    [430] "*srdce*", 
    [431] ":-)", 
    [432] "*apple*", 
    [433] "*notes*", 
    [434] "*prase*", 
    [435] "*lebka*", 
    [436] ":-(", 
    [437] ";-)", 
    [438] "*dabel*", 
    [439] "*nastvany*", 
    [440] "*nastvany*", 
    [441] "*pusa*", 
    [442] "*pavouk*", 
    [443] "*krokodyl*", 
    [444] "*smutnej*", 
    [445] "*madarko*", 
    [446] "*mava*", 
    [447] "*hehehe*", 
    [448] "*pavoucek*", 
    [449] "*tresne*", 
    [450] "*blabla*", 
    [451] "*letadlo*", 
    [452] "*beruska*", 
    [453] "*cvok*", 
    [454] "*k prasknuti*", 
    [455] "*bomba*", 
    [456] "*zaba*", 
    [457] "*premyslim*", 
    [458] "*zaba*", 
    [459] "*spendlik*", 
    [460] "*kuzle*", 
    [461] "*lebka*", 
    [462] ":-D", 
    [463] "*svicka*", 
    [464] "*vidim vas*", 
    [465] "*neverim*", 
    [466] ";-)", 
    [467] "*hehe*", 
    [468] "*jeee*", 
    [469] "*kladivko*", 
    [470] "*brusle*", 
    [471] "*jazycek*", 
    [472] "*klavesy*", 
    [473] "*RIP*", 
    [474] "*slepa baba*", 
    [475] "*sova*", 
    [476] "*hulic*", 
    [477] "*peace*", 
    [478] "*lol*", 
    [479] "*bulharsko*", 
    [480] "*smisek*", 
    [481] "*juu*", 
    [482] "*pohoda*", 
    [483] "*nemecko*", 
    [484] "*dansko*", 
    [485] "*raketa*", 
    [486] "*cinan*", 
    [487] ":p", 
    [488] "*nastvany*", 
    [489] "*kyta*", 
    [490] "*britanie*", 
    [491] "*boj*", 
    [492] "*hmm*", 
    [493] "*cervena se*", 
    [494] "*kouli se*", 
    [495] "*spanelsko*", 
    [496] "*francie*", 
    [497] "*horu dolu*", 
    [498] "*to on*", 
    [499] "*nastvany*", 
    [500] "*zklamany*", 
    [501] "*kuk*", 
    [502] "*mrk*", 
    [503] "*ninja*", 
    [504] "*rezac*", 
    [505] "*tiskarna*", 
    [506] ":.(", 
    [507] "*zombie*", 
    [508] "*zelenej kuk*", 
    [509] "*zvraci*", 
    [510] "8-|", 
    [511] "*ninja*", 
    [512] "*recko*", 
    [513] "*italie*", 
    [514] ":-$", 
    [515] "*znacka*", 
    [516] ":-{", 
    [517] "*mobil*", 
    [518] "*Rx*", 
    [519] "*ukecanej*", 
    [520] "*propichlej sipem*", 
    [521] "*deleni*", 
    [522] ":-\\", /* georgo10 */ 
    [523] "*CS*", 
    [524] "*blabla*", 
    [525] "*upir*", 
    [526] "*OK*", 
    [527] "*gta*", 
    [528] "*polsko*", 
    [529] "*turecko*", 
    [530] "*spatnej dech*", 
    [531] "*love*", 
    [532] "*student*", 
    [533] "*japonsko*", 
    [534] "*flekatej :D*", 
    [535] "*ozrala*", 
    [536] ":-)", 
    [537] "8-)", 
    [538] ":-**", 
    [539] "*ach jo*", 
    [540] "*zaba*", 
    [541] "*pizza*", 
    [542] "*zmlaceny*", 
    [543] "*chlastam*", 
    [544] "*zmrzlina*", 
    [545] "*darek*", 
    [546] "*dort*", 
    [547] "*hot dog*", 
    [548] ":DD", 
    [549] "*pikachu*", 
    [550] ":\')", 
    [551] "*klaun*", 
    [552] "*spion*", 
    [553] "*blee*", 
    [554] "^_^", 
    [555] "XD", 
    [556] "^D", 
    [557] "*rozpacity*", 
    [558] "*1. pusa*", 
    [559] "*hihi*", 
    [560] ";-D", 
    [561] "*kde jsi*", 
    [562] "*tanec*", 
    [563] "*odchazim*", 
    [564] ">:-(", 
    [565] "8-D", 
    [566] "*oboci*", 
    [567] ":-)", 
    [568] ":p", 
    [569] "*dansko*", 
    [570] "*ziv*", 
    [571] "*piisk*", 
    [572] "*naraz*", 
    [573] ":-)", 
    [574] "*hc vsetin*", 
    [575] "*andel*", 
    [576] "*dabel*", 
    [577] "*laska*", 
    [578] "*paranoidni*", 
    [579] "*salute*", 
    [580] "*sarkasticky*", 
    [581] "*darebaci*", /* onovy */ 
    [582] "*silak*", 
    [583] "*kuli oci*", 
    [584] "*haha*", 
    [585] ":-)", 
    [586] "*z5*", 
    [587] ":-)", 
    [588] "*oslava*", 
    [589] "*zzZZ*", 
    [590] "*cernobily*", 
    [591] ".-)", 
    [592] ":-|", 
    [593] ":-((", 
    [594] "*stribny usmev*", 
    [595] "*kytara*", 
    [596] ":-/", 
    [597] "*cervena se*", 
    [598] "*alien*", 
    [599] "*georgia*", 
    [600] "*jablko*", 
    [601] "*dabelsky smich*", 
    [602] "*snek*", 
    [603] "*srdce*", 
    [604] "*kurba*", 
    [605] "8(", 
    [606] "*kejvani hlavou*", 
    [607] "*hihihi*", /* andilek.c */ 
    [608] "*jen pojd*", 
    [609] "*srdce*", 
    [610] "*haha*", 
    [611] "*andilek*", 
    [612] "*ach jo*", 
    [613] "*omraceny*", 
    [614] "*neviditelny*", 
    [615] "*eh*", 
    [616] "*fousac*", 
    [617] "*pes*", 
    [618] "*auto*", 
    [619] "*bmw*", 
    [620] "*sluchatka*", 
    [621] "*brej den*", 
    [622] "*vypadenej zub*", 
    [623] "*mnam*", 
    [624] "*chat*", 
    [625] ":-D", /* onovy */ 
    [626] "*jeje*", 
    [627] "*to snad ne*", 
    [628] "*hruska*", 
    [629] "*potapec*", 
    [630] "*smutny*", 
    [631] ":D", 
    [632] "*rak*", 
    [633] "*[A]*", 
    [634] "*vodnar*", 
    [635] "*beran*", 
    [636] ":-(", 
    [637] "*pero*", 
    [638] "*micek*", 
    [639] "*flekatej ;)*", 
    [640] "*lebka*", 
    [641] "*drzim palec*", 
    [642] "*kyle*", 
    [643] "*nevim*", 
    [644] "*vcela*", 
    [645] "*nechci*", 
    [646] "*beruska*", 
    [647] "*smoking*", 
    [648] "*posta*", 
    [649] "*nekourit*", 
    [650] "*zranice*", 
    [651] "*ruzovej*", /* onovy */ 
    [652] "*kvetina*", 
    [653] "*monokl*", /* onovy */ 
    [654] "*letadlo*", /* onovy */ 
    [655] "*cinzano*", /* onovy */ 
    [656] "*ruka V*", /* alien */ 
    [657] "*lebka*", /* onovy */ 
    [658] "*odpocet*", /* alien */ 
    [659] "*klic*", /* alien */ 
    [660] "*myska*", /* alien */ 
    [661] "*zhulenej*", /* alien */ 
    [662] "*srdicka*", /* alien */ 
    [663] "*prasatko*", /* alien */ 
    [664] "*zmateny*", /* alien */ 
    [665] "*juuu*", /* alien */ 
    [666] "*ach ne*", /* alien */ 
    [667] "*papa*", /* alien */ 
    [668] "*policajt*", /* alien */ 
    [669] "*ksiltovka*", /* onovy */ 
    [670] "*strom*", /* alien */ 
    [671] "*letadlo*", /* alien */ 
    [672] "*ruze*", /* alien */ 
    [673] "*tak co?*", /* alien */ 
    [674] "*tcha*", /* alien */ 
    [675] "*vysmata smrtka*", /* alien */ 
    [676] "*hihi, trapny*", /* alien */ 
    [677] "*demon*", /* alien */ 
    [678] "*problem?*", /* alien */ 
    [679] "*kvetinka*", /* mathou */ 
    [680] "*zurici*", /* alien */ 
    [681] "*hec*", /* alien */ 
    [682] "*zamilovany*", /* alien */ 
    [683] ":-|", /* alien */ 
    [684] "*lyzovani*", /* alien */ 
    [685] "*jahoda*", /* alien */ 
    [686] "*zaba*", /* alien */ 
    [687] "*prsten*", /* alien */ 
    [688] "*citron*", /* alien */ 
    [689] "*pozor*", /* alien */ 
    [690] "*zvraci*", /* alien */ 
    [691] ":-*", /* alien */ 
    [692] ":D", /* alien */ 
    [693] "*televize*", /* alien */ 
    [694] "*plakajici*", /* alien */ 
    [695] "*korunka*", /* alien */ 
    [696] "*muchomurka*", /* alien */ 
    [697] ":-(", /* alien */ 
    [698] "*chicht*", /* alien */ 
    [699] "*cokl*", /* alien */ 
    [700] "*muheh*", /* alien */ 
    [701] "*dudlik*", /* alien */ 
    [702] "*ty si*", /* georgo10 */ 
    [703] "*zubatej*", /* alien */ 
    [704] ">:-(", /* alien */ 
    [705] "*rocker*", /* alien */ 
    [706] "*mnami*", /* alien */ 
    [707] "*tuc tuc*", /* alien */ 
    [708] "*nene*", /* alien */ 
    [709] "*buch buch*", /* alien */ 
    [710] "*stopy*", /* alien */ 
    [711] "*papa*", /* alien */ 
    [712] "*ja nic*", /* alien */ 
    [713] ":-S", /* alien */ 
    [714] "*trumpeta*", /* alien */ 
    [715] "*ziva*", /* alien */ 
    [716] "*promin*", /* alien */ 
    [717] "*princezna*", /* alien */ 
    [718] "*skejtak*", /* alien */ 
    [719] "*bleee*", /* alien */ 
    [720] ":-*", /* alien */ 
    [721] "*hulic*", /* alien */ 
    [722] ">:-P", /* alien */ 
    [723] ";-(", /* alien */ 
    [724] "*zmrzlej*", /* alien */ 
    [725] "*klaun*", /* alien */ 
    [726] "*zubejda*", /* alien */ 
    [727] "*mrka*", /* alien */ 
    [728] "*beran*", /* alien */ 
    [729] "*jj*", /* alien */ 
    [730] ":)", /* alien */ 
    [731] ":-*", /* alien */ 
    [732] ";-)", /* alien */ 
    [733] ".-)", /* alien */ 
    [734] ":)", /* alien */ 
    [735] "=)", /* alien */ 
    [736] ":-**", /* onovy */ 
    [737] "*kytara*", /* alien */ 
    [738] "*laska*", /* alien */ 
    [739] "*letadlo*", /* alien */ 
    [740] "*sanitka*", /* alien */ 
    [741] "*plakajici*", /* alien */ 
    [742] ":-\\", /* alien */ 
    [743] "*hruska*", /* alien */ 
    [744] "*zere*", /* alien */ 
    [745] "*hvezda*", /* alien */ 
    [746] ":-(", /* alien */ 
    [747] "*posila pusu*", /* alien */ 
    [748] ">:-$", /* alien */ 
    [749] ":-o", /* alien */ 
    [750] ":-/", /* alien */ 
    [751] ":)", /* alien */ 
    [752] ">:-z", /* alien */ 
    [753] "*mava*", /* alien */ 
    [754] ":)", /* alien */ 
    [755] "*to ne!*", /* onovy */ 
    [756] "*tuli tuli*", /* alien */ 
    [757] "*beran*", /* alien */ 
    [758] "*jedu*", /* alien */ 
    [759] "*laska*", /* alien */ 
    [760] "*pistol*", /* alien */ 
    [761] ":-|", /* alien */ 
    [762] ":-|", /* alien */ 
    [763] "*kvet*", /* alien */ 
    [764] "*pac*", /* alien */ 
    [765] "*meda*", /* alien */ 
    [766] "*mnam*", /* alien */ 
    [767] "*SMS*", /* alien */ 
    [768] "*ping pong*", /* alien */ 
    [769] "*bila kytka*", /* alien */ 
    [770] ";-)", /* alien */ 
    [771] ":-(", /* alien */ 
    [772] "*ziva*", /* alien */ 
    [773] "*cervena kytka*", /* alien */ 
    [774] "8-)", /* alien */ 
    [775] "*stopa*", /* onovy */ 
    [776] "*stydi se*", /* alien */ 
    [777] "*fu*", /* alien */ 
    [778] "*usina*", /* alien */ 
    [779] "*X*", /* alien */ 
    [780] "*banan*", /* alien */ 
    [781] "*srdce*", /* alien */ 
    [782] "*ksiltovka*", /* alien */ 
    [783] "*palec nahoru*", /* alien */ 
    [784] "*radioaktivni*", /* alien */ 
    [785] "*muhehe*", /* alien */ 
    [786] "*vidi 4x*", /* alien */ 
    [787] "*palec dolu*", /* alien */ 
    [788] "*$$ v ocich*", /* alien */ 
    [789] "*letadlo*", /* alien */ 
    [790] "*sibenice*", /* alien */ 
    [791] "*palec nahoru*", /* alien */ 
    [792] "*palec dolu*", /* alien */ 
    [793] "*tudle nudle*", /* alien */ 
    [794] "*divka*", /* alien */ 
    [795] "*obalka*", /* alien */ 
    [796] "*medvidek*", /* alien */ 
    [797] "*kyticka*", /* alien */ 
    [798] "*zakazano*", /* alien */ 
    [799] "*Svedsko*", /* alien */ 
    [800] "*sachy*", /* alien */ 
    [801] "*joystick*", /* alien */ 
    [802] "*gramofon*", /* alien */ 
    [803] "*ptak zobe*", /* alien */ 
    [804] "*mesic*", /* alien */ 
    [805] "*zatazeno*", /* alien */ 
    [806] "*divka*", /* alien */ 
    [807] "*ty ty ty!*", /* alien */ 
    [808] "*kobliha*", /* alien */ 
    [809] "*pocitace*", /* alien */ 
    [810] "*Keny*", /* alien */ 
    [811] "*juuu*", /* alien */ 
    [812] "*hranolky*", /* andilek.c */ 
    [813] "*bonbon*", /* alien */ 
    [814] "*hulic*", /* alien */ 
    [815] "*znudenej*", /* alien */ 
    [816] "*skakajici kure*", /* alien */ 
    [817] "O_o", /* alien */ 
    [818] "*hehe*", /* alien */ 
    [819] "*slunicko*", /* alien */ 
    [820] "*vyjevenej*", /* alien */ 
    [821] "*prohlizec Opera*", /* alien */ 
    [822] ":)", /* onovy */ 
    [823] "*barevnej :)*", /* alien */ 
    [824] ":D", /* alien */ 
    [825] "*brejloun*", /* alien */ 
    [826] "*beran*", /* alien */ 
    [827] "*zvracim*", /* alien */ 
    [828] "*dest*", /* alien */ 
    [829] "*zaba**", /* mathou */ 
    [830] "*nagelovanej dement*", /* alien */ 
    [831] "*traktor*", /* alien */ 
    [832] "*lebka*", /* alien */ 
    [833] "*zmrzlina*", /* mathou */ 
    [834] ":-*", /* alien */ 
    [835] "*demon*", /* alien */ 
    [836] "*ano*", /* alien */ 
    [837] ":-P", /* alien */ 
    [838] "*s cepickou*", /* alien */ 
    [839] "*nene*", /* alien */ 
    [840] "*kralicek*", /* alien */ 
    [841] "*chci pusu*", /* alien */ 
    [842] "*logo Etnies*", /* alien */ 
    [843] "*vyplazenej jazyk*", /* georgo10 */ 
    [844] "*sonar*", /* georgo10 */ 
    [845] "*zamilovany*", /* andilek.c */ 
    [846] "*mikrofon*", /* andilek.c */ 
    [847] "*ses no!*", /* andilek.c */ 
    [848] "*tancici*", /* andilek.c */ 
    [849] "*popelnice*", /* andilek.c */ 
    [850] "*klaun*", /* andilek.c */ 
    [851] "*destnik*", /* andilek.c */ 
    [852] "*fialka*", /* andilek.c */ 
    [853] "*work*", /* andilek.c */ 
    [854] "*srdce-sip*", /* andilek.c */ 
    [855] "*papa*", /* andilek.c */ 
    [856] ":)", /* andilek.c */ 
    [857] "*A*", /* andilek.c */ 
    [858] ":D", /* andilek.c */ 
    [859] "*ruze*", /* andilek.c */ 
    [860] "*kis kis*", /* andilek.c */ 
    [861] "*tak co?*", /* andilek.c */ 
    [862] "*vybuch*", /* andilek.c */ 
    [863] "*stridani*", /* andilek.c */ 
    [864] "*cernoch*", /* andilek.c */ 
    [865] "*stop*", /* andilek.c */ 
    [866] "*cert*", /* georgo10 */ 
    [867] "*zakaz zastaveni*", /* andilek.c */ 
    [868] "*dynamit*", /* andilek.c */ 
    [869] "*HA HA*", /* andilek.c */ 
    [870] "*slepy usmev*", /* andilek.c */ 
    [871] "*ajo!*", /* andilek.c */ 
    [872] "*peace*", /* andilek.c */ 
    [873] "*pinokio*", /* andilek.c */ 
    [874] ":/", /* andilek.c */ 
    [875] ":P", /* andilek.c */ 
    [876] ":((", /* andilek.c */ 
    [877] ":-s", /* onovy */ 
    [878] "*nevolnost*", /* onovy */ 
    [879] ":-|", /* andilek.c */ 
    [880] "*propichnuti sipem*", /* alien */ 
    [881] "*hvezdicky*", /* alien */ 
    [882] ":\'(", /* alien */ 
    [883] "*fnuk*", /* alien */ 
    [884] "*srdce :)*", /* mathou */ 
    [885] "*depka*", /* alien */ 
    [886] ":DD", /* onovy */ 
    [887] "*:) :P*", /* alien */ 
    [888] "*velkej nos*", /* alien */ 
    [889] "*znechucen*", /* alien */ 
    [890] "*blondynka*", /* georgo10 */ 
    [891] "*prase*", /* alien */ 
    [892] ":(", /* alien */ 
    [893] ":-X", /* alien */ 
    [894] "*pssst*", /* alien */ 
    [895] "*:D :)*", /* alien */ 
    [896] "*lizani zadku*", /* alien */ 
    [897] ">:-z", /* alien */ 
    [898] "*kral pirat*", /* alien */ 
    [899] "*nevolnost*", /* alien */ 
    [900] "*brunetka*", /* georgo10 */ 
    [901] "8)", /* georgo10 */ 
    [902] "*pusa*", /* georgo10 */ 
    [903] "*spim*", /* georgo10 */ 
    [904] "*zzZZ*", /* georgo10 */ 
    [905] "*spatny signal*", /* georgo10 */ 
    [906] "*zelena vlajka*", /* georgo10 */ 
    [907] "*zrzka*", /* georgo10 */ 
    [908] "*zamilovany dopis*", /* georgo10 */ 
    [909] "*blah*", /* georgo10 */ 
    [910] "*kyle*", /* georgo10 */ 
    [911] "*olympiada*", /* georgo10 */ 
    [912] "*samopal*", /* georgo10 */ 
    [913] "*boxer*", /* georgo10 */ 
    [914] "*bezim*", /* georgo10 */ 
    [915] "*fnuka*", /* georgo10 */ 
    [916] "*ofacovanej*", /* georgo10 */ 
    [917] "*bleje*", /* georgo10 */ 
    [918] "*kocicka*", /* georgo10 */ 
    [919] "*urazenej*", /* georgo10 */ 
    [920] "*znudeny*", /* georgo10 */ 
    [921] "*zelenac*", /* georgo10 */ 
    [922] "*milka*", /* georgo10 */ 
    [923] "*rohlik*", /* georgo10 */ 
    [924] "*hmm*", /* georgo10 */ 
    [925] ":-|", /* onovy */ 
    [926] "*piska si*", /* georgo10 */ 
    [927] "*noty*", /* georgo10 */ 
    [928] ":((", /* georgo10 */ 
    [929] ":\'(", /* georgo10 */ 
    [930] "*psst*", /* georgo10 */ 
    [931] "*ospaly*", /* georgo10 */ 
    [932] "*wmp*", /* georgo10 */ 
    [933] "*paleta*", /* georgo10 */ 
    [934] ":((", /* georgo10 */ 
    [935] "*word*", /* georgo10 */ 
    [936] "*msn*", /* georgo10 */ 
    [937] "*totalcmd*", /* georgo10 */ 
    [938] "*dokument*", /* georgo10 */ 
    [939] "*Lisa Simpson*", /* alien */ 
    [940] "*slunicko*", /* alien */ 
    [941] "*banan*", /* georgo10 */ 
    [942] "*kostka*", /* alien */ 
    [943] "*souhlas*", /* alien */ 
    [944] "*Darth Vader*", /* alien */ 
    [945] "*klic*", /* alien */ 
    [946] "*kriz*", /* alien */ 
    [947] "*zamek*", /* alien */ 
    [948] "*kouli ocima*", /* alien */ 
    [949] "*krtek*", /* alien */ 
    [950] "*ruzovy panter*", /* alien */ 
    [951] "*brejloun*", /* alien */ 
    [952] "*zurim*", /* alien */ 
    [953] "*krev z oci*", /* alien */ 
    [954] "*prase*", /* alien */ 
    [955] "*asi praskne*", /* alien */ 
    [956] "*andilek*", /* alien */ 
    [957] ":-|", /* alien */ 
    [958] "*ham*", /* alien */ 
    [959] "*haha*", /* alien */ 
    [960] "*mizejici drzka*", /* alien */ 
    [961] "*dabel*", /* alien */ 
    [962] ":-PP", /* alien */ 
    [963] "*Slavia*", /* alien */ 
    [964] "*hvezda*", /* alien */ 
    [965] "*tucnak*", /* alien */ 
    [966] "*auto*", /* alien */ 
    [967] ">:-(", /* alien */ 
    [968] "*dabel*", /* onovy */ 
    [969] "*mimi*", /* mathou */ 
    [970] "*ospala*", /* mathou */ 
    [971] "*kocka*", /* mathou */ 
    [972] "*sova*", /* mathou */ 
    [973] "*mafian*", /* mathou */ 
    [974] "*kutalejici se*", /* mathou */ 
    [975] "*rozhodne NE!*", /* mathou */ 
    [976] "*rozhodne ANO!*", /* mathou */ 
    [977] "*srdce*", /* onovy */ 
    [978] "*auto*", /* mathou */ 
    [979] "*na disketu*", /* onovy */ 
    [980] "*s pistalkou*", /* mathou */ 
    [981] "*derbi*", /* mathou */ 
    [982] "*^, ^*", /* mathou */ 
    [983] "*srdce*", /* mathou */ 
    [984] "*lokomotiva*", /* mathou */ 
    [985] "*pomrkavajici*", /* mathou */ 
    [986] "*xchat*", /* mathou */ 
    [987] "*netopyr*", /* mathou */ 
    [988] "*maskovany*", /* mathou */ 
    [989] "*slepy*", /* mathou */ 
    [990] "*slunce*", /* mathou */ 
    [991] "*krava*", /* mathou */ 
    [992] "*yes*", /* mathou */ 
    [993] "*opicka*", /* mathou */ 
    [994] "*prichazi*", /* onovy */ 
    [995] "*prase*", /* mathou */ 
    [996] "*coze???*", /* mathou */ 
    [997] "*coze???*", /* mathou */ 
    [998] ":D", /* onovy */ 
    [999] "8)", /* onovy */ 
    [1000] "8)", /* mathou */ 
    [1001] "*ratatata*", /* mathou */ 
    [1002] "O=", /* mathou */ 
    [1003] "*bomba*", /* mathou */ 
    [1004] "*kutalejici se*", /* mathou */ 
    [1005] "*parohac*", /* onovy */ 
    [1006] "8((", /* mathou */ 
    [1007] "8I", /* mathou */ 
    [1008] "8))", /* mathou */ 
    [1009] "*to rozhodne!*", /* mathou */ 
    [1010] "8((", /* onovy */ 
    [1011] "*bojim*", /* onovy */ 
    [1012] "*ziiiiv*", /* mathou */ 
    [1013] "*alkac 8-D*", /* freezart_ */ 
    [1014] "*kasparek*", /* mathou */ 
    [1015] "*blabla*", /* mathou */ 
    [1016] "*duhovej :)*", /* onovy */ 
    [1017] "8))", /* mathou */ 
    [1018] "^,^", /* mathou */ 
    [1019] "*kuk*", /* mathou */ 
    [1020] "*hiphop man*", /* mathou */ 
    [1021] "*kondom*", /* mathou */ 
    [1022] ":D", /* onovy */ 
    [1023] "):E", /* mathou */ 
    [1024] "[:P]", /* mathou */ 
    [1025] "[:D]", /* mathou */ 
    [1026] "[:DD]", /* mathou */ 
    [1027] "*dyne*", /* mathou */ 
    [1028] "[mimi]", /* mathou */ 
    [1029] "[;.(]", /* mathou */ 
    [1030] "[8)]", /* onovy */ 
    [1031] "[:((]", /* mathou */ 
    [1032] "[X-I]", /* mathou */ 
    [1033] "[8-I]", /* mathou */ 
    [1034] "[8(]", /* onovy */ 
    [1035] "8((", /* mathou */ 
    [1036] "*fuck off*", /* mathou */ 
    [1037] "*hodneeej*", /* mathou */ 
    [1038] ":)", /* onovy */ 
    [1039] "*ano :)*", /* onovy */ 
    [1040] "*sipka dolu*", /* onovy */ 
    [1041] "X-I", /* mathou */ 
    [1042] "X-D", /* mathou */ 
    [1043] "*slunicko*", /* mathou */ 
    [1044] "*strom*", /* mathou */ 
    [1045] "*meloun*", /* mathou */ 
    [1046] "*!*", /* mathou */ 
    [1047] "*king*", /* mathou */ 
    [1048] "*love!*", /* mathou */ 
    [1049] "):D", /* mathou */ 
    [1050] "*namysleny*", /* mathou */ 
    [1051] "*duhovej :)*", /* mathou */ 
    [1052] "*cyclop*", /* mathou */ 
    [1053] "*nesmelej*", /* mathou */ 
    [1054] "8-I", /* mathou */ 
    [1055] "*kafe*", /* mathou */ 
    [1056] "8))", /* mathou */ 
    [1057] "*letajici muska*", /* mathou */ 
    [1058] "*smutny*", /* mathou */ 
    [1059] ":-*", /* mathou */ 
    [1060] "8-0", /* mathou */ 
    [1061] "8-I", /* mathou */ 
    [1062] "*hodne place*", /* mathou */ 
    [1063] "X-((", /* mathou */ 
    [1064] "*rude oko*", /* mathou */ 
    [1065] "*ehm..ne*", /* mathou */ 
    [1066] "*ROFL*", /* mathou */ 
    [1067] "8-/", /* mathou */ 
    [1068] "*Thunderbird*", /* mathou */ 
    [1069] "*snek*", /* mathou */ 
    [1070] "*zelva*", /* mathou */ 
    [1071] "*martan*", /* mathou */ 
    [1072] "*Maxthon*", /* mathou */ 
    [1073] "*zakaz*", /* mathou */ 
    [1074] "*sparta :D*", /* mathou */ 
    [1075] ";-/", /* mathou */ 
    [1076] "*presipaci hodiny*", /* mathou */ 
    [1077] "*zobe*", /* mathou */ 
    [1078] ":-P", /* onovy */ 
    [1079] "*ctyrlistek*", /* mathou */ 
    [1080] "8-O", /* mathou */ 
    [1081] ":-)", /* mathou */ 
    [1082] ":-((", /* mathou */ 
    [1083] "):[", /* mathou */ 
    [1084] ":-D", /* mathou */ 
    [1085] "d:D", /* mathou */ 
    [1086] "*zubatej*", /* mathou */ 
    [1087] "*pi*", /* onovy */ 
    [1088] "*bomba*", /* mathou */ 
    [1089] "*zakaz*", /* mathou */ 
    [1090] "*myska*", /* mathou */ 
    [1091] "*krecek*", /* mathou */ 
    [1092] "*kralik*", /* mathou */ 
    [1093] "*skakajici*", /* mathou */ 
    [1094] ":)(", /* mathou */ 
    [1095] "*s dudlikem*", /* mathou */ 
    [1096] "*jednoocko*", /* mathou */ 
    [1097] "*piska si*", /* onovy */ 
    [1098] "*had*", /* mathou */ 
    [1099] ":´(", /* mathou */ 
    [1100] "*souhlasi*", /* mathou */ 
    [1101] "*nee-e*", /* wollf */ 
    [1102] "*hambac*", /* wollf */ 
    [1103] "*kriz*", /* wollf */ 
    [1104] "*jamaica*", /* wollf */ 
    [1105] "*apple*", /* wollf */ 
    [1106] "*jahudka*", /* wollf */ 
    [1107] "*kohout*", /* wollf */ 
    [1108] "*foto*", /* wollf */ 
    [1109] "*finsko*", /* onovy */ 
    [1110] "*bublina*", /* wollf */ 
    [1111] "*had*", /* wollf */ 
    [1112] "*kun*", /* wollf */ 
    [1113] "*ovce*", /* wollf */ 
    [1114] "*rozciluje se*", /* georgo10 */ 
    [1115] "*windows*", /* onovy */ 
    [1116] "*piskvorky*", /* wollf */ 
    [1117] "*polibek*", /* wollf */ 
    [1118] "*terminator*", /* wollf */ 
    [1119] "*AVG*", /* wollf */ 
    [1120] "*zizala*", /* wollf */ 
    [1121] "*krokodyl*", /* onovy */ 
    [1122] "*pistole*", /* freezart_ */ 
    [1123] "*duch*", /* wollf */ 
    [1124] "*pyramida*", /* wollf */ 
    [1125] ":(((", /* wollf */ 
    [1126] "://", /* wollf */ 
    [1127] "*juuuj*", /* wollf */ 
    [1128] "*notak*", /* wollf */ 
    [1129] "*king*", /* wollf */ 
    [1130] "*ninja*", /* wollf */ 
    [1131] "*!!!*", /* wollf */ 
    [1132] "*rybka*", /* wollf */ 
    [1133] "*bus*", /* wollf */ 
    [1134] ":)", /* onovy */ 
    [1135] "*stars*", /* wollf */ 
    [1136] "*kytka*", /* freezart_ */ 
    [1137] "*hatmatilka*", /* onovy */ 
    [1138] "*srdicka*", /* wollf */ 
    [1139] "*tesco*", /* wollf */ 
    [1140] "*chobotnice*", /* freezart_ */ 
    [1141] "*WC*", /* wollf */ 
    [1142] "*hlavou do zdi*", /* mathou */ 
    [1143] ":-D", /* onovy */ 
    [1144] "*hatmatilka*", /* mathou */ 
    [1145] "*hatmatilka*", /* mathou */ 
    [1146] "*hatmatilka*", /* mathou */ 
    [1147] "*erb*", /* mathou */ 
    [1148] "*erb*", /* mathou */ 
    [1149] "*erb*", /* mathou */ 
    [1150] "*erb*", /* mathou */ 
    [1151] "*erb*", /* mathou */ 
    [1152] "*erb*", /* mathou */ 
    [1153] "*erb*", /* mathou */ 
    [1154] "*dymka*", /* mathou */ 
    [1155] "*srdce*", /* andilek.c */ 
    [1156] "*oslava*", /* mathou */ 
    [1157] ":-j", /* mathou */ 
    [1158] "8-/", /* mathou */ 
    [1159] "8-)", /* onovy */ 
    [1160] "*srdce*", /* mathou */ 
    [1161] "*text*", /* mathou */ 
    [1162] "8))", /* mathou */ 
    [1163] "*liska*", /* mathou */ 
    [1164] "*toaletak*", /* mathou */ 
    [1165] "*kopretina*", /* mathou */ 
    [1166] "):D", /* mathou */ 
    [1167] ">:D", /* mathou */ 
    [1168] "*metro*", /* mathou */ 
    [1169] "8-/", /* mathou */ 
    [1170] ":-E", /* onovy */ 
    [1171] "*had*", /* mathou */ 
    [1172] "*mozek?*", /* onovy */ 
    [1173] "*mrk*", /* onovy */ 
    [1174] "*andel*", /* freezart_ */ 
    [1175] "*nevidim*", /* freezart_ */ 
    [1176] "*potlesk*", /* onovy */ 
    [1177] "8-E", /* freezart_ */ 
    [1178] ">8-/", /* freezart_ */ 
    [1179] "*kytka :)*", /* freezart_ */ 
    [1180] "*lebka breci*", /* onovy */ 
    [1181] "*omg*", /* freezart_ */ 
    [1182] "*srdce+sip*", /* onovy */ 
    [1183] "8-)", /* freezart_ */ 
    [1184] "*oboci :)*", /* freezart_ */ 
    [1185] "*skoda*", /* freezart_ */ 
    [1186] "*vodarna*", /* onovy */ 
    [1187] "*ohen*", /* freezart_ */ 
    [1188] "*mimino*", /* freezart_ */ 
    [1189] "*kiss*", /* onovy */ 
    [1190] "*psycho*", /* freezart_ */ 
    [1191] "8-.", /* freezart_ */ 
    [1192] "*bublinky*", /* freezart_ */ 
    [1193] "*sparta*", /* freezart_ */ 
    [1194] ":-/", /* onovy */ 
    [1195] "*zelenej kralik*", /* onovy */ 
    [1196] "*ne 8-((*", /* freezart_ */ 
    [1197] "*pes*", /* freezart_ */ 
    [1198] "*broucek*", /* georgo10 */ 
    [1199] "*ufo*", /* onovy */ 
    [1200] "*meda*", /* freezart_ */ 
    [1201] ":\'((", /* freezart_ */ 
    [1202] "*indian*", /* freezart_ */ 
    [1203] "8-|", /* onovy */ 
    [1204] "*toaletak*", /* freezart_ */ 
    [1205] "*nene*", /* freezart_ */ 
    [1206] ":D", /* freezart_ */ 
    [1207] "*cert ne*", /* freezart_ */ 
    [1208] "*au*", /* freezart_ */ 
    [1209] "*hmm*", /* freezart_ */ 
    [1210] "*fuj*", /* freezart_ */ 
    [1211] "8)", /* freezart_ */ 
    [1212] ":D", /* onovy */ 
    [1213] ":-*", /* freezart_ */ 
    [1214] "*kun*", /* onovy */ 
    [1215] "*poklona*", /* freezart_ */ 
    [1216] ":-)", /* freezart_ */ 
    [1217] "*ptak*", /* freezart_ */ 
    [1218] ";))", /* freezart_ */ 
    [1219] "*snoopy*", /* freezart_ */ 
    [1220] ">:O", /* onovy */ 
    [1221] "*cr:)*", /* freezart_ */ 
    [1222] "*mrk*", /* freezart_ */ 
    [1223] "*rozhlizi*", /* freezart_ */ 
    [1224] "*bomba*", /* freezart_ */ 
    [1225] "*vodnice*", /* freezart_ */ 
    [1226] "8-|", /* onovy */ 
    [1227] "*rusko*", /* freezart_ */ 
    [1228] "*klaun*", /* freezart_ */ 
    [1229] "8))", /* freezart_ */ 
    [1230] "8-(", /* onovy */ 
    [1231] ">:|", /* freezart_ */ 
    [1232] "*?*", /* freezart_ */ 
    [1233] "*skener*", /* freezart_ */ 
    [1234] ":P", /* freezart_ */ 
    [1235] ":)", /* freezart_ */ 
    [1236] "*cr:)*", /* onovy */ 
    [1237] ">:D", /* freezart_ */ 
    [1238] ":/", /* freezart_ */ 
    [1239] ":D", /* freezart_ */ 
    [1240] "*ano*", /* freezart_ */ 
    [1241] "*ok*", /* freezart_ */ 
    [1242] "*svatba*", /* freezart_ */ 
    [1243] "*pes*", /* freezart_ */ 
    [1244] "*spi*", /* freezart_ */ 
    [1245] "*kytka*", /* freezart_ */ 
    [1246] "*kamosi*", /* freezart_ */ 
    [1247] "8D", /* freezart_ */ 
    [1248] "*vojak*", /* freezart_ */ 
    [1249] "8)", /* freezart_ */ 
    [1250] "*ufo*", /* freezart_ */ 
    [1251] ".)", /* freezart_ */ 
    [1252] "8(", /* onovy */ 
    [1253] "*psst*", /* onovy */ 
    [1254] ":D", /* freezart_ */ 
    [1255] "*ptak*", /* freezart_ */ 
    [1256] "*zena*", /* freezart_ */ 
    [1257] "*muz*", /* freezart_ */ 
    [1258] "*blesk*", /* freezart_ */ 
    [1259] "*granat*", /* freezart_ */ 
    [1260] "*snek*", /* freezart_ */ 
    [1261] "*kofola*", /* freezart_ */ 
    [1262] "*strom*", /* freezart_ */ 
    [1263] "*svicky*", /* freezart_ */ 
    [1264] ";))", /* freezart_ */ 
    [1265] "*nene*", /* freezart_ */ 
    [1266] ":E", /* freezart_ */ 
    [1267] "8E", /* freezart_ */ 
    [1268] "*santa*", /* freezart_ */ 
    [1269] "*pes*", /* freezart_ */ 
    [1270] "*nee*", /* freezart_ */ 
    [1271] ":P", /* freezart_ */ 
    [1272] "*usaty*", /* freezart_ */ 
    [1273] "*haha*", /* onovy */ 
    [1274] "*kocka*", /* freezart_ */ 
    [1275] "8))", /* freezart_ */ 
    [1276] "X|", /* freezart_ */ 
    [1277] "*srdce*", /* freezart_ */ 
    [1278] "8-E", /* onovy */ 
    [1279] "*hvezda*", /* freezart_ */ 
    [1280] "$))", /* freezart_ */ 
    [1281] "*brouk*", /* freezart_ */ 
    [1282] "*zarovka*", /* freezart_ */ 
    [1283] "*kafe*", /* freezart_ */ 
    [1284] "*dyne*", /* freezart_ */ 
    [1285] "*usa*", /* freezart_ */ 
    [1286] "*skace zelenej*", /* freezart_ */ 
    [1287] "*hasici*", /* freezart_ */ 
    [1288] "*hasici*", /* freezart_ */ 
    [1289] "*kapela*", /* freezart_ */ 
    [1290] "*vojaci*", /* freezart_ */ 
    [1291] "*tank*", /* freezart_ */ 
    [1292] "*zarovka*", /* freezart_ */ 
    [1293] "8-D", /* onovy */ 
    [1294] "*kolotoc*", /* freezart_ */ 
    [1295] "*oci*", /* freezart_ */ 
    [1296] "*zaba*", /* freezart_ */ 
    [1297] "*zvuk*", /* freezart_ */ 
    [1298] "*preclik*", /* freezart_ */ 
    [1299] ":D", /* freezart_ */ 
    [1300] "*houpacka*", /* freezart_ */ 
    [1301] "*houpacka*", /* freezart_ */ 
    [1302] "*policie*", /* freezart_ */ 
    [1303] "*hasici*", /* freezart_ */ 
    [1304] "*hasici*", /* freezart_ */ 
    [1305] "*hasici*", /* freezart_ */ 
    [1306] "*rybar*", /* freezart_ */ 
    [1307] ":D", /* freezart_ */ 
    [1308] "*polib mi*", /* freezart_ */ 
    [1309] "*ptak*", /* freezart_ */ 
    [1310] "*muz*", /* freezart_ */ 
    [1311] "*zena*", /* freezart_ */ 
    [1312] "*popelari*", /* freezart_ */ 
    [1313] "*snehulak*", /* freezart_ */ 
    [1314] "*krabice*", /* freezart_ */ 
    [1315] "8-E", /* freezart_ */ 
    [1316] "8-|", /* freezart_ */ 
    [1317] "*pochoden*", /* freezart_ */ 
    [1318] "*coko*", /* freezart_ */ 
    [1319] ":P", /* freezart_ */ 
    [1320] "*pluh*", /* freezart_ */ 
    [1321] ":(", /* freezart_ */ 
    [1322] "*pivko*", /* freezart_ */ 
    [1323] "*pistole*", /* onovy */ 
    [1324] ";))", /* freezart_ */ 
    [1325] "*cisterna*", /* freezart_ */ 
    [1326] "*hasici*", /* freezart_ */ 
    [1327] "*hasici*", /* freezart_ */ 
    [1328] "*hasici*", /* freezart_ */ 
    [1329] "*hasici*", /* freezart_ */ 
    [1330] "8-)", /* freezart_ */ 
    [1331] "8-o", /* onovy */ 
    [1332] ":D", /* freezart_ */ 
    [1333] "*nota*", /* freezart_ */ 
    [1334] "*sklenka*", /* freezart_ */ 
    [1335] ":/", /* freezart_ */ 
    [1336] ":))", /* freezart_ */ 
    [1337] "*milenci*", /* freezart_ */ 
    [1338] "*s bicem*", /* freezart_ */ 
    [1339] "*jin-jang*", /* freezart_ */ 
    [1340] "*bomba*", /* freezart_ */ 
    [1341] "*:P pusa*", /* freezart_ */ 
    [1342] ":-o", /* freezart_ */ 
    [1343] "*septa*", /* freezart_ */ 
    [1344] "*strili*", /* freezart_ */ 
    [1345] "*kos*", /* freezart_ */ 
    [1346] ":-|", /* freezart_ */ 
    [1347] "*ano*", /* freezart_ */ 
    [1348] "8-|", /* freezart_ */ 
    [1349] ":(", /* freezart_ */ 
    [1350] "*wtf?*", /* freezart_ */ 
    [1351] "*vana*", /* freezart_ */ 
    [1352] "*wc*", /* freezart_ */ 
    [1353] "8-X", /* freezart_ */ 
    [1354] "X-D", /* freezart_ */ 
    [1355] "$$-o", /* freezart_ */ 
    [1356] "*bus*", /* freezart_ */ 
    [1357] "8-E", /* freezart_ */ 
    [1358] ">:)", /* freezart_ */ 
    [1359] "*8) ano*", /* freezart_ */ 
    [1360] "*pribor*", /* freezart_ */ 
    [1361] "8-o", /* freezart_ */ 
    [1362] "*coze?*", /* freezart_ */ 
    [1363] "*potapec*", /* freezart_ */ 
    [1364] "*rofl*", /* freezart_ */ 
    [1365] "*AMD*", /* freezart_ */ 
    [1366] "*ubuntu*", /* freezart_ */ 
    [1367] "*dabel*", /* freezart_ */ 
    [1368] "*cert*", /* freezart_ */ 
    [1369] "X-D", /* freezart_ */ 
    [1370] "*co?*", /* freezart_ */ 
    [1371] "*toci se 8-|*", /* freezart_ */ 
    [1372] "*jejda*", /* freezart_ */ 
    [1373] "8-))", /* freezart_ */ 
    [1374] "*fousaty*", /* freezart_ */ 
    [1375] "*fousaty*", /* freezart_ */ 
    [1376] ";))", /* freezart_ */ 
    [1377] "8-)", /* freezart_ */ 
    [1378] ":-o", /* freezart_ */ 
    [1379] ">:(", /* freezart_ */ 
    [1380] ":)", /* freezart_ */ 
    [1381] "*papa*", /* freezart_ */ 
    [1382] "*lama*", /* freezart_ */ 
    [1383] "*snek*", /* onovy */ 
    [1384] "*zhulenej*", /* freezart_ */ 
    [1385] "*klaun*", /* freezart_ */ 
    [1386] "*sip srdce*", /* freezart_ */ 
    [1387] "*oboci :)*", /* freezart_ */ 
    [1388] ">:(", /* onovy */ 
    [1389] "*prase*", /* freezart_ */ 
    [1390] "8-/", /* freezart_ */ 
    [1391] ">8-)", /* freezart_ */ 
    [1392] "*lebka*", /* freezart_ */ 
    [1393] "*rozhlizi se :O*", /* freezart_ */ 
    [1394] "*pohreb*", /* freezart_ */ 
    [1395] "*viking*", /* freezart_ */ 
    [1396] ":-S", /* freezart_ */ 
    [1397] "*bublina srdce*", /* freezart_ */ 
    [1398] "*tweety*", /* freezart_ */ 
    [1399] ":D", /* freezart_ */ 
    [1400] "8-)", /* freezart_ */ 
    [1401] "8-|", /* freezart_ */ 
    [1402] "*krecek*", /* freezart_ */ 
    [1403] "8-.", /* onovy */ 
    [1404] "*?*", /* onovy */ 
    [1405] "*holka*", /* freezart_ */ 
    [1406] "*kluk*", /* freezart_ */ 
    [1407] ";)", /* freezart_ */ 
    [1408] "*dement*", /* freezart_ */ 
    [1409] ":-*", /* freezart_ */ 
    [1410] ":P", /* freezart_ */ 
    [1411] "*srdce*", /* freezart_ */ 
    [1412] "*zahlizi*", /* freezart_ */ 
    [1413] "*certik*", /* freezart_ */ 
    [1414] "*ano*", /* freezart_ */ 
    [1415] "X-)", /* freezart_ */ 
    [1416] "*puk :)*", /* freezart_ */ 
    [1417] ":-)", /* freezart_ */ 
    [1418] "*zmlaceny*", /* freezart_ */ 
    [1419] "*eifelovka*", /* freezart_ */ 
    [1420] "*piskvorky*", /* onovy */ 
    [1421] "*dabel*", /* freezart_ */ 
    [1422] "*iloveyou*", /* freezart_ */ 
    [1423] "*star*", /* freezart_ */ 
    [1424] "*packa*", /* freezart_ */ 
    [1425] "*sikana*", /* freezart_ */ 
    [1426] "*kotva*", /* freezart_ */ 
    [1427] "*medved*", /* freezart_ */ 
    [1428] "*tlacitko*", /* freezart_ */ 
    [1429] "*sipka :(*", /* freezart_ */ 
    [1430] "*kladivo*", /* freezart_ */ 
    [1431] "*jesterka*", /* freezart_ */ 
    [1432] "*magnet*", /* freezart_ */ 
    [1433] ":((", /* freezart_ */ 
    [1434] "*bic*", /* freezart_ */ 
    [1435] "*pesti do nosu*", /* freezart_ */ 
    [1436] "*kouri*", /* freezart_ */ 
    [1437] "*ufon*", /* freezart_ */ 
    [1438] ":-)", /* onovy */ 
    [1439] ".-|", /* freezart_ */ 
    [1440] "*bonzak*", /* freezart_ */ 
    [1441] "*panda*", /* freezart_ */ 
    [1442] "*ji dort*", /* freezart_ */ 
    [1443] "*off topic*", /* freezart_ */ 
    [1444] "8-)", /* freezart_ */ 
    [1445] "*motyl*", /* freezart_ */ 
    [1446] "*mimino*", /* freezart_ */ 
    [1447] "*hokej*", /* freezart_ */ 
    [1448] "*srdce*", /* freezart_ */ 
    [1449] "*kulich :)*", /* freezart_ */ 
    [1450] ":-E", /* onovy */ 
    [1451] "*se sluchatky*", /* freezart_ */ 
    [1452] "*fiona*", /* onovy */ 
    [1453] "*bez zubu*", /* freezart_ */ 
    [1454] "*slinta*", /* freezart_ */ 
    [1455] "*box*", /* freezart_ */ 
    [1456] "*zubar*", /* freezart_ */ 
    [1457] "*upir*", /* andilek.c */ 
    [1458] "*ryba*", /* freezart_ */ 
    [1459] "*hrnek*", /* freezart_ */ 
    [1460] "*PLUS*", /* freezart_ */ 
    [1461] "8-|", /* freezart_ */ 
    [1462] "*trioky :|*", /* onovy */ 
    [1463] "*oko nene*", /* freezart_ */ 
    [1464] "*oko ano*", /* freezart_ */ 
    [1465] "*sni druheho*", /* freezart_ */ 
    [1466] "*ruze*", /* freezart_ */ 
    [1467] ":\'((", /* onovy */ 
    [1468] "*kafe*", /* freezart_ */ 
    [1469] "*blazne*", /* freezart_ */ 
    [1470] "8-/", /* freezart_ */ 
    [1471] "*kaktus*", /* freezart_ */ 
    [1472] "*Stan*", /* freezart_ */ 
    [1473] "*mec*", /* freezart_ */ 
    [1474] "*sekera*", /* freezart_ */ 
    [1475] "*mrk*", /* onovy */ 
    [1476] "*nastvany*", /* freezart_ */ 
    [1477] "8-|", /* freezart_ */ 
    [1478] "*ano*", /* freezart_ */ 
    [1479] "*brouk*", /* freezart_ */ 
    [1480] "*pozdrav*", /* georgo10 */ 
    [1481] "*strili*", /* freezart_ */ 
    [1482] "*had*", /* freezart_ */ 
    [1483] "*kouka dolu*", /* georgo10 */ 
    [1484] "*mesic*", /* freezart_ */ 
    [1485] ":D", /* freezart_ */ 
    [1486] "*kytka*", /* freezart_ */ 
    [1487] "*andel/dabel*", /* onovy */ 
    [1488] "*zhulenec*", /* freezart_ */ 
    [1489] "8\'-((", /* freezart_ */ 
    [1490] "*srdicka*", /* freezart_ */ 
    [1491] ":-)", /* freezart_ */ 
    [1492] "*stopy*", /* freezart_ */ 
    [1493] ":-D", /* onovy */ 
    [1494] "*cau*", /* onovy */ 
    [1495] ":-P", /* mathou */ 
    [1496] "*vlajka*", /* freezart_ */ 
    [1497] ">:(", /* freezart_ */ 
    [1498] "*dudlik*", /* freezart_ */ 
    [1499] ";))", /* freezart_ */ 
    [1500] ":-P", /* onovy */ 
    [1501] ">:/", /* freezart_ */ 
    [1502] "*kos*", /* freezart_ */ 
    [1503] "*lebka*", /* freezart_ */ 
    [1504] "*bubak*", /* freezart_ */ 
    [1505] "*kuzelka*", /* freezart_ */ 
    [1506] ":-|", /* onovy */ 
    [1507] "*toci se*", /* freezart_ */ 
    [1508] "*blikaci*", /* freezart_ */ 
    [1509] ">:/", /* onovy */ 
    [1510] "*kytka*", /* freezart_ */ 
    [1511] "*sem no*", /* freezart_ */ 
    [1512] ":-P", /* onovy */ 
    [1513] ":\'((", /* freezart_ */ 
    [1514] "*spona*", /* freezart_ */ 
    [1515] "*tovarna*", /* freezart_ */ 
    [1516] "*motyl*", /* freezart_ */ 
    [1517] "*kulicka*", /* freezart_ */ 
    [1518] "*SG Chevron*", /* freezart_ */ 
    [1519] "*Pepsi*", /* onovy */ 
    [1520] "=|", /* onovy */ 
    [1521] "*znak HC*", /* freezart_ */ 
    [1522] "*kytka*", /* freezart_ */ 
    [1523] "*cerveny oci*", /* onovy */ 
    [1524] ":|", /* freezart_ */ 
    [1525] "*certik*", /* freezart_ */ 
    [1526] "*budik*", /* freezart_ */ 
    [1527] "*grrr*", /* onovy */ 
    [1528] ":) :(", /* onovy */ 
    [1529] ">:D", /* freezart_ */ 
    [1530] "*kun*", /* freezart_ */ 
    [1531] "8-o", /* freezart_ */ 
    [1532] "*sysel*", /* freezart_ */ 
    [1533] "8))", /* freezart_ */ 
    [1534] "*rozhlizi :/*", /* freezart_ */ 
    [1535] "*Japanka*", /* onovy */ 
    [1536] "*Sparta*", /* georgo10 */ 
    [1537] "*formule*", /* freezart_ */ 
    [1538] "*Intel inside*", /* onovy */ 
    [1539] "*ryba*", /* georgo10 */ 
    [1540] ":DD", /* georgo10 */ 
    [1541] "*cervici*", /* georgo10 */ 
    [1542] "*nota*", /* georgo10 */ 
    [1543] ":-|", /* georgo10 */ 
    [1544] "*COD2*", /* georgo10 */ 
    [1545] "*COD*", /* georgo10 */ 
    [1546] "*slinta*", /* georgo10 */ 
    [1547] "*afromen*", /* georgo10 */ 
    [1548] ";-)", /* georgo10 */ 
    [1549] "*zzZZ*", /* georgo10 */ 
    [1550] "*svalovec*", /* georgo10 */ 
    [1551] "*v uvozovkach*", /* georgo10 */ 
    [1552] "*s flaskou*", /* mathou */ 
    [1553] "|-8", /* georgo10 */ 
    [1554] "*pije*", /* onovy */ 
    [1555] "*unaveny*", /* mathou */ 
    [1556] "*pivo*", /* onovy */ 
    [1557] "*motorka*", /* mathou */ 
    [1558] "*s modrinou*", /* mathou */ 
    [1559] "$$-)", /* mathou */ 
    [1560] "X-P", /* mathou */ 
    [1561] "*brk...*", /* mathou */ 
    [1562] "*kocka*", /* mathou */ 
    [1563] ">:-I", /* mathou */ 
    [1564] ":-)", /* onovy */ 
    [1565] "*zvraci*", /* onovy */ 
    [1566] "=}", /* mathou */ 
    [1567] "*(_._)*", /* mathou */ 
    [1568] "*zamilovane oci*", /* mathou */ 
    [1569] "8-I", /* mathou */ 
    [1570] ":-P", /* mathou */ 
    [1571] "8-0", /* mathou */ 
    [1572] "8-I", /* mathou */ 
    [1573] "*had*", /* mathou */ 
    [1574] ":-)", /* mathou */ 
    [1575] "*pivo*", /* georgo10 */ 
    [1576] "*darce krve*", /* mathou */ 
    [1577] "8-S", /* mathou */ 
    [1578] ">=]", /* mathou */ 
    [1579] "*lebka*", /* mathou */ 
    [1580] ">:))", /* mathou */ 
    [1581] "*s lizatkem*", /* onovy */ 
    [1582] "*dostanes!*", /* onovy */ 
    [1583] ":-D", /* mathou */ 
    [1584] ":P", /* mathou */ 
    [1585] "*kacenka*", /* mathou */ 
    [1586] ":-P", /* georgo10 */ 
    [1587] ">8-I", /* mathou */ 
    [1588] "*Kanada*", /* mathou */ 
    [1589] "*Cr*", /* mathou */ 
    [1590] "*Finsko*", /* mathou */ 
    [1591] "*Italie*", /* mathou */ 
    [1592] "*Rakousko*", /* mathou */ 
    [1593] "*Nemecko*", /* mathou */ 
    [1594] "*Rusko*", /* mathou */ 
    [1595] "*Slovensko*", /* mathou */ 
    [1596] "*Svedsko*", /* mathou */ 
    [1597] "*Svycarsko*", /* mathou */ 
    [1598] "*Kazachstan*", /* mathou */ 
    [1599] "*Amerika*", /* mathou */ 
    [1600] "8-I", /* mathou */ 
    [1601] "*pukle srdce*", /* mathou */ 
    [1602] ":-*", /* mathou */ 
    [1603] "*letajici muska*", /* mathou */ 
    [1604] "8-I", /* mathou */ 
    [1605] "*odpal!*", /* mathou */ 
    [1606] ";-)", /* mathou */ 
    [1607] "*nasloucham*", /* mathou */ 
    [1608] "*Ferda mravenec*", /* mathou */ 
    [1609] "*kutalejici se*", /* mathou */ 
    [1610] "*BSD*", /* onovy */ 
    [1611] "*duhovej :)*", /* mathou */ 
    [1612] ":-)", /* mathou */ 
    [1613] "*peace*", /* georgo10 */ 
    [1614] "=D", /* andilek.c */ 
    [1615] "*kvetina*", /* mathou */ 
    [1616] "*sasek*", /* mathou */ 
    [1617] "*jamaica :)*", /* mathou */ 
    [1618] "*bubinek*", /* mathou */ 
    [1619] "8o(", /* mathou */ 
    [1620] ":o)", /* mathou */ 
    [1621] "*slunce*", /* georgo10 */ 
    [1622] "*nota*", /* mathou */ 
    [1623] "*kvetina*", /* mathou */ 
    [1624] "*Pardubice*", /* onovy */ 
    [1625] "*love you*", /* georgo10 */ 
    [1626] ":-(", /* mathou */ 
    [1627] ":-)", /* onovy */ 
    [1628] ":-/", /* mathou */ 
    [1629] "8-)", /* mathou */ 
    [1630] ":-)", /* mathou */ 
    [1631] ":-)", /* mathou */ 
    [1632] ":-*", /* mathou */ 
    [1633] "*girl+boy*", /* mathou */ 
    [1634] "*krava*", /* mathou */ 
    [1635] "*Krtecek*", /* mathou */ 
    [1636] "*zelva*", /* mathou */ 
    [1637] "*srdce:)*", /* mathou */ 
    [1638] ";-)", /* onovy */ 
    [1639] ":-/", /* mathou */ 
    [1640] ">:D", /* mathou */ 
    [1641] "=)", /* mathou */ 
    [1642] ">:P", /* mathou */ 
    [1643] "*HC Pardubice*", /* andilek.c */ 
    [1644] "8-(", /* mathou */ 
    [1645] "*obliznuti*", /* onovy */ 
    [1646] ">:-(", /* mathou */ 
    [1647] "*pusa+kytka*", /* onovy */ 
    [1648] "*Cerne Srdce*", /* mathou */ 
    [1649] "*Corel*", /* onovy */ 
    [1650] "[:-P]", /* mathou */ 
    [1651] "[>:-P]", /* mathou */ 
    [1652] "*motyl*", /* mathou */ 
    [1653] "*hribek*", /* mathou */ 
    [1654] ":-*", /* andilek.c */ 
    [1655] "*malina*", /* andilek.c */ 
    [1656] "*nafukuje ho*", /* onovy */ 
    [1657] "[:-)", /* mathou */ 
    [1658] "*usi*", /* mathou */ 
    [1659] ":-*", /* mathou */ 
    [1660] "*!*", /* mathou */ 
    [1661] "*Vodafone*", /* mathou */ 
    [1662] "*Barca*", /* andilek.c */ 
    [1663] "*Barca*", /* andilek.c */ 
    [1664] "*pirat*", /* mathou */ 
    [1665] "*-->*", /* mathou */ 
    [1666] "*orech*", /* andilek.c */ 
    [1667] "*:D zizala*", /* paces */ 
    [1668] "*srdce*", /* mathou */ 
    [1669] "*spinka*", /* mathou */ 
    [1670] ":-D", /* mathou */ 
    [1671] ">:-d", /* mathou */ 
    [1672] "8-D", /* mathou */ 
    [1673] ":-(", /* mathou */ 
    [1674] ":-I", /* mathou */ 
    [1675] "*mochomurka*", /* mathou */ 
    [1676] "*ziva :-I*", /* mathou */ 
    [1677] "[;-)", /* mathou */ 
    [1678] "*hadaji se*", /* onovy */ 
    [1679] "8-0", /* mathou */ 
    [1680] "*dloube se do oka*", /* mathou */ 
    [1681] "*naprsi a uschne*", /* mathou */ 
    [1682] "*zamilovany*", /* mathou */ 
    [1683] "*Brazilie*", /* mathou */ 
    [1684] "*kocicka cervena se*", /* mathou */ 
    [1685] "*kocka ;)*", /* onovy */ 
    [1686] "*drak*", /* mathou */ 
    [1687] "*kocka :(*", /* onovy */ 
    [1688] "*kocka spinka*", /* mathou */ 
    [1689] "*kocka >:(*", /* mathou */ 
    [1690] "*VltavaTour*", /* onovy */ 
    [1691] "*housle*", /* mathou */ 
    [1692] "*trumpeta*", /* mathou */ 
    [1693] "*knihy*", /* mathou */ 
    [1694] "*cisti si zuby*", /* mathou */ 
    [1695] "*stydi se*", /* mathou */ 
    [1696] "*coze?!?*", /* mathou */ 
    [1697] "*cerne srdce*", /* mathou */ 
    [1698] "*kralik*", /* mathou */ 
    [1699] "*postavicky SouthParku*", /* mathou */ 
    [1700] "*modra je dobra :)*", /* mathou */ 
    [1701] "*opicka*", /* mathou */ 
    [1702] "*beruska*", /* mathou */ 
    [1703] "*jesterka*", /* mathou */ 
    [1704] "*noty*", /* mathou */ 
    [1705] ">:-))", /* mathou */ 
    [1706] "*mracek :)*", /* mathou */ 
    [1707] "*KoRn*", /* onovy */ 
    [1708] "*;) palec nahoru *", /* mathou */ 
    [1709] "*palec nahoru*", /* onovy */ 
    [1710] "*Rammstein*", /* mathou */ 
    [1711] "*alien*", /* mathou */ 
    [1712] "*DC*", /* mathou */ 
    [1713] "*prase*", /* mathou */ 
    [1714] "*-->*", /* mathou */ 
    [1715] "*zuby*", /* mathou */ 
    [1716] ":-/", /* mathou */ 
    [1717] "*hvezdicka :-P*", /* mathou */ 
    [1718] "*Ukrajina*", /* onovy */ 
    [1719] "*zamotanej xicht*", /* mathou */ 
    [1720] "*ruze*", /* mathou */ 
    [1721] ":-O", /* mathou */ 
    [1722] "8-)", /* mathou */ 
    [1723] "=(", /* mathou */ 
    [1724] ">:-((", /* mathou */ 
    [1725] "=P", /* mathou */ 
    [1726] "=)", /* mathou */ 
    [1727] ";-)", /* mathou */ 
    [1728] "*zakaz koureni*", /* mathou */ 
    [1729] ":-P", /* mathou */ 
    [1730] "*opice*", /* mathou */ 
    [1731] "*pes*", /* mathou */ 
    [1732] "*tocici certik*", /* mathou */ 
    [1733] "8-O", /* mathou */ 
    [1734] "*pes*", /* mathou */ 
    [1735] "*pusa*", /* mathou */ 
    [1736] "*mrkev*", /* mathou */ 
    [1737] "*kocka*", /* onovy */ 
    [1738] "*barevnej :)*", /* mathou */ 
    [1739] "*FreeBSD*", /* andilek.c */ 
    [1740] ":-S", /* mathou */ 
    [1741] "*nevim*", /* mathou */ 
    [1742] "*souhlasi*", /* mathou */ 
    [1743] ";)", /* mathou */ 
    [1744] "*;) palec nahoru *", /* mathou */ 
    [1745] "*kocka*", /* mathou */ 
    [1746] "*smutny*", /* mathou */ 
    [1747] ":-o", /* onovy */ 
    [1748] "*rychle souhlasi*", /* onovy */ 
    [1749] ";-)", /* onovy */ 
    [1750] "*cervena se*", /* mathou */ 
    [1751] "=´(", /* mathou */ 
    [1752] "=)", /* mathou */ 
    [1753] "*bitka*", /* mathou */ 
    [1754] "*spinka*", /* mathou */ 
    [1755] "*Linkin Park*", /* andilek.c */ 
    [1756] "[:)", /* mathou */ 
    [1757] "*MeatFly*", /* mathou */ 
    [1758] ":-]", /* mathou */ 
    [1759] "*Ach jo*", /* mathou */ 
    [1760] "*sraci lama*", /* mathou */ 
    [1761] "8-|", /* onovy */ 
    [1762] "*unaveny*", /* mathou */ 
    [1763] "*smrtka chlapec*", /* mathou */ 
    [1764] "*smrtka divka*", /* mathou */ 
    [1765] "8-O", /* mathou */ 
    [1766] "*censored*", /* onovy */ 
    [1767] "*Everlast*", /* andilek.c */ 
    [1768] "d:)", /* mathou */ 
    [1769] "8-Z", /* mathou */ 
    [1770] "*ne-e*", /* onovy */ 
    [1771] "*sklo*", /* mathou */ 
    [1772] "8-(", /* mathou */ 
    [1773] "*Lentilky*", /* andilek.c */ 
    [1774] "*packman*", /* mathou */ 
    [1775] "*snowboardak*", /* onovy */ 
    [1776] "*klanim se*", /* mathou */ 
    [1777] "*dolu*", /* onovy */ 
    [1778] "*nahoru*", /* onovy */ 
    [1779] "8-(", /* mathou */ 
    [1780] "*ponozky*", /* mathou */ 
    [1781] "*mava*", /* mathou */ 
    [1782] "*andilek*", /* mathou */ 
    [1783] "*uz mlcim*", /* mathou */ 
    [1784] "*trool*", /* mathou */ 
    [1785] "*kristalova koule*", /* mathou */ 
    [1786] "*hlava*", /* mathou */ 
    [1787] "*ptak leti*", /* mathou */ 
    [1788] "*motorkar*", /* mathou */ 
    [1789] "*delo*", /* mathou */ 
    [1790] "*papousek*", /* mathou */ 
    [1791] "*@*", /* onovy */ 
    [1792] "*notebook*", /* mathou */ 
    [1793] "*pohoda*", /* mathou */ 
    [1794] "*zzZzz*", /* mathou */ 
    [1795] "*andilek*", /* mathou */ 
    [1796] "*ICQ Invisible*", /* mathou */ 
    [1797] "*ptak kiwi*", /* mathou */ 
    [1798] "*pirat*", /* mathou */ 
    [1799] "*zimnice*", /* mathou */ 
    [1800] ":-(", /* mathou */ 
    [1801] ":D", /* mathou */ 
    [1802] ";-)", /* mathou */ 
    [1803] "*to snad ne*", /* andilek.c */ 
    [1804] ":o", /* mathou */ 
    [1805] ":P", /* mathou */ 
    [1806] ";)", /* mathou */ 
    [1807] "*mimozemstan*", /* mathou */ 
    [1808] ":PP", /* mathou */ 
    [1809] ",´-)", /* mathou */ 
    [1810] "*kral*", /* mathou */ 
    [1811] "8-))", /* mathou */ 
    [1812] ":-/", /* mathou */ 
    [1813] "*cervena se*", /* mathou */ 
    [1814] ";-)", /* mathou */ 
    [1815] "*Horsefeathers*", /* mathou */ 
    [1816] "*stridani nalad*", /* mathou */ 
    [1817] "*pecinka*", /* andilek.c */ 
    [1818] "*ryba*", /* mathou */ 
    [1819] "*hahaha*", /* mathou */ 
    [1820] "*opice*", /* mathou */ 
    [1821] "8-O", /* mathou */ 
    [1822] ">:D", /* mathou */ 
    [1823] "*stydi se*", /* mathou */ 
    [1824] "*BAKA*", /* andilek.c */ 
    [1825] "=)", /* mathou */ 
    [1826] "*urazlivec*", /* paces */ 
    [1827] "*mnam mnam*", /* mathou */ 
    [1828] ":-X", /* mathou */ 
    [1829] "*ufnukanej*", /* mathou */ 
    [1830] ":-P", /* mathou */ 
    [1831] "*hihihi*", /* mathou */ 
    [1832] "*zamerovac*", /* mathou */ 
    [1833] "*Volkswagen*", /* onovy */ 
    [1834] "´,-)", /* mathou */ 
    [1835] ";-)", /* mathou */ 
    [1836] "*Crazy Factory*", /* andilek.c */ 
    [1837] "[:-E", /* mathou */ 
    [1838] "*budik*", /* mathou */ 
    [1839] "*kouka*", /* mathou */ 
    [1840] ":-I", /* mathou */ 
    [1841] "*mele hubou*", /* mathou */ 
    [1842] "*dostava na (_._)*", /* mathou */ 
    [1843] ":-D", /* mathou */ 
    [1844] ":-P", /* mathou */ 
    [1845] "*hahaha*", /* mathou */ 
    [1846] ":-D", /* mathou */ 
    [1847] "*audio cd*", /* mathou */ 
    [1848] "*kocka*", /* mathou */ 
    [1849] "*posloucha hudbu*", /* mathou */ 
    [1850] "*mava*", /* mathou */ 
    [1851] ":-D", /* mathou */ 
    [1852] "*upir*", /* mathou */ 
    [1853] "=(", /* mathou */ 
    [1854] "*cd*", /* mathou */ 
    [1855] "*hranolky*", /* mathou */ 
    [1856] "*ninja* ", /* mathou */ 
    [1857] "*palec nahoru*", /* mathou */ 
    [1858] "*KoRn*", /* mathou */ 
    [1859] "*ufnukanej*", /* mathou */ 
    [1860] "*bublifuk*", /* mathou */ 
    [1861] "*CZ :)*", /* mathou */ 
    [1862] "*srdce*", /* mathou */ 
    [1863] ">:D", /* mathou */ 
    [1864] "*nabijeni*", /* mathou */ 
    [1865] "*zamilovany :)*", /* mathou */ 
    [1866] "=(", /* mathou */ 
    [1867] "*?*", /* mathou */ 
    [1868] "*Italie :)*", /* mathou */ 
    [1869] "8-(", /* mathou */ 
    [1870] "*zaslapnu te*", /* mathou */ 
    [1871] "*Nestea*", /* andilek.c */ 
    [1872] "*vyjevenej*", /* paces */ 
    [1873] "*hop hop*", /* andilek.c */ 
    [1874] "*Tiesto*", /* onovy */ 
    [1875] "*Ferrari*", /* andilek.c */ 
    [1876] "*kralicek*", /* andilek.c */ 
    [1877] "*megafon*", /* andilek.c */ 
    [1878] "*:) ?*", /* andilek.c */ 
    [1879] "*cvok*", /* andilek.c */ 
    [1880] "*Offspring*", /* onovy */ 
    [1881] "*in love*", /* andilek.c */ 
    [1882] "*Graffy*", /* andilek.c */ 
    [1883] "*domecek*", /* andilek.c */ 
    [1884] "*new*", /* onovy */ 
    [1885] "*siamska dvojcata*", /* andilek.c */ 
    [1886] "*balonky*", /* andilek.c */ 
    [1887] "*plamen*", /* andilek.c */ 
    [1888] "*EAMooN-Volcom*", /* andilek.c */ 
    [1889] "*Finlandia*", /* onovy */ 
    [1890] "*blikajici star*", /* andilek.c */ 
    [1891] "*vlk*", /* andilek.c */ 
    [1892] "*fuck*", /* andilek.c */ 
    [1893] "*cisti zuby*", /* andilek.c */ 
    [1894] "*hrob*", /* andilek.c */ 
    [1895] "*Fanta*", /* onovy */ 
    [1896] "*HC Znojemsti Orli*", /* andilek.c */ 
    [1897] "*vytacim cislo*", /* andilek.c */ 
    [1898] "*slepicka*", /* andilek.c */ 
    [1899] "*bleee*", /* andilek.c */ 
    [1900] "*hopsig*", /* andilek.c */ 
    [1901] "*Vriskot*", /* andilek.c */ 
    [1902] "*lampa*", /* andilek.c */ 
    [1903] "*Quiksilver*", /* andilek.c */ 
    [1904] "*PSX logo*", /* andilek.c */ 
    [1905] "*TopTopic*", /* onovy */ 
    [1906] "*Ati*", /* onovy */ 
    [1907] "*dymka*", /* andilek.c */ 
    [1908] "*hroch*", /* andilek.c */ 
    [1909] "*cerna ruze*", /* andilek.c */ 
    [1910] "*bila ruze*", /* andilek.c */ 
    [1911] "*drink*", /* andilek.c */ 
    [1912] "*Slipknot*", /* onovy */ 
    [1913] "*zere*", /* paces */ 
    [1914] "*ruze*", /* andilek.c */ 
    [1915] "*nota*", /* onovy */ 
    [1916] "*kytara*", /* andilek.c */ 
    [1917] "*sprej*", /* andilek.c */ 
    [1918] "*posluchac*", /* paces */ 
    [1919] "*lizatko*", /* andilek.c */ 
    [1920] "*doprava*", /* andilek.c */ 
    [1921] ":-|", /* andilek.c */ 
    [1922] "*duhove srdce*", /* andilek.c */ 
    [1923] ":-)", /* onovy */ 
    [1924] "*MTR*", /* andilek.c */ 
    [1925] "*hop hop*", /* andilek.c */ 
    [1926] "*kyticka*", /* andilek.c */ 
    [1927] ";-)", /* onovy */ 
    [1928] "*kyticka*", /* andilek.c */ 
    [1929] "*bota*", /* andilek.c */ 
    [1930] "8-/", /* onovy */ 
    [1931] "*brekeke*", /* andilek.c */ 
    [1932] "*pijan*", /* andilek.c */ 
    [1933] ";-)", /* onovy */ 
    [1934] "*rotujici :PP*", /* paces */ 
    [1935] "*tanyny*", /* andilek.c */ 
    [1936] "*oko*", /* paces */ 
    [1937] "=(", /* andilek.c */ 
    [1938] "x-P", /* onovy */ 
    [1939] "*sroub*", /* andilek.c */ 
    [1940] "*Frankenstein*", /* alien */ 
    [1941] "*vydeseny*", /* alien */ 
    [1942] "*zolik*", /* andilek.c */ 
    [1943] "*tulipany*", /* andilek.c */ 
    [1944] "*urazeny*", /* paces */ 
    [1945] "*krokodyl*", /* paces */ 
    [1946] "*feferonka*", /* andilek.c */ 
    [1947] "*dB meter*", /* onovy */ 
    [1948] "*plechovka*", /* andilek.c */ 
    [1949] "*kuk*", /* paces */ 
    [1950] "*zere*", /* alien */ 
    [1951] "*bezici lev*", /* andilek.c */ 
    [1952] "*Windows*", /* onovy */ 
    [1953] ":-)*", /* onovy */ 
    [1954] "*mizici*", /* paces */ 
    [1955] ":-)", /* onovy */ 
    [1956] "*duhovej*", /* andilek.c */ 
    [1957] "*zzZZ*", /* onovy */ 
    [1958] "*vysmaty*", /* paces */ 
    [1959] "*zavolam lasko*", /* andilek.c */ 
    [1960] "*erb*", /* alien */ 
    [1961] "*ruze*", /* andilek.c */ 
    [1962] "*kytka*", /* paces */ 
    [1963] "*USA*", /* andilek.c */ 
    [1964] "*50cent*", /* onovy */ 
    [1965] "*Slovensko ;-)*", /* andilek.c */ 
    [1966] ":-s", /* onovy */ 
    [1967] "*robot smile*", /* paces */ 
    [1968] "*?*", /* andilek.c */ 
    [1969] "*zere*", /* paces */ 
    [1970] "8D", /* paces */ 
    [1971] "*muhehe*", /* alien */ 
    [1972] ":-D", /* onovy */ 
    [1973] "*bezici kure*", /* andilek.c */ 
    [1974] "8-|", /* onovy */ 
    [1975] "*clovek*", /* paces */ 
    [1976] "*:\'-( nee*", /* onovy */ 
    [1977] "*dablik*", /* andilek.c */ 
    [1978] "*mysicka*", /* andilek.c */ 
    [1979] "*klani se*", /* onovy */ 
    [1980] "*pink*", /* andilek.c */ 
    [1981] "*cerna ruze*", /* onovy */ 
    [1982] "*cerne srdce*", /* onovy */ 
    [1983] "*had*", /* andilek.c */ 
    [1984] ":\'-o", /* onovy */ 
    [1985] ":-)", /* onovy */ 
    [1986] "*letiim*", /* paces */ 
    [1987] "*mrkev*", /* andilek.c */ 
    [1988] "*semafor*", /* alien */ 
    [1989] "*srdce terc*", /* andilek.c */ 
    [1990] "*MT*", /* andilek.c */ 
    [1991] "*obojek*", /* onovy */ 
    [1992] "*nanuk*", /* andilek.c */ 
    [1993] "*bublina*", /* andilek.c */ 
    [1994] "*OK*", /* andilek.c */ 
    [1995] "*jezibaba*", /* paces */ 
    [1996] "*kvetina s ocima*", /* paces */ 
    [1997] "*zralok*", /* andilek.c */ 
    [1998] "*kapr*", /* andilek.c */ 
    [1999] "*sklenicky*", /* andilek.c */ 
    [2000] "*letici dopis*", /* andilek.c */ 
    [2001] "*srdce*", /* andilek.c */ 
    [2002] ":-)(", /* onovy */ 
    [2003] "*diamant*", /* andilek.c */ 
    [2004] "*kopacak*", /* onovy */ 
    [2005] "*usmev*", /* paces */ 
    [2006] "*velorex*", /* andilek.c */ 
    [2007] "*zamilovany*", /* andilek.c */ 
    [2008] "*obejmuti*", /* andilek.c */ 
    [2009] "*posmiva se*", /* paces */ 
    [2010] "*Kaufland*", /* andilek.c */ 
    [2011] "*pirat*", /* andilek.c */ 
    [2012] "*Albert*", /* onovy */ 
    [2013] "*Nutella*", /* onovy */ 
    [2014] "*mrak*", /* andilek.c */ 
    [2015] "*prsi*", /* andilek.c */ 
    [2016] "*semafor*", /* onovy */ 
    [2017] "*FK Teplice*", /* andilek.c */ 
    [2018] "*oranzovy grr*", /* onovy */ 
    [2019] ":-(", /* onovy */ 
    [2020] "*upir*", /* paces */ 
    [2021] "*ohnivac*", /* paces */ 
    [2022] "*zubac*", /* paces */ 
    [2023] "*mrtvy*", /* paces */ 
    [2024] "*mudrc*", /* andilek.c */ 
    [2025] "*motylek*", /* andilek.c */ 
    [2026] "*netopyr :(*", /* andilek.c */ 
    [2027] "*upir :)*", /* onovy */ 
    [2028] "*hamburger*", /* onovy */ 
    [2029] "*ziva*", /* alien */ 
    [2030] "*bananova slupka*", /* andilek.c */ 
    [2031] ":-DD", /* onovy */ 
    [2032] "*semafor*", /* alien */ 
    [2033] "*stuj*", /* andilek.c */ 
    [2034] "*mrtvolka*", /* andilek.c */ 
    [2035] "*pomeranc*", /* andilek.c */ 
    [2036] "*certik*", /* andilek.c */ 
    [2037] "*pivecko*", /* paces */ 
    [2038] "*zuri*", /* alien */ 
    [2039] "*pruhovanej*", /* paces */ 
    [2040] ":-(", /* onovy */ 
    [2041] "*:-) :-( fuck*", /* onovy */ 
    [2042] "*brylaty*", /* paces */ 
    [2043] "*laser :[*", /* onovy */ 
    [2044] "*polojasno*", /* andilek.c */ 
    [2045] "=)", /* alien */ 
    [2046] "*dymka*", /* onovy */ 
    [2047] "*dymka*", /* onovy */ 
    [2048] "*parkoviste*", /* andilek.c */ 
    [2049] "*nadava*", /* mathou */ 
    [2050] ";-.", /* onovy */ 
    [2051] ":-*", /* andilek.c */ 
    [2052] "*tleska*", /* alien */ 
    [2053] "*piska*", /* paces */ 
    [2054] "*jsi mimo*", /* paces */ 
    [2055] ":-|", /* onovy */ 
    [2056] "*STFU!*", /* onovy */ 
    [2057] "*pavoucek*", /* andilek.c */ 
    [2058] "*dum*", /* andilek.c */ 
    [2059] "*Fred Perry*", /* andilek.c */ 
    [2060] "*zarivy usmev*", /* andilek.c */ 
    [2061] "*CR :)*", /* onovy */ 
    [2062] ":-S", /* onovy */ 
    [2063] "*KFOR*", /* andilek.c */ 
    [2064] "*uplne mimo*", /* paces */ 
    [2065] "*slunecnice*", /* alien */ 
    [2066] "*motyl*", /* onovy */ 
    [2067] "*Indulona*", /* andilek.c */ 
    [2068] "*HC Vitkovice*", /* andilek.c */ 
    [2069] "*cervena karta*", /* andilek.c */ 
    [2070] "*FC Barcelona*", /* andilek.c */ 
    [2071] "*FK Teplice*", /* andilek.c */ 
    [2072] "*zluta karta*", /* andilek.c */ 
    [2073] "*New Jersey*", /* andilek.c */ 
    [2074] "*lasicka*", /* andilek.c */ 
    [2075] "*palickuje*", /* andilek.c */ 
    [2076] "*Red Dwarf*", /* andilek.c */ 
    [2077] "8>", /* paces */ 
    [2078] "*zradny usmev*", /* paces */ 
    [2079] ":-)", /* onovy */ 
    [2080] "*ne ne ne*", /* andilek.c */ 
    [2081] ":-/", /* onovy */ 
    [2082] "*Londsdale*", /* onovy */ 
    [2083] "*pohodar*", /* paces */ 
    [2084] "*puk*", /* andilek.c */ 
    [2085] "*Sigma Olomouc*", /* andilek.c */ 
    [2086] "*FC Banik Ostrava*", /* andilek.c */ 
    [2087] "*haha*", /* alien */ 
    [2088] "*cervena se*", /* alien */ 
    [2089] "*smile/oko*", /* paces */ 
    [2090] "*(arg)*", /* andilek.c */ 
    [2091] "*(hey)*", /* andilek.c */ 
    [2092] "*(thanks)*", /* andilek.c */ 
    [2093] "*(yo)*", /* andilek.c */ 
    [2094] "*(no)*", /* andilek.c */ 
    [2095] "*(oh)*", /* andilek.c */ 
    [2096] "*(OK)*", /* andilek.c */ 
    [2097] "*radsi nevidim*", /* andilek.c */ 
    [2098] "*nafoukany*", /* onovy */ 
    [2099] ":-P", /* onovy */ 
    [2100] "*komar*", /* andilek.c */ 
    [2101] "*motyl*", /* alien */ 
    [2102] "*vysava*", /* onovy */ 
    [2103] ";-)", /* onovy */ 
    [2104] "*komentator*", /* andilek.c */ 
    [2105] "*kytarista*", /* andilek.c */ 
    [2106] "*WC*", /* andilek.c */ 
    [2107] "*oko :)*", /* onovy */ 
    [2108] "*dromedar*", /* andilek.c */ 
    [2109] ":-)", /* onovy */ 
    [2110] "*jojo*", /* onovy */ 
    [2111] "*nene*", /* onovy */ 
    [2112] ":-X", /* mathou */ 
    [2113] "*rozhlizi se*", /* onovy */ 
    [2114] "*disko koule*", /* andilek.c */ 
    [2115] "=)", /* alien */ 
    [2116] "*spokojeny*", /* paces */ 
    [2117] "*pise*", /* andilek.c */ 
    [2118] "*Neo*", /* andilek.c */ 
    [2119] "*prasatko*", /* onovy */ 
    [2120] "*hafik*", /* andilek.c */ 
    [2121] "*Pilsner Urquell*", /* andilek.c */ 
    [2122] "*:-( ne*", /* onovy */ 
    [2123] ":PP", /* onovy */ 
    [2124] ";-)", /* onovy */ 
    [2125] "*veverka*", /* andilek.c */ 
    [2126] "*zviratko*", /* andilek.c */ 
    [2127] "*lodicka*", /* andilek.c */ 
    [2128] "*CT1*", /* andilek.c */ 
    [2129] "*CT2*", /* andilek.c */ 
    [2130] "*Loveparade*", /* andilek.c */ 
    [2131] "*mracek*", /* andilek.c */ 
    [2132] "*plamen*", /* andilek.c */ 
    [2133] "*FC Barca*", /* andilek.c */ 
    [2134] ":-X", /* andilek.c */ 
    [2135] "*skakaci kocka*", /* onovy */ 
    [2136] ";))", /* paces */ 
    [2137] "*zanet spojivek*", /* paces */ 
    [2138] ":-s", /* onovy */ 
    [2139] ";-|", /* onovy */ 
    [2140] ":-P", /* onovy */ 
    [2141] "*rozpacity*", /* alien */ 
    [2142] "*Metallica*", /* andilek.c */ 
    [2143] "*dablik*", /* andilek.c */ 
    [2144] "*Linkin Park*", /* andilek.c */ 
    [2145] "*spi*", /* alien */ 
    [2146] ":-)", /* onovy */ 
    [2147] "*bily :-)*", /* andilek.c */ 
    [2148] "*byk*", /* andilek.c */ 
    [2149] "*uuups*", /* mathou */ 
    [2150] "*lod*", /* paces */ 
    [2151] "*dest*", /* alien */ 
    [2152] "*polibek*", /* alien */ 
    [2153] "*srdicka*", /* paces */ 
    [2154] "*srdce*", /* alien */ 
    [2155] "*medanek*", /* andilek.c */ 
    [2156] "*Intel*", /* andilek.c */ 
    [2157] "*jezdec*", /* andilek.c */ 
    [2158] "*prase*", /* onovy */ 
    [2159] "*vitej*", /* andilek.c */ 
    [2160] "*Meat is murder*", /* andilek.c */ 
    [2161] "*Simple Plan*", /* andilek.c */ 
    [2162] ":-P", /* onovy */ 
    [2163] "*kytarista*", /* andilek.c */ 
    [2164] "*Tokio Hotel*", /* andilek.c */ 
    [2165] "*komar*", /* onovy */ 
    [2166] "*postel ZzZz*", /* andilek.c */ 
    [2167] "*ne ne*", /* andilek.c */ 
    [2168] "*vyplazuje jazyk*", /* paces */ 
    [2169] "*:-( ne*", /* onovy */ 
    [2170] "*motyl*", /* alien */ 
    [2171] "*rozhlizi se*", /* paces */ 
    [2172] ":-DD", /* onovy */ 
    [2173] "*AMD*", /* onovy */ 
    [2174] "*tresnicky*", /* andilek.c */ 
    [2175] "*bonbonek*", /* andilek.c */ 
    [2176] ":-o", /* onovy */ 
    [2177] "*zklamany*", /* mathou */ 
    [2178] "*smutny*", /* paces */ 
    [2179] "8|", /* paces */ 
    [2180] ";-|", /* onovy */ 
    [2181] ":-P", /* mathou */ 
    [2182] "*nene*", /* onovy */ 
    [2183] "*breci*", /* paces */ 
    [2184] "*maluje se*", /* andilek.c */ 
    [2185] ":-**", /* onovy */ 
    [2186] "*tampa bay*", /* georgo10 */ 
    [2187] ";-)", /* onovy */ 
    [2188] "*snek*", /* andilek.c */ 
    [2189] "*zpevak*", /* andilek.c */ 
    [2190] "*Armin van Buuren*", /* andilek.c */ 
    [2191] "*ID&T*", /* andilek.c */ 
    [2192] "*prsi*", /* onovy */ 
    [2193] "*bourka*", /* andilek.c */ 
    [2194] "*Punk*", /* onovy */ 
    [2195] "*DC*", /* andilek.c */ 
    [2196] "*Etnies*", /* andilek.c */ 
    [2197] "*usinajici*", /* andilek.c */ 
    [2198] "*ovce*", /* andilek.c */ 
    [2199] "*Neplac*", /* andilek.c */ 
    [2200] "*fackujici*", /* paces */ 
    [2201] "*dabel*", /* andilek.c */ 
    [2202] "*ptacek*", /* paces */ 
    [2203] "*pohar*", /* andilek.c */ 
    [2204] "*zvonecek*", /* andilek.c */ 
    [2205] "=´(", /* mathou */ 
    [2206] "*Slavia*", /* andilek.c */ 
    [2207] "*!*", /* alien */ 
    [2208] "%-|", /* onovy */ 
    [2209] "=]", /* paces */ 
    [2210] "*hlemyzd*", /* andilek.c */ 
    [2211] "*zdar*", /* andilek.c */ 
    [2212] "*grrr*", /* onovy */ 
    [2213] "*zemekoule*", /* onovy */ 
    [2214] "*fu*", /* alien */ 
    [2215] "*svarec*", /* andilek.c */ 
    [2216] "*:) rotuje*", /* onovy */ 
    [2217] "*horecka*", /* andilek.c */ 
    [2218] "*s balonem*", /* paces */ 
    [2219] "*sprejer*", /* andilek.c */ 
    [2220] "*prsi*", /* onovy */ 
    [2221] "*mrak*", /* alien */ 
    [2222] "*bourka*", /* andilek.c */ 
    [2223] ":-.", /* onovy */ 
    [2224] ":-)", /* onovy */ 
    [2225] "*oslnivy :D*", /* andilek.c */ 
    [2226] "*hooligan*", /* andilek.c */ 
    [2227] "8-)", /* andilek.c */ 
    [2228] "*leje*", /* onovy */ 
    [2229] "*polojasno*", /* andilek.c */ 
    [2230] "*mys*", /* andilek.c */ 
    [2231] "*PitBull*", /* onovy */ 
    [2232] "*teplomer*", /* andilek.c */ 
    [2233] "*krizek*", /* paces */ 
    [2234] "*parohac*", /* paces */ 
    [2235] "*mrka*", /* onovy */ 
    [2236] "*soad*", /* andilek.c */ 
    [2237] "*mIRC*", /* onovy */ 
    [2238] "*prekvapeny*", /* mathou */ 
    [2239] "*hepci*", /* andilek.c */ 
    [2240] "*telefonuje*", /* alien */ 
    [2241] ";-)", /* onovy */ 
    [2242] ":-x", /* onovy */ 
    [2243] ":-)P", /* onovy */ 
    [2244] "*:-) pacman*", /* paces */ 
    [2245] "*smutne kouka*", /* mathou */ 
    [2246] "*CD*", /* andilek.c */ 
    [2247] "*LOL*", /* andilek.c */ 
    [2248] "*jojo*", /* onovy */ 
    [2249] ":-)", /* onovy */ 
    [2250] "*armada*", /* andilek.c */ 
    [2251] "*skakajici ruzovej curak*", /* onovy */ 
    [2252] "*cervena se*", /* onovy */ 
    [2253] ":-X", /* mathou */ 
    [2254] "*hluchy*", /* mathou */ 
    [2255] "*zakryva oci*", /* paces */ 
    [2256] ":)", /* andilek.c */ 
    [2257] "*nenene*", /* paces */ 
    [2258] ":-X", /* alien */ 
    [2259] "*nevim :(*", /* andilek.c */ 
    [2260] "*Pralinky*", /* andilek.c */ 
    [2261] "*vitr*", /* andilek.c */ 
    [2262] "*prekvapenej*", /* paces */ 
    [2263] "*Diesel*", /* andilek.c */ 
    [2264] "*strilejici luk*", /* andilek.c */ 
    [2265] "*ufoun*", /* alien */ 
    [2266] "*R2D2*", /* andilek.c */ 
    [2267] "*Star Wars - Maul*", /* mathou */ 
    [2268] "*banan*", /* andilek.c */ 
    [2269] "*:-( nene*", /* onovy */ 
    [2270] "*mrk*", /* paces */ 
    [2271] "*lituje*", /* paces */ 
    [2272] ";-*", /* onovy */ 
    [2273] "*Vandit*", /* andilek.c */ 
    [2274] "*tucnak*", /* andilek.c */ 
    [2275] "*Debian*", /* onovy */ 
    [2276] "*tam*", /* onovy */ 
    [2277] "*Philips*", /* andilek.c */ 
    [2278] "*a jo!*", /* andilek.c */ 
    [2279] "*kachnicka*", /* andilek.c */ 
    [2280] "*prase*", /* alien */ 
    [2281] "*zblbnuty*", /* paces */ 
    [2282] "*slavi*", /* onovy */ 
    [2283] ":-P", /* alien */ 
    [2284] "*Ghana*", /* andilek.c */ 
    [2285] "*kompas*", /* alien */ 
    [2286] "*jeje*", /* georgo10 */ 
    [2287] "*modra kytka*", /* paces */ 
    [2288] "*sekera*", /* andilek.c */ 
    [2289] "*stit*", /* alien */ 
    [2290] "*helma*", /* alien */ 
    [2291] "*mec*", /* alien */ 
    [2292] "*halapartna*", /* andilek.c */ 
    [2293] ":-)", /* onovy */ 
    [2294] "*duch*", /* andilek.c */ 
    [2295] "*martan*", /* paces */ 
    [2296] "*skinhead*", /* andilek.c */ 
    [2297] ":-P", /* alien */ 
    [2298] "*list*", /* andilek.c */ 
    [2299] "*tepla fronta*", /* andilek.c */ 
    [2300] "*studena fronta*", /* andilek.c */ 
    [2301] "*okluze*", /* andilek.c */ 
    [2302] ">D", /* paces */ 
    [2303] "*trapaku!*", /* paces */ 
    [2304] "*Warcraft*", /* andilek.c */ 
    [2305] "*strom*", /* andilek.c */ 
    [2306] "*most*", /* andilek.c */ 
    [2307] "*upir*", /* paces */ 
    [2308] "*upir*", /* andilek.c */ 
    [2309] "*krava*", /* andilek.c */ 
    [2310] "|,,|", /* onovy */ 
    [2311] "*ale joo*", /* andilek.c */ 
    [2312] "*fotim*", /* andilek.c */ 
    [2313] "*vystraseny*", /* paces */ 
    [2314] "*Xhodonin.net*", /* paces */ 
    [2315] ":-**", /* andilek.c */ 
    [2316] "*to ne*", /* andilek.c */ 
    [2317] "*spiklenci*", /* alien */ 
    [2318] "*volam*", /* onovy */ 
    [2319] "*Kozel*", /* paces */ 
    [2320] "*Velvet*", /* alien */ 
    [2321] "*Branik*", /* alien */ 
    [2322] "*Ostravar*", /* alien */ 
    [2323] "*bonz it !*", /* andilek.c */ 
    [2324] "*cervena se*", /* onovy */ 
    [2325] "*tuc tuc*", /* paces */ 
    [2326] "*motyl*", /* paces */ 
    [2327] "*motyl*", /* paces */ 
    [2328] "*Outlook*", /* onovy */ 
    [2329] "*jednozubej*", /* paces */ 
    [2330] "*auto*", /* paces */ 
    [2331] "*HC Vsetin*", /* mathou */ 
    [2332] "*Staropramen*", /* onovy */ 
    [2333] "*Radegast*", /* onovy */ 
    [2334] "*Gambrinus*", /* onovy */ 
    [2335] "*Hostan*", /* onovy */ 
    [2336] "*Budweiser*", /* onovy */ 
    [2337] "*Starobrno*", /* onovy */ 
    [2338] "*Krusovice*", /* onovy */ 
    [2339] "*Primator*", /* onovy */ 
    [2340] "*platan*", /* onovy */ 
    [2341] "*FOG*", /* onovy */ 
    [2342] "*Nissan*", /* mathou */ 
    [2343] "*dloubani v nose*", /* mathou */ 
    [2344] "%-|", /* onovy */ 
    [2345] "*ne-e*", /* mathou */ 
    [2346] "*vejce*", /* paces */ 
    [2347] "*sachovnice*", /* onovy */ 
    [2348] "*papousek*", /* mathou */ 
    [2349] "*havran*", /* mathou */ 
    [2350] "*nesouhlasi*", /* mathou */ 
    [2351] "8-I", /* mathou */ 
    [2352] "*cyclop*", /* mathou */ 
    [2353] "*unaveny*", /* mathou */ 
    [2354] "*rozhlizi se*", /* mathou */ 
    [2355] "*fotak*", /* onovy */ 
    [2356] "*darek*", /* onovy */ 
    [2357] ":-<", /* mathou */ 
    [2358] "*Parkour*", /* mathou */ 
    [2359] "*bota*", /* mathou */ 
    [2360] "*bota*", /* mathou */ 
    [2361] "*Q3 4ever*", /* onovy */ 
    [2362] "*breci*", /* paces */ 
    [2363] "=)", /* mathou */ 
    [2364] "*Ferrari*", /* onovy */ 
    [2365] "*durex*", /* onovy */ 
    [2366] "*Peugeot*", /* onovy */ 
    [2367] "*Recko*", /* mathou */ 
    [2368] "*zamilovana kocka*", /* paces */ 
    [2370] "*Google*", /* onovy */ 
    [2371] "*jokingly*", /* mathou */ 
    [2372] "*jj chutna*", /* mathou */ 
    [2373] "*stydi se*", /* mathou */ 
    [2374] "*rofl*", /* mathou */ 
    [2375] "*teleport*", /* georgo10 */ 
    [2376] "*prvni pusa*", /* mathou */ 
    [2377] "*karatista*", /* andilek.c */ 
    [2378] "*dribluje*", /* andilek.c */ 
    [2379] "*to on*", /* georgo10 */ 
    [2380] "*play*", /* andilek.c */ 
    [2381] "*Quote*", /* andilek.c */ 
    [2382] "*usmivak*", /* paces */ 
    [2383] "*libani bot*", /* andilek.c */ 
    [2385] "*cunik*", /* andilek.c */ 
    [2386] "*tuuudle*", /* andilek.c */ 
    [2387] "*kometa*", /* andilek.c */ 
    [2388] "*schema*", /* andilek.c */ 
    [2389] "*Punks not dead*", /* onovy */ 
    [2390] "*jezecek*", /* andilek.c */ 
    [2391] "*Prima*", /* andilek.c */ 
    [2392] "*Arabie*", /* andilek.c */ 
    [2393] "[>:-(]", /* andilek.c */ 
    [2394] "*hvezdicka*", /* andilek.c */ 
    [2395] "*Oi!*", /* onovy */ 
    [2396] "*NOD32*", /* andilek.c */ 
    [2398] "*kecy, kecy*", /* paces */ 
    [2399] "*listecky*", /* andilek.c */ 
    [2400] "*srdicka*", /* andilek.c */ 
    [2401] "*vyplazuje jazyk*", /* paces */ 
    [2402] "*fuck off*", /* onovy */ 
    [2403] "*pozor!*", /* onovy */ 
    [2404] "*ha ha*", /* andilek.c */ 
    [2405] "*Ra*", /* andilek.c */ 
    [2406] "[8(]", /* andilek.c */ 
    [2407] "=)", /* georgo10 */ 
    [2408] "*FK Jablonec 97*", /* andilek.c */ 
    [2410] "*hledam*", /* andilek.c */ 
    [2411] "8(", /* paces */ 
    [2412] "*zvon do pusy*", /* georgo10 */ 
    [2413] "*papa s raneckem*", /* andilek.c */ 
    [2414] "*pistolnik*", /* georgo10 */ 
    [2415] "*splachuje se*", /* andilek.c */ 
    [2416] "*s doutnikem*", /* andilek.c */ 
    [2417] "*par :-* *", /* andilek.c */ 
    [2418] "*drink*", /* andilek.c */ 
    [2425] "*rotujici star*", /* andilek.c */ 
    [2426] "*Opel*", /* andilek.c */ 
    [2427] "*Honda*", /* andilek.c */ 
    [2428] "*Honda*", /* andilek.c */ 
    [2431] "*fnuka*", /* georgo10 */ 
    [2432] "*hopsa*", /* georgo10 */ 
    [2434] "*rozhlizi se*", /* georgo10 */ 
    [2436] "*kriketak*", /* andilek.c */ 
    [2437] "*zeleny micek*", /* georgo10 */ 
    [2438] "*slunecnice*", /* andilek.c */ 
    [2439] "*jablko*", /* andilek.c */ 
    [2440] "*jablko*", /* andilek.c */ 
    [2441] "*ryba*", /* andilek.c */ 
    [2442] "*zamereno*", /* andilek.c */ 
    [2443] "*prustrel*", /* andilek.c */ 
    [2444] "*svicka*", /* georgo10 */ 
    [2445] "*Bakalar*", /* andilek.c */ 
    [2446] "*zamilovane oci*", /* georgo10 */ 
    [2447] "*pocitace*", /* georgo10 */ 
    [2448] "*Kaufland*", /* andilek.c */ 
    [2449] "*Bulldogs*", /* andilek.c */ 
    [2450] "*flegmous*", /* andilek.c */ 
    [2451] "*ninja*", /* georgo10 */ 
    [2452] "*Dudak*", /* onovy */ 
    [2456] "*Florbal*", /* andilek.c */ 
    [2458] "*mic rugby*", /* andilek.c */ 
    [2459] "*miluji Te*", /* georgo10 */ 
    [2460] "*voda*", /* georgo10 */ 
    [2461] "*kopretina*", /* andilek.c */ 
    [2462] "*ryba*", /* andilek.c */ 
    [2463] "*kytice*", /* andilek.c */ 
    [2464] "*urazenej*", /* paces */ 
    [2466] "*boxer*", /* andilek.c */ 
    [2467] "*dostanes*", /* georgo10 */ 
    [2468] "*:( papa*", /* andilek.c */ 
    [2470] "*septy septy*", /* andilek.c */ 
    [2471] "*omdli*", /* georgo10 */ 
    [2472] "*Coca Cola*", /* andilek.c */ 
    [2473] "*Taure*", /* andilek.c */ 
    [2474] "*Nestea*", /* andilek.c */ 
    [2475] "*blondezwerg*", /* georgo10 */ 
    [2476] "*fandici*", /* andilek.c */ 
    [2479] "*slunce*", /* andilek.c */ 
    [2480] "*Sprite*", /* andilek.c */ 
    [2481] "*u pc*", /* georgo10 */ 
    [2483] "*nene :)*", /* andilek.c */ 
    [2484] "*jojo :)*", /* andilek.c */ 
    [2486] "8-|", /* andilek.c */ 
    [2487] "*kurak*", /* georgo10 */ 
    [2488] "*utika*", /* paces */ 
    [2489] "*Hippies*", /* andilek.c */ 
    [2490] "*mimino*", /* andilek.c */ 
    [2491] "*lodicka*", /* andilek.c */ 
    [2493] "*brecici*", /* georgo10 */ 
    [2494] "*Santa a sob*", /* andilek.c */ 
    [2495] "*tancici*", /* georgo10 */ 
    [2497] "*halooo*", /* georgo10 */ 
    [2498] "*I love Fierfox*", /* andilek.c */ 
    [2500] "*Bulharsko*", /* andilek.c */ 
    [2501] "*papa*", /* andilek.c */ 
    [2502] "*potlesk*", /* andilek.c */ 
    [2503] "*smrtka :D*", /* andilek.c */ 
    [2504] "*ruze*", /* andilek.c */ 
    [2506] "*Fallout*", /* andilek.c */ 
    [2507] "*kytka*", /* andilek.c */ 
    [2511] "*zmrzlinar*", /* georgo10 */ 
    [2512] "*Heineken*", /* andilek.c */ 
    [2513] "*Stella Artois*", /* andilek.c */ 
    [2514] "*SuperSmajlik*", /* andilek.c */ 
    [2515] "*tweety*", /* andilek.c */ 
    [2516] "*nVidia*", /* andilek.c */ 
    [2517] "*o berlich*", /* georgo10 */ 
    [2518] "*vysmatej*", /* paces */ 
    [2520] "*kanadan*", /* georgo10 */ 
    [2521] "*Nike*", /* andilek.c */ 
    [2522] "*O2*", /* georgo10 */ 
    [2523] "*Havirov*", /* andilek.c */ 
    [2524] "*Ska*", /* andilek.c */ 
    [2525] "*Zetor*", /* andilek.c */ 
    [2526] "*puska*", /* andilek.c */ 
    [2527] "*snezenka*", /* andilek.c */ 
    [2528] "*HUP*", /* andilek.c */ 
    [2529] "*motylek*", /* andilek.c */ 
    [2530] "*destnik*", /* andilek.c */ 
    [2532] "*peprmintka*", /* andilek.c */ 
    [2533] "*pito*", /* onovy */ 
    [2534] "*jeanette*", /* andilek.c */ 
    [2535] "*presypaci hodiny*", /* andilek.c */ 
    [2536] "*tancici*", /* andilek.c */ 
    [2537] "*motyl*", /* andilek.c */ 
    [2538] "*ruzovy film*", /* andilek.c */ 
    [2539] "*dinosaurek*", /* andilek.c */ 
    [2540] "*No Nazis!*", /* onovy */ 
    [2541] "*zijeme offline*", /* andilek.c */ 
    [2542] "*magie*", /* andilek.c */ 
    [2543] "*Nitro Snowboards*", /* andilek.c */ 
    [2544] "*Anubis*", /* andilek.c */ 
    [2545] "*nastvany*", /* georgo10 */ 
    [2547] "*Freddy*", /* georgo10 */ 
    [2548] "*Jason*", /* georgo10 */ 
    [2549] "*boji se*", /* georgo10 */ 
    [2550] "*nechapavej*", /* paces */ 
    [2554] "*gridlock*", /* georgo10 */ 
    [2555] "*zarovka*", /* georgo10 */ 
    [2556] "*radioactive*", /* andilek.c */ 
    [2557] "*live*", /* onovy */ 
    [2558] "*zubatej*", /* georgo10 */ 
    [2559] "*hrachovka*", /* andilek.c */ 
    [2560] "*X Look*", /* andilek.c */ 
    [2561] "*dum*", /* andilek.c */ 
    [2564] ":(", /* andilek.c */ 
    [2566] "*nene :)*", /* andilek.c */ 
    [2568] "*jeanette*", /* andilek.c */ 
    [2570] "*hvezda*", /* georgo10 */ 
    [2571] "*refresh*", /* andilek.c */ 
    [2573] "*smutnej*", /* paces */ 
    [2574] "*na zdravi*", /* georgo10 */ 
    [2575] "*medvidek*", /* georgo10 */ 
    [2577] "*abstinent*", /* andilek.c */ 
    [2580] "*srdce*", /* georgo10 */ 
    [2582] "*KO*", /* andilek.c */ 
    [2583] "*blazne*", /* georgo10 */ 
    [2584] "*mava*", /* georgo10 */ 
    [2586] "*Holba*", /* onovy */ 
    [2587] "*boxer*", /* andilek.c */ 
    [2589] "*ZzZz*", /* andilek.c */ 
    [2597] "*Morrowind*", /* andilek.c */ 
    [2598] "*Gothic*", /* andilek.c */ 
    [2599] "*Gothic II*", /* andilek.c */ 
    [2600] "*Black Isle*", /* andilek.c */ 
    [2602] "*Fallout*", /* georgo10 */ 
    [2603] "*Fallout*", /* onovy */ 
    [2604] "*hvezda orange*", /* andilek.c */ 
    [2605] "*hvezda pink*", /* andilek.c */ 
    [2606] "*hvezda grey*", /* andilek.c */ 
    [2607] "*hvezda green*", /* andilek.c */ 
    [2609] "*kosticky*", /* georgo10 */ 
    [2612] "*nesouhlas*", /* georgo10 */ 
    [2614] "*vlna*", /* andilek.c */ 
    [2615] "*mne si oci*", /* georgo10 */ 
    [2617] "*na vozicku*", /* georgo10 */ 
    [2618] "*kachnicky*", /* andilek.c */ 
    [2619] "*HC Havirov Panthers*", /* andilek.c */ 
    [2620] "*had*", /* andilek.c */ 
    [2621] "*pozor kick*", /* andilek.c */ 
    [2623] "*vodni dymka*", /* georgo10 */ 
    [2626] "*v desti*", /* georgo10 */ 
    [2627] "*loituma*", /* georgo10 */ 
    [2628] "*CD*", /* onovy */ 
    [2629] "*dlouhy nos*", /* andilek.c */ 
    [2630] "*potlesk*", /* georgo10 */ 
    [2634] "*cte noviny*", /* andilek.c */ 
    [2636] "*tleska*", /* andilek.c */ 
    [2637] "=))", /* georgo10 */ 
    [2639] "*blondezwerg*", /* georgo10 */ 
    [2641] "*kure*", /* georgo10 */ 
    [2642] "*Deli*", /* andilek.c */ 
    [2644] "*Xchat*", /* onovy */ 
    [2645] "*ahojky*", /* onovy */ 
    [2646] "*??*", /* andilek.c */ 
    [2649] "*kyticka*", /* andilek.c */ 
    [2650] "*andel*", /* andilek.c */ 
    [2653] "*bye*", /* andilek.c */ 
    [2655] "*dobrou noc*", /* andilek.c */ 
    [2656] "*love you*", /* andilek.c */ 
    [2657] "*I love Techno*", /* andilek.c */ 
    [2659] "*I love Trance*", /* andilek.c */ 
    [2662] "*zamilovani*", /* georgo10 */ 
    [2666] ":)", /* andilek.c */ 
    [2667] "*diky*", /* andilek.c */ 
    [2668] "*uaa ziva*", /* andilek.c */ 
    [2670] ":D", /* andilek.c */ 
    [2671] "*zajda*", /* andilek.c */ 
    [2672] "*kyticka*", /* georgo10 */ 
    [2673] "*no sex*", /* georgo10 */ 
    [2681] "*Star Wars*", /* andilek.c */ 
    [2682] "*??*", /* onovy */ 
    [2683] "*tancici*", /* andilek.c */ 
    [2685] "*Pokemon*", /* andilek.c */ 
    [2687] "*tancuje*", /* andilek.c */ 
    [2690] "*hvezda*", /* andilek.c */ 
    [2691] "*Apple*", /* andilek.c */ 
    [2692] "*pozor kill*", /* andilek.c */ 
    [2693] "*Gambrinus*", /* andilek.c */ 
    [2694] "*Kawasaki*", /* andilek.c */ 
    [2696] "=)", /* andilek.c */ 
    [2697] "*sluchatka*", /* andilek.c */ 
    [2699] "*FREE!*", /* andilek.c */ 
    [2700] "*Valid XHTML 1.0*", /* onovy */ 
    [2704] "*HIM*", /* andilek.c */ 
    [2705] "*stastnej*", /* paces */ 
    [2706] "*Unicov*", /* andilek.c */ 
    [2707] "*zakaz koureni*", /* andilek.c */ 
    [2708] "*helikoptera*", /* andilek.c */ 
    [2709] "*Call of Duty 2*", /* andilek.c */ 
    [2711] ":O", /* andilek.c */ 
    [2712] "*MTV*", /* andilek.c */ 
    [2713] "*VIVA*", /* andilek.c */ 
    [2718] "*Vanilla ninja*", /* andilek.c */ 
    [2719] "*Björk*", /* andilek.c */ 
    [2720] "*boxer*", /* georgo10 */ 
    [2721] "*drsnak*", /* andilek.c */ 
    [2722] "*Opava*", /* andilek.c */ 
    [2723] "*kocka*", /* andilek.c */ 
    [2726] "*darecek*", /* andilek.c */ 
    [2731] "*vydeseny*", /* georgo10 */ 
    [2732] "*sejmout*", /* andilek.c */ 
    [2733] "*andilek*", /* andilek.c */ 
    [2734] "*se samopalem*", /* andilek.c */ 
    [2739] "*ozralove*", /* georgo10 */ 
    [2740] "*se svickou*", /* georgo10 */ 
    [2741] "*milenci*", /* georgo10 */ 
    [2742] "*mimino*", /* georgo10 */ 
    [2744] "*japonec*", /* georgo10 */ 
    [2748] "*vcela*", /* andilek.c */ 
    [2751] "*holubar*", /* georgo10 */ 
    [2752] "*vsemi milovany*", /* georgo10 */ 
    [2753] "*zalesak*", /* georgo10 */ 
    [2755] "*lahvacek*", /* georgo10 */ 
    [2756] "*boxeri*", /* georgo10 */ 
    [2760] "*alkoholik*", /* andilek.c */ 
    [2761] "*tanecnici*", /* georgo10 */ 
    [2765] "*My chemical romance*", /* andilek.c */ 
    [2766] "*Benny Benassi*", /* andilek.c */ 
    [2767] "*kamaradi*", /* georgo10 */ 
    [2770] "*posta*", /* andilek.c */ 
    [2772] "*pistolnik*", /* georgo10 */ 
    [2776] "*bolavy zub*", /* georgo10 */ 
    [2777] "*urazeny*", /* georgo10 */ 
    [2781] "*s prakem*", /* georgo10 */ 
    [2782] "*lukostrelec*", /* georgo10 */ 
    [2784] "*kouri*", /* andilek.c */ 
    [2786] "*hraci kostka*", /* andilek.c */ 
    [2793] "*ninja*", /* andilek.c */ 
    [2794] "*kdo tam?*", /* andilek.c */ 
    [2797] "*kukacka*", /* georgo10 */ 
    [2799] "*lov*", /* georgo10 */ 
    [2800] "*Users*", /* andilek.c */ 
    [2801] "*pohrebaci*", /* andilek.c */ 
    [2802] "*nese strom*", /* georgo10 */ 
    [2803] "*svatebcane*", /* georgo10 */ 
    [2805] "*Srbsko*", /* andilek.c */ 
    [2810] "*s pilnikem*", /* georgo10 */ 
    [2811] "*breakdance*", /* andilek.c */ 
    [2815] "*pavouk*", /* andilek.c */ 
    [2821] "*napadnik*", /* andilek.c */ 
    [2827] "*facka*", /* andilek.c */ 
    [2832] "*flasinetar*", /* andilek.c */ 
    [2833] "*Rasmus*", /* andilek.c */ 
    [2835] "*sumo*", /* andilek.c */ 
    [2836] "*banned*", /* andilek.c */ 
    [2837] "*Viking*", /* andilek.c */ 
    [2839] "*Duff*", /* andilek.c */ 
    [2842] "*Lame*", /* andilek.c */ 
    [2844] "*parohac*", /* andilek.c */ 
    [2845] "*plete*", /* andilek.c */ 
    [2846] "*duch*", /* andilek.c */ 
    [2847] "*s teplomerem*", /* georgo10 */ 
    [2849] "*volske oko*", /* andilek.c */ 
    [2850] "*s bicem*", /* georgo10 */ 
    [2851] "*Titanic*", /* andilek.c */ 
    [2852] "*souloz*", /* georgo10 */ 
    [2855] "*venci psa*", /* andilek.c */ 
    [2856] "*Jamajka*", /* andilek.c */ 
    [2857] "*leguan*", /* georgo10 */ 
    [2858] "*slon*", /* georgo10 */ 
    [2859] "*s teplomerem*", /* georgo10 */ 
    [2864] "*ozrala*", /* georgo10 */ 
    [2868] "*broucek*", /* andilek.c */ 
    [2869] "*Domazlice*", /* andilek.c */ 
    [2870] "*BillaBong*", /* andilek.c */ 
    [2871] "*Vans*", /* andilek.c */ 
    [2877] "*Zero*", /* andilek.c */ 
    [2878] "*play*", /* andilek.c */ 
    [2879] "*film*", /* andilek.c */ 
    [2880] "*tenista*", /* georgo10 */ 
    [2882] "*harfista*", /* andilek.c */ 
    [2884] "*skejtak*", /* georgo10 */ 
    [2886] "*s paratkem*", /* andilek.c */ 
    [2887] "*klavirista*", /* georgo10 */ 
    [2888] "*cte noviny*", /* andilek.c */ 
    [2889] "*tesi me*", /* andilek.c */ 
    [2892] "*tuli tuli*", /* andilek.c */ 
    [2894] "*ty ty ty!*", /* andilek.c */ 
    [2895] "*smich*", /* georgo10 */ 
    [2897] "*8 a pak :D*", /* andilek.c */ 
    [2902] "*srdce*", /* georgo10 */ 
    [2904] "*s kytarou*", /* georgo10 */ 
    [2905] "*cream*", /* georgo10 */ 
    [2916] "*KLH Chomutov*", /* andilek.c */ 
    [2917] "*Uh. Brod*", /* andilek.c */ 
    [2918] "*Cesky Tesin*", /* andilek.c */ 
    [2919] "*ziiiv*", /* andilek.c */ 
    [2920] "*offline*", /* andilek.c */ 
    [2921] "*online*", /* andilek.c */ 
    [2922] "*Broumov*", /* andilek.c */ 
    [2923] "*Breclav*", /* andilek.c */ 
    [2924] "*muz*", /* onovy */ 
    [2926] "*KFOR*", /* andilek.c */ 
    [2927] "*CZSK17*", /* andilek.c */ 
    [2929] "*Sonic-X*", /* andilek.c */ 
    [2930] "*Witch*", /* andilek.c */ 
    [2933] "*cigareta*", /* andilek.c */ 
    [2939] "*ROFL*", /* andilek.c */ 
    [2941] "*lokomotiva*", /* andilek.c */ 
    [2942] "*vagonek A*", /* andilek.c */ 
    [2943] "*vagonek B*", /* andilek.c */ 
    [2944] "*vagonek C*", /* andilek.c */ 
    [2945] "*vagonek D*", /* andilek.c */ 
    [2946] "*vagonek E*", /* andilek.c */ 
    [2947] "*vagonek F*", /* andilek.c */ 
    [2948] "*vagonek G*", /* andilek.c */ 
    [2949] "*vagonek H*", /* andilek.c */ 
    [2950] "*vagonek I*", /* andilek.c */ 
    [2951] "*vagonek J*", /* andilek.c */ 
    [2952] "*vagonek K*", /* andilek.c */ 
    [2953] "*vagonek L*", /* andilek.c */ 
    [2954] "*vagonek M*", /* andilek.c */ 
    [2955] "*vagonek N*", /* andilek.c */ 
    [2956] "*vagonek O*", /* andilek.c */ 
    [2957] "*vagonek P*", /* andilek.c */ 
    [2958] "*vagonek Q*", /* andilek.c */ 
    [2959] "*vagonek R*", /* andilek.c */ 
    [2960] "*vagonek S*", /* andilek.c */ 
    [2961] "*vagonek T*", /* andilek.c */ 
    [2962] "*vagonek U*", /* andilek.c */ 
    [2963] "*vagonek V*", /* andilek.c */ 
    [2964] "*vagonek W*", /* andilek.c */ 
    [2965] "*vagonek X*", /* andilek.c */ 
    [2966] "*vagonek Y*", /* andilek.c */ 
    [2967] "*vagonek Z*", /* andilek.c */ 
    [2968] "*vagonek 1*", /* andilek.c */ 
    [2969] "*vagonek 2*", /* andilek.c */ 
    [2971] "*vagonek 3*", /* andilek.c */ 
    [2972] "*vagonek 4*", /* andilek.c */ 
    [2973] "*vagonek 5*", /* andilek.c */ 
    [2974] "*vagonek 6*", /* andilek.c */ 
    [2975] "*vagonek 7*", /* andilek.c */ 
    [2976] "*vagonek 8*", /* andilek.c */ 
    [2977] "*vagonek 9*", /* andilek.c */ 
    [2978] "*vagonek 0*", /* andilek.c */ 
    [2979] "*vagonek !*", /* andilek.c */ 
    [2980] "*vagonek ?*", /* andilek.c */ 
    [2981] "*vagonek .*", /* andilek.c */ 
    [2982] "*vagonek :*", /* andilek.c */ 
    [2983] "*vagonek @*", /* andilek.c */ 
    [2984] "*vagonek -*", /* andilek.c */ 
    [2985] "*vagonek &*", /* andilek.c */ 
    [2986] "*prazdny vagon*", /* andilek.c */ 
    [2991] "*spam*", /* andilek.c */ 
    [2992] "*papousek*", /* andilek.c */ 
    [2993] "*Jetix*", /* andilek.c */ 
    [2994] "*locked*", /* andilek.c */ 
    [2995] "*klaun*", /* andilek.c */ 
    [2996] "*polojasno*", /* andilek.c */ 
    [2997] "*slunicko*", /* andilek.c */ 
    [2999] "*na zdravi*", /* andilek.c */ 
    [3001] "*prsi*", /* andilek.c */ 
    [3004] "*to ne ne*", /* andilek.c */ 
    [3005] "*klanim se*", /* andilek.c */ 
    [3006] "*mlha*", /* andilek.c */ 
    [3007] "*mrak*", /* andilek.c */ 
    [3009] "*andilek*", /* andilek.c */ 
    [3010] "*Brno*", /* andilek.c */ 
    [3014] "*Pohadka*", /* andilek.c */ 
    [3015] "*Vita te Krajina za zrcadlem*", /* andilek.c */ 
    [3016] "*HC Slavia*", /* andilek.c */ 
    [3017] "*Carolina*", /* andilek.c */ 
    [3018] "*Mam se dobre*", /* andilek.c */ 
    [3021] "*Lineage*", /* andilek.c */ 
    [3026] "*telefonuje*", /* andilek.c */ 
    [3027] "*ROFL*", /* andilek.c */ 
    [3030] "*hneda hvezda*", /* andilek.c */ 
    [3031] "*Pilne vcelicky*", /* andilek.c */ 
    [3037] "*kytara*", /* andilek.c */ 
    [3038] "*Pro7*", /* andilek.c */ 
    [3039] "*kytara*", /* andilek.c */ 
    [3040] ":-)", /* andilek.c */ 
    [3041] ":-(", /* andilek.c */ 
    [3048] "*svudny pohled*", /* andilek.c */ 
    [3049] "*Fallout*", /* andilek.c */ 
    [3051] "*Simpsons*", /* andilek.c */ 
    [3052] "*Vsetin*", /* andilek.c */ 
    [3053] "*Roznov p. R.*", /* andilek.c */ 
    [3058] "*Vitej v Oaze*", /* andilek.c */ 
    [3059] "*Srub zavislaku*", /* andilek.c */ 
    [3060] "*Amerika*", /* andilek.c */ 
    [3066] "*kamion*", /* andilek.c */ 
    [3067] "*vaha*", /* andilek.c */ 
    [3068] "*doktor*", /* andilek.c */ 
    [3070] "*rozcestnik*", /* andilek.c */ 
    [3071] "*drink*", /* andilek.c */ 
    [3075] "*slunicko*", /* andilek.c */ 
    [3078] "*motorkar*", /* andilek.c */ 
    [3079] "*sorry*", /* andilek.c */ 
    [3080] "*ruze*", /* andilek.c */ 
    [3083] "*no smoking*", /* andilek.c */ 
    [3089] "*kamion*", /* andilek.c */ 
    [3091] "*vajicko*", /* andilek.c */ 
    [3093] "*pomoooc*", /* georgo10 */ 
    [3096] "*bozi oko*", /* andilek.c */ 
    [3097] "*plot*", /* andilek.c */ 
    [3100] "*netopyr*", /* andilek.c */ 
    [3101] "*rybar*", /* andilek.c */ 
    [3102] "*telefonuje*", /* andilek.c */ 
    [3108] "*SKA*", /* andilek.c */ 
    [3109] "*Liberec-Doubi*", /* andilek.c */ 
    [3110] "*Emo*", /* andilek.c */ 
    [3111] "*AC/DC*", /* andilek.c */ 
    [3114] "*ruze*", /* andilek.c */ 
    [3117] "*fixa*", /* andilek.c */ 
    [3118] "*Zubri*", /* andilek.c */ 
    [3119] "*Kamikaze*", /* andilek.c */ 
    [3139] "*prsi*", /* andilek.c */ 
    [3140] "*tlakova vyse*", /* andilek.c */ 
    [3141] "*tlakova nize*", /* andilek.c */ 
    [3142] "*tropicka boure*", /* andilek.c */ 
    [3143] "*hurikan*", /* andilek.c */ 
    [3144] "*Sokolov*", /* andilek.c */ 
    [3146] ":-x", /* andilek.c */ 
    [3155] "*certifikovany muz*", /* andilek.c */ 
    [3156] "*certifikovana zena*", /* andilek.c */ 
    [3161] "*kriz*", /* andilek.c */ 
    [3162] "*ruze*", /* andilek.c */ 
    [3163] "*kytka*", /* andilek.c */ 
    [3175] "*nuda*", /* andilek.c */ 
    [3176] "*tanec*", /* georgo10 */ 
    [3178] "*WC*", /* andilek.c */ 
    [3180] "*Francie*", /* andilek.c */ 
    [3182] "*stir*", /* andilek.c */ 
    [3185] "*Brno*", /* andilek.c */ 
    [3188] "*vyprask*", /* andilek.c */ 
    [3197] "*Chorvatsko*", /* andilek.c */ 
    [3198] "*Pivni vuz*", /* andilek.c */ 
    [3199] "*zluta hvezda*", /* andilek.c */ 
    [3200] "*cervena hvezda*", /* andilek.c */ 
    [3201] "*cerna hvezda*", /* andilek.c */ 
    [3202] "*modra hvezda*", /* andilek.c */ 
    [3203] "*zelena hvezda*", /* andilek.c */ 
    [3206] "*ruze*", /* andilek.c */ 
    [3207] "*boxer*", /* andilek.c */ 
    [3210] "*Metaxa*", /* andilek.c */ 
    [3212] "*Gool*", /* andilek.c */ 
    [3215] "*kdo neskace...*", /* andilek.c */ 
    [3217] "*metal*", /* andilek.c */ 
    [3224] "*achjo*", /* andilek.c */ 
    [3225] "*telefonuje*", /* georgo10 */ 
    [3226] "*duha*", /* andilek.c */ 
    [3230] "*prosiim*", /* andilek.c */ 
    [3250] "*true*", /* andilek.c */ 
    [3252] "*false*", /* andilek.c */ 
    [3262] "*moment*", /* andilek.c */ 
    [3263] "*vodni dymka*", /* georgo10 */ 
    [3273] "*nuzky*", /* andilek.c */ 
    [3279] "*MMNT*", /* andilek.c */ 
    [3280] "*z5*", /* andilek.c */ 
    [3306] "*Word*", /* andilek.c */ 
    [3307] "*Excel*", /* andilek.c */ 
    [3308] "*Dance*", /* andilek.c */ 
    [3310] "*Litomerice*", /* andilek.c */ 
    [3320] "*!*", /* onovy */ 
    [3338] "*dort*", /* andilek.c */ 
    [3347] "*hop hop*", /* andilek.c */ 
    [3351] ":DD", /* andilek.c */ 
    [3354] ":)", /* andilek.c */ 
    [3356] "*MNT*", /* andilek.c */ 
    [3359] "*Zlin je in*", /* andilek.c */ 
    [3360] "* ! *", /* andilek.c */ 
    [3361] "*Hnusnej banner*", /* andilek.c */ 
    [3370] "*skakajici cervenej curak*", /* georgo10 */ 
    [3390] "*Kelt*", /* georgo10 */ 
    [3392] "*ruze*", /* georgo10 */ 
    [3404] "*UT*", /* georgo10 */ 
    [3405] "*UT*", /* georgo10 */ 
    [3409] "*rodinka*", /* andilek.c */ 
    [3424] "*3D party*", /* andilek.c */ 
    [3429] "*xpce*", /* andilek.c */ 
    [3434] "*lol*", /* andilek.c */ 
    [3439] "*boxer*", /* andilek.c */ 
    [3442] "*Santa Klaus*", /* andilek.c */ 
    [3444] "*srdicko*", /* andilek.c */ 
    [3445] "*Rofl :)*", /* andilek.c */ 
    [3446] "*OMG*", /* andilek.c */ 
    [3449] "*ATOM*", /* andilek.c */ 
    [3450] "*Digimon*", /* andilek.c */ 
    [3451] "*Power Rangers*", /* andilek.c */ 
    [3452] "*Fiat*", /* andilek.c */ 
    [3453] "*Gool*", /* andilek.c */ 
    [3454] "*Banik Ostrava*", /* andilek.c */ 
    [3455] "*Nactileti*", /* andilek.c */ 
    [3456] "*prasatko*", /* andilek.c */ 
    [3457] "*Audi*", /* andilek.c */ 
    [3458] "*Audi*", /* andilek.c */ 
    [3461] "*andilek*", /* andilek.c */ 
    [3463] "*bomba*", /* andilek.c */ 
    [3464] "*cert*", /* andilek.c */ 
    [3467] ":-*", /* andilek.c */ 
    [3470] "*Kryton*", /* andilek.c */ 
    [3471] "*Kocour*", /* andilek.c */ 
    [3472] "*Lister*", /* andilek.c */ 
    [3473] "*Kochanska*", /* andilek.c */ 
    [3474] "*Holly*", /* andilek.c */ 
    [3475] "*Holly*", /* andilek.c */ 
    [3476] "*Rimmer*", /* andilek.c */ 
    [3477] "*papa*", /* andilek.c */ 
    [3481] "*You are stupid*", /* andilek.c */ 
    [3482] "*Ups! Sorry*", /* andilek.c */ 
    [3483] "*Jak je?*", /* andilek.c */ 
    [3484] "*Vitej u nas*", /* andilek.c */ 
    [3486] "*z5*", /* onovy */ 
    [3489] "*cyklista*", /* andilek.c */ 
    [3490] "*pracka*", /* andilek.c */ 
    [3491] "*Help me!*", /* andilek.c */ 
    [3493] "*jj ;)*", /* andilek.c */ 
    [3497] "*hladovy*", /* andilek.c */ 
    [3501] "*fuck*", /* andilek.c */ 
    [3515] ";-)", /* andilek.c */ 
    [3519] "*cervena se*", /* andilek.c */ 
    [3525] ":-X", /* andilek.c */ 
    [3526] "*andel*", /* andilek.c */ 
    [3528] "*sova*", /* andilek.c */ 
    [3529] "*batman*", /* andilek.c */ 
    [3541] "*Ferarri*", /* andilek.c */ 
    [3544] "*hasic*", /* andilek.c */ 
    [3545] "*Ferarri*", /* andilek.c */ 
    [3549] "*musim jit*", /* onovy */ 
    [3551] "*Jak je?:)*", /* andilek.c */ 
    [3552] "*I love U :)*", /* andilek.c */ 
    [3553] "*Miss U :\'(*", /* andilek.c */ 
    [3554] "*hop hop*", /* andilek.c */ 
    [3555] "*musim jit*", /* andilek.c */ 
    [3558] "*problem?*", /* andilek.c */ 
    [3560] "*carodejnice*", /* andilek.c */ 
    [3561] "*Oh no!*", /* andilek.c */ 
    [3569] "*hokejista*", /* andilek.c */ 
    [3572] "*recycle*", /* andilek.c */ 
    [3574] "*tenista*", /* andilek.c */ 
    [3575] "*pooojd =)*", /* andilek.c */ 
    [3576] "*Hm*", /* andilek.c */ 
    [3577] "*zlomene srdce*", /* andilek.c */ 
    [3587] "*Suzuki*", /* andilek.c */ 
    [3588] "*KTM-G*", /* andilek.c */ 
    [3590] "*no kiss*", /* andilek.c */ 
    [3599] "*trampolina*", /* andilek.c */ 
    [3601] "*kiss*", /* andilek.c */ 
    [3603] "*RObotka*", /* andilek.c */ 
    [3606] "*hokejista*", /* andilek.c */ 
    [3611] "*srdicko*", /* andilek.c */ 
    [3620] "*moment*", /* andilek.c */ 
    [3626] ":-*", /* andilek.c */ 
    [3630] "*Seat*", /* andilek.c */ 
    [3631] "*tlapka*", /* andilek.c */ 
    [3636] "*Miele*", /* andilek.c */ 
    [3637] "*zelvik*", /* andilek.c */ 
    [3638] "*Kenvelo*", /* andilek.c */ 
    [3639] "*skateboard*", /* andilek.c */ 
    [3642] "*civava*", /* andilek.c */ 
    [3645] "*Vesele vanoce*", /* andilek.c */ 
    [3646] "*Santa Klaus*", /* andilek.c */ 
    [3665] "*kachnicka*", /* andilek.c */ 
    [3668] "*Dobrou noc*", /* andilek.c */ 
    [3683] "*ruze*", /* andilek.c */ 
    [3685] "*OK*", /* andilek.c */ 
    [3691] "*Hoj*", /* andilek.c */ 
    [3693] "*lebka*", /* andilek.c */ 
    [3695] "*zralok*", /* andilek.c */ 
    [3696] ":D", /* andilek.c */ 
    [3697] ":)", /* andilek.c */ 
    [3700] ":(", /* andilek.c */ 
    [3713] "*breakdance*", /* andilek.c */ 
    [3716] "*JHMD*", /* andilek.c */ 
    [3718] "*srdicko*", /* andilek.c */ 
    [3719] "*MUV 69u 968*", /* andilek.c */ 
    [3737] "*ICQ kytka*", /* andilek.c */ 
    [3740] "*pusa*", /* andilek.c */ 
    [3750] "*Ne*", /* andilek.c */ 
    [3751] "*Jo*", /* andilek.c */ 
    [3753] "*Slavia Praha*", /* andilek.c */ 
    [3760] "*kriz*", /* andilek.c */ 
    [3761] "*panda*", /* andilek.c */ 
    [3762] "*kocka*", /* andilek.c */ 
    [3773] "*toaletak*", /* andilek.c */ 
    [3779] "*krvacejici srdce*", /* andilek.c */ 
    [3786] "*NHL*", /* andilek.c */ 
    [3787] "*KZC*", /* andilek.c */ 
    [3796] "*ctyrlistek*", /* andilek.c */ 
    [3818] "*Mejte se*", /* andilek.c */ 
    [3832] "*pejsek*", /* andilek.c */ 
    [3834] "*2*", /* andilek.c */ 
    [3835] "*3*", /* andilek.c */ 
    [3836] "*5*", /* andilek.c */ 
    [3837] "*6*", /* andilek.c */ 
    [3838] "*7*", /* andilek.c */ 
    [3839] "*8*", /* andilek.c */ 
    [3840] "*4*", /* andilek.c */ 
    [3841] "*9*", /* andilek.c */ 
    [3842] "*0*", /* andilek.c */ 
    [3851] "*sampus*", /* andilek.c */ 
    [3872] "*Bart*", /* andilek.c */ 
    [3877] "*Mam se fajn*", /* andilek.c */ 
    [3878] "*Nespete!*", /* andilek.c */ 
    [3904] "*andilek*", /* andilek.c */ 
    [3905] ":-X", /* andilek.c */ 
    [3913] "*Kenvelo*", /* andilek.c */ 
    [3915] "*srdce*", /* andilek.c */ 
    [3916] "*ahoj*", /* andilek.c */ 
    [3917] "*Jde to*", /* andilek.c */ 
    [3918] "*Zdravim prichozi*", /* andilek.c */ 
    [3938] "*prosiiim*", /* andilek.c */ 
    [3939] ";-|", /* andilek.c */ 
    [3942] "*septajici*", /* andilek.c */ 
    [3943] "*ovce*", /* andilek.c */ 
    [3945] "*ovce*", /* andilek.c */ 
    [3946] "*cert*", /* andilek.c */ 
    [3948] "*HELP*", /* andilek.c */ 
    [3951] "*Den na h...*", /* andilek.c */ 
    [3956] "*MAKRO*", /* andilek.c */ 
    [3966] "*Narodni muzeum*", /* andilek.c */ 
    [3968] "*mrkaci*", /* andilek.c */ 
    [3969] "*ten treti*", /* andilek.c */ 
    [3972] "*ruze*", /* andilek.c */ 
    [3982] "*IKEA*", /* andilek.c */ 
    [3983] "*Tullamore Dew*", /* andilek.c */ 
    [3984] "*kouzelnik*", /* andilek.c */ 
    [3985] "*Globus*", /* andilek.c */ 
    [3994] "*srdicko*", /* andilek.c */ 
    [3997] "*sipka vpravo*", /* andilek.c */ 
    [3998] "*sipka vlevo*", /* andilek.c */ 
    [3999] "*vidle*", /* andilek.c */ 
    [4000] "*srdce ;)*", /* andilek.c */ 
    [4001] "*Lasko*", /* andilek.c */ 
    [4002] "*sipka dolu*", /* andilek.c */ 
    [4003] "*sipka nahoru*", /* andilek.c */ 
    [4004] "*Cechy*", /* andilek.c */ 
    [4008] "*tyrkysova hvezda*", /* paces */ 
    [4012] "*Happy Valentine\'s day*", /* andilek.c */ 
    [4017] "*zralok*", /* andilek.c */ 
    [4019] "*Hello!*", /* andilek.c */ 
    [4020] "*alfa*", /* andilek.c */ 
    [4021] "*omega*", /* andilek.c */ 
    [4024] "*dlouhy polibek*", /* andilek.c */ 
    [4025] "*srdicka*", /* andilek.c */ 
    [4027] "*back*", /* andilek.c */ 
    [4032] "*s frkackou*", /* andilek.c */ 
    [4033] "*dlouhy nos*", /* andilek.c */ 
    [4040] "*pomnenka*", /* andilek.c */ 
    [4041] "*ryby*", /* andilek.c */ 
    [4042] "*vahy*", /* andilek.c */ 
    [4047] "*plati*", /* andilek.c */ 
    [4049] "*vezen*", /* andilek.c */ 
    [4050] "*facka*", /* andilek.c */ 
    [4051] "*certik*", /* andilek.c */ 
    [4058] "*tancujici tucnak*", /* andilek.c */ 
    [4059] "*balonky*", /* andilek.c */ 
    [4060] "*Jak je?*", /* andilek.c */ 
    [4061] "*Pohoda*", /* andilek.c */ 
    [4066] "*XProgramatori*", /* andilek.c */ 
    [4087] "*ne ne ne*", /* andilek.c */ 
    [4091] "*xUL*", /* andilek.c */ 
    [4092] "*steel*", /* andilek.c */ 
    [4096] "*srdce*", /* paces */ 
    [4097] "*prase*", /* andilek.c */ 
    [4101] "*ridic*", /* andilek.c */ 
    [4105] "*i love you*", /* paces */ 
    [4107] "*zamilovane hovna*", /* paces */ 
    [4110] "*ty ty ty*", /* andilek.c */ 
    [4117] "*I love NY*", /* andilek.c */ 
    [4121] "*stavka*", /* andilek.c */ 
    [4122] "*uvazuje*", /* paces */ 
    [4125] "*false*", /* andilek.c */ 
    [4126] "*right*", /* andilek.c */ 
    [4128] "*srdicko*", /* andilek.c */ 
    [4132] "*o106*", /* andilek.c */ 
    [4133] "*jezek*", /* onovy */ 
    [4134] "*ambulance*", /* andilek.c */ 
    [4135] "*jim prasek*", /* andilek.c */ 
    [4143] "*bye!*", /* andilek.c */ 
    [4146] "*roztleskavacka*", /* andilek.c */ 
    [4147] "*andel:-* *", /* andilek.c */ 
    [4149] "*papa*", /* andilek.c */ 
    [4151] "*liba ruku*", /* paces */ 
    [4155] "*hokejista*", /* andilek.c */ 
    [4164] "*Ford*", /* paces */ 
    [4165] "*Volkswagen*", /* andilek.c */ 
    [4166] "*Citroen*", /* onovy */ 
    [4169] "*krouti ocima*", /* paces */ 
    [4170] "*mrkajici*", /* paces */ 
    [4171] "*Znojmo*", /* paces */ 
    [4188] "*jahudka*", /* andilek.c */ 
    [4209] "*ahoj*", /* andilek.c */ 
    [4211] "*nota*", /* andilek.c */ 
    [4214] "*kuchar*", /* andilek.c */ 
    [4215] "*s pomlazkou*", /* andilek.c */ 
    [4220] "*Baywatch*", /* andilek.c */ 
    [4221] "*sk8*", /* andilek.c */ 
    [4230] "*Gnome*", /* andilek.c */ 
    [4235] "*MNT cigo*", /* andilek.c */ 
    [4236] "*cajik*", /* andilek.c */ 
    [4248] "*Last.fm*", /* andilek.c */ 
    [4249] "*Air Soft Elite*", /* andilek.c */ 
    [4250] "*Yamaha*", /* andilek.c */ 
    [4255] "*Drum and bass*", /* andilek.c */ 
    [4264] "*krizek*", /* andilek.c */ 
    [4268] "*RSS*", /* andilek.c */ 
    [4270] "*BlackHawks*", /* andilek.c */ 
    [4276] "*Tuning*", /* andilek.c */ 
    [4279] "*ICS*", /* andilek.c */ 
    [4280] "*Skype*", /* andilek.c */ 
    [4281] "*ICQ*", /* andilek.c */ 
    [4293] "*potlesk*", /* andilek.c */ 
    [4305] "*motyl*", /* andilek.c */ 
    [4309] "*toaletak*", /* andilek.c */ 
    [4310] "*nakupni kosik*", /* andilek.c */ 
    [4312] "*odpocet*", /* andilek.c */ 
    [4315] "*pes*", /* andilek.c */ 
    [4316] "*fax*", /* andilek.c */ 
    [4317] "*jin jang*", /* andilek.c */ 
    [4319] "*P&G*", /* andilek.c */ 
    [4326] "*trznice*", /* andilek.c */ 
    [4332] "*illy cafe*", /* andilek.c */ 
    [4333] "*klasA*", /* andilek.c */ 
    [4334] "*Simpsonovi*", /* andilek.c */ 
    [4336] "*tancici tux*", /* andilek.c */ 
    [4337] "*tux tux*", /* andilek.c */ 
    [4340] "*televize*", /* andilek.c */ 
    [4342] "*BK*", /* andilek.c */ 
    [4343] "*UAX*", /* andilek.c */ 
    [4345] "*ZzZz*", /* andilek.c */ 
    [4346] "*MMNT cigo*", /* andilek.c */ 
    [4347] "*MMNT telefon*", /* andilek.c */ 
    [4356] "*Ubuntu*", /* andilek.c */ 
    [4357] "*toaletak*", /* andilek.c */ 
    [4381] "*slon*", /* andilek.c */ 
    [4385] "*Toyota*", /* andilek.c */ 
    [4399] "*hlina*", /* andilek.c */ 
    [4400] "*drevo*", /* andilek.c */ 
    [4401] "*obili*", /* andilek.c */ 
    [4402] "*zelezo*", /* andilek.c */ 
    [4410] "*mizim*", /* andilek.c */ 
    [4413] "*hej*", /* andilek.c */ 
    [4414] "*baf*", /* andilek.c */ 
    [4425] "*Usti n/L*", /* andilek.c */ 
    [4428] "*ryba :)*", /* andilek.c */ 
    [4429] "*ryba :))*", /* andilek.c */ 
    [4431] "*ryba :D*", /* andilek.c */ 
    [4432] "*ryba 8)*", /* andilek.c */ 
    [4433] "*ryba :P*", /* andilek.c */ 
    [4434] "*ryba ;)*", /* andilek.c */ 
    [4449] "*ruze*", /* andilek.c */ 
    [4459] "*kdo neskace...*", /* andilek.c */ 
    [4463] "*Vitej u nas*", /* andilek.c */ 
    [4464] "*Goool*", /* andilek.c */ 
    [4479] "*VSE*", /* andilek.c */ 
    [4483] "*horici srdce*", /* andilek.c */ 
    [4484] "*xVrchlabi*", /* andilek.c */ 
    [4489] "*noticka*", /* andilek.c */ 
    [4502] "*Staflik*", /* andilek.c */ 
    [4503] "*Spagetka*", /* andilek.c */ 
    [4510] "*postel*", /* andilek.c */ 
    [4516] "*BiteFight*", /* andilek.c */ 
    [4521] "*plavec*", /* andilek.c */ 
    [4533] "*dalnice*", /* andilek.c */ 
    [4535] "*xhodiny*", /* andilek.c */ 
    [4536] "*pyramidy*", /* andilek.c */ 
    [4537] "*teplo*", /* andilek.c */ 
    [4538] "*zima*", /* andilek.c */ 
    [4539] "*prsi*", /* andilek.c */ 
    [4540] "*zatazeno*", /* andilek.c */ 
    [4543] "*tresnicky*", /* andilek.c */ 
    [4544] "*TVR*", /* andilek.c */ 
    [4545] "*xPardubice*", /* andilek.c */ 
    [4546] "*StuntPlayground*", /* andilek.c */ 
    [4549] "*drevo*", /* andilek.c */ 
    [4550] "*hlina*", /* andilek.c */ 
    [4551] "*obili*", /* andilek.c */ 
    [4552] "*zelezo*", /* andilek.c */ 
    [4556] "*Team Galaxy*", /* andilek.c */ 
    [4557] "*Captain Flamingo*", /* andilek.c */ 
    [4558] "*Nascar Racers*", /* andilek.c */ 
    [4559] "*Galactik Football*", /* andilek.c */ 
    [4560] "*Cartoon Network*", /* andilek.c */ 
    [4575] "*pyramida*", /* andilek.c */ 
    [4584] "*datlik*", /* andilek.c */ 
    [4585] "*sovicka*", /* andilek.c */ 
    [4592] "*Big Shock*", /* andilek.c */ 
    [4612] "*Kiss Me*", /* andilek.c */ 
    [4614] "*Irsko*", /* andilek.c */ 
    [4623] "*brecici*", /* andilek.c */ 
    [4639] "*psi voci*", /* andilek.c */ 
    [4641] "*klokan*", /* andilek.c */ 
    [4642] "*zabicka*", /* andilek.c */ 
    [4643] "*zamilovane zabicky*", /* andilek.c */ 
    [4648] "*carodejnice*", /* andilek.c */ 
    [4651] "*cesko-slovensko*", /* andilek.c */ 
    [4659] "*zelena*", /* andilek.c */ 
    [4660] "*vodka*", /* andilek.c */ 
    [4669] "*SuSE*", /* andilek.c */ 
    [4676] "*medvidek*", /* andilek.c */ 
    [4683] "*Jameson*", /* andilek.c */ 
    [4686] "*Fernet Citrus*", /* andilek.c */ 
    [4701] "*fotbalovy mic*", /* andilek.c */ 
    [4712] "*Lidl*", /* andilek.c */ 
    [4714] "*Opava*", /* andilek.c */ 
    [4727] "*telefon*", /* andilek.c */ 
    [4728] "*kliky*", /* andilek.c */ 
    [4729] "*srdce*", /* andilek.c */ 
    [4736] "*delfin*", /* andilek.c */ 
    [4754] "*gay narozeniny*", /* andilek.c */ 
    [4755] "*punk!*", /* andilek.c */ 
    [4771] "*kun*", /* andilek.c */ 
    [4790] "*Ragnarök*", /* andilek.c */ 
    [4791] "*Rock & Roll*", /* andilek.c */ 
    [4792] "*Merida*", /* andilek.c */ 
    [4793] "*nuda*", /* andilek.c */ 
    [4806] "*Zubr*", /* andilek.c */ 
    [4807] "*motyl*", /* andilek.c */ 
    [4810] "*motylek*", /* andilek.c */ 
    [4813] "*citron*", /* andilek.c */ 
    [4814] "*Takt LBC*", /* andilek.c */ 
    [4815] "*citron*", /* andilek.c */ 
    [4816] "*citron*", /* andilek.c */ 
    [4817] "*pomeranc*", /* andilek.c */ 
    [4819] "*Alfa Romeo*", /* andilek.c */ 
    [4821] "*Bye*", /* andilek.c */ 
    [4822] "*Ha ha ha*", /* andilek.c */ 
    [4823] "*Flirt*", /* andilek.c */ 
    [4824] "*Kiss*", /* andilek.c */ 
    [4825] "*Hey!*", /* andilek.c */ 
    [4830] "*Alfa Romeo*", /* andilek.c */ 
    [4831] "*Twix*", /* andilek.c */ 
    [4832] "*bastard*", /* andilek.c */ 
    [4846] "*ruzova smrtka*", /* andilek.c */ 
    [4853] "*Australie*", /* andilek.c */ 
    [4854] "*Evropa2*", /* onovy */ 
    [4856] "*ILoveYou*", /* onovy */ 
    [4859] "*exit*", /* onovy */ 
    [4879] "*RAC*", /* andilek.c */ 
    [4889] "*?*", /* andilek.c */ 
    [4892] "*bla bla*", /* andilek.c */ 
    [4893] "*darecek*", /* andilek.c */ 
    [4895] "*fotograf*", /* andilek.c */ 
};

/**
 * \brief Number of smiles in the #smiles table.
 */
const int smiles_count = sizeof(smiles)/sizeof(*smiles);
