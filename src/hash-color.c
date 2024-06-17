#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include "hash-color.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Colour names as returned by "colors()"
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
char *col_name[] = {
  "NA", "transparent",
  "white", "aliceblue", "antiquewhite", "antiquewhite1", "antiquewhite2", 
  "antiquewhite3", "antiquewhite4", "aquamarine", "aquamarine1", 
  "aquamarine2", "aquamarine3", "aquamarine4", "azure", "azure1", 
  "azure2", "azure3", "azure4", "beige", "bisque", "bisque1", "bisque2", 
  "bisque3", "bisque4", "black", "blanchedalmond", "blue", "blue1", 
  "blue2", "blue3", "blue4", "blueviolet", "brown", "brown1", "brown2", 
  "brown3", "brown4", "burlywood", "burlywood1", "burlywood2", 
  "burlywood3", "burlywood4", "cadetblue", "cadetblue1", "cadetblue2", 
  "cadetblue3", "cadetblue4", "chartreuse", "chartreuse1", "chartreuse2", 
  "chartreuse3", "chartreuse4", "chocolate", "chocolate1", "chocolate2", 
  "chocolate3", "chocolate4", "coral", "coral1", "coral2", "coral3", 
  "coral4", "cornflowerblue", "cornsilk", "cornsilk1", "cornsilk2", 
  "cornsilk3", "cornsilk4", "cyan", "cyan1", "cyan2", "cyan3", 
  "cyan4", "darkblue", "darkcyan", "darkgoldenrod", "darkgoldenrod1", 
  "darkgoldenrod2", "darkgoldenrod3", "darkgoldenrod4", "darkgray", 
  "darkgreen", "darkgrey", "darkkhaki", "darkmagenta", "darkolivegreen", 
  "darkolivegreen1", "darkolivegreen2", "darkolivegreen3", "darkolivegreen4", 
  "darkorange", "darkorange1", "darkorange2", "darkorange3", "darkorange4", 
  "darkorchid", "darkorchid1", "darkorchid2", "darkorchid3", "darkorchid4", 
  "darkred", "darksalmon", "darkseagreen", "darkseagreen1", "darkseagreen2", 
  "darkseagreen3", "darkseagreen4", "darkslateblue", "darkslategray", 
  "darkslategray1", "darkslategray2", "darkslategray3", "darkslategray4", 
  "darkslategrey", "darkturquoise", "darkviolet", "deeppink", "deeppink1", 
  "deeppink2", "deeppink3", "deeppink4", "deepskyblue", "deepskyblue1", 
  "deepskyblue2", "deepskyblue3", "deepskyblue4", "dimgray", "dimgrey", 
  "dodgerblue", "dodgerblue1", "dodgerblue2", "dodgerblue3", "dodgerblue4", 
  "firebrick", "firebrick1", "firebrick2", "firebrick3", "firebrick4", 
  "floralwhite", "forestgreen", "gainsboro", "ghostwhite", "gold", 
  "gold1", "gold2", "gold3", "gold4", "goldenrod", "goldenrod1", 
  "goldenrod2", "goldenrod3", "goldenrod4", "gray", "gray0", "gray1", 
  "gray2", "gray3", "gray4", "gray5", "gray6", "gray7", "gray8", 
  "gray9", "gray10", "gray11", "gray12", "gray13", "gray14", "gray15", 
  "gray16", "gray17", "gray18", "gray19", "gray20", "gray21", "gray22", 
  "gray23", "gray24", "gray25", "gray26", "gray27", "gray28", "gray29", 
  "gray30", "gray31", "gray32", "gray33", "gray34", "gray35", "gray36", 
  "gray37", "gray38", "gray39", "gray40", "gray41", "gray42", "gray43", 
  "gray44", "gray45", "gray46", "gray47", "gray48", "gray49", "gray50", 
  "gray51", "gray52", "gray53", "gray54", "gray55", "gray56", "gray57", 
  "gray58", "gray59", "gray60", "gray61", "gray62", "gray63", "gray64", 
  "gray65", "gray66", "gray67", "gray68", "gray69", "gray70", "gray71", 
  "gray72", "gray73", "gray74", "gray75", "gray76", "gray77", "gray78", 
  "gray79", "gray80", "gray81", "gray82", "gray83", "gray84", "gray85", 
  "gray86", "gray87", "gray88", "gray89", "gray90", "gray91", "gray92", 
  "gray93", "gray94", "gray95", "gray96", "gray97", "gray98", "gray99", 
  "gray100", "green", "green1", "green2", "green3", "green4", "greenyellow", 
  "grey", "grey0", "grey1", "grey2", "grey3", "grey4", "grey5", 
  "grey6", "grey7", "grey8", "grey9", "grey10", "grey11", "grey12", 
  "grey13", "grey14", "grey15", "grey16", "grey17", "grey18", "grey19", 
  "grey20", "grey21", "grey22", "grey23", "grey24", "grey25", "grey26", 
  "grey27", "grey28", "grey29", "grey30", "grey31", "grey32", "grey33", 
  "grey34", "grey35", "grey36", "grey37", "grey38", "grey39", "grey40", 
  "grey41", "grey42", "grey43", "grey44", "grey45", "grey46", "grey47", 
  "grey48", "grey49", "grey50", "grey51", "grey52", "grey53", "grey54", 
  "grey55", "grey56", "grey57", "grey58", "grey59", "grey60", "grey61", 
  "grey62", "grey63", "grey64", "grey65", "grey66", "grey67", "grey68", 
  "grey69", "grey70", "grey71", "grey72", "grey73", "grey74", "grey75", 
  "grey76", "grey77", "grey78", "grey79", "grey80", "grey81", "grey82", 
  "grey83", "grey84", "grey85", "grey86", "grey87", "grey88", "grey89", 
  "grey90", "grey91", "grey92", "grey93", "grey94", "grey95", "grey96", 
  "grey97", "grey98", "grey99", "grey100", "honeydew", "honeydew1", 
  "honeydew2", "honeydew3", "honeydew4", "hotpink", "hotpink1", 
  "hotpink2", "hotpink3", "hotpink4", "indianred", "indianred1", 
  "indianred2", "indianred3", "indianred4", "ivory", "ivory1", 
  "ivory2", "ivory3", "ivory4", "khaki", "khaki1", "khaki2", "khaki3", 
  "khaki4", "lavender", "lavenderblush", "lavenderblush1", "lavenderblush2", 
  "lavenderblush3", "lavenderblush4", "lawngreen", "lemonchiffon", 
  "lemonchiffon1", "lemonchiffon2", "lemonchiffon3", "lemonchiffon4", 
  "lightblue", "lightblue1", "lightblue2", "lightblue3", "lightblue4", 
  "lightcoral", "lightcyan", "lightcyan1", "lightcyan2", "lightcyan3", 
  "lightcyan4", "lightgoldenrod", "lightgoldenrod1", "lightgoldenrod2", 
  "lightgoldenrod3", "lightgoldenrod4", "lightgoldenrodyellow", 
  "lightgray", "lightgreen", "lightgrey", "lightpink", "lightpink1", 
  "lightpink2", "lightpink3", "lightpink4", "lightsalmon", "lightsalmon1", 
  "lightsalmon2", "lightsalmon3", "lightsalmon4", "lightseagreen", 
  "lightskyblue", "lightskyblue1", "lightskyblue2", "lightskyblue3", 
  "lightskyblue4", "lightslateblue", "lightslategray", "lightslategrey", 
  "lightsteelblue", "lightsteelblue1", "lightsteelblue2", "lightsteelblue3", 
  "lightsteelblue4", "lightyellow", "lightyellow1", "lightyellow2", 
  "lightyellow3", "lightyellow4", "limegreen", "linen", "magenta", 
  "magenta1", "magenta2", "magenta3", "magenta4", "maroon", "maroon1", 
  "maroon2", "maroon3", "maroon4", "mediumaquamarine", "mediumblue", 
  "mediumorchid", "mediumorchid1", "mediumorchid2", "mediumorchid3", 
  "mediumorchid4", "mediumpurple", "mediumpurple1", "mediumpurple2", 
  "mediumpurple3", "mediumpurple4", "mediumseagreen", "mediumslateblue", 
  "mediumspringgreen", "mediumturquoise", "mediumvioletred", "midnightblue", 
  "mintcream", "mistyrose", "mistyrose1", "mistyrose2", "mistyrose3", 
  "mistyrose4", "moccasin", "navajowhite", "navajowhite1", "navajowhite2", 
  "navajowhite3", "navajowhite4", "navy", "navyblue", "oldlace", 
  "olivedrab", "olivedrab1", "olivedrab2", "olivedrab3", "olivedrab4", 
  "orange", "orange1", "orange2", "orange3", "orange4", "orangered", 
  "orangered1", "orangered2", "orangered3", "orangered4", "orchid", 
  "orchid1", "orchid2", "orchid3", "orchid4", "palegoldenrod", 
  "palegreen", "palegreen1", "palegreen2", "palegreen3", "palegreen4", 
  "paleturquoise", "paleturquoise1", "paleturquoise2", "paleturquoise3", 
  "paleturquoise4", "palevioletred", "palevioletred1", "palevioletred2", 
  "palevioletred3", "palevioletred4", "papayawhip", "peachpuff", 
  "peachpuff1", "peachpuff2", "peachpuff3", "peachpuff4", "peru", 
  "pink", "pink1", "pink2", "pink3", "pink4", "plum", "plum1", 
  "plum2", "plum3", "plum4", "powderblue", "purple", "purple1", 
  "purple2", "purple3", "purple4", "red", "red1", "red2", "red3", 
  "red4", "rosybrown", "rosybrown1", "rosybrown2", "rosybrown3", 
  "rosybrown4", "royalblue", "royalblue1", "royalblue2", "royalblue3", 
  "royalblue4", "saddlebrown", "salmon", "salmon1", "salmon2", 
  "salmon3", "salmon4", "sandybrown", "seagreen", "seagreen1", 
  "seagreen2", "seagreen3", "seagreen4", "seashell", "seashell1", 
  "seashell2", "seashell3", "seashell4", "sienna", "sienna1", "sienna2", 
  "sienna3", "sienna4", "skyblue", "skyblue1", "skyblue2", "skyblue3", 
  "skyblue4", "slateblue", "slateblue1", "slateblue2", "slateblue3", 
  "slateblue4", "slategray", "slategray1", "slategray2", "slategray3", 
  "slategray4", "slategrey", "snow", "snow1", "snow2", "snow3", 
  "snow4", "springgreen", "springgreen1", "springgreen2", "springgreen3", 
  "springgreen4", "steelblue", "steelblue1", "steelblue2", "steelblue3", 
  "steelblue4", "tan", "tan1", "tan2", "tan3", "tan4", "thistle", 
  "thistle1", "thistle2", "thistle3", "thistle4", "tomato", "tomato1", 
  "tomato2", "tomato3", "tomato4", "turquoise", "turquoise1", "turquoise2", 
  "turquoise3", "turquoise4", "violet", "violetred", "violetred1", 
  "violetred2", "violetred3", "violetred4", "wheat", "wheat1", 
  "wheat2", "wheat3", "wheat4", "whitesmoke", "yellow", "yellow1", 
  "yellow2", "yellow3", "yellow4", "yellowgreen"
};


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Actual colour values
//
// Mostly created with the following code:
//
//  mat <- t(col2rgb(colours(), alpha = TRUE))
//  apply(mat, 1, \(x) paste0("{", paste(x, collapse = ", "), "},")) |>
//  cat(sep = "\n")
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static uint32_t packed_int[659] = {
  0x00FFFFFF, // NA
  0x00FFFFFF, // transparent
  0xFFFFFFFF, // white
  0xFFFFF8F0, // aliceblue
  0xFFD7EBFA, // antiquewhite
  0xFFDBEFFF, // antiquewhite1
  0xFFCCDFEE, // antiquewhite2
  0xFFB0C0CD, // antiquewhite3
  0xFF78838B, // antiquewhite4
  0xFFD4FF7F, // aquamarine
  0xFFD4FF7F, // aquamarine1
  0xFFC6EE76, // aquamarine2
  0xFFAACD66, // aquamarine3
  0xFF748B45, // aquamarine4
  0xFFFFFFF0, // azure
  0xFFFFFFF0, // azure1
  0xFFEEEEE0, // azure2
  0xFFCDCDC1, // azure3
  0xFF8B8B83, // azure4
  0xFFDCF5F5, // beige
  0xFFC4E4FF, // bisque
  0xFFC4E4FF, // bisque1
  0xFFB7D5EE, // bisque2
  0xFF9EB7CD, // bisque3
  0xFF6B7D8B, // bisque4
  0xFF000000, // black
  0xFFCDEBFF, // blanchedalmond
  0xFFFF0000, // blue
  0xFFFF0000, // blue1
  0xFFEE0000, // blue2
  0xFFCD0000, // blue3
  0xFF8B0000, // blue4
  0xFFE22B8A, // blueviolet
  0xFF2A2AA5, // brown
  0xFF4040FF, // brown1
  0xFF3B3BEE, // brown2
  0xFF3333CD, // brown3
  0xFF23238B, // brown4
  0xFF87B8DE, // burlywood
  0xFF9BD3FF, // burlywood1
  0xFF91C5EE, // burlywood2
  0xFF7DAACD, // burlywood3
  0xFF55738B, // burlywood4
  0xFFA09E5F, // cadetblue
  0xFFFFF598, // cadetblue1
  0xFFEEE58E, // cadetblue2
  0xFFCDC57A, // cadetblue3
  0xFF8B8653, // cadetblue4
  0xFF00FF7F, // chartreuse
  0xFF00FF7F, // chartreuse1
  0xFF00EE76, // chartreuse2
  0xFF00CD66, // chartreuse3
  0xFF008B45, // chartreuse4
  0xFF1E69D2, // chocolate
  0xFF247FFF, // chocolate1
  0xFF2176EE, // chocolate2
  0xFF1D66CD, // chocolate3
  0xFF13458B, // chocolate4
  0xFF507FFF, // coral
  0xFF5672FF, // coral1
  0xFF506AEE, // coral2
  0xFF455BCD, // coral3
  0xFF2F3E8B, // coral4
  0xFFED9564, // cornflowerblue
  0xFFDCF8FF, // cornsilk
  0xFFDCF8FF, // cornsilk1
  0xFFCDE8EE, // cornsilk2
  0xFFB1C8CD, // cornsilk3
  0xFF78888B, // cornsilk4
  0xFFFFFF00, // cyan
  0xFFFFFF00, // cyan1
  0xFFEEEE00, // cyan2
  0xFFCDCD00, // cyan3
  0xFF8B8B00, // cyan4
  0xFF8B0000, // darkblue
  0xFF8B8B00, // darkcyan
  0xFF0B86B8, // darkgoldenrod
  0xFF0FB9FF, // darkgoldenrod1
  0xFF0EADEE, // darkgoldenrod2
  0xFF0C95CD, // darkgoldenrod3
  0xFF08658B, // darkgoldenrod4
  0xFFA9A9A9, // darkgray
  0xFF006400, // darkgreen
  0xFFA9A9A9, // darkgrey
  0xFF6BB7BD, // darkkhaki
  0xFF8B008B, // darkmagenta
  0xFF2F6B55, // darkolivegreen
  0xFF70FFCA, // darkolivegreen1
  0xFF68EEBC, // darkolivegreen2
  0xFF5ACDA2, // darkolivegreen3
  0xFF3D8B6E, // darkolivegreen4
  0xFF008CFF, // darkorange
  0xFF007FFF, // darkorange1
  0xFF0076EE, // darkorange2
  0xFF0066CD, // darkorange3
  0xFF00458B, // darkorange4
  0xFFCC3299, // darkorchid
  0xFFFF3EBF, // darkorchid1
  0xFFEE3AB2, // darkorchid2
  0xFFCD329A, // darkorchid3
  0xFF8B2268, // darkorchid4
  0xFF00008B, // darkred
  0xFF7A96E9, // darksalmon
  0xFF8FBC8F, // darkseagreen
  0xFFC1FFC1, // darkseagreen1
  0xFFB4EEB4, // darkseagreen2
  0xFF9BCD9B, // darkseagreen3
  0xFF698B69, // darkseagreen4
  0xFF8B3D48, // darkslateblue
  0xFF4F4F2F, // darkslategray
  0xFFFFFF97, // darkslategray1
  0xFFEEEE8D, // darkslategray2
  0xFFCDCD79, // darkslategray3
  0xFF8B8B52, // darkslategray4
  0xFF4F4F2F, // darkslategrey
  0xFFD1CE00, // darkturquoise
  0xFFD30094, // darkviolet
  0xFF9314FF, // deeppink
  0xFF9314FF, // deeppink1
  0xFF8912EE, // deeppink2
  0xFF7610CD, // deeppink3
  0xFF500A8B, // deeppink4
  0xFFFFBF00, // deepskyblue
  0xFFFFBF00, // deepskyblue1
  0xFFEEB200, // deepskyblue2
  0xFFCD9A00, // deepskyblue3
  0xFF8B6800, // deepskyblue4
  0xFF696969, // dimgray
  0xFF696969, // dimgrey
  0xFFFF901E, // dodgerblue
  0xFFFF901E, // dodgerblue1
  0xFFEE861C, // dodgerblue2
  0xFFCD7418, // dodgerblue3
  0xFF8B4E10, // dodgerblue4
  0xFF2222B2, // firebrick
  0xFF3030FF, // firebrick1
  0xFF2C2CEE, // firebrick2
  0xFF2626CD, // firebrick3
  0xFF1A1A8B, // firebrick4
  0xFFF0FAFF, // floralwhite
  0xFF228B22, // forestgreen
  0xFFDCDCDC, // gainsboro
  0xFFFFF8F8, // ghostwhite
  0xFF00D7FF, // gold
  0xFF00D7FF, // gold1
  0xFF00C9EE, // gold2
  0xFF00ADCD, // gold3
  0xFF00758B, // gold4
  0xFF20A5DA, // goldenrod
  0xFF25C1FF, // goldenrod1
  0xFF22B4EE, // goldenrod2
  0xFF1D9BCD, // goldenrod3
  0xFF14698B, // goldenrod4
  0xFFBEBEBE, // gray
  0xFF000000, // gray0
  0xFF030303, // gray1
  0xFF050505, // gray2
  0xFF080808, // gray3
  0xFF0A0A0A, // gray4
  0xFF0D0D0D, // gray5
  0xFF0F0F0F, // gray6
  0xFF121212, // gray7
  0xFF141414, // gray8
  0xFF171717, // gray9
  0xFF1A1A1A, // gray10
  0xFF1C1C1C, // gray11
  0xFF1F1F1F, // gray12
  0xFF212121, // gray13
  0xFF242424, // gray14
  0xFF262626, // gray15
  0xFF292929, // gray16
  0xFF2B2B2B, // gray17
  0xFF2E2E2E, // gray18
  0xFF303030, // gray19
  0xFF333333, // gray20
  0xFF363636, // gray21
  0xFF383838, // gray22
  0xFF3B3B3B, // gray23
  0xFF3D3D3D, // gray24
  0xFF404040, // gray25
  0xFF424242, // gray26
  0xFF454545, // gray27
  0xFF474747, // gray28
  0xFF4A4A4A, // gray29
  0xFF4D4D4D, // gray30
  0xFF4F4F4F, // gray31
  0xFF525252, // gray32
  0xFF545454, // gray33
  0xFF575757, // gray34
  0xFF595959, // gray35
  0xFF5C5C5C, // gray36
  0xFF5E5E5E, // gray37
  0xFF616161, // gray38
  0xFF636363, // gray39
  0xFF666666, // gray40
  0xFF696969, // gray41
  0xFF6B6B6B, // gray42
  0xFF6E6E6E, // gray43
  0xFF707070, // gray44
  0xFF737373, // gray45
  0xFF757575, // gray46
  0xFF787878, // gray47
  0xFF7A7A7A, // gray48
  0xFF7D7D7D, // gray49
  0xFF7F7F7F, // gray50
  0xFF828282, // gray51
  0xFF858585, // gray52
  0xFF878787, // gray53
  0xFF8A8A8A, // gray54
  0xFF8C8C8C, // gray55
  0xFF8F8F8F, // gray56
  0xFF919191, // gray57
  0xFF949494, // gray58
  0xFF969696, // gray59
  0xFF999999, // gray60
  0xFF9C9C9C, // gray61
  0xFF9E9E9E, // gray62
  0xFFA1A1A1, // gray63
  0xFFA3A3A3, // gray64
  0xFFA6A6A6, // gray65
  0xFFA8A8A8, // gray66
  0xFFABABAB, // gray67
  0xFFADADAD, // gray68
  0xFFB0B0B0, // gray69
  0xFFB3B3B3, // gray70
  0xFFB5B5B5, // gray71
  0xFFB8B8B8, // gray72
  0xFFBABABA, // gray73
  0xFFBDBDBD, // gray74
  0xFFBFBFBF, // gray75
  0xFFC2C2C2, // gray76
  0xFFC4C4C4, // gray77
  0xFFC7C7C7, // gray78
  0xFFC9C9C9, // gray79
  0xFFCCCCCC, // gray80
  0xFFCFCFCF, // gray81
  0xFFD1D1D1, // gray82
  0xFFD4D4D4, // gray83
  0xFFD6D6D6, // gray84
  0xFFD9D9D9, // gray85
  0xFFDBDBDB, // gray86
  0xFFDEDEDE, // gray87
  0xFFE0E0E0, // gray88
  0xFFE3E3E3, // gray89
  0xFFE5E5E5, // gray90
  0xFFE8E8E8, // gray91
  0xFFEBEBEB, // gray92
  0xFFEDEDED, // gray93
  0xFFF0F0F0, // gray94
  0xFFF2F2F2, // gray95
  0xFFF5F5F5, // gray96
  0xFFF7F7F7, // gray97
  0xFFFAFAFA, // gray98
  0xFFFCFCFC, // gray99
  0xFFFFFFFF, // gray100
  0xFF00FF00, // green
  0xFF00FF00, // green1
  0xFF00EE00, // green2
  0xFF00CD00, // green3
  0xFF008B00, // green4
  0xFF2FFFAD, // greenyellow
  0xFFBEBEBE, // grey
  0xFF000000, // grey0
  0xFF030303, // grey1
  0xFF050505, // grey2
  0xFF080808, // grey3
  0xFF0A0A0A, // grey4
  0xFF0D0D0D, // grey5
  0xFF0F0F0F, // grey6
  0xFF121212, // grey7
  0xFF141414, // grey8
  0xFF171717, // grey9
  0xFF1A1A1A, // grey10
  0xFF1C1C1C, // grey11
  0xFF1F1F1F, // grey12
  0xFF212121, // grey13
  0xFF242424, // grey14
  0xFF262626, // grey15
  0xFF292929, // grey16
  0xFF2B2B2B, // grey17
  0xFF2E2E2E, // grey18
  0xFF303030, // grey19
  0xFF333333, // grey20
  0xFF363636, // grey21
  0xFF383838, // grey22
  0xFF3B3B3B, // grey23
  0xFF3D3D3D, // grey24
  0xFF404040, // grey25
  0xFF424242, // grey26
  0xFF454545, // grey27
  0xFF474747, // grey28
  0xFF4A4A4A, // grey29
  0xFF4D4D4D, // grey30
  0xFF4F4F4F, // grey31
  0xFF525252, // grey32
  0xFF545454, // grey33
  0xFF575757, // grey34
  0xFF595959, // grey35
  0xFF5C5C5C, // grey36
  0xFF5E5E5E, // grey37
  0xFF616161, // grey38
  0xFF636363, // grey39
  0xFF666666, // grey40
  0xFF696969, // grey41
  0xFF6B6B6B, // grey42
  0xFF6E6E6E, // grey43
  0xFF707070, // grey44
  0xFF737373, // grey45
  0xFF757575, // grey46
  0xFF787878, // grey47
  0xFF7A7A7A, // grey48
  0xFF7D7D7D, // grey49
  0xFF7F7F7F, // grey50
  0xFF828282, // grey51
  0xFF858585, // grey52
  0xFF878787, // grey53
  0xFF8A8A8A, // grey54
  0xFF8C8C8C, // grey55
  0xFF8F8F8F, // grey56
  0xFF919191, // grey57
  0xFF949494, // grey58
  0xFF969696, // grey59
  0xFF999999, // grey60
  0xFF9C9C9C, // grey61
  0xFF9E9E9E, // grey62
  0xFFA1A1A1, // grey63
  0xFFA3A3A3, // grey64
  0xFFA6A6A6, // grey65
  0xFFA8A8A8, // grey66
  0xFFABABAB, // grey67
  0xFFADADAD, // grey68
  0xFFB0B0B0, // grey69
  0xFFB3B3B3, // grey70
  0xFFB5B5B5, // grey71
  0xFFB8B8B8, // grey72
  0xFFBABABA, // grey73
  0xFFBDBDBD, // grey74
  0xFFBFBFBF, // grey75
  0xFFC2C2C2, // grey76
  0xFFC4C4C4, // grey77
  0xFFC7C7C7, // grey78
  0xFFC9C9C9, // grey79
  0xFFCCCCCC, // grey80
  0xFFCFCFCF, // grey81
  0xFFD1D1D1, // grey82
  0xFFD4D4D4, // grey83
  0xFFD6D6D6, // grey84
  0xFFD9D9D9, // grey85
  0xFFDBDBDB, // grey86
  0xFFDEDEDE, // grey87
  0xFFE0E0E0, // grey88
  0xFFE3E3E3, // grey89
  0xFFE5E5E5, // grey90
  0xFFE8E8E8, // grey91
  0xFFEBEBEB, // grey92
  0xFFEDEDED, // grey93
  0xFFF0F0F0, // grey94
  0xFFF2F2F2, // grey95
  0xFFF5F5F5, // grey96
  0xFFF7F7F7, // grey97
  0xFFFAFAFA, // grey98
  0xFFFCFCFC, // grey99
  0xFFFFFFFF, // grey100
  0xFFF0FFF0, // honeydew
  0xFFF0FFF0, // honeydew1
  0xFFE0EEE0, // honeydew2
  0xFFC1CDC1, // honeydew3
  0xFF838B83, // honeydew4
  0xFFB469FF, // hotpink
  0xFFB46EFF, // hotpink1
  0xFFA76AEE, // hotpink2
  0xFF9060CD, // hotpink3
  0xFF623A8B, // hotpink4
  0xFF5C5CCD, // indianred
  0xFF6A6AFF, // indianred1
  0xFF6363EE, // indianred2
  0xFF5555CD, // indianred3
  0xFF3A3A8B, // indianred4
  0xFFF0FFFF, // ivory
  0xFFF0FFFF, // ivory1
  0xFFE0EEEE, // ivory2
  0xFFC1CDCD, // ivory3
  0xFF838B8B, // ivory4
  0xFF8CE6F0, // khaki
  0xFF8FF6FF, // khaki1
  0xFF85E6EE, // khaki2
  0xFF73C6CD, // khaki3
  0xFF4E868B, // khaki4
  0xFFFAE6E6, // lavender
  0xFFF5F0FF, // lavenderblush
  0xFFF5F0FF, // lavenderblush1
  0xFFE5E0EE, // lavenderblush2
  0xFFC5C1CD, // lavenderblush3
  0xFF86838B, // lavenderblush4
  0xFF00FC7C, // lawngreen
  0xFFCDFAFF, // lemonchiffon
  0xFFCDFAFF, // lemonchiffon1
  0xFFBFE9EE, // lemonchiffon2
  0xFFA5C9CD, // lemonchiffon3
  0xFF70898B, // lemonchiffon4
  0xFFE6D8AD, // lightblue
  0xFFFFEFBF, // lightblue1
  0xFFEEDFB2, // lightblue2
  0xFFCDC09A, // lightblue3
  0xFF8B8368, // lightblue4
  0xFF8080F0, // lightcoral
  0xFFFFFFE0, // lightcyan
  0xFFFFFFE0, // lightcyan1
  0xFFEEEED1, // lightcyan2
  0xFFCDCDB4, // lightcyan3
  0xFF8B8B7A, // lightcyan4
  0xFF82DDEE, // lightgoldenrod
  0xFF8BECFF, // lightgoldenrod1
  0xFF82DCEE, // lightgoldenrod2
  0xFF70BECD, // lightgoldenrod3
  0xFF4C818B, // lightgoldenrod4
  0xFFD2FAFA, // lightgoldenrodyellow
  0xFFD3D3D3, // lightgray
  0xFF90EE90, // lightgreen
  0xFFD3D3D3, // lightgrey
  0xFFC1B6FF, // lightpink
  0xFFB9AEFF, // lightpink1
  0xFFADA2EE, // lightpink2
  0xFF958CCD, // lightpink3
  0xFF655F8B, // lightpink4
  0xFF7AA0FF, // lightsalmon
  0xFF7AA0FF, // lightsalmon1
  0xFF7295EE, // lightsalmon2
  0xFF6281CD, // lightsalmon3
  0xFF42578B, // lightsalmon4
  0xFFAAB220, // lightseagreen
  0xFFFACE87, // lightskyblue
  0xFFFFE2B0, // lightskyblue1
  0xFFEED3A4, // lightskyblue2
  0xFFCDB68D, // lightskyblue3
  0xFF8B7B60, // lightskyblue4
  0xFFFF7084, // lightslateblue
  0xFF998877, // lightslategray
  0xFF998877, // lightslategrey
  0xFFDEC4B0, // lightsteelblue
  0xFFFFE1CA, // lightsteelblue1
  0xFFEED2BC, // lightsteelblue2
  0xFFCDB5A2, // lightsteelblue3
  0xFF8B7B6E, // lightsteelblue4
  0xFFE0FFFF, // lightyellow
  0xFFE0FFFF, // lightyellow1
  0xFFD1EEEE, // lightyellow2
  0xFFB4CDCD, // lightyellow3
  0xFF7A8B8B, // lightyellow4
  0xFF32CD32, // limegreen
  0xFFE6F0FA, // linen
  0xFFFF00FF, // magenta
  0xFFFF00FF, // magenta1
  0xFFEE00EE, // magenta2
  0xFFCD00CD, // magenta3
  0xFF8B008B, // magenta4
  0xFF6030B0, // maroon
  0xFFB334FF, // maroon1
  0xFFA730EE, // maroon2
  0xFF9029CD, // maroon3
  0xFF621C8B, // maroon4
  0xFFAACD66, // mediumaquamarine
  0xFFCD0000, // mediumblue
  0xFFD355BA, // mediumorchid
  0xFFFF66E0, // mediumorchid1
  0xFFEE5FD1, // mediumorchid2
  0xFFCD52B4, // mediumorchid3
  0xFF8B377A, // mediumorchid4
  0xFFDB7093, // mediumpurple
  0xFFFF82AB, // mediumpurple1
  0xFFEE799F, // mediumpurple2
  0xFFCD6889, // mediumpurple3
  0xFF8B475D, // mediumpurple4
  0xFF71B33C, // mediumseagreen
  0xFFEE687B, // mediumslateblue
  0xFF9AFA00, // mediumspringgreen
  0xFFCCD148, // mediumturquoise
  0xFF8515C7, // mediumvioletred
  0xFF701919, // midnightblue
  0xFFFAFFF5, // mintcream
  0xFFE1E4FF, // mistyrose
  0xFFE1E4FF, // mistyrose1
  0xFFD2D5EE, // mistyrose2
  0xFFB5B7CD, // mistyrose3
  0xFF7B7D8B, // mistyrose4
  0xFFB5E4FF, // moccasin
  0xFFADDEFF, // navajowhite
  0xFFADDEFF, // navajowhite1
  0xFFA1CFEE, // navajowhite2
  0xFF8BB3CD, // navajowhite3
  0xFF5E798B, // navajowhite4
  0xFF800000, // navy
  0xFF800000, // navyblue
  0xFFE6F5FD, // oldlace
  0xFF238E6B, // olivedrab
  0xFF3EFFC0, // olivedrab1
  0xFF3AEEB3, // olivedrab2
  0xFF32CD9A, // olivedrab3
  0xFF228B69, // olivedrab4
  0xFF00A5FF, // orange
  0xFF00A5FF, // orange1
  0xFF009AEE, // orange2
  0xFF0085CD, // orange3
  0xFF005A8B, // orange4
  0xFF0045FF, // orangered
  0xFF0045FF, // orangered1
  0xFF0040EE, // orangered2
  0xFF0037CD, // orangered3
  0xFF00258B, // orangered4
  0xFFD670DA, // orchid
  0xFFFA83FF, // orchid1
  0xFFE97AEE, // orchid2
  0xFFC969CD, // orchid3
  0xFF89478B, // orchid4
  0xFFAAE8EE, // palegoldenrod
  0xFF98FB98, // palegreen
  0xFF9AFF9A, // palegreen1
  0xFF90EE90, // palegreen2
  0xFF7CCD7C, // palegreen3
  0xFF548B54, // palegreen4
  0xFFEEEEAF, // paleturquoise
  0xFFFFFFBB, // paleturquoise1
  0xFFEEEEAE, // paleturquoise2
  0xFFCDCD96, // paleturquoise3
  0xFF8B8B66, // paleturquoise4
  0xFF9370DB, // palevioletred
  0xFFAB82FF, // palevioletred1
  0xFF9F79EE, // palevioletred2
  0xFF8968CD, // palevioletred3
  0xFF5D478B, // palevioletred4
  0xFFD5EFFF, // papayawhip
  0xFFB9DAFF, // peachpuff
  0xFFB9DAFF, // peachpuff1
  0xFFADCBEE, // peachpuff2
  0xFF95AFCD, // peachpuff3
  0xFF65778B, // peachpuff4
  0xFF3F85CD, // peru
  0xFFCBC0FF, // pink
  0xFFC5B5FF, // pink1
  0xFFB8A9EE, // pink2
  0xFF9E91CD, // pink3
  0xFF6C638B, // pink4
  0xFFDDA0DD, // plum
  0xFFFFBBFF, // plum1
  0xFFEEAEEE, // plum2
  0xFFCD96CD, // plum3
  0xFF8B668B, // plum4
  0xFFE6E0B0, // powderblue
  0xFFF020A0, // purple
  0xFFFF309B, // purple1
  0xFFEE2C91, // purple2
  0xFFCD267D, // purple3
  0xFF8B1A55, // purple4
  0xFF0000FF, // red
  0xFF0000FF, // red1
  0xFF0000EE, // red2
  0xFF0000CD, // red3
  0xFF00008B, // red4
  0xFF8F8FBC, // rosybrown
  0xFFC1C1FF, // rosybrown1
  0xFFB4B4EE, // rosybrown2
  0xFF9B9BCD, // rosybrown3
  0xFF69698B, // rosybrown4
  0xFFE16941, // royalblue
  0xFFFF7648, // royalblue1
  0xFFEE6E43, // royalblue2
  0xFFCD5F3A, // royalblue3
  0xFF8B4027, // royalblue4
  0xFF13458B, // saddlebrown
  0xFF7280FA, // salmon
  0xFF698CFF, // salmon1
  0xFF6282EE, // salmon2
  0xFF5470CD, // salmon3
  0xFF394C8B, // salmon4
  0xFF60A4F4, // sandybrown
  0xFF578B2E, // seagreen
  0xFF9FFF54, // seagreen1
  0xFF94EE4E, // seagreen2
  0xFF80CD43, // seagreen3
  0xFF578B2E, // seagreen4
  0xFFEEF5FF, // seashell
  0xFFEEF5FF, // seashell1
  0xFFDEE5EE, // seashell2
  0xFFBFC5CD, // seashell3
  0xFF82868B, // seashell4
  0xFF2D52A0, // sienna
  0xFF4782FF, // sienna1
  0xFF4279EE, // sienna2
  0xFF3968CD, // sienna3
  0xFF26478B, // sienna4
  0xFFEBCE87, // skyblue
  0xFFFFCE87, // skyblue1
  0xFFEEC07E, // skyblue2
  0xFFCDA66C, // skyblue3
  0xFF8B704A, // skyblue4
  0xFFCD5A6A, // slateblue
  0xFFFF6F83, // slateblue1
  0xFFEE677A, // slateblue2
  0xFFCD5969, // slateblue3
  0xFF8B3C47, // slateblue4
  0xFF908070, // slategray
  0xFFFFE2C6, // slategray1
  0xFFEED3B9, // slategray2
  0xFFCDB69F, // slategray3
  0xFF8B7B6C, // slategray4
  0xFF908070, // slategrey
  0xFFFAFAFF, // snow
  0xFFFAFAFF, // snow1
  0xFFE9E9EE, // snow2
  0xFFC9C9CD, // snow3
  0xFF89898B, // snow4
  0xFF7FFF00, // springgreen
  0xFF7FFF00, // springgreen1
  0xFF76EE00, // springgreen2
  0xFF66CD00, // springgreen3
  0xFF458B00, // springgreen4
  0xFFB48246, // steelblue
  0xFFFFB863, // steelblue1
  0xFFEEAC5C, // steelblue2
  0xFFCD944F, // steelblue3
  0xFF8B6436, // steelblue4
  0xFF8CB4D2, // tan
  0xFF4FA5FF, // tan1
  0xFF499AEE, // tan2
  0xFF3F85CD, // tan3
  0xFF2B5A8B, // tan4
  0xFFD8BFD8, // thistle
  0xFFFFE1FF, // thistle1
  0xFFEED2EE, // thistle2
  0xFFCDB5CD, // thistle3
  0xFF8B7B8B, // thistle4
  0xFF4763FF, // tomato
  0xFF4763FF, // tomato1
  0xFF425CEE, // tomato2
  0xFF394FCD, // tomato3
  0xFF26368B, // tomato4
  0xFFD0E040, // turquoise
  0xFFFFF500, // turquoise1
  0xFFEEE500, // turquoise2
  0xFFCDC500, // turquoise3
  0xFF8B8600, // turquoise4
  0xFFEE82EE, // violet
  0xFF9020D0, // violetred
  0xFF963EFF, // violetred1
  0xFF8C3AEE, // violetred2
  0xFF7832CD, // violetred3
  0xFF52228B, // violetred4
  0xFFB3DEF5, // wheat
  0xFFBAE7FF, // wheat1
  0xFFAED8EE, // wheat2
  0xFF96BACD, // wheat3
  0xFF667E8B, // wheat4
  0xFFF5F5F5, // whitesmoke
  0xFF00FFFF, // yellow
  0xFF00FFFF, // yellow1
  0xFF00EEEE, // yellow2
  0xFF00CDCD, // yellow3
  0xFF008B8B, // yellow4
  0xFF32CD9A  // yellowgreen
};


