


#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include "colour-from-r.h"

// rgb(t(col2rgb(colours()[1:10])), maxColorValue = 255)

// Modified djb2 hash
 int djb2_hash(uint8_t *str) {
    unsigned int hash = 5381;
    int c;

    while ((c = *str++))
      hash = ((hash << 5) + hash) + (unsigned int)c; /* hash * 33 + c */

    return (int)hash;
}


SEXP djb2_hash_(SEXP str_) {
  return ScalarInteger( djb2_hash((uint8_t *)CHAR(asChar(str_))) );
}


uint32_t rcolour_to_int(const char *rcol) {
  int hash = djb2_hash((uint8_t *)rcol);
  uint32_t res = 0;

  // Result is in 0xAABBGGRR 
  switch(hash) {
  case 5862548:
    res = 0x00FFFFFF; // NA
    break;
  case 1851223447:
    res = 0x00FFFFFF; // transparent
    break;
  case 279132550:
    res = 0xFFFFFFFF; // white
    break;
  case 1106644811:
    res = 0xFFFFF8F0; // aliceblue
    break;
  case 976036093:
    res = 0xFFD7EBFA; // antiquewhite
    break;
  case 2144420046:
    res = 0xFFDBEFFF; // antiquewhite1
    break;
  case 2144420047:
    res = 0xFFCCDFEE; // antiquewhite2
    break;
  case 2144420048:
    res = 0xFFB0C0CD; // antiquewhite3
    break;
  case 2144420049:
    res = 0xFF78838B; // antiquewhite4
    break;
  case 1390539785:
    res = 0xFFD4FF7F; // aquamarine
    break;
  case -1356827302:
    res = 0xFFD4FF7F; // aquamarine1
    break;
  case -1356827301:
    res = 0xFFC6EE76; // aquamarine2
    break;
  case -1356827300:
    res = 0xFFAACD66; // aquamarine3
    break;
  case -1356827299:
    res = 0xFF748B45; // aquamarine4
    break;
  case 253702156:
    res = 0xFFFFFFF0; // azure
    break;
  case -217763395:
    res = 0xFFFFFFF0; // azure1
    break;
  case -217763394:
    res = 0xFFEEEEE0; // azure2
    break;
  case -217763393:
    res = 0xFFCDCDC1; // azure3
    break;
  case -217763392:
    res = 0xFF8B8B83; // azure4
    break;
  case 254119969:
    res = 0xFFDCF5F5; // beige
    break;
  case -198861042:
    res = 0xFFC4E4FF; // bisque
    break;
  case 2027520255:
    res = 0xFFC4E4FF; // bisque1
    break;
  case 2027520256:
    res = 0xFFB7D5EE; // bisque2
    break;
  case 2027520257:
    res = 0xFF9EB7CD; // bisque3
    break;
  case 2027520258:
    res = 0xFF6B7D8B; // bisque4
    break;
  case 254362690:
    res = 0xFF000000; // black
    break;
  case 631338833:
    res = 0xFFCDEBFF; // blanchedalmond
    break;
  case 2090117005:
    res = 0xFFFF0000; // blue
    break;
  case 254384478:
    res = 0xFFFF0000; // blue1
    break;
  case 254384479:
    res = 0xFFEE0000; // blue2
    break;
  case 254384480:
    res = 0xFFCD0000; // blue3
    break;
  case 254384481:
    res = 0xFF8B0000; // blue4
    break;
  case 1509219168:
    res = 0xFFE22B8A; // blueviolet
    break;
  case 254594221:
    res = 0xFF2A2AA5; // brown
    break;
  case -188325250:
    res = 0xFF4040FF; // brown1
    break;
  case -188325249:
    res = 0xFF3B3BEE; // brown2
    break;
  case -188325248:
    res = 0xFF3333CD; // brown3
    break;
  case -188325247:
    res = 0xFF23238B; // brown4
    break;
  case -805108052:
    res = 0xFF87B8DE; // burlywood
    break;
  case -798761891:
    res = 0xFF9BD3FF; // burlywood1
    break;
  case -798761890:
    res = 0xFF91C5EE; // burlywood2
    break;
  case -798761889:
    res = 0xFF7DAACD; // burlywood3
    break;
  case -798761888:
    res = 0xFF55738B; // burlywood4
    break;
  case -1997448018:
    res = 0xFFA09E5F; // cadetblue
    break;
  case -1491275105:
    res = 0xFFFFF598; // cadetblue1
    break;
  case -1491275104:
    res = 0xFFEEE58E; // cadetblue2
    break;
  case -1491275103:
    res = 0xFFCDC57A; // cadetblue3
    break;
  case -1491275102:
    res = 0xFF8B8653; // cadetblue4
    break;
  case -74361797:
    res = 0xFF00FF7F; // chartreuse
    break;
  case 1841028044:
    res = 0xFF00FF7F; // chartreuse1
    break;
  case 1841028045:
    res = 0xFF00EE76; // chartreuse2
    break;
  case 1841028046:
    res = 0xFF00CD66; // chartreuse3
    break;
  case 1841028047:
    res = 0xFF008B45; // chartreuse4
    break;
  case 1216302135:
    res = 0xFF1E69D2; // chocolate
    break;
  case 1483264840:
    res = 0xFF247FFF; // chocolate1
    break;
  case 1483264841:
    res = 0xFF2176EE; // chocolate2
    break;
  case 1483264842:
    res = 0xFF1D66CD; // chocolate3
    break;
  case 1483264843:
    res = 0xFF13458B; // chocolate4
    break;
  case 255674870:
    res = 0xFF507FFF; // coral
    break;
  case -152663833:
    res = 0xFF5672FF; // coral1
    break;
  case -152663832:
    res = 0xFF506AEE; // coral2
    break;
  case -152663831:
    res = 0xFF455BCD; // coral3
    break;
  case -152663830:
    res = 0xFF2F3E8B; // coral4
    break;
  case 1746496750:
    res = 0xFFED9564; // cornflowerblue
    break;
  case 1268543594:
    res = 0xFFDCF8FF; // cornsilk
    break;
  case -1087734309:
    res = 0xFFDCF8FF; // cornsilk1
    break;
  case -1087734308:
    res = 0xFFCDE8EE; // cornsilk2
    break;
  case -1087734307:
    res = 0xFFB1C8CD; // cornsilk3
    break;
  case -1087734306:
    res = 0xFF78888B; // cornsilk4
    break;
  case 2090166448:
    res = 0xFFFFFF00; // cyan
    break;
  case 256016097:
    res = 0xFFFFFF00; // cyan1
    break;
  case 256016098:
    res = 0xFFEEEE00; // cyan2
    break;
  case 256016099:
    res = 0xFFCDCD00; // cyan3
    break;
  case 256016100:
    res = 0xFF8B8B00; // cyan4
    break;
  case 32466095:
    res = 0xFF8B0000; // darkblue
    break;
  case 32515538:
    res = 0xFF8B8B00; // darkcyan
    break;
  case -958856571:
    res = 0xFF0B86B8; // darkgoldenrod
    break;
  case -1577495722:
    res = 0xFF0FB9FF; // darkgoldenrod1
    break;
  case -1577495721:
    res = 0xFF0EADEE; // darkgoldenrod2
    break;
  case -1577495720:
    res = 0xFF0C95CD; // darkgoldenrod3
    break;
  case -1577495719:
    res = 0xFF08658B; // darkgoldenrod4
    break;
  case 32651674:
    res = 0xFFA9A9A9; // darkgray
    break;
  case 1077509048:
    res = 0xFF006400; // darkgreen
    break;
  case 32651806:
    res = 0xFFA9A9A9; // darkgrey
    break;
  case 1081889199:
    res = 0xFF6BB7BD; // darkkhaki
    break;
  case -622771228:
    res = 0xFF8B008B; // darkmagenta
    break;
  case 153909911:
    res = 0xFF2F6B55; // darkolivegreen
    break;
  case 784059816:
    res = 0xFF70FFCA; // darkolivegreen1
    break;
  case 784059817:
    res = 0xFF68EEBC; // darkolivegreen2
    break;
  case 784059818:
    res = 0xFF5ACDA2; // darkolivegreen3
    break;
  case 784059819:
    res = 0xFF3D8B6E; // darkolivegreen4
    break;
  case 1511009283:
    res = 0xFF008CFF; // darkorange
    break;
  case -1676301164:
    res = 0xFF007FFF; // darkorange1
    break;
  case -1676301163:
    res = 0xFF0076EE; // darkorange2
    break;
  case -1676301162:
    res = 0xFF0066CD; // darkorange3
    break;
  case -1676301161:
    res = 0xFF00458B; // darkorange4
    break;
  case 1511074688:
    res = 0xFFCC3299; // darkorchid
    break;
  case -1674142799:
    res = 0xFFFF3EBF; // darkorchid1
    break;
  case -1674142798:
    res = 0xFFEE3AB2; // darkorchid2
    break;
  case -1674142797:
    res = 0xFFCD329A; // darkorchid3
    break;
  case -1674142796:
    res = 0xFF8B2268; // darkorchid4
    break;
  case 1000994:
    res = 0xFF00008B; // darkred
    break;
  case 1647784689:
    res = 0xFF7A96E9; // darksalmon
    break;
  case -425402223:
    res = 0xFF8FBC8F; // darkseagreen
    break;
  case -1153371422:
    res = 0xFFC1FFC1; // darkseagreen1
    break;
  case -1153371421:
    res = 0xFFB4EEB4; // darkseagreen2
    break;
  case -1153371420:
    res = 0xFF9BCD9B; // darkseagreen3
    break;
  case -1153371419:
    res = 0xFF698B69; // darkseagreen4
    break;
  case 1316229224:
    res = 0xFF8B3D48; // darkslateblue
    break;
  case 1316414803:
    res = 0xFF4F4F2F; // darkslategray
    break;
  case 492015588:
    res = 0xFFFFFF97; // darkslategray1
    break;
  case 492015589:
    res = 0xFFEEEE8D; // darkslategray2
    break;
  case 492015590:
    res = 0xFFCDCD79; // darkslategray3
    break;
  case 492015591:
    res = 0xFF8B8B52; // darkslategray4
    break;
  case 1316414935:
    res = 0xFF4F4F2F; // darkslategrey
    break;
  case 671623224:
    res = 0xFFD1CE00; // darkturquoise
    break;
  case 1774784634:
    res = 0xFFD30094; // darkviolet
    break;
  case 401039797:
    res = 0xFF9314FF; // deeppink
    break;
  case 349411462:
    res = 0xFF9314FF; // deeppink1
    break;
  case 349411463:
    res = 0xFF8912EE; // deeppink2
    break;
  case 349411464:
    res = 0xFF7610CD; // deeppink3
    break;
  case 349411465:
    res = 0xFF500A8B; // deeppink4
    break;
  case -2072509918:
    res = 0xFFFFBF00; // deepskyblue
    break;
  case 326649491:
    res = 0xFFFFBF00; // deepskyblue1
    break;
  case 326649492:
    res = 0xFFEEB200; // deepskyblue2
    break;
  case 326649493:
    res = 0xFFCD9A00; // deepskyblue3
    break;
  case 326649494:
    res = 0xFF8B6800; // deepskyblue4
    break;
  case 308010674:
    res = 0xFF696969; // dimgray
    break;
  case 308010806:
    res = 0xFF696969; // dimgrey
    break;
  case -2113359326:
    res = 0xFFFF901E; // dodgerblue
    break;
  case -1021380973:
    res = 0xFFFF901E; // dodgerblue1
    break;
  case -1021380972:
    res = 0xFFEE861C; // dodgerblue2
    break;
  case -1021380971:
    res = 0xFFCD7418; // dodgerblue3
    break;
  case -1021380970:
    res = 0xFF8B4E10; // dodgerblue4
    break;
  case 2095556406:
    res = 0xFF2222B2; // firebrick
    break;
  case 433884711:
    res = 0xFF3030FF; // firebrick1
    break;
  case 433884712:
    res = 0xFF2C2CEE; // firebrick2
    break;
  case 433884713:
    res = 0xFF2626CD; // firebrick3
    break;
  case 433884714:
    res = 0xFF1A1A8B; // firebrick4
    break;
  case -1451792442:
    res = 0xFFF0FAFF; // floralwhite
    break;
  case -1898313175:
    res = 0xFF228B22; // forestgreen
    break;
  case -256724471:
    res = 0xFFDCDCDC; // gainsboro
    break;
  case 1152083595:
    res = 0xFFFFF8F8; // ghostwhite
    break;
  case 2090299659:
    res = 0xFF00D7FF; // gold
    break;
  case 260412060:
    res = 0xFF00D7FF; // gold1
    break;
  case 260412061:
    res = 0xFF00C9EE; // gold2
    break;
  case 260412062:
    res = 0xFF00ADCD; // gold3
    break;
  case 260412063:
    res = 0xFF00758B; // gold4
    break;
  case -1427062749:
    res = 0xFF20A5DA; // goldenrod
    break;
  case 151569588:
    res = 0xFF25C1FF; // goldenrod1
    break;
  case 151569589:
    res = 0xFF22B4EE; // goldenrod2
    break;
  case 151569590:
    res = 0xFF1D9BCD; // goldenrod3
    break;
  case 151569591:
    res = 0xFF14698B; // goldenrod4
    break;
  case 2090302584:
    res = 0xFFBEBEBE; // gray
    break;
  case 260508584:
    res = 0xFF000000; // gray0
    break;
  case 260508585:
    res = 0xFF030303; // gray1
    break;
  case 260508586:
    res = 0xFF050505; // gray2
    break;
  case 260508587:
    res = 0xFF080808; // gray3
    break;
  case 260508588:
    res = 0xFF0A0A0A; // gray4
    break;
  case 260508589:
    res = 0xFF0D0D0D; // gray5
    break;
  case 260508590:
    res = 0xFF0F0F0F; // gray6
    break;
  case 260508591:
    res = 0xFF121212; // gray7
    break;
  case 260508592:
    res = 0xFF141414; // gray8
    break;
  case 260508593:
    res = 0xFF171717; // gray9
    break;
  case 6848761:
    res = 0xFF1A1A1A; // gray10
    break;
  case 6848762:
    res = 0xFF1C1C1C; // gray11
    break;
  case 6848763:
    res = 0xFF1F1F1F; // gray12
    break;
  case 6848764:
    res = 0xFF212121; // gray13
    break;
  case 6848765:
    res = 0xFF242424; // gray14
    break;
  case 6848766:
    res = 0xFF262626; // gray15
    break;
  case 6848767:
    res = 0xFF292929; // gray16
    break;
  case 6848768:
    res = 0xFF2B2B2B; // gray17
    break;
  case 6848769:
    res = 0xFF2E2E2E; // gray18
    break;
  case 6848770:
    res = 0xFF303030; // gray19
    break;
  case 6848794:
    res = 0xFF333333; // gray20
    break;
  case 6848795:
    res = 0xFF363636; // gray21
    break;
  case 6848796:
    res = 0xFF383838; // gray22
    break;
  case 6848797:
    res = 0xFF3B3B3B; // gray23
    break;
  case 6848798:
    res = 0xFF3D3D3D; // gray24
    break;
  case 6848799:
    res = 0xFF404040; // gray25
    break;
  case 6848800:
    res = 0xFF424242; // gray26
    break;
  case 6848801:
    res = 0xFF454545; // gray27
    break;
  case 6848802:
    res = 0xFF474747; // gray28
    break;
  case 6848803:
    res = 0xFF4A4A4A; // gray29
    break;
  case 6848827:
    res = 0xFF4D4D4D; // gray30
    break;
  case 6848828:
    res = 0xFF4F4F4F; // gray31
    break;
  case 6848829:
    res = 0xFF525252; // gray32
    break;
  case 6848830:
    res = 0xFF545454; // gray33
    break;
  case 6848831:
    res = 0xFF575757; // gray34
    break;
  case 6848832:
    res = 0xFF595959; // gray35
    break;
  case 6848833:
    res = 0xFF5C5C5C; // gray36
    break;
  case 6848834:
    res = 0xFF5E5E5E; // gray37
    break;
  case 6848835:
    res = 0xFF616161; // gray38
    break;
  case 6848836:
    res = 0xFF636363; // gray39
    break;
  case 6848860:
    res = 0xFF666666; // gray40
    break;
  case 6848861:
    res = 0xFF696969; // gray41
    break;
  case 6848862:
    res = 0xFF6B6B6B; // gray42
    break;
  case 6848863:
    res = 0xFF6E6E6E; // gray43
    break;
  case 6848864:
    res = 0xFF707070; // gray44
    break;
  case 6848865:
    res = 0xFF737373; // gray45
    break;
  case 6848866:
    res = 0xFF757575; // gray46
    break;
  case 6848867:
    res = 0xFF787878; // gray47
    break;
  case 6848868:
    res = 0xFF7A7A7A; // gray48
    break;
  case 6848869:
    res = 0xFF7D7D7D; // gray49
    break;
  case 6848893:
    res = 0xFF7F7F7F; // gray50
    break;
  case 6848894:
    res = 0xFF828282; // gray51
    break;
  case 6848895:
    res = 0xFF858585; // gray52
    break;
  case 6848896:
    res = 0xFF878787; // gray53
    break;
  case 6848897:
    res = 0xFF8A8A8A; // gray54
    break;
  case 6848898:
    res = 0xFF8C8C8C; // gray55
    break;
  case 6848899:
    res = 0xFF8F8F8F; // gray56
    break;
  case 6848900:
    res = 0xFF919191; // gray57
    break;
  case 6848901:
    res = 0xFF949494; // gray58
    break;
  case 6848902:
    res = 0xFF969696; // gray59
    break;
  case 6848926:
    res = 0xFF999999; // gray60
    break;
  case 6848927:
    res = 0xFF9C9C9C; // gray61
    break;
  case 6848928:
    res = 0xFF9E9E9E; // gray62
    break;
  case 6848929:
    res = 0xFFA1A1A1; // gray63
    break;
  case 6848930:
    res = 0xFFA3A3A3; // gray64
    break;
  case 6848931:
    res = 0xFFA6A6A6; // gray65
    break;
  case 6848932:
    res = 0xFFA8A8A8; // gray66
    break;
  case 6848933:
    res = 0xFFABABAB; // gray67
    break;
  case 6848934:
    res = 0xFFADADAD; // gray68
    break;
  case 6848935:
    res = 0xFFB0B0B0; // gray69
    break;
  case 6848959:
    res = 0xFFB3B3B3; // gray70
    break;
  case 6848960:
    res = 0xFFB5B5B5; // gray71
    break;
  case 6848961:
    res = 0xFFB8B8B8; // gray72
    break;
  case 6848962:
    res = 0xFFBABABA; // gray73
    break;
  case 6848963:
    res = 0xFFBDBDBD; // gray74
    break;
  case 6848964:
    res = 0xFFBFBFBF; // gray75
    break;
  case 6848965:
    res = 0xFFC2C2C2; // gray76
    break;
  case 6848966:
    res = 0xFFC4C4C4; // gray77
    break;
  case 6848967:
    res = 0xFFC7C7C7; // gray78
    break;
  case 6848968:
    res = 0xFFC9C9C9; // gray79
    break;
  case 6848992:
    res = 0xFFCCCCCC; // gray80
    break;
  case 6848993:
    res = 0xFFCFCFCF; // gray81
    break;
  case 6848994:
    res = 0xFFD1D1D1; // gray82
    break;
  case 6848995:
    res = 0xFFD4D4D4; // gray83
    break;
  case 6848996:
    res = 0xFFD6D6D6; // gray84
    break;
  case 6848997:
    res = 0xFFD9D9D9; // gray85
    break;
  case 6848998:
    res = 0xFFDBDBDB; // gray86
    break;
  case 6848999:
    res = 0xFFDEDEDE; // gray87
    break;
  case 6849000:
    res = 0xFFE0E0E0; // gray88
    break;
  case 6849001:
    res = 0xFFE3E3E3; // gray89
    break;
  case 6849025:
    res = 0xFFE5E5E5; // gray90
    break;
  case 6849026:
    res = 0xFFE8E8E8; // gray91
    break;
  case 6849027:
    res = 0xFFEBEBEB; // gray92
    break;
  case 6849028:
    res = 0xFFEDEDED; // gray93
    break;
  case 6849029:
    res = 0xFFF0F0F0; // gray94
    break;
  case 6849030:
    res = 0xFFF2F2F2; // gray95
    break;
  case 6849031:
    res = 0xFFF5F5F5; // gray96
    break;
  case 6849032:
    res = 0xFFF7F7F7; // gray97
    break;
  case 6849033:
    res = 0xFFFAFAFA; // gray98
    break;
  case 6849034:
    res = 0xFFFCFCFC; // gray99
    break;
  case 226009161:
    res = 0xFFFFFFFF; // gray100
    break;
  case 260512342:
    res = 0xFF00FF00; // green
    break;
  case 6972743:
    res = 0xFF00FF00; // green1
    break;
  case 6972744:
    res = 0xFF00EE00; // green2
    break;
  case 6972745:
    res = 0xFF00CD00; // green3
    break;
  case 6972746:
    res = 0xFF008B00; // green4
    break;
  case -1062996750:
    res = 0xFF2FFFAD; // greenyellow
    break;
  case 2090302716:
    res = 0xFFBEBEBE; // grey
    break;
  case 260512940:
    res = 0xFF000000; // grey0
    break;
  case 260512941:
    res = 0xFF030303; // grey1
    break;
  case 260512942:
    res = 0xFF050505; // grey2
    break;
  case 260512943:
    res = 0xFF080808; // grey3
    break;
  case 260512944:
    res = 0xFF0A0A0A; // grey4
    break;
  case 260512945:
    res = 0xFF0D0D0D; // grey5
    break;
  case 260512946:
    res = 0xFF0F0F0F; // grey6
    break;
  case 260512947:
    res = 0xFF121212; // grey7
    break;
  case 260512948:
    res = 0xFF141414; // grey8
    break;
  case 260512949:
    res = 0xFF171717; // grey9
    break;
  case 6992509:
    res = 0xFF1A1A1A; // grey10
    break;
  case 6992510:
    res = 0xFF1C1C1C; // grey11
    break;
  case 6992511:
    res = 0xFF1F1F1F; // grey12
    break;
  case 6992512:
    res = 0xFF212121; // grey13
    break;
  case 6992513:
    res = 0xFF242424; // grey14
    break;
  case 6992514:
    res = 0xFF262626; // grey15
    break;
  case 6992515:
    res = 0xFF292929; // grey16
    break;
  case 6992516:
    res = 0xFF2B2B2B; // grey17
    break;
  case 6992517:
    res = 0xFF2E2E2E; // grey18
    break;
  case 6992518:
    res = 0xFF303030; // grey19
    break;
  case 6992542:
    res = 0xFF333333; // grey20
    break;
  case 6992543:
    res = 0xFF363636; // grey21
    break;
  case 6992544:
    res = 0xFF383838; // grey22
    break;
  case 6992545:
    res = 0xFF3B3B3B; // grey23
    break;
  case 6992546:
    res = 0xFF3D3D3D; // grey24
    break;
  case 6992547:
    res = 0xFF404040; // grey25
    break;
  case 6992548:
    res = 0xFF424242; // grey26
    break;
  case 6992549:
    res = 0xFF454545; // grey27
    break;
  case 6992550:
    res = 0xFF474747; // grey28
    break;
  case 6992551:
    res = 0xFF4A4A4A; // grey29
    break;
  case 6992575:
    res = 0xFF4D4D4D; // grey30
    break;
  case 6992576:
    res = 0xFF4F4F4F; // grey31
    break;
  case 6992577:
    res = 0xFF525252; // grey32
    break;
  case 6992578:
    res = 0xFF545454; // grey33
    break;
  case 6992579:
    res = 0xFF575757; // grey34
    break;
  case 6992580:
    res = 0xFF595959; // grey35
    break;
  case 6992581:
    res = 0xFF5C5C5C; // grey36
    break;
  case 6992582:
    res = 0xFF5E5E5E; // grey37
    break;
  case 6992583:
    res = 0xFF616161; // grey38
    break;
  case 6992584:
    res = 0xFF636363; // grey39
    break;
  case 6992608:
    res = 0xFF666666; // grey40
    break;
  case 6992609:
    res = 0xFF696969; // grey41
    break;
  case 6992610:
    res = 0xFF6B6B6B; // grey42
    break;
  case 6992611:
    res = 0xFF6E6E6E; // grey43
    break;
  case 6992612:
    res = 0xFF707070; // grey44
    break;
  case 6992613:
    res = 0xFF737373; // grey45
    break;
  case 6992614:
    res = 0xFF757575; // grey46
    break;
  case 6992615:
    res = 0xFF787878; // grey47
    break;
  case 6992616:
    res = 0xFF7A7A7A; // grey48
    break;
  case 6992617:
    res = 0xFF7D7D7D; // grey49
    break;
  case 6992641:
    res = 0xFF7F7F7F; // grey50
    break;
  case 6992642:
    res = 0xFF828282; // grey51
    break;
  case 6992643:
    res = 0xFF858585; // grey52
    break;
  case 6992644:
    res = 0xFF878787; // grey53
    break;
  case 6992645:
    res = 0xFF8A8A8A; // grey54
    break;
  case 6992646:
    res = 0xFF8C8C8C; // grey55
    break;
  case 6992647:
    res = 0xFF8F8F8F; // grey56
    break;
  case 6992648:
    res = 0xFF919191; // grey57
    break;
  case 6992649:
    res = 0xFF949494; // grey58
    break;
  case 6992650:
    res = 0xFF969696; // grey59
    break;
  case 6992674:
    res = 0xFF999999; // grey60
    break;
  case 6992675:
    res = 0xFF9C9C9C; // grey61
    break;
  case 6992676:
    res = 0xFF9E9E9E; // grey62
    break;
  case 6992677:
    res = 0xFFA1A1A1; // grey63
    break;
  case 6992678:
    res = 0xFFA3A3A3; // grey64
    break;
  case 6992679:
    res = 0xFFA6A6A6; // grey65
    break;
  case 6992680:
    res = 0xFFA8A8A8; // grey66
    break;
  case 6992681:
    res = 0xFFABABAB; // grey67
    break;
  case 6992682:
    res = 0xFFADADAD; // grey68
    break;
  case 6992683:
    res = 0xFFB0B0B0; // grey69
    break;
  case 6992707:
    res = 0xFFB3B3B3; // grey70
    break;
  case 6992708:
    res = 0xFFB5B5B5; // grey71
    break;
  case 6992709:
    res = 0xFFB8B8B8; // grey72
    break;
  case 6992710:
    res = 0xFFBABABA; // grey73
    break;
  case 6992711:
    res = 0xFFBDBDBD; // grey74
    break;
  case 6992712:
    res = 0xFFBFBFBF; // grey75
    break;
  case 6992713:
    res = 0xFFC2C2C2; // grey76
    break;
  case 6992714:
    res = 0xFFC4C4C4; // grey77
    break;
  case 6992715:
    res = 0xFFC7C7C7; // grey78
    break;
  case 6992716:
    res = 0xFFC9C9C9; // grey79
    break;
  case 6992740:
    res = 0xFFCCCCCC; // grey80
    break;
  case 6992741:
    res = 0xFFCFCFCF; // grey81
    break;
  case 6992742:
    res = 0xFFD1D1D1; // grey82
    break;
  case 6992743:
    res = 0xFFD4D4D4; // grey83
    break;
  case 6992744:
    res = 0xFFD6D6D6; // grey84
    break;
  case 6992745:
    res = 0xFFD9D9D9; // grey85
    break;
  case 6992746:
    res = 0xFFDBDBDB; // grey86
    break;
  case 6992747:
    res = 0xFFDEDEDE; // grey87
    break;
  case 6992748:
    res = 0xFFE0E0E0; // grey88
    break;
  case 6992749:
    res = 0xFFE3E3E3; // grey89
    break;
  case 6992773:
    res = 0xFFE5E5E5; // grey90
    break;
  case 6992774:
    res = 0xFFE8E8E8; // grey91
    break;
  case 6992775:
    res = 0xFFEBEBEB; // grey92
    break;
  case 6992776:
    res = 0xFFEDEDED; // grey93
    break;
  case 6992777:
    res = 0xFFF0F0F0; // grey94
    break;
  case 6992778:
    res = 0xFFF2F2F2; // grey95
    break;
  case 6992779:
    res = 0xFFF5F5F5; // grey96
    break;
  case 6992780:
    res = 0xFFF7F7F7; // grey97
    break;
  case 6992781:
    res = 0xFFFAFAFA; // grey98
    break;
  case 6992782:
    res = 0xFFFCFCFC; // grey99
    break;
  case 230752845:
    res = 0xFFFFFFFF; // grey100
    break;
  case -554611224:
    res = 0xFFF0FFF0; // honeydew
    break;
  case -1122301159:
    res = 0xFFF0FFF0; // honeydew1
    break;
  case -1122301158:
    res = 0xFFE0EEE0; // honeydew2
    break;
  case -1122301157:
    res = 0xFFC1CDC1; // honeydew3
    break;
  case -1122301156:
    res = 0xFF838B83; // honeydew4
    break;
  case 1422343106:
    res = 0xFFB469FF; // hotpink
    break;
  case -307317709:
    res = 0xFFB46EFF; // hotpink1
    break;
  case -307317708:
    res = 0xFFA76AEE; // hotpink2
    break;
  case -307317707:
    res = 0xFF9060CD; // hotpink3
    break;
  case -307317706:
    res = 0xFF623A8B; // hotpink4
    break;
  case 1261915923:
    res = 0xFF5C5CCD; // indianred
    break;
  case -1306447452:
    res = 0xFF6A6AFF; // indianred1
    break;
  case -1306447451:
    res = 0xFF6363EE; // indianred2
    break;
  case -1306447450:
    res = 0xFF5555CD; // indianred3
    break;
  case -1306447449:
    res = 0xFF3A3A8B; // indianred4
    break;
  case 263039262:
    res = 0xFFF0FFFF; // ivory
    break;
  case 90361103:
    res = 0xFFF0FFFF; // ivory1
    break;
  case 90361104:
    res = 0xFFE0EEEE; // ivory2
    break;
  case 90361105:
    res = 0xFFC1CDCD; // ivory3
    break;
  case 90361106:
    res = 0xFF838B8B; // ivory4
    break;
  case 264892493:
    res = 0xFF8CE6F0; // khaki
    break;
  case 151517726:
    res = 0xFF8FF6FF; // khaki1
    break;
  case 151517727:
    res = 0xFF85E6EE; // khaki2
    break;
  case 151517728:
    res = 0xFF73C6CD; // khaki3
    break;
  case 151517729:
    res = 0xFF4E868B; // khaki4
    break;
  case 1827441590:
    res = 0xFFFAE6E6; // lavender
    break;
  case 1295050932:
    res = 0xFFF5F0FF; // lavenderblush
    break;
  case -212992155:
    res = 0xFFF5F0FF; // lavenderblush1
    break;
  case -212992154:
    res = 0xFFE5E0EE; // lavenderblush2
    break;
  case -212992153:
    res = 0xFFC5C1CD; // lavenderblush3
    break;
  case -212992152:
    res = 0xFF86838B; // lavenderblush4
    break;
  case 1811918184:
    res = 0xFF00FC7C; // lawngreen
    break;
  case 446933885:
    res = 0xFFCDFAFF; // lemonchiffon
    break;
  case 1863916366:
    res = 0xFFCDFAFF; // lemonchiffon1
    break;
  case 1863916367:
    res = 0xFFBFE9EE; // lemonchiffon2
    break;
  case 1863916368:
    res = 0xFFA5C9CD; // lemonchiffon3
    break;
  case 1863916369:
    res = 0xFF70898B; // lemonchiffon4
    break;
  case -246534939:
    res = 0xFFE6D8AD; // lightblue
    break;
  case 454281654:
    res = 0xFFFFEFBF; // lightblue1
    break;
  case 454281655:
    res = 0xFFEEDFB2; // lightblue2
    break;
  case 454281656:
    res = 0xFFCDC09A; // lightblue3
    break;
  case 454281657:
    res = 0xFF8B8368; // lightblue4
    break;
  case 455572046:
    res = 0xFF8080F0; // lightcoral
    break;
  case -246485496:
    res = 0xFFFFFFE0; // lightcyan
    break;
  case 455913273:
    res = 0xFFFFFFE0; // lightcyan1
    break;
  case 455913274:
    res = 0xFFEEEED1; // lightcyan2
    break;
  case 455913275:
    res = 0xFFCDCDB4; // lightcyan3
    break;
  case 455913276:
    res = 0xFF8B8B7A; // lightcyan4
    break;
  case 111893627:
    res = 0xFF82DDEE; // lightgoldenrod
    break;
  case -602477556:
    res = 0xFF8BECFF; // lightgoldenrod1
    break;
  case -602477555:
    res = 0xFF82DCEE; // lightgoldenrod2
    break;
  case -602477554:
    res = 0xFF70BECD; // lightgoldenrod3
    break;
  case -602477553:
    res = 0xFF4C818B; // lightgoldenrod4
    break;
  case 647790295:
    res = 0xFFD2FAFA; // lightgoldenrodyellow
    break;
  case -246349360:
    res = 0xFFD3D3D3; // lightgray
    break;
  case 460409518:
    res = 0xFF90EE90; // lightgreen
    break;
  case -246349228:
    res = 0xFFD3D3D3; // lightgrey
    break;
  case -246035313:
    res = 0xFFC1B6FF; // lightpink
    break;
  case 470769312:
    res = 0xFFB9AEFF; // lightpink1
    break;
  case 470769313:
    res = 0xFFADA2EE; // lightpink2
    break;
  case 470769314:
    res = 0xFF958CCD; // lightpink3
    break;
  case 470769315:
    res = 0xFF655F8B; // lightpink4
    break;
  case -1536630617:
    res = 0xFF7AA0FF; // lightsalmon
    break;
  case 830797240:
    res = 0xFF7AA0FF; // lightsalmon1
    break;
  case 830797241:
    res = 0xFF7295EE; // lightsalmon2
    break;
  case 830797242:
    res = 0xFF6281CD; // lightsalmon3
    break;
  case 830797243:
    res = 0xFF42578B; // lightsalmon4
    break;
  case 2079904711:
    res = 0xFFAAB220; // lightseagreen
    break;
  case 1237169852:
    res = 0xFFFACE87; // lightskyblue
    break;
  case -2123067795:
    res = 0xFFFFE2B0; // lightskyblue1
    break;
  case -2123067794:
    res = 0xFFEED3A4; // lightskyblue2
    break;
  case -2123067793:
    res = 0xFFCDB68D; // lightskyblue3
    break;
  case -2123067792:
    res = 0xFF8B7B60; // lightskyblue4
    break;
  case -1907987874:
    res = 0xFFFF7084; // lightslateblue
    break;
  case -1907802295:
    res = 0xFF998877; // lightslategray
    break;
  case -1907802163:
    res = 0xFF998877; // lightslategrey
    break;
  case 29314690:
    res = 0xFFDEC4B0; // lightsteelblue
    break;
  case 967384819:
    res = 0xFFFFE1CA; // lightsteelblue1
    break;
  case 967384820:
    res = 0xFFEED2BC; // lightsteelblue2
    break;
  case 967384821:
    res = 0xFFCDB5A2; // lightsteelblue3
    break;
  case 967384822:
    res = 0xFF8B7B6E; // lightsteelblue4
    break;
  case -1297075655:
    res = 0xFFE0FFFF; // lightyellow
    break;
  case 146176394:
    res = 0xFFE0FFFF; // lightyellow1
    break;
  case 146176395:
    res = 0xFFD1EEEE; // lightyellow2
    break;
  case 146176396:
    res = 0xFFB4CDCD; // lightyellow3
    break;
  case 146176397:
    res = 0xFF7A8B8B; // lightyellow4
    break;
  case -1219918019:
    res = 0xFF32CD32; // limegreen
    break;
  case 266128315:
    res = 0xFFE6F0FA; // linen
    break;
  case -1273953790:
    res = 0xFFFF00FF; // magenta
    break;
  case 909197939:
    res = 0xFFFF00FF; // magenta1
    break;
  case 909197940:
    res = 0xFFEE00EE; // magenta2
    break;
  case 909197941:
    res = 0xFFCD00CD; // magenta3
    break;
  case 909197942:
    res = 0xFF8B008B; // magenta4
    break;
  case 222102609:
    res = 0xFF6030B0; // maroon
    break;
  case -1260548446:
    res = 0xFFB334FF; // maroon1
    break;
  case -1260548445:
    res = 0xFFA730EE; // maroon2
    break;
  case -1260548444:
    res = 0xFF9029CD; // maroon3
    break;
  case -1260548443:
    res = 0xFF621C8B; // maroon4
    break;
  case 1402160266:
    res = 0xFFAACD66; // mediumaquamarine
    break;
  case -663229618:
    res = 0xFFCD0000; // mediumblue
    break;
  case -187312673:
    res = 0xFFD355BA; // mediumorchid
    break;
  case -1886350864:
    res = 0xFFFF66E0; // mediumorchid1
    break;
  case -1886350863:
    res = 0xFFEE5FD1; // mediumorchid2
    break;
  case -1886350862:
    res = 0xFFCD52B4; // mediumorchid3
    break;
  case -1886350861:
    res = 0xFF8B377A; // mediumorchid4
    break;
  case -144071650:
    res = 0xFFDB7093; // mediumpurple
    break;
  case -459397105:
    res = 0xFFFF82AB; // mediumpurple1
    break;
  case -459397104:
    res = 0xFFEE799F; // mediumpurple2
    break;
  case -459397103:
    res = 0xFFCD6889; // mediumpurple3
    break;
  case -459397102:
    res = 0xFF8B475D; // mediumpurple4
    break;
  case 1161666224:
    res = 0xFF71B33C; // mediumseagreen
    break;
  case -2145086873:
    res = 0xFFEE687B; // mediumslateblue
    break;
  case -2126101558:
    res = 0xFF9AFA00; // mediumspringgreen
    break;
  case 1505274423:
    res = 0xFFCCD148; // mediumturquoise
    break;
  case 1004825236:
    res = 0xFF8515C7; // mediumvioletred
    break;
  case 1603474337:
    res = 0xFF701919; // midnightblue
    break;
  case -1686817819:
    res = 0xFFFAFFF5; // mintcream
    break;
  case 501656468:
    res = 0xFFE1E4FF; // mistyrose
    break;
  case -625205691:
    res = 0xFFE1E4FF; // mistyrose1
    break;
  case -625205690:
    res = 0xFFD2D5EE; // mistyrose2
    break;
  case -625205689:
    res = 0xFFB5B7CD; // mistyrose3
    break;
  case -625205688:
    res = 0xFF7B7D8B; // mistyrose4
    break;
  case 1650498962:
    res = 0xFFB5E4FF; // moccasin
    break;
  case -490969563:
    res = 0xFFADDEFF; // navajowhite
    break;
  case 977873654:
    res = 0xFFADDEFF; // navajowhite1
    break;
  case 977873655:
    res = 0xFFA1CFEE; // navajowhite2
    break;
  case 977873656:
    res = 0xFF8BB3CD; // navajowhite3
    break;
  case 977873657:
    res = 0xFF5E798B; // navajowhite4
    break;
  case 2090536323:
    res = 0xFF800000; // navy
    break;
  case 1188278027:
    res = 0xFF800000; // navyblue
    break;
  case 1736150553:
    res = 0xFFE6F5FD; // oldlace
    break;
  case -854101763:
    res = 0xFF238E6B; // olivedrab
    break;
  case 1879412942:
    res = 0xFF3EFFC0; // olivedrab1
    break;
  case 1879412943:
    res = 0xFF3AEEB3; // olivedrab2
    break;
  case 1879412944:
    res = 0xFF32CD9A; // olivedrab3
    break;
  case 1879412945:
    res = 0xFF228B69; // olivedrab4
    break;
  case 319921761:
    res = 0xFF00A5FF; // orange
    break;
  case 1967483570:
    res = 0xFF00A5FF; // orange1
    break;
  case 1967483571:
    res = 0xFF009AEE; // orange2
    break;
  case 1967483572:
    res = 0xFF0085CD; // orange3
    break;
  case 1967483573:
    res = 0xFF005A8B; // orange4
    break;
  case -598998756:
    res = 0xFF0045FF; // orangered
    break;
  case 1707877581:
    res = 0xFF0045FF; // orangered1
    break;
  case 1707877582:
    res = 0xFF0040EE; // orangered2
    break;
  case 1707877583:
    res = 0xFF0037CD; // orangered3
    break;
  case 1707877584:
    res = 0xFF00258B; // orangered4
    break;
  case 319987166:
    res = 0xFFD670DA; // orchid
    break;
  case 1969641935:
    res = 0xFFFA83FF; // orchid1
    break;
  case 1969641936:
    res = 0xFFE97AEE; // orchid2
    break;
  case 1969641937:
    res = 0xFFC969CD; // orchid3
    break;
  case 1969641938:
    res = 0xFF89478B; // orchid4
    break;
  case 1189203525:
    res = 0xFFAAE8EE; // palegoldenrod
    break;
  case -634230920:
    res = 0xFF98FB98; // palegreen
    break;
  case 545216169:
    res = 0xFF9AFF9A; // palegreen1
    break;
  case 545216170:
    res = 0xFF90EE90; // palegreen2
    break;
  case 545216171:
    res = 0xFF7CCD7C; // palegreen3
    break;
  case 545216172:
    res = 0xFF548B54; // palegreen4
    break;
  case -1475283976:
    res = 0xFFEEEEAF; // paleturquoise
    break;
  case -1439730903:
    res = 0xFFFFFFBB; // paleturquoise1
    break;
  case -1439730902:
    res = 0xFFEEEEAE; // paleturquoise2
    break;
  case -1439730901:
    res = 0xFFCDCD96; // paleturquoise3
    break;
  case -1439730900:
    res = 0xFF8B8B66; // paleturquoise4
    break;
  case -1975733163:
    res = 0xFF9370DB; // palevioletred
    break;
  case -774684890:
    res = 0xFFAB82FF; // palevioletred1
    break;
  case -774684889:
    res = 0xFF9F79EE; // palevioletred2
    break;
  case -774684888:
    res = 0xFF8968CD; // palevioletred3
    break;
  case -774684887:
    res = 0xFF5D478B; // palevioletred4
    break;
  case -965681895:
    res = 0xFFD5EFFF; // papayawhip
    break;
  case 937431383:
    res = 0xFFB9DAFF; // peachpuff
    break;
  case 870464616:
    res = 0xFFB9DAFF; // peachpuff1
    break;
  case 870464617:
    res = 0xFFADCBEE; // peachpuff2
    break;
  case 870464618:
    res = 0xFF95AFCD; // peachpuff3
    break;
  case 870464619:
    res = 0xFF65778B; // peachpuff4
    break;
  case 2090612417:
    res = 0xFF3F85CD; // peru
    break;
  case 2090616631:
    res = 0xFFCBC0FF; // pink
    break;
  case 270872136:
    res = 0xFFC5B5FF; // pink1
    break;
  case 270872137:
    res = 0xFFB8A9EE; // pink2
    break;
  case 270872138:
    res = 0xFF9E91CD; // pink3
    break;
  case 270872139:
    res = 0xFF6C638B; // pink4
    break;
  case 2090620131:
    res = 0xFFDDA0DD; // plum
    break;
  case 270987636:
    res = 0xFFFFBBFF; // plum1
    break;
  case 270987637:
    res = 0xFFEEAEEE; // plum2
    break;
  case 270987638:
    res = 0xFFCD96CD; // plum3
    break;
  case 270987639:
    res = 0xFF8B668B; // plum4
    break;
  case 1417385502:
    res = 0xFFE6E0B0; // powderblue
    break;
  case 363228189:
    res = 0xFFF020A0; // purple
    break;
  case -898371602:
    res = 0xFFFF309B; // purple1
    break;
  case -898371601:
    res = 0xFFEE2C91; // purple2
    break;
  case -898371600:
    res = 0xFFCD267D; // purple3
    break;
  case -898371599:
    res = 0xFF8B1A55; // purple4
    break;
  case 193504576:
    res = 0xFF0000FF; // red
    break;
  case 2090683761:
    res = 0xFF0000FF; // red1
    break;
  case 2090683762:
    res = 0xFF0000EE; // red2
    break;
  case 2090683763:
    res = 0xFF0000CD; // red3
    break;
  case 2090683764:
    res = 0xFF00008B; // red4
    break;
  case -135694854:
    res = 0xFF8F8FBC; // rosybrown
    break;
  case -182962837:
    res = 0xFFC1C1FF; // rosybrown1
    break;
  case -182962836:
    res = 0xFFB4B4EE; // rosybrown2
    break;
  case -182962835:
    res = 0xFF9B9BCD; // rosybrown3
    break;
  case -182962834:
    res = 0xFF69698B; // rosybrown4
    break;
  case -1904790188:
    res = 0xFFE16941; // royalblue
    break;
  case 1566433285:
    res = 0xFFFF7648; // royalblue1
    break;
  case 1566433286:
    res = 0xFFEE6E43; // royalblue2
    break;
  case 1566433287:
    res = 0xFFCD5F3A; // royalblue3
    break;
  case 1566433288:
    res = 0xFF8B4027; // royalblue4
    break;
  case -1833176230:
    res = 0xFF13458B; // saddlebrown
    break;
  case 456697167:
    res = 0xFF7280FA; // salmon
    break;
  case -2108862624:
    res = 0xFF698CFF; // salmon1
    break;
  case -2108862623:
    res = 0xFF6282EE; // salmon2
    break;
  case -2108862622:
    res = 0xFF5470CD; // salmon3
    break;
  case -2108862621:
    res = 0xFF394C8B; // salmon4
    break;
  case -519366868:
    res = 0xFF60A4F4; // sandybrown
    break;
  case -439590289:
    res = 0xFF578B2E; // seagreen
    break;
  case -1621577600:
    res = 0xFF9FFF54; // seagreen1
    break;
  case -1621577599:
    res = 0xFF94EE4E; // seagreen2
    break;
  case -1621577598:
    res = 0xFF80CD43; // seagreen3
    break;
  case -1621577597:
    res = 0xFF578B2E; // seagreen4
    break;
  case -425718378:
    res = 0xFFEEF5FF; // seashell
    break;
  case -1163804537:
    res = 0xFFEEF5FF; // seashell1
    break;
  case -1163804536:
    res = 0xFFDEE5EE; // seashell2
    break;
  case -1163804535:
    res = 0xFFBFC5CD; // seashell3
    break;
  case -1163804534:
    res = 0xFF82868B; // seashell4
    break;
  case 465934019:
    res = 0xFF2D52A0; // sienna
    break;
  case -1804046508:
    res = 0xFF4782FF; // sienna1
    break;
  case -1804046507:
    res = 0xFF4279EE; // sienna2
    break;
  case -1804046506:
    res = 0xFF3968CD; // sienna3
    break;
  case -1804046505:
    res = 0xFF26478B; // sienna4
    break;
  case -1702490012:
    res = 0xFFEBCE87; // skyblue
    break;
  case -347595499:
    res = 0xFFFFCE87; // skyblue1
    break;
  case -347595498:
    res = 0xFFEEC07E; // skyblue2
    break;
  case -347595497:
    res = 0xFFCDA66C; // skyblue3
    break;
  case -347595496:
    res = 0xFF8B704A; // skyblue4
    break;
  case 848023046:
    res = 0xFFCD5A6A; // slateblue
    break;
  case -2080010505:
    res = 0xFFFF6F83; // slateblue1
    break;
  case -2080010504:
    res = 0xFFEE677A; // slateblue2
    break;
  case -2080010503:
    res = 0xFFCD5969; // slateblue3
    break;
  case -2080010502:
    res = 0xFF8B3C47; // slateblue4
    break;
  case 848208625:
    res = 0xFF908070; // slategray
    break;
  case -2073886398:
    res = 0xFFFFE2C6; // slategray1
    break;
  case -2073886397:
    res = 0xFFEED3B9; // slategray2
    break;
  case -2073886396:
    res = 0xFFCDB69F; // slategray3
    break;
  case -2073886395:
    res = 0xFF8B7B6C; // slategray4
    break;
  case 848208757:
    res = 0xFF908070; // slategrey
    break;
  case 2090729932:
    res = 0xFFFAFAFF; // snow
    break;
  case 274611069:
    res = 0xFFFAFAFF; // snow1
    break;
  case 274611070:
    res = 0xFFE9E9EE; // snow2
    break;
  case 274611071:
    res = 0xFFC9C9CD; // snow3
    break;
  case 274611072:
    res = 0xFF89898B; // snow4
    break;
  case 1785389865:
    res = 0xFF7FFF00; // springgreen
    break;
  case -1211676550:
    res = 0xFF7FFF00; // springgreen1
    break;
  case -1211676549:
    res = 0xFF76EE00; // springgreen2
    break;
  case -1211676548:
    res = 0xFF66CD00; // springgreen3
    break;
  case -1211676547:
    res = 0xFF458B00; // springgreen4
    break;
  case -1509641686:
    res = 0xFFB48246; // steelblue
    break;
  case 1721431963:
    res = 0xFFFFB863; // steelblue1
    break;
  case 1721431964:
    res = 0xFFEEAC5C; // steelblue2
    break;
  case 1721431965:
    res = 0xFFCD944F; // steelblue3
    break;
  case 1721431966:
    res = 0xFF8B6436; // steelblue4
    break;
  case 193506632:
    res = 0xFF8CB4D2; // tan
    break;
  case 2090751609:
    res = 0xFF4FA5FF; // tan1
    break;
  case 2090751610:
    res = 0xFF499AEE; // tan2
    break;
  case 2090751611:
    res = 0xFF3F85CD; // tan3
    break;
  case 2090751612:
    res = 0xFF2B5A8B; // tan4
    break;
  case -546783614:
    res = 0xFFD8BFD8; // thistle
    break;
  case -863990029:
    res = 0xFFFFE1FF; // thistle1
    break;
  case -863990028:
    res = 0xFFEED2EE; // thistle2
    break;
  case -863990027:
    res = 0xFFCDB5CD; // thistle3
    break;
  case -863990026:
    res = 0xFF8B7B8B; // thistle4
    break;
  case 512458489:
    res = 0xFF4763FF; // tomato
    break;
  case -268738998:
    res = 0xFF4763FF; // tomato1
    break;
  case -268738997:
    res = 0xFF425CEE; // tomato2
    break;
  case -268738996:
    res = 0xFF394FCD; // tomato3
    break;
  case -268738995:
    res = 0xFF26368B; // tomato4
    break;
  case 203417046:
    res = 0xFFD0E040; // turquoise
    break;
  case -1877172025:
    res = 0xFFFFF500; // turquoise1
    break;
  case -1877172024:
    res = 0xFFEEE500; // turquoise2
    break;
  case -1877172023:
    res = 0xFFCDC500; // turquoise3
    break;
  case -1877172022:
    res = 0xFF8B8600; // turquoise4
    break;
  case 583697112:
    res = 0xFFEE82EE; // violet
    break;
  case -297032141:
    res = 0xFF9020D0; // violetred
    break;
  case -1212126012:
    res = 0xFF963EFF; // violetred1
    break;
  case -1212126011:
    res = 0xFF8C3AEE; // violetred2
    break;
  case -1212126010:
    res = 0xFF7832CD; // violetred3
    break;
  case -1212126009:
    res = 0xFF52228B; // violetred4
    break;
  case 279127582:
    res = 0xFFB3DEF5; // wheat
    break;
  case 621275663:
    res = 0xFFBAE7FF; // wheat1
    break;
  case 621275664:
    res = 0xFFAED8EE; // wheat2
    break;
  case 621275665:
    res = 0xFF96BACD; // wheat3
    break;
  case 621275666:
    res = 0xFF667E8B; // wheat4
    break;
  case 629197541:
    res = 0xFFF5F5F5; // whitesmoke
    break;
  case 696252129:
    res = 0xFF00FFFF; // yellow
    break;
  case 1501483826:
    res = 0xFF00FFFF; // yellow1
    break;
  case 1501483827:
    res = 0xFF00EEEE; // yellow2
    break;
  case 1501483828:
    res = 0xFF00CDCD; // yellow3
    break;
  case 1501483829:
    res = 0xFF008B8B; // yellow4
    break;
  case -632650318:
    res = 0xFF32CD9A; // yellowgreen
    break;
  default:
    Rprintf("Unknown colour: %s\n", rcol);
    res = 16777215; // #ffffff00
  }
  return res;
}
