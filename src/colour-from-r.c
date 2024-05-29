


#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>


// rgb(t(col2rgb(colours()[1:10])), maxColorValue = 255)

// Modified djb2 hash
 int djb2_hash(uint8_t *str) {
     int hash = 5381;
    int c;

    while ((c = *str++))
      hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}


SEXP djb2_hash_(SEXP str_) {
  return ScalarInteger( djb2_hash((uint8_t *)CHAR(asChar(str_))) );
}


int rcolour_to_int(const char *rcol) {
  int hash = djb2_hash((uint8_t *)rcol);
  long res = 0;

  switch(hash) {
  case 279132550:
    res = -1;
    break;
  case 1106644811:
    res = -1808;
    break;
  case 976036093:
    res = -2626566;
    break;
  case 2144420046:
    res = -2363393;
    break;
  case 2144420047:
    res = -3350546;
    break;
  case 2144420048:
    res = -5193523;
    break;
  case 2144420049:
    res = -8879221;
    break;
  case 1390539785:
    res = -2818177;
    break;
  case -1356827302:
    res = -2818177;
    break;
  case -1356827301:
    res = -3740042;
    break;
  case -1356827300:
    res = -5583514;
    break;
  case -1356827299:
    res = -9139387;
    break;
  case 253702156:
    res = -16;
    break;
  case -217763395:
    res = -16;
    break;
  case -217763394:
    res = -1118496;
    break;
  case -217763393:
    res = -3289663;
    break;
  case -217763392:
    res = -7631997;
    break;
  case 254119969:
    res = -2296331;
    break;
  case -198861042:
    res = -3873537;
    break;
  case 2027520255:
    res = -3873537;
    break;
  case 2027520256:
    res = -4729362;
    break;
  case 2027520257:
    res = -6375475;
    break;
  case 2027520258:
    res = -9732725;
    break;
  case 254362690:
    res = -16777216;
    break;
  case 631338833:
    res = -3281921;
    break;
  case 2090117005:
    res = -65536;
    break;
  case 254384478:
    res = -65536;
    break;
  case 254384479:
    res = -1179648;
    break;
  case 254384480:
    res = -3342336;
    break;
  case 254384481:
    res = -7667712;
    break;
  case 1509219168:
    res = -1954934;
    break;
  case 254594221:
    res = -14013787;
    break;
  case -188325250:
    res = -12566273;
    break;
  case -188325249:
    res = -12895250;
    break;
  case -188325248:
    res = -13421619;
    break;
  case -188325247:
    res = -14474357;
    break;
  case -805108052:
    res = -7882530;
    break;
  case -798761891:
    res = -6564865;
    break;
  case -798761890:
    res = -7223826;
    break;
  case -798761889:
    res = -8541491;
    break;
  case -798761888:
    res = -11177077;
    break;
  case -1997448018:
    res = -6250913;
    break;
  case -1491275105:
    res = -2664;
    break;
  case -1491275104:
    res = -1120882;
    break;
  case -1491275103:
    res = -3291782;
    break;
  case -1491275102:
    res = -7633325;
    break;
  case -74361797:
    res = -16711809;
    break;
  case 1841028044:
    res = -16711809;
    break;
  case 1841028045:
    res = -16716170;
    break;
  case 1841028046:
    res = -16724634;
    break;
  case 1841028047:
    res = -16741563;
    break;
  case 1216302135:
    res = -14784046;
    break;
  case 1483264840:
    res = -14385153;
    break;
  case 1483264841:
    res = -14584082;
    break;
  case 1483264842:
    res = -14850355;
    break;
  case 1483264843:
    res = -15514229;
    break;
  case 255674870:
    res = -11501569;
    break;
  case -152663833:
    res = -11111681;
    break;
  case -152663832:
    res = -11506962;
    break;
  case -152663831:
    res = -12231731;
    break;
  case -152663830:
    res = -13681013;
    break;
  case 1746496750:
    res = -1206940;
    break;
  case 1268543594:
    res = -2295553;
    break;
  case -1087734309:
    res = -2295553;
    break;
  case -1087734308:
    res = -3282706;
    break;
  case -1087734307:
    res = -5125939;
    break;
  case -1087734306:
    res = -8877941;
    break;
  case 2090166448:
    res = -256;
    break;
  case 256016097:
    res = -256;
    break;
  case 256016098:
    res = -1118720;
    break;
  case 256016099:
    res = -3289856;
    break;
  case 256016100:
    res = -7632128;
    break;
  case 32466095:
    res = -7667712;
    break;
  case 32515538:
    res = -7632128;
    break;
  case -958856571:
    res = -16021832;
    break;
  case -1577495722:
    res = -15746561;
    break;
  case -1577495721:
    res = -15815186;
    break;
  case -1577495720:
    res = -15952435;
    break;
  case -1577495719:
    res = -16226933;
    break;
  case 32651674:
    res = -5658199;
    break;
  case 1077509048:
    res = -16751616;
    break;
  case 32651806:
    res = -5658199;
    break;
  case 1081889199:
    res = -9717827;
    break;
  case -622771228:
    res = -7667573;
    break;
  case 153909911:
    res = -13669547;
    break;
  case 784059816:
    res = -9371702;
    break;
  case 784059817:
    res = -9900356;
    break;
  case 784059818:
    res = -10826334;
    break;
  case 784059819:
    res = -12743826;
    break;
  case 1511009283:
    res = -16741121;
    break;
  case -1676301164:
    res = -16744449;
    break;
  case -1676301163:
    res = -16746770;
    break;
  case -1676301162:
    res = -16750899;
    break;
  case -1676301161:
    res = -16759413;
    break;
  case 1511074688:
    res = -3394919;
    break;
  case -1674142799:
    res = -49473;
    break;
  case -1674142798:
    res = -1164622;
    break;
  case -1674142797:
    res = -3329382;
    break;
  case -1674142796:
    res = -7658904;
    break;
  case 1000994:
    res = -16777077;
    break;
  case 1647784689:
    res = -8743191;
    break;
  case -425402223:
    res = -7357297;
    break;
  case -1153371422:
    res = -4063295;
    break;
  case -1153371421:
    res = -4919628;
    break;
  case -1153371420:
    res = -6566501;
    break;
  case -1153371419:
    res = -9860247;
    break;
  case 1316229224:
    res = -7652024;
    break;
  case 1316414803:
    res = -11579601;
    break;
  case 492015588:
    res = -105;
    break;
  case 492015589:
    res = -1118579;
    break;
  case 492015590:
    res = -3289735;
    break;
  case 492015591:
    res = -7632046;
    break;
  case 1316414935:
    res = -11579601;
    break;
  case 671623224:
    res = -3027456;
    break;
  case 1774784634:
    res = -2948972;
    break;
  case 401039797:
    res = -7138049;
    break;
  case 349411462:
    res = -7138049;
    break;
  case 349411463:
    res = -7793938;
    break;
  case 349411464:
    res = -9039667;
    break;
  case 349411465:
    res = -11531637;
    break;
  case -2072509918:
    res = -16640;
    break;
  case 326649491:
    res = -16640;
    break;
  case 326649492:
    res = -1134080;
    break;
  case 326649493:
    res = -3302912;
    break;
  case 326649494:
    res = -7641088;
    break;
  case 308010674:
    res = -9868951;
    break;
  case 308010806:
    res = -9868951;
    break;
  case -2113359326:
    res = -28642;
    break;
  case -1021380973:
    res = -28642;
    break;
  case -1021380972:
    res = -1145316;
    break;
  case -1021380971:
    res = -3312616;
    break;
  case -1021380970:
    res = -7647728;
    break;
  case 2095556406:
    res = -14540110;
    break;
  case 433884711:
    res = -13618945;
    break;
  case 433884712:
    res = -13882130;
    break;
  case 433884713:
    res = -14276915;
    break;
  case 433884714:
    res = -15066485;
    break;
  case -1451792442:
    res = -984321;
    break;
  case -1898313175:
    res = -14513374;
    break;
  case -256724471:
    res = -2302756;
    break;
  case 1152083595:
    res = -1800;
    break;
  case 2090299659:
    res = -16721921;
    break;
  case 260412060:
    res = -16721921;
    break;
  case 260412061:
    res = -16725522;
    break;
  case 260412062:
    res = -16732723;
    break;
  case 260412063:
    res = -16747125;
    break;
  case -1427062749:
    res = -14637606;
    break;
  case 151569588:
    res = -14302721;
    break;
  case 151569589:
    res = -14502674;
    break;
  case 151569590:
    res = -14836787;
    break;
  case 151569591:
    res = -15439477;
    break;
  case 2090302584:
    res = -4276546;
    break;
  case 260508584:
    res = -16777216;
    break;
  case 260508585:
    res = -16579837;
    break;
  case 260508586:
    res = -16448251;
    break;
  case 260508587:
    res = -16250872;
    break;
  case 260508588:
    res = -16119286;
    break;
  case 260508589:
    res = -15921907;
    break;
  case 260508590:
    res = -15790321;
    break;
  case 260508591:
    res = -15592942;
    break;
  case 260508592:
    res = -15461356;
    break;
  case 260508593:
    res = -15263977;
    break;
  case 6848761:
    res = -15066598;
    break;
  case 6848762:
    res = -14935012;
    break;
  case 6848763:
    res = -14737633;
    break;
  case 6848764:
    res = -14606047;
    break;
  case 6848765:
    res = -14408668;
    break;
  case 6848766:
    res = -14277082;
    break;
  case 6848767:
    res = -14079703;
    break;
  case 6848768:
    res = -13948117;
    break;
  case 6848769:
    res = -13750738;
    break;
  case 6848770:
    res = -13619152;
    break;
  case 6848794:
    res = -13421773;
    break;
  case 6848795:
    res = -13224394;
    break;
  case 6848796:
    res = -13092808;
    break;
  case 6848797:
    res = -12895429;
    break;
  case 6848798:
    res = -12763843;
    break;
  case 6848799:
    res = -12566464;
    break;
  case 6848800:
    res = -12434878;
    break;
  case 6848801:
    res = -12237499;
    break;
  case 6848802:
    res = -12105913;
    break;
  case 6848803:
    res = -11908534;
    break;
  case 6848827:
    res = -11711155;
    break;
  case 6848828:
    res = -11579569;
    break;
  case 6848829:
    res = -11382190;
    break;
  case 6848830:
    res = -11250604;
    break;
  case 6848831:
    res = -11053225;
    break;
  case 6848832:
    res = -10921639;
    break;
  case 6848833:
    res = -10724260;
    break;
  case 6848834:
    res = -10592674;
    break;
  case 6848835:
    res = -10395295;
    break;
  case 6848836:
    res = -10263709;
    break;
  case 6848860:
    res = -10066330;
    break;
  case 6848861:
    res = -9868951;
    break;
  case 6848862:
    res = -9737365;
    break;
  case 6848863:
    res = -9539986;
    break;
  case 6848864:
    res = -9408400;
    break;
  case 6848865:
    res = -9211021;
    break;
  case 6848866:
    res = -9079435;
    break;
  case 6848867:
    res = -8882056;
    break;
  case 6848868:
    res = -8750470;
    break;
  case 6848869:
    res = -8553091;
    break;
  case 6848893:
    res = -8421505;
    break;
  case 6848894:
    res = -8224126;
    break;
  case 6848895:
    res = -8026747;
    break;
  case 6848896:
    res = -7895161;
    break;
  case 6848897:
    res = -7697782;
    break;
  case 6848898:
    res = -7566196;
    break;
  case 6848899:
    res = -7368817;
    break;
  case 6848900:
    res = -7237231;
    break;
  case 6848901:
    res = -7039852;
    break;
  case 6848902:
    res = -6908266;
    break;
  case 6848926:
    res = -6710887;
    break;
  case 6848927:
    res = -6513508;
    break;
  case 6848928:
    res = -6381922;
    break;
  case 6848929:
    res = -6184543;
    break;
  case 6848930:
    res = -6052957;
    break;
  case 6848931:
    res = -5855578;
    break;
  case 6848932:
    res = -5723992;
    break;
  case 6848933:
    res = -5526613;
    break;
  case 6848934:
    res = -5395027;
    break;
  case 6848935:
    res = -5197648;
    break;
  case 6848959:
    res = -5000269;
    break;
  case 6848960:
    res = -4868683;
    break;
  case 6848961:
    res = -4671304;
    break;
  case 6848962:
    res = -4539718;
    break;
  case 6848963:
    res = -4342339;
    break;
  case 6848964:
    res = -4210753;
    break;
  case 6848965:
    res = -4013374;
    break;
  case 6848966:
    res = -3881788;
    break;
  case 6848967:
    res = -3684409;
    break;
  case 6848968:
    res = -3552823;
    break;
  case 6848992:
    res = -3355444;
    break;
  case 6848993:
    res = -3158065;
    break;
  case 6848994:
    res = -3026479;
    break;
  case 6848995:
    res = -2829100;
    break;
  case 6848996:
    res = -2697514;
    break;
  case 6848997:
    res = -2500135;
    break;
  case 6848998:
    res = -2368549;
    break;
  case 6848999:
    res = -2171170;
    break;
  case 6849000:
    res = -2039584;
    break;
  case 6849001:
    res = -1842205;
    break;
  case 6849025:
    res = -1710619;
    break;
  case 6849026:
    res = -1513240;
    break;
  case 6849027:
    res = -1315861;
    break;
  case 6849028:
    res = -1184275;
    break;
  case 6849029:
    res = -986896;
    break;
  case 6849030:
    res = -855310;
    break;
  case 6849031:
    res = -657931;
    break;
  case 6849032:
    res = -526345;
    break;
  case 6849033:
    res = -328966;
    break;
  case 6849034:
    res = -197380;
    break;
  case 226009161:
    res = -1;
    break;
  case 260512342:
    res = -16711936;
    break;
  case 6972743:
    res = -16711936;
    break;
  case 6972744:
    res = -16716288;
    break;
  case 6972745:
    res = -16724736;
    break;
  case 6972746:
    res = -16741632;
    break;
  case -1062996750:
    res = -13631571;
    break;
  case 2090302716:
    res = -4276546;
    break;
  case 260512940:
    res = -16777216;
    break;
  case 260512941:
    res = -16579837;
    break;
  case 260512942:
    res = -16448251;
    break;
  case 260512943:
    res = -16250872;
    break;
  case 260512944:
    res = -16119286;
    break;
  case 260512945:
    res = -15921907;
    break;
  case 260512946:
    res = -15790321;
    break;
  case 260512947:
    res = -15592942;
    break;
  case 260512948:
    res = -15461356;
    break;
  case 260512949:
    res = -15263977;
    break;
  case 6992509:
    res = -15066598;
    break;
  case 6992510:
    res = -14935012;
    break;
  case 6992511:
    res = -14737633;
    break;
  case 6992512:
    res = -14606047;
    break;
  case 6992513:
    res = -14408668;
    break;
  case 6992514:
    res = -14277082;
    break;
  case 6992515:
    res = -14079703;
    break;
  case 6992516:
    res = -13948117;
    break;
  case 6992517:
    res = -13750738;
    break;
  case 6992518:
    res = -13619152;
    break;
  case 6992542:
    res = -13421773;
    break;
  case 6992543:
    res = -13224394;
    break;
  case 6992544:
    res = -13092808;
    break;
  case 6992545:
    res = -12895429;
    break;
  case 6992546:
    res = -12763843;
    break;
  case 6992547:
    res = -12566464;
    break;
  case 6992548:
    res = -12434878;
    break;
  case 6992549:
    res = -12237499;
    break;
  case 6992550:
    res = -12105913;
    break;
  case 6992551:
    res = -11908534;
    break;
  case 6992575:
    res = -11711155;
    break;
  case 6992576:
    res = -11579569;
    break;
  case 6992577:
    res = -11382190;
    break;
  case 6992578:
    res = -11250604;
    break;
  case 6992579:
    res = -11053225;
    break;
  case 6992580:
    res = -10921639;
    break;
  case 6992581:
    res = -10724260;
    break;
  case 6992582:
    res = -10592674;
    break;
  case 6992583:
    res = -10395295;
    break;
  case 6992584:
    res = -10263709;
    break;
  case 6992608:
    res = -10066330;
    break;
  case 6992609:
    res = -9868951;
    break;
  case 6992610:
    res = -9737365;
    break;
  case 6992611:
    res = -9539986;
    break;
  case 6992612:
    res = -9408400;
    break;
  case 6992613:
    res = -9211021;
    break;
  case 6992614:
    res = -9079435;
    break;
  case 6992615:
    res = -8882056;
    break;
  case 6992616:
    res = -8750470;
    break;
  case 6992617:
    res = -8553091;
    break;
  case 6992641:
    res = -8421505;
    break;
  case 6992642:
    res = -8224126;
    break;
  case 6992643:
    res = -8026747;
    break;
  case 6992644:
    res = -7895161;
    break;
  case 6992645:
    res = -7697782;
    break;
  case 6992646:
    res = -7566196;
    break;
  case 6992647:
    res = -7368817;
    break;
  case 6992648:
    res = -7237231;
    break;
  case 6992649:
    res = -7039852;
    break;
  case 6992650:
    res = -6908266;
    break;
  case 6992674:
    res = -6710887;
    break;
  case 6992675:
    res = -6513508;
    break;
  case 6992676:
    res = -6381922;
    break;
  case 6992677:
    res = -6184543;
    break;
  case 6992678:
    res = -6052957;
    break;
  case 6992679:
    res = -5855578;
    break;
  case 6992680:
    res = -5723992;
    break;
  case 6992681:
    res = -5526613;
    break;
  case 6992682:
    res = -5395027;
    break;
  case 6992683:
    res = -5197648;
    break;
  case 6992707:
    res = -5000269;
    break;
  case 6992708:
    res = -4868683;
    break;
  case 6992709:
    res = -4671304;
    break;
  case 6992710:
    res = -4539718;
    break;
  case 6992711:
    res = -4342339;
    break;
  case 6992712:
    res = -4210753;
    break;
  case 6992713:
    res = -4013374;
    break;
  case 6992714:
    res = -3881788;
    break;
  case 6992715:
    res = -3684409;
    break;
  case 6992716:
    res = -3552823;
    break;
  case 6992740:
    res = -3355444;
    break;
  case 6992741:
    res = -3158065;
    break;
  case 6992742:
    res = -3026479;
    break;
  case 6992743:
    res = -2829100;
    break;
  case 6992744:
    res = -2697514;
    break;
  case 6992745:
    res = -2500135;
    break;
  case 6992746:
    res = -2368549;
    break;
  case 6992747:
    res = -2171170;
    break;
  case 6992748:
    res = -2039584;
    break;
  case 6992749:
    res = -1842205;
    break;
  case 6992773:
    res = -1710619;
    break;
  case 6992774:
    res = -1513240;
    break;
  case 6992775:
    res = -1315861;
    break;
  case 6992776:
    res = -1184275;
    break;
  case 6992777:
    res = -986896;
    break;
  case 6992778:
    res = -855310;
    break;
  case 6992779:
    res = -657931;
    break;
  case 6992780:
    res = -526345;
    break;
  case 6992781:
    res = -328966;
    break;
  case 6992782:
    res = -197380;
    break;
  case 230752845:
    res = -1;
    break;
  case -554611224:
    res = -983056;
    break;
  case -1122301159:
    res = -983056;
    break;
  case -1122301158:
    res = -2036000;
    break;
  case -1122301157:
    res = -4076095;
    break;
  case -1122301156:
    res = -8156285;
    break;
  case 1422343106:
    res = -4953601;
    break;
  case -307317709:
    res = -4952321;
    break;
  case -307317708:
    res = -5805330;
    break;
  case -307317707:
    res = -7315251;
    break;
  case -307317706:
    res = -10339701;
    break;
  case 1261915923:
    res = -10724147;
    break;
  case -1306447452:
    res = -9803009;
    break;
  case -1306447451:
    res = -10263570;
    break;
  case -1306447450:
    res = -11184691;
    break;
  case -1306447449:
    res = -12961141;
    break;
  case 263039262:
    res = -983041;
    break;
  case 90361103:
    res = -983041;
    break;
  case 90361104:
    res = -2035986;
    break;
  case 90361105:
    res = -4076083;
    break;
  case 90361106:
    res = -8156277;
    break;
  case 264892493:
    res = -7543056;
    break;
  case 151517726:
    res = -7342337;
    break;
  case 151517727:
    res = -8001810;
    break;
  case 151517728:
    res = -9189683;
    break;
  case 151517729:
    res = -11630965;
    break;
  case 1827441590:
    res = -334106;
    break;
  case 1295050932:
    res = -659201;
    break;
  case -212992155:
    res = -659201;
    break;
  case -212992154:
    res = -1711890;
    break;
  case -212992153:
    res = -3817011;
    break;
  case -212992152:
    res = -7961717;
    break;
  case 1811918184:
    res = -16712580;
    break;
  case 446933885:
    res = -3278081;
    break;
  case 1863916366:
    res = -3278081;
    break;
  case 1863916367:
    res = -4199954;
    break;
  case 1863916368:
    res = -5912115;
    break;
  case 1863916369:
    res = -9401973;
    break;
  case -246534939:
    res = -1648467;
    break;
  case 454281654:
    res = -4161;
    break;
  case 454281655:
    res = -1122382;
    break;
  case 454281656:
    res = -3293030;
    break;
  case 454281657:
    res = -7634072;
    break;
  case 455572046:
    res = -8355600;
    break;
  case -246485496:
    res = -32;
    break;
  case 455913273:
    res = -32;
    break;
  case 455913274:
    res = -1118511;
    break;
  case 455913275:
    res = -3289676;
    break;
  case 455913276:
    res = -7632006;
    break;
  case 111893627:
    res = -8200722;
    break;
  case -602477556:
    res = -7607041;
    break;
  case -602477555:
    res = -8200978;
    break;
  case -602477554:
    res = -9388339;
    break;
  case -602477553:
    res = -11763317;
    break;
  case 647790295:
    res = -2950406;
    break;
  case -246349360:
    res = -2894893;
    break;
  case 460409518:
    res = -7278960;
    break;
  case -246349228:
    res = -2894893;
    break;
  case -246035313:
    res = -4081921;
    break;
  case 470769312:
    res = -4608257;
    break;
  case 470769313:
    res = -5397778;
    break;
  case 470769314:
    res = -6976307;
    break;
  case 470769315:
    res = -10133621;
    break;
  case -1536630617:
    res = -8740609;
    break;
  case 830797240:
    res = -8740609;
    break;
  case 830797241:
    res = -9267730;
    break;
  case 830797242:
    res = -10321459;
    break;
  case 830797243:
    res = -12429429;
    break;
  case 2079904711:
    res = -5590496;
    break;
  case 1237169852:
    res = -340345;
    break;
  case -2123067795:
    res = -7504;
    break;
  case -2123067794:
    res = -1125468;
    break;
  case -2123067793:
    res = -3295603;
    break;
  case -2123067792:
    res = -7636128;
    break;
  case -1907987874:
    res = -36732;
    break;
  case -1907802295:
    res = -6715273;
    break;
  case -1907802163:
    res = -6715273;
    break;
  case 29314690:
    res = -2177872;
    break;
  case 967384819:
    res = -7734;
    break;
  case 967384820:
    res = -1125700;
    break;
  case 967384821:
    res = -3295838;
    break;
  case 967384822:
    res = -7636114;
    break;
  case -1297075655:
    res = -2031617;
    break;
  case 146176394:
    res = -2031617;
    break;
  case 146176395:
    res = -3019026;
    break;
  case 146176396:
    res = -4928051;
    break;
  case 146176397:
    res = -8746101;
    break;
  case -1219918019:
    res = -13447886;
    break;
  case 266128315:
    res = -1642246;
    break;
  case -1273953790:
    res = -65281;
    break;
  case 909197939:
    res = -65281;
    break;
  case 909197940:
    res = -1179410;
    break;
  case 909197941:
    res = -3342131;
    break;
  case 909197942:
    res = -7667573;
    break;
  case 222102609:
    res = -10473296;
    break;
  case -1260548446:
    res = -5032705;
    break;
  case -1260548445:
    res = -5820178;
    break;
  case -1260548444:
    res = -7329331;
    break;
  case -1260548443:
    res = -10347381;
    break;
  case 1402160266:
    res = -5583514;
    break;
  case -663229618:
    res = -3342336;
    break;
  case -187312673:
    res = -2927174;
    break;
  case -1886350864:
    res = -39200;
    break;
  case -1886350863:
    res = -1155119;
    break;
  case -1886350862:
    res = -3321164;
    break;
  case -1886350861:
    res = -7653510;
    break;
  case -144071650:
    res = -2396013;
    break;
  case -459397105:
    res = -32085;
    break;
  case -459397104:
    res = -1148513;
    break;
  case -459397103:
    res = -3315575;
    break;
  case -459397102:
    res = -7649443;
    break;
  case 1161666224:
    res = -9325764;
    break;
  case -2145086873:
    res = -1152901;
    break;
  case -2126101558:
    res = -6620672;
    break;
  case 1505274423:
    res = -3354296;
    break;
  case 1004825236:
    res = -8055353;
    break;
  case 1603474337:
    res = -9430759;
    break;
  case -1686817819:
    res = -327691;
    break;
  case 501656468:
    res = -1972993;
    break;
  case -625205691:
    res = -1972993;
    break;
  case -625205690:
    res = -2959890;
    break;
  case -625205689:
    res = -4868147;
    break;
  case -625205688:
    res = -8684149;
    break;
  case 1650498962:
    res = -4856577;
    break;
  case -490969563:
    res = -5382401;
    break;
  case 977873654:
    res = -5382401;
    break;
  case 977873655:
    res = -6172690;
    break;
  case 977873656:
    res = -7621683;
    break;
  case 977873657:
    res = -10585717;
    break;
  case 2090536323:
    res = -8388608;
    break;
  case 1188278027:
    res = -8388608;
    break;
  case 1736150553:
    res = -1640963;
    break;
  case -854101763:
    res = -14446997;
    break;
  case 1879412942:
    res = -12648512;
    break;
  case 1879412943:
    res = -12915021;
    break;
  case 1879412944:
    res = -13447782;
    break;
  case 1879412945:
    res = -14513303;
    break;
  case 319921761:
    res = -16734721;
    break;
  case 1967483570:
    res = -16734721;
    break;
  case 1967483571:
    res = -16737554;
    break;
  case 1967483572:
    res = -16742963;
    break;
  case 1967483573:
    res = -16754037;
    break;
  case -598998756:
    res = -16759297;
    break;
  case 1707877581:
    res = -16759297;
    break;
  case 1707877582:
    res = -16760594;
    break;
  case 1707877583:
    res = -16762931;
    break;
  case 1707877584:
    res = -16767605;
    break;
  case 319987166:
    res = -2723622;
    break;
  case 1969641935:
    res = -359425;
    break;
  case 1969641936:
    res = -1475858;
    break;
  case 1969641937:
    res = -3577395;
    break;
  case 1969641938:
    res = -7780469;
    break;
  case 1189203525:
    res = -5576466;
    break;
  case -634230920:
    res = -6751336;
    break;
  case 545216169:
    res = -6619238;
    break;
  case 545216170:
    res = -7278960;
    break;
  case 545216171:
    res = -8598148;
    break;
  case 545216172:
    res = -11236524;
    break;
  case -1475283976:
    res = -1118545;
    break;
  case -1439730903:
    res = -69;
    break;
  case -1439730902:
    res = -1118546;
    break;
  case -1439730901:
    res = -3289706;
    break;
  case -1439730900:
    res = -7632026;
    break;
  case -1975733163:
    res = -7114533;
    break;
  case -774684890:
    res = -5537025;
    break;
  case -774684889:
    res = -6325778;
    break;
  case -774684888:
    res = -7771955;
    break;
  case -774684887:
    res = -10664053;
    break;
  case -965681895:
    res = -2756609;
    break;
  case 937431383:
    res = -4596993;
    break;
  case 870464616:
    res = -4596993;
    break;
  case 870464617:
    res = -5387282;
    break;
  case 870464618:
    res = -6967347;
    break;
  case 870464619:
    res = -10127477;
    break;
  case 2090612417:
    res = -12614195;
    break;
  case 2090616631:
    res = -3424001;
    break;
  case 270872136:
    res = -3820033;
    break;
  case 270872137:
    res = -4675090;
    break;
  case 270872138:
    res = -6385203;
    break;
  case 270872139:
    res = -9673845;
    break;
  case 2090620131:
    res = -2252579;
    break;
  case 270987636:
    res = -17409;
    break;
  case 270987637:
    res = -1134866;
    break;
  case 270987638:
    res = -3303731;
    break;
  case 270987639:
    res = -7641461;
    break;
  case 1417385502:
    res = -1646416;
    break;
  case 363228189:
    res = -1040224;
    break;
  case -898371602:
    res = -53093;
    break;
  case -898371601:
    res = -1168239;
    break;
  case -898371600:
    res = -3332483;
    break;
  case -898371599:
    res = -7660971;
    break;
  case 193504576:
    res = -16776961;
    break;
  case 2090683761:
    res = -16776961;
    break;
  case 2090683762:
    res = -16776978;
    break;
  case 2090683763:
    res = -16777011;
    break;
  case 2090683764:
    res = -16777077;
    break;
  case -135694854:
    res = -7368772;
    break;
  case -182962837:
    res = -4079105;
    break;
  case -182962836:
    res = -4934418;
    break;
  case -182962835:
    res = -6579251;
    break;
  case -182962834:
    res = -9868917;
    break;
  case -1904790188:
    res = -2004671;
    break;
  case 1566433285:
    res = -35256;
    break;
  case 1566433286:
    res = -1151421;
    break;
  case 1566433287:
    res = -3317958;
    break;
  case 1566433288:
    res = -7651289;
    break;
  case -1833176230:
    res = -15514229;
    break;
  case 456697167:
    res = -9273094;
    break;
  case -2108862624:
    res = -9859841;
    break;
  case -2108862623:
    res = -10321170;
    break;
  case -2108862622:
    res = -11243315;
    break;
  case -2108862621:
    res = -13022069;
    break;
  case -519366868:
    res = -10443532;
    break;
  case -439590289:
    res = -11039954;
    break;
  case -1621577600:
    res = -6291628;
    break;
  case -1621577599:
    res = -7016882;
    break;
  case -1621577598:
    res = -8336061;
    break;
  case -1621577597:
    res = -11039954;
    break;
  case -425718378:
    res = -1116673;
    break;
  case -1163804537:
    res = -1116673;
    break;
  case -1163804536:
    res = -2169362;
    break;
  case -1163804535:
    res = -4209203;
    break;
  case -1163804534:
    res = -8223093;
    break;
  case 465934019:
    res = -13806944;
    break;
  case -1804046508:
    res = -12090625;
    break;
  case -1804046507:
    res = -12420626;
    break;
  case -1804046506:
    res = -13014835;
    break;
  case -1804046505:
    res = -14268533;
    break;
  case -1702490012:
    res = -1323385;
    break;
  case -347595499:
    res = -12665;
    break;
  case -347595498:
    res = -1130370;
    break;
  case -347595497:
    res = -3299732;
    break;
  case -347595496:
    res = -7638966;
    break;
  case 848023046:
    res = -3319190;
    break;
  case -2080010505:
    res = -36989;
    break;
  case -2080010504:
    res = -1153158;
    break;
  case -2080010503:
    res = -3319447;
    break;
  case -2080010502:
    res = -7652281;
    break;
  case 848208625:
    res = -7307152;
    break;
  case -2073886398:
    res = -7482;
    break;
  case -2073886397:
    res = -1125447;
    break;
  case -2073886396:
    res = -3295585;
    break;
  case -2073886395:
    res = -7636116;
    break;
  case 848208757:
    res = -7307152;
    break;
  case 2090729932:
    res = -328961;
    break;
  case 274611069:
    res = -328961;
    break;
  case 274611070:
    res = -1447442;
    break;
  case 274611071:
    res = -3552819;
    break;
  case 274611072:
    res = -7763573;
    break;
  case 1785389865:
    res = -8388864;
    break;
  case -1211676550:
    res = -8388864;
    break;
  case -1211676549:
    res = -8983040;
    break;
  case -1211676548:
    res = -10040064;
    break;
  case -1211676547:
    res = -12219648;
    break;
  case -1509641686:
    res = -4947386;
    break;
  case 1721431963:
    res = -18333;
    break;
  case 1721431964:
    res = -1135524;
    break;
  case 1721431965:
    res = -3304369;
    break;
  case 1721431966:
    res = -7642058;
    break;
  case 193506632:
    res = -7555886;
    break;
  case 2090751609:
    res = -11557377;
    break;
  case 2090751610:
    res = -11953426;
    break;
  case 2090751611:
    res = -12614195;
    break;
  case 2090751612:
    res = -13935989;
    break;
  case -546783614:
    res = -2572328;
    break;
  case -863990029:
    res = -7681;
    break;
  case -863990028:
    res = -1125650;
    break;
  case -863990027:
    res = -3295795;
    break;
  case -863990026:
    res = -7636085;
    break;
  case 512458489:
    res = -12098561;
    break;
  case -268738998:
    res = -12098561;
    break;
  case -268738997:
    res = -12428050;
    break;
  case -268738996:
    res = -13021235;
    break;
  case -268738995:
    res = -14272885;
    break;
  case 203417046:
    res = -3088320;
    break;
  case -1877172025:
    res = -2816;
    break;
  case -1877172024:
    res = -1121024;
    break;
  case -1877172023:
    res = -3291904;
    break;
  case -1877172022:
    res = -7633408;
    break;
  case 583697112:
    res = -1146130;
    break;
  case -297032141:
    res = -7331632;
    break;
  case -1212126012:
    res = -6930689;
    break;
  case -1212126011:
    res = -7587090;
    break;
  case -1212126010:
    res = -8899891;
    break;
  case -1212126009:
    res = -11394421;
    break;
  case 279127582:
    res = -4989195;
    break;
  case 621275663:
    res = -4528129;
    break;
  case 621275664:
    res = -5318418;
    break;
  case 621275665:
    res = -6898995;
    break;
  case 621275666:
    res = -10060149;
    break;
  case 629197541:
    res = -657931;
    break;
  case 696252129:
    res = -16711681;
    break;
  case 1501483826:
    res = -16711681;
    break;
  case 1501483827:
    res = -16716050;
    break;
  case 1501483828:
    res = -16724531;
    break;
  case 1501483829:
    res = -16741493;
    break;
  case -632650318:
    res = -13447782;
    break;

  default:
    Rprintf("Unknown colour: %s\n", rcol);
    res = -1; // white
  }
  return res;
}