/*
 * d=3
 * n=811
 * m=659
 * c=1.23
 * maxlen=20
 * minklen=2
 * maxklen=20
 * minchar=48
 * maxchar=122
 * loop=0
 * numiter=58
 * seed=
 */

static int g[] = {
  365, 49, 567, 498, 639, 6, 616, 90, 320, -1,
  20, 473, 207, 0, 154, 41, 92, 392, 0, 6,
  559, 371, 0, 403, 494, 283, 200, 428, 199, 101,
  542, 568, 652, 130, 586, 580, 0, 21, 638, 204,
  0, 583, 91, 188, 372, -1, 347, 383, 427, 472,
  152, 540, 584, 0, 86, 319, 133, 347, 298, 547,
  109, 168, 353, 135, 469, 294, 484, 540, 635, 0,
  501, 580, 400, -1, -1, -1, 613, 387, 245, 48,
  96, 0, 155, 37, 0, 119, 176, 324, 410, 304,
  367, 481, 21, 162, 267, 231, 266, 349, 654, 554,
  440, 233, 240, 536, 413, 281, 629, 20, -1, 565,
  56, 473, 216, -1, 41, 350, 370, 126, 54, 602,
  596, 435, 571, 488, 571, 221, 322, 0, 551, 319,
  530, -1, 289, 518, 272, 443, 161, 305, 181, 561,
  251, -1, 521, 19, 645, 649, 390, 605, 370, -1,
  226, 0, 201, 396, 370, 630, 558, 640, 303, 305,
  607, 161, 565, 0, 0, -1, 0, 627, 156, 86,
  27, 120, 213, -1, 404, 0, 509, 519, 650, 0,
  598, 0, 632, 434, 643, 240, 110, -1, 32, 222,
  493, 79, 12, 657, 295, 186, 638, 498, 188, 288,
  36, 618, 26, -1, 10, 98, 63, 278, 416, 367,
  57, 619, 279, 4, 430, 308, 415, -1, 595, 552,
  12, 61, 538, -1, 531, 251, 333, 27, 526, 349,
  465, 443, 655, 106, 235, 13, 296, -1, 271, -1,
  262, 429, 3, 301, 84, 597, 549, 185, 405, -1,
  300, 102, 146, 88, 517, -1, 503, 0, 35, 7,
  0, -1, 457, 626, 424, 277, 417, 203, 279, 456,
  239, 558, 370, -1, 584, 5, 484, 547, -1, 273,
  598, 255, 51, 142, 216, -1, 153, 368, 601, 246,
  501, 228, 185, 132, 140, 79, 10, 108, 482, 453,
  648, -1, 553, 0, 163, 148, 641, 437, 536, 216,
  263, 474, 163, 583, 351, 223, 96, 620, 367, 180,
  -1, 204, 242, 101, 565, 92, 423, 555, 343, 70,
  472, 502, 420, -1, 190, 0, 0, -1, 658, 429,
  482, 476, 376, 178, 263, 64, 274, 604, 45, 0,
  306, -1, 344, 533, 231, -1, 267, 118, 450, 12,
  349, 560, 0, 151, 340, 468, 191, 453, 449, 425,
  15, 451, 240, 654, 532, 0, 261, 547, 52, 0,
  0, 431, 469, 419, 510, 339, 443, 74, -1, 554,
  181, 178, 0, 103, 76, 514, 13, -1, 82, 613,
  206, 0, 306, 446, 519, 435, 163, 412, 385, 467,
  71, 149, 0, 290, 218, 536, 402, 64, 0, 580,
  194, 516, 0, 305, 382, 63, 48, 393, 435, 324,
  545, 128, 621, -1, 103, 351, 227, 458, 644, 385,
  0, 33, -1, 31, 145, 333, 53, 0, 500, 557,
  0, 182, 0, 156, 648, 410, 584, 218, 555, 299,
  0, 233, 292, 639, 42, 509, 434, 0, 397, 24,
  317, 638, 499, 394, 181, -1, 196, 438, 258, 481,
  654, 231, 553, 232, 654, 81, 457, 0, 218, 605,
  0, 272, 351, 33, 350, 574, 579, 634, 333, 139,
  634, 531, 0, 655, 0, 22, 0, 0, 532, 467,
  135, 0, 85, 306, 621, 638, -1, 432, 58, 47,
  69, 345, 586, 652, 475, -1, 248, 28, 0, 252,
  139, 63, 496, 20, 174, 229, 479, 268, 606, 375,
  38, 89, 470, 579, 383, 219, 208, 17, 74, 1,
  170, 379, 325, 478, 0, -1, 0, 0, 342, -1,
  166, 549, 503, -1, 592, 349, 19, 575, 558, 450,
  656, -1, -1, 0, 581, 358, 26, 479, 211, 214,
  191, 321, 409, 0, 121, 219, 13, 132, -1, 0,
  75, -1, 611, 190, 258, 66, 111, 0, 551, 0,
  488, 367, 406, 285, 610, 406, 472, 512, 320, 207,
  69, 0, 547, 92, -1, -1, 133, 0, 301, 381,
  376, 0, 646, 153, -1, 615, 38, 0, 13, 224,
  515, 441, 0, 430, 351, 20, 356, 47, 588, 205,
  463, 0, 355, 69, 0, -1, 29, -1, 116, 34,
  640, 371, 193, 321, 183, -1, 331, 187, 637, 158,
  293, 0, 148, 373, 0, 433, 219, 171, 112, 297,
  266, 11, 0, 652, 79, 47, 494, 364, 275, 290,
  371, 113, 240, 456, 196, 373, 449, 338, 23, -1,
  0, 364, 634, 614, 520, 49, 237, 607, 548, -1,
  0, 146, 636, -1, 581, 135, 112, 268, 333, 0,
  296, 0, 75, -1, -1, -1, 0, 631, 305, 394,
  0, 552, 523, 96, 236, 325, 0, 526, 421, 113,
  386, -1, 244, 102, 428, 298, 55, 198, 129, -1,
  0, 10, -1, 48, 597, 329, 0, -1, 561, 253,
  -1, 278, 393, 541, 310, 0, 605, 485, 0, 287,
  182, 17, 0, 467, 376, 567, 0, 368, 0, 582,
  518, 500, 559, -1, 207, 92, 604, 0, 463, 76,
  0, 0, 480, 441, 604, 437, 40, 320, 0, 26,
  -1, 0, 462, 0, 432, 301, 530, 0, 0, -1,
  0, 514, 413, 595, 0, 0, -1, 437, 442, 167,
  466, 
};

static int T0[] = {
  150, 727, 93, 425, 583, 763, 521, 407, 337, 237,
  424, 568, 234, 748, 680, 116, 517, 512, 687, 612,
  673, 435, 652, 646, 363, 303, 641, 448, 46, 155,
  163, 196, 629, 257, 369, 401, 767, 79, 809, 293,
  63, 422, 608, 298, 359, 477, 161, 623, 737, 37,
  425, 346, 473, 13, 181, 583, 316, 570, 220, 109,
  472, 384, 52, 291, 388, 421, 692, 91, 248, 437,
  131, 311, 607, 486, 609, 223, 513, 6, 475, 74,
  586, 509, 418, 540, 591, 401, 86, 353, 70, 503,
  226, 519, 231, 254, 651, 472, 504, 175, 680, 732,
  537, 373, 250, 587, 777, 712, 557, 479, 465, 779,
  553, 798, 224, 718, 527, 4, 308, 614, 104, 379,
  53, 330, 87, 284, 331, 485, 756, 24, 660, 372,
  504, 133, 746, 754, 720, 712, 655, 213, 381, 310,
  739, 681, 44, 152, 589, 572, 714, 86, 375, 7,
  726, 118, 92, 728, 555, 339, 740, 631, 501, 551,
  564, 393, 41, 247, 52, 454, 392, 737, 657, 467,
  477, 400, 760, 552, 675, 309, 214, 479, 752, 807,
  156, 667, 114, 248, 331, 416, 334, 260, 794, 24,
  558, 548, 165, 347, 542, 217, 548, 123, 701, 141,
  590, 115, 541, 539, 414, 405, 596, 628, 632, 284,
  624, 788, 698, 485, 784, 219, 90, 307, 226, 632,
  79, 532, 116, 244, 68, 552, 38, 642, 466, 23,
  359, 472, 425, 227, 200, 742, 140, 743, 425, 476,
  217, 39, 770, 352, 661, 782, 65, 623, 582, 120,
  626, 665, 410, 710, 357, 225, 198, 395, 614, 664,
  166, 162, 73, 591, 389, 273, 270, 529, 205, 442,
  194, 170, 482, 712, 269, 332, 683, 334, 702, 201,
  454, 517, 614, 53, 163, 160, 25, 361, 555, 639,
  773, 468, 801, 593, 807, 127, 55, 266, 656, 7,
  387, 310, 432, 497, 469, 501, 40, 430, 349, 793,
  737, 109, 516, 249, 554, 711, 239, 94, 421, 152,
  597, 39, 501, 474, 734, 477, 165, 278, 331, 608,
  807, 465, 665, 175, 709, 323, 676, 496, 500, 215,
  479, 426, 324, 184, 675, 625, 642, 661, 467, 252,
  2, 0, 291, 251, 475, 215, 728, 387, 240, 807,
  742, 236, 461, 344, 159, 360, 414, 24, 45, 104,
  239, 271, 530, 310, 455, 700, 25, 237, 766, 452,
  111, 771, 529, 536, 43, 106, 141, 368, 636, 314,
  285, 364, 373, 372, 320, 180, 538, 346, 304, 323,
  236, 768, 66, 112, 27, 130, 1, 52, 114, 515,
  252, 783, 222, 781, 509, 265, 634, 650, 380, 207,
  711, 412, 571, 273, 785, 638, 201, 259, 174, 252,
  582, 410, 209, 648, 522, 237, 526, 271, 36, 387,
  533, 288, 360, 755, 6, 58, 209, 640, 455, 589,
  597, 18, 614, 323, 433, 134, 733, 543, 674, 16,
  421, 740, 251, 744, 513, 575, 687, 169, 202, 755,
  145, 346, 505, 83, 558, 210, 371, 568, 732, 39,
  238, 265, 615, 41, 589, 237, 733, 258, 528, 597,
  274, 696, 273, 272, 629, 534, 36, 253, 703, 238,
  197, 37, 331, 450, 120, 79, 660, 238, 647, 328,
  277, 633, 594, 640, 421, 372, 624, 91, 377, 341,
  688, 651, 226, 708, 670, 27, 460, 585, 248, 709,
  509, 448, 11, 177, 502, 462, 11, 163, 711, 201,
  89, 793, 443, 451, 556, 703, 175, 64, 149, 775,
  630, 456, 374, 737, 580, 422, 764, 787, 196, 201,
  432, 705, 397, 443, 629, 646, 94, 387, 556, 552,
  588, 393, 535, 778, 33, 27, 417, 766, 650, 313,
  731, 469, 769, 294, 395, 538, 463, 348, 261, 407,
  297, 694, 48, 441, 73, 678, 23, 726, 254, 580,
  257, 765, 504, 498, 698, 204, 637, 697, 688, 602,
  573, 595, 653, 273, 58, 724, 33, 495, 5, 190,
  754, 338, 562, 469, 557, 600, 523, 769, 270, 650,
  433, 274, 351, 127, 773, 238, 331, 346, 682, 766,
  137, 445, 550, 538, 465, 355, 198, 498, 597, 203,
  689, 540, 542, 187, 757, 35, 787, 216, 551, 805,
  55, 173, 268, 153, 47, 788, 392, 378, 70, 10,
  80, 766, 455, 377, 493, 420, 736, 452, 502, 413,
  592, 784, 685, 535, 650, 726, 65, 358, 305, 665,
  142, 222, 402, 540, 401, 771, 756, 142, 664, 325,
  652, 607, 75, 277, 668, 612, 444, 593, 0, 693,
  753, 340, 667, 375, 64, 253, 290, 129, 612, 343,
  541, 754, 565, 132, 231, 713, 650, 734, 45, 503,
  806, 444, 300, 628, 721, 715, 429, 101, 245, 177,
  795, 187, 517, 651, 562, 581, 93, 600, 457, 452,
  673, 259, 234, 31, 506, 598, 415, 308, 227, 280,
  119, 8, 187, 280, 85, 225, 1, 12, 682, 453,
  472, 393, 252, 213, 666, 716, 357, 112, 108, 107,
  368, 781, 366, 349, 560, 620, 136, 164, 117, 110,
  191, 237, 676, 126, 264, 761, 351, 265, 520, 222,
  719, 182, 363, 718, 395, 218, 623, 753, 330, 479,
  49, 445, 196, 416, 794, 756, 783, 678, 667, 89,
  535, 606, 73, 401, 732, 189, 73, 627, 198, 64,
  126, 791, 522, 156, 769, 239, 614, 290, 133, 117,
  800, 35, 756, 326, 499, 391, 200, 775, 386, 49,
  349, 123, 248, 725, 426, 513, 661, 247, 329, 49,
  58, 456, 587, 580, 359, 293, 567, 163, 583, 700,
  280, 572, 482, 783, 645, 729, 363, 34, 693, 496,
  641, 789, 620, 637, 450, 235, 339, 301, 229, 668,
  97, 288, 60, 684, 615, 420, 166, 371, 583, 750,
  544, 575, 52, 710, 526, 437, 384, 627, 167, 688,
  181, 596, 333, 425, 315, 653, 128, 719, 392, 722,
  549, 734, 291, 779, 454, 172, 481, 221, 543, 680,
  598, 276, 444, 397, 175, 717, 582, 560, 280, 749,
  437, 461, 281, 770, 633, 597, 360, 508, 505, 499,
  166, 243, 422, 458, 769, 66, 377, 439, 34, 109,
  308, 632, 386, 499, 219, 561, 152, 801, 310, 432,
  486, 495, 641, 767, 201, 669, 286, 438, 85, 485,
  111, 764, 78, 310, 654, 463, 703, 585, 633, 708,
  561, 795, 52, 806, 802, 593, 348, 581, 232, 700,
  628, 798, 442, 771, 577, 549, 377, 53, 176, 209,
  285, 34, 162, 363, 92, 563, 574, 542, 84, 396,
  439, 646, 127, 492, 641, 119, 21, 737, 447, 254,
  626, 11, 241, 257, 782, 8, 553, 348, 619, 476,
  304, 93, 511, 466, 204, 603, 777, 778, 334, 50,
  34, 632, 10, 721, 26, 579, 699, 152, 778, 708,
  414, 478, 114, 597, 721, 221, 329, 618, 97, 488,
  341, 324, 571, 604, 790, 12, 785, 539, 33, 481,
  628, 67, 303, 385, 788, 76, 154, 676, 228, 679,
  573, 389, 346, 434, 176, 257, 402, 252, 64, 247,
  740, 152, 571, 248, 756, 297, 260, 730, 25, 40,
  400, 654, 107, 450, 786, 84, 527, 129, 507, 502,
  556, 17, 81, 91, 198, 631, 581, 622, 165, 423,
  736, 268, 773, 715, 125, 242, 605, 340, 449, 227,
  124, 698, 101, 684, 33, 280, 734, 317, 577, 18,
  73, 152, 801, 462, 512, 108, 283, 29, 477, 448,
  453, 149, 463, 415, 53, 588, 405, 406, 675, 43,
  380, 799, 488, 228, 420, 521, 508, 90, 586, 21,
  108, 659, 731, 99, 310, 432, 765, 593, 461, 179,
  230, 661, 328, 440, 13, 382, 775, 418, 535, 640,
  248, 692, 462, 391, 472, 304, 790, 664, 710, 693,
  184, 145, 73, 431, 342, 591, 167, 410, 507, 91,
  651, 420, 162, 228, 613, 271, 646, 525, 137, 265,
  374, 386, 704, 25, 524, 365, 77, 250, 777, 787,
  691, 150, 679, 764, 328, 769, 544, 495, 368, 240,
  586, 766, 407, 496, 183, 209, 514, 577, 734, 652,
  589, 44, 227, 483, 70, 498, 37, 147, 748, 3,
  681, 628, 711, 550, 328, 243, 794, 792, 93, 145,
  254, 182, 223, 282, 429, 98, 167, 445, 151, 444,
  694, 808, 298, 742, 300, 565, 645, 549, 775, 143,
  393, 744, 352, 550, 189, 565, 793, 730, 546, 633,
  65, 547, 5, 35, 19, 434, 133, 744, 626, 284,
  124, 257, 28, 423, 746, 328, 735, 328, 67, 699,
  471, 207, 632, 571, 758, 11, 72, 740, 488, 619,
  310, 553, 102, 315, 335, 679, 496, 468, 612, 312,
  600, 400, 464, 754, 585, 258, 223, 704, 104, 156,
  677, 6, 232, 442, 147, 596, 648, 639, 740, 42,
  582, 5, 741, 767, 186, 309, 445, 250, 184, 604,
  451, 784, 193, 104, 475, 526, 362, 698, 419, 466,
  601, 285, 220, 23, 728, 367, 619, 312, 753, 295,
  355, 271, 301, 285, 227, 234, 341, 419, 484, 525,
  212, 124, 246, 153, 787, 721, 679, 338, 355, 34,
  552, 145, 67, 519, 168, 673, 218, 121, 508, 270,
  596, 679, 295, 497, 511, 328, 195, 627, 750, 120,
  320, 5, 472, 469, 667, 256, 667, 296, 798, 439,
  206, 683, 6, 723, 205, 487, 585, 170, 355, 29,
  187, 698, 708, 229, 385, 408, 558, 327, 783, 244,
  448, 39, 249, 109, 509, 105, 112, 365, 401, 99,
  551, 355, 529, 557, 267, 734, 792, 599, 651, 336,
  376, 28, 224, 273, 4, 356, 429, 562, 683, 401,
};

static int T1[] = {
  358, 351, 773, 218, 148, 116, 411, 472, 745, 671,
  292, 232, 222, 269, 756, 637, 797, 477, 489, 553,
  616, 69, 632, 116, 765, 134, 625, 429, 388, 600,
  419, 494, 141, 129, 459, 289, 245, 617, 508, 737,
  477, 800, 158, 446, 5, 104, 272, 803, 328, 761,
  292, 133, 20, 113, 808, 785, 805, 622, 151, 130,
  411, 317, 624, 552, 446, 19, 588, 691, 636, 285,
  618, 49, 22, 776, 495, 569, 261, 243, 512, 187,
  396, 40, 636, 739, 581, 277, 292, 212, 773, 33,
  320, 126, 773, 44, 231, 183, 78, 246, 354, 78,
  59, 100, 343, 146, 577, 563, 462, 28, 806, 164,
  215, 138, 762, 598, 66, 279, 623, 106, 491, 332,
  139, 559, 458, 659, 350, 689, 31, 429, 682, 133,
  254, 741, 233, 344, 635, 810, 96, 286, 585, 649,
  197, 548, 535, 706, 335, 348, 175, 705, 454, 413,
  619, 692, 657, 8, 87, 522, 726, 648, 116, 52,
  360, 195, 461, 475, 798, 469, 60, 809, 609, 284,
  145, 717, 574, 586, 274, 345, 700, 460, 56, 757,
  364, 422, 638, 768, 431, 725, 226, 93, 309, 89,
  145, 669, 285, 353, 333, 19, 570, 140, 575, 368,
  424, 468, 21, 187, 243, 295, 279, 690, 502, 336,
  383, 613, 758, 210, 570, 125, 682, 543, 777, 180,
  632, 669, 38, 664, 212, 547, 425, 217, 33, 796,
  57, 106, 775, 222, 764, 451, 671, 507, 574, 761,
  182, 416, 12, 216, 582, 603, 288, 713, 719, 535,
  276, 713, 254, 225, 752, 637, 519, 366, 601, 553,
  351, 659, 406, 62, 628, 107, 260, 488, 614, 24,
  186, 796, 440, 756, 201, 770, 548, 237, 419, 204,
  772, 695, 106, 773, 667, 47, 600, 376, 160, 390,
  676, 258, 238, 271, 321, 56, 125, 581, 291, 740,
  311, 580, 496, 41, 261, 773, 252, 21, 534, 42,
  662, 426, 441, 310, 461, 398, 546, 50, 282, 401,
  70, 280, 98, 141, 742, 35, 45, 313, 609, 540,
  301, 109, 309, 797, 151, 317, 506, 150, 338, 229,
  193, 747, 403, 381, 247, 611, 779, 793, 661, 250,
  383, 731, 278, 229, 620, 209, 11, 665, 269, 620,
  394, 317, 730, 450, 303, 628, 767, 557, 778, 294,
  786, 718, 230, 378, 289, 238, 41, 581, 442, 266,
  497, 810, 153, 710, 241, 281, 585, 495, 125, 742,
  192, 748, 436, 296, 698, 803, 725, 687, 49, 366,
  786, 421, 603, 262, 577, 771, 247, 618, 288, 690,
  73, 533, 436, 784, 432, 424, 1, 764, 108, 127,
  695, 47, 622, 320, 344, 510, 59, 5, 386, 108,
  118, 361, 529, 722, 370, 295, 429, 365, 660, 464,
  244, 481, 186, 680, 201, 365, 40, 203, 318, 148,
  549, 621, 801, 761, 35, 1, 667, 513, 66, 676,
  238, 102, 231, 350, 687, 268, 202, 663, 522, 754,
  6, 203, 85, 121, 363, 726, 735, 439, 148, 667,
  809, 697, 477, 546, 647, 259, 547, 504, 772, 360,
  116, 758, 209, 347, 297, 85, 363, 246, 495, 74,
  190, 248, 24, 22, 369, 388, 748, 40, 574, 85,
  454, 572, 783, 120, 54, 366, 126, 601, 59, 87,
  150, 734, 592, 359, 17, 388, 477, 340, 361, 254,
  173, 45, 299, 104, 632, 421, 176, 188, 675, 781,
  665, 389, 180, 340, 413, 782, 516, 588, 462, 418,
  301, 580, 657, 187, 681, 530, 322, 94, 617, 683,
  95, 790, 476, 141, 641, 297, 563, 6, 232, 427,
  534, 644, 563, 461, 731, 723, 433, 437, 500, 84,
  602, 802, 411, 448, 736, 28, 725, 247, 681, 531,
  119, 776, 257, 595, 107, 645, 639, 670, 651, 60,
  758, 531, 362, 800, 761, 398, 357, 72, 621, 370,
  681, 804, 81, 414, 672, 434, 265, 433, 509, 288,
  572, 472, 99, 534, 59, 468, 169, 4, 287, 174,
  796, 792, 706, 347, 781, 403, 745, 75, 476, 555,
  192, 93, 296, 20, 508, 715, 455, 520, 337, 153,
  556, 656, 372, 655, 127, 431, 59, 296, 183, 94,
  471, 168, 75, 113, 515, 46, 516, 196, 679, 181,
  752, 60, 22, 795, 80, 701, 724, 277, 143, 740,
  598, 121, 266, 578, 682, 12, 52, 767, 73, 331,
  775, 422, 400, 151, 605, 444, 94, 210, 769, 657,
  126, 323, 561, 737, 54, 504, 627, 778, 529, 518,
  455, 316, 639, 468, 641, 511, 227, 694, 214, 300,
  772, 178, 722, 361, 329, 264, 552, 170, 474, 257,
  574, 347, 580, 71, 273, 381, 576, 90, 95, 294,
  608, 550, 357, 183, 765, 187, 441, 181, 70, 655,
  357, 312, 514, 278, 35, 693, 163, 191, 678, 187,
  295, 489, 450, 741, 495, 373, 59, 595, 235, 244,
  706, 508, 344, 710, 617, 803, 773, 157, 325, 701,
  636, 429, 203, 339, 707, 796, 221, 617, 176, 646,
  551, 219, 71, 190, 707, 567, 311, 514, 351, 546,
  758, 804, 801, 38, 704, 355, 589, 666, 259, 103,
  303, 84, 533, 506, 423, 176, 492, 391, 541, 415,
  226, 281, 634, 44, 219, 515, 399, 333, 661, 785,
  667, 222, 505, 601, 571, 247, 531, 201, 258, 152,
  95, 265, 751, 579, 368, 65, 345, 761, 27, 542,
  451, 537, 127, 533, 1, 428, 796, 147, 762, 646,
  680, 365, 615, 121, 713, 375, 115, 181, 576, 373,
  333, 672, 639, 20, 187, 754, 643, 533, 451, 670,
  11, 91, 143, 138, 372, 144, 567, 104, 39, 265,
  497, 719, 377, 302, 29, 280, 424, 144, 461, 190,
  455, 356, 676, 414, 341, 79, 470, 189, 57, 48,
  428, 173, 399, 596, 249, 656, 517, 215, 229, 472,
  796, 10, 73, 452, 213, 517, 44, 115, 252, 750,
  105, 707, 296, 781, 58, 637, 49, 528, 573, 665,
  577, 190, 27, 723, 534, 23, 568, 798, 238, 544,
  460, 781, 554, 533, 422, 514, 798, 467, 630, 239,
  153, 735, 135, 196, 453, 193, 581, 249, 469, 343,
  103, 793, 281, 688, 705, 229, 0, 249, 107, 166,
  621, 511, 543, 361, 300, 712, 734, 217, 487, 439,
  757, 68, 270, 118, 513, 722, 461, 406, 444, 303,
  74, 640, 664, 716, 20, 402, 693, 21, 398, 800,
  745, 208, 500, 224, 316, 547, 683, 239, 764, 106,
  425, 457, 733, 695, 323, 435, 353, 531, 588, 545,
  23, 662, 374, 688, 314, 141, 26, 754, 162, 172,
  743, 654, 127, 179, 67, 444, 473, 497, 430, 174,
  207, 245, 572, 224, 187, 281, 684, 538, 698, 519,
  561, 649, 688, 578, 617, 260, 203, 19, 528, 656,
  411, 549, 676, 358, 665, 106, 776, 797, 232, 75,
  708, 440, 67, 217, 411, 254, 498, 31, 539, 132,
  550, 37, 781, 175, 615, 587, 435, 565, 353, 710,
  410, 511, 196, 275, 59, 50, 382, 24, 36, 614,
  657, 745, 801, 724, 709, 148, 725, 396, 180, 454,
  275, 477, 491, 245, 652, 413, 117, 793, 68, 165,
  786, 407, 57, 259, 644, 723, 136, 343, 631, 498,
  329, 474, 423, 159, 509, 326, 775, 325, 741, 687,
  467, 196, 410, 295, 356, 706, 709, 473, 435, 777,
  639, 410, 374, 696, 416, 765, 355, 552, 44, 733,
  797, 373, 144, 409, 279, 400, 483, 244, 725, 160,
  678, 128, 356, 25, 424, 713, 731, 322, 375, 102,
  288, 761, 512, 409, 393, 675, 363, 495, 163, 407,
  529, 349, 711, 286, 510, 193, 62, 720, 591, 444,
  449, 648, 631, 153, 421, 255, 710, 802, 28, 47,
  346, 302, 3, 205, 711, 479, 40, 429, 137, 803,
  110, 666, 88, 10, 700, 599, 761, 762, 255, 288,
  142, 704, 125, 520, 604, 294, 776, 504, 32, 551,
  298, 125, 42, 302, 331, 754, 528, 118, 119, 413,
  668, 229, 15, 757, 797, 715, 292, 747, 413, 547,
  225, 555, 188, 97, 265, 439, 402, 69, 123, 387,
  130, 388, 67, 279, 522, 269, 582, 143, 107, 148,
  634, 83, 693, 71, 402, 613, 671, 553, 540, 110,
  506, 450, 767, 207, 150, 139, 393, 552, 766, 264,
  128, 85, 652, 753, 112, 110, 212, 694, 253, 66,
  31, 634, 150, 471, 706, 299, 273, 313, 41, 560,
  423, 295, 200, 380, 249, 97, 519, 642, 396, 474,
  653, 525, 307, 495, 214, 419, 352, 173, 49, 606,
  579, 214, 648, 622, 202, 132, 244, 171, 254, 776,
  226, 543, 215, 386, 802, 292, 330, 493, 592, 19,
  716, 431, 63, 399, 586, 54, 488, 287, 561, 557,
  575, 330, 771, 412, 699, 720, 291, 132, 638, 292,
  97, 53, 24, 312, 187, 15, 352, 517, 256, 133,
  283, 719, 311, 346, 55, 86, 401, 543, 121, 709,
  36, 696, 228, 808, 45, 116, 717, 83, 806, 545,
  376, 93, 345, 400, 152, 649, 470, 104, 622, 379,
  194, 810, 491, 329, 203, 411, 518, 206, 694, 290,
  581, 687, 447, 680, 721, 552, 675, 581, 661, 806,
  794, 147, 85, 778, 485, 761, 616, 702, 55, 174,
  270, 249, 173, 509, 325, 376, 667, 590, 329, 551,
  69, 100, 174, 263, 780, 84, 5, 391, 412, 413,
  387, 396, 560, 219, 110, 792, 170, 473, 683, 783,
  647, 143, 779, 9, 399, 40, 385, 255, 378, 462,
};

static int T2[] = {
  245, 773, 735, 612, 67, 285, 754, 132, 587, 448,
  123, 690, 56, 75, 227, 618, 345, 59, 628, 599,
  295, 658, 43, 670, 464, 242, 428, 757, 663, 762,
  487, 97, 472, 411, 456, 286, 696, 399, 165, 473,
  595, 288, 99, 651, 363, 327, 206, 709, 133, 23,
  244, 175, 428, 287, 34, 81, 530, 209, 585, 129,
  160, 261, 784, 379, 419, 429, 665, 52, 576, 20,
  272, 360, 308, 371, 758, 290, 651, 567, 797, 581,
  682, 97, 798, 201, 465, 755, 126, 729, 581, 387,
  524, 25, 374, 522, 351, 529, 178, 517, 396, 452,
  476, 570, 378, 152, 335, 770, 189, 175, 273, 176,
  756, 703, 20, 491, 93, 232, 435, 219, 150, 763,
  353, 421, 788, 474, 691, 76, 192, 58, 593, 588,
  258, 258, 95, 383, 157, 430, 90, 347, 605, 363,
  270, 297, 255, 37, 788, 348, 269, 160, 314, 166,
  87, 588, 515, 293, 102, 711, 413, 780, 108, 532,
  239, 346, 24, 11, 359, 0, 153, 587, 212, 433,
  626, 742, 260, 471, 309, 168, 514, 160, 480, 389,
  182, 567, 725, 444, 50, 574, 345, 210, 543, 453,
  489, 782, 546, 514, 541, 652, 261, 694, 429, 474,
  63, 244, 152, 70, 715, 461, 796, 165, 621, 465,
  302, 551, 222, 216, 742, 19, 537, 276, 229, 269,
  476, 466, 799, 770, 169, 94, 622, 302, 590, 214,
  388, 319, 526, 792, 198, 504, 480, 620, 88, 151,
  356, 301, 261, 504, 58, 43, 326, 120, 26, 369,
  556, 69, 303, 696, 539, 701, 790, 351, 750, 317,
  312, 327, 636, 27, 309, 581, 532, 536, 391, 367,
  687, 747, 415, 695, 440, 474, 485, 514, 594, 511,
  630, 86, 327, 123, 529, 56, 571, 509, 154, 510,
  573, 213, 585, 398, 240, 641, 169, 519, 366, 560,
  567, 361, 481, 807, 631, 267, 85, 756, 531, 806,
  319, 137, 701, 110, 364, 60, 29, 125, 237, 765,
  652, 106, 83, 54, 320, 370, 249, 620, 423, 429,
  125, 737, 538, 353, 734, 358, 367, 8, 50, 88,
  3, 369, 225, 452, 226, 589, 512, 256, 461, 496,
  768, 50, 602, 40, 662, 111, 410, 100, 479, 23,
  277, 351, 760, 4, 705, 430, 109, 8, 185, 717,
  96, 747, 276, 68, 388, 100, 609, 603, 84, 621,
  675, 343, 187, 235, 159, 743, 554, 484, 618, 793,
  634, 539, 361, 517, 370, 585, 53, 10, 578, 559,
  549, 708, 500, 104, 22, 59, 762, 378, 662, 594,
  746, 273, 126, 681, 255, 33, 613, 557, 517, 167,
  539, 341, 706, 648, 47, 13, 169, 658, 581, 747,
  406, 320, 391, 654, 171, 160, 713, 122, 538, 311,
  716, 221, 331, 590, 91, 586, 623, 704, 79, 76,
  81, 272, 716, 234, 42, 660, 261, 346, 176, 285,
  148, 121, 796, 577, 604, 251, 585, 252, 170, 776,
  321, 448, 391, 539, 429, 404, 789, 380, 721, 651,
  274, 549, 112, 738, 784, 713, 334, 234, 806, 510,
  266, 143, 379, 809, 720, 172, 249, 494, 171, 419,
  206, 239, 615, 597, 778, 791, 190, 756, 360, 659,
  344, 381, 397, 203, 55, 117, 105, 389, 352, 658,
  89, 365, 801, 215, 364, 107, 512, 761, 98, 782,
  259, 525, 660, 781, 791, 351, 708, 147, 232, 657,
  234, 254, 206, 673, 620, 121, 535, 118, 435, 100,
  184, 794, 30, 119, 618, 690, 784, 66, 388, 630,
  37, 394, 344, 444, 364, 71, 795, 261, 777, 217,
  107, 200, 218, 60, 620, 585, 181, 344, 703, 364,
  444, 77, 94, 474, 754, 459, 101, 474, 526, 236,
  293, 563, 630, 385, 755, 184, 456, 739, 192, 169,
  709, 119, 249, 557, 609, 285, 50, 472, 792, 114,
  796, 495, 50, 624, 135, 307, 223, 107, 67, 719,
  443, 714, 518, 48, 384, 238, 307, 497, 483, 624,
  245, 382, 490, 494, 686, 35, 779, 483, 508, 760,
  597, 240, 192, 648, 53, 327, 702, 276, 434, 516,
  742, 66, 167, 196, 672, 551, 434, 726, 237, 107,
  539, 482, 236, 777, 724, 669, 1, 692, 342, 256,
  389, 686, 496, 581, 270, 627, 113, 152, 165, 485,
  283, 744, 253, 780, 334, 659, 579, 345, 32, 645,
  520, 427, 510, 21, 70, 502, 656, 248, 495, 637,
  26, 143, 480, 684, 441, 460, 248, 302, 359, 160,
  787, 642, 93, 787, 359, 428, 382, 685, 520, 415,
  519, 787, 31, 218, 809, 659, 720, 401, 655, 151,
  785, 681, 295, 201, 301, 483, 662, 549, 785, 210,
  709, 508, 600, 550, 485, 148, 725, 614, 22, 181,
  394, 458, 777, 655, 208, 328, 317, 490, 514, 497,
  435, 136, 243, 701, 143, 133, 407, 750, 351, 484,
  98, 516, 107, 717, 324, 407, 92, 600, 259, 679,
  490, 400, 73, 456, 802, 29, 532, 309, 266, 235,
  553, 701, 119, 796, 592, 262, 118, 188, 760, 217,
  420, 47, 480, 527, 511, 804, 681, 351, 341, 687,
  777, 20, 276, 597, 223, 267, 626, 502, 323, 81,
  485, 65, 783, 604, 608, 378, 192, 51, 435, 632,
  585, 326, 273, 457, 456, 780, 381, 38, 125, 806,
  356, 94, 213, 525, 201, 279, 527, 214, 141, 600,
  693, 365, 715, 303, 398, 354, 428, 590, 405, 610,
  158, 737, 125, 179, 131, 328, 148, 512, 114, 20,
  508, 217, 115, 721, 743, 316, 189, 206, 277, 331,
  553, 159, 443, 458, 209, 30, 559, 384, 620, 153,
  741, 526, 638, 56, 705, 769, 131, 600, 470, 803,
  53, 98, 359, 706, 609, 310, 540, 192, 491, 353,
  14, 113, 79, 551, 762, 617, 723, 505, 360, 514,
  490, 605, 49, 804, 547, 428, 173, 111, 83, 666,
  652, 694, 511, 758, 337, 309, 258, 624, 249, 749,
  166, 263, 610, 246, 562, 561, 610, 474, 256, 159,
  177, 493, 512, 227, 486, 248, 402, 407, 359, 232,
  9, 758, 674, 267, 453, 200, 323, 711, 13, 572,
  396, 738, 25, 195, 731, 26, 493, 806, 651, 67,
  50, 176, 474, 307, 47, 109, 740, 215, 641, 304,
  320, 335, 576, 640, 455, 643, 637, 26, 610, 77,
  571, 106, 465, 35, 612, 481, 61, 294, 476, 712,
  362, 274, 635, 583, 581, 682, 439, 257, 644, 269,
  561, 711, 605, 73, 288, 807, 717, 114, 22, 263,
  191, 593, 116, 657, 629, 728, 74, 437, 770, 297,
  86, 68, 571, 721, 651, 88, 340, 26, 92, 731,
  446, 548, 663, 133, 421, 71, 288, 207, 681, 624,
  122, 75, 266, 298, 19, 581, 290, 513, 8, 366,
  252, 657, 350, 477, 649, 104, 186, 647, 668, 572,
  334, 303, 56, 187, 183, 478, 5, 219, 685, 433,
  590, 555, 509, 45, 42, 528, 627, 79, 788, 382,
  446, 787, 787, 796, 453, 372, 89, 639, 208, 757,
  147, 543, 250, 203, 477, 180, 681, 229, 146, 303,
  663, 737, 47, 361, 529, 757, 618, 255, 97, 158,
  180, 598, 345, 551, 236, 307, 13, 639, 13, 455,
  38, 489, 127, 590, 562, 773, 126, 594, 236, 593,
  115, 788, 149, 530, 15, 330, 223, 380, 585, 320,
  285, 512, 665, 631, 253, 648, 685, 266, 476, 698,
  468, 262, 123, 342, 599, 433, 304, 725, 216, 287,
  254, 78, 12, 403, 355, 27, 734, 578, 154, 255,
  87, 439, 768, 499, 259, 210, 336, 691, 223, 2,
  37, 201, 476, 376, 350, 595, 45, 489, 141, 396,
  28, 247, 379, 83, 173, 701, 172, 750, 610, 46,
  731, 286, 759, 182, 509, 758, 779, 310, 792, 204,
  195, 577, 405, 671, 142, 502, 455, 187, 180, 343,
  330, 209, 590, 709, 39, 763, 347, 211, 702, 704,
  257, 370, 737, 206, 552, 436, 711, 267, 493, 439,
  218, 688, 205, 623, 295, 95, 62, 497, 29, 242,
  29, 107, 451, 366, 563, 792, 253, 507, 132, 575,
  397, 253, 713, 682, 379, 449, 302, 252, 720, 168,
  490, 619, 157, 541, 763, 616, 87, 245, 249, 397,
  389, 783, 447, 236, 143, 647, 218, 143, 91, 97,
  719, 488, 97, 368, 106, 223, 564, 409, 475, 473,
  577, 712, 29, 481, 442, 792, 33, 276, 784, 282,
  420, 109, 254, 615, 346, 144, 198, 311, 34, 289,
  408, 753, 525, 252, 57, 378, 475, 621, 534, 697,
  291, 412, 594, 581, 594, 205, 220, 105, 350, 183,
  638, 435, 582, 651, 600, 539, 88, 403, 784, 116,
  744, 599, 686, 149, 299, 339, 749, 268, 439, 452,
  108, 730, 611, 449, 500, 142, 654, 468, 247, 752,
  651, 633, 376, 422, 220, 723, 708, 55, 316, 681,
  172, 249, 470, 605, 398, 516, 692, 83, 785, 320,
  282, 640, 239, 83, 279, 486, 783, 680, 143, 777,
  621, 542, 599, 744, 153, 433, 28, 234, 499, 14,
  363, 690, 772, 480, 626, 439, 314, 599, 300, 346,
  428, 514, 508, 661, 574, 800, 761, 781, 445, 421,
  3, 588, 334, 475, 243, 624, 97, 271, 605, 597,
  286, 157, 223, 805, 384, 38, 433, 446, 385, 733,
  539, 560, 184, 236, 410, 758, 783, 107, 476, 418,
  275, 479, 195, 357, 701, 439, 728, 546, 710, 522,
  79, 743, 426, 302, 738, 558, 88, 107, 751, 473,
};

int hash_color(const unsigned char *key) {
	int i;
	int f0, f1, f2;
	const unsigned char *kp = key;

	for (i=-48, f0=f1=f2=0; *kp; ++kp) {
	  f0 += T0[i + *kp];
	  f1 += T1[i + *kp];
	  f2 += T2[i + *kp];
	  i += 75;
	}
	
	f0 %= 811;
	f1 %= 811;
	f2 %= 811;
	
	return (g[f0] + g[f1] + g[f2]) % 659;
}


uint32_t rcolor_to_int(const char *col) {
  int idx = hash_color((const unsigned char *)col);
  if (idx < 0 || idx > 658 || memcmp(col, col_name[idx], 2) != 0) {
    error("Not a valid colour name: %s", col);
  }
  return packed_int[idx];
}
