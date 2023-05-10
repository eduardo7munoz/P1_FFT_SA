/*
 * DMA.c
 * this file provides 2 initialization functions
 * 1 for memory to memory transfers
 * and another for peripheral to memory transfers utilizing the ADC
 * the number to copy #define can be found in the DMA.h file and modified there
 * this file also contains the IRQ handlers for both of the channels that were utilized
 *  Created on: Apr 15, 2023
 *      Author: eduardomunoz
 */
#include "main.h"
#include <stdlib.h>
#include "DMA.h"
#include "cmsis_os.h"
#include "semphr.h"

float32_t hanningwind[HANNING_SIZE] = {0,3.77965772740962e-05,0.000151180594771427,0.000340134910380874,0.000604630956796859,0.000944628745838338,0.00136007687449446,0.00185091253269609,0.00241706151281168,0.00305843822086654,
		0.00377494568948339,0.00456647559254264,0.00543290826155973,0.00637411270377730,0.00738994662196968,0.00848025643595607,0.00964487730581998,0.0108836331568305,0.0121963367060627,0.0135827894907121,
		0.0150427818980994,0.0165760931973613,0.0181824915728215,0.0198617341590390,0.0216135670775249,0.0234377254751261,0.0253339335640674,0.0273019046636466,0.0293413412435765,0.0314519349689681,
		0.0336333667469460,0.0358853067748912,0.0382074145903025,0.0405993391222701,0.0430607187445522,0.0455911813302485,0.0481903443080604,0.0508578147201305,0.0535931892814526,0.0563960544408427,
		0.0592659864434626,0.0622025513948853,0.0652053053266945,0.0682737942636061,0.0714075542921032,0.0746061116305735,0.0778689827009385,0.0811956742017641,0.0845856831828408,0.0880384971212229,
		0.0915535939987150,0.0951304423807943,0.0987685014969555,0.102467221322469,0.106226042661535,0.110044397231829,0.113921707750418,0.117857388021034,0.121850843022703,0.125901468999704,
		0.130008653552846,0.134171775732054,0.138390206130253,0.142663306978520,0.146990432242509,0.151370927720124,0.155804131140420,0.160289372263735,0.164825972983019,0.169413247426353,
		0.174050502060644,0.178737035796480,0.183472140094124,0.188255099070633,0.193085189608094,0.197961681462944,0.202883837376380,0.207850913185816,0.212862157937393,0.217916813999513,
		0.223014117177384,0.228153296828549,0.233333575979408,0.238554171442674,0.243814293935788,0.249113148200246,0.254449933121827,0.259823841851719,0.265234061928494,0.270679775400948,
		0.276160158951759,0.281674384021968,0.287221616936238,0.292801019028898,0.298411746770740,0.304052951896545,0.309723781533331,0.315423378329296,0.321150880583437,0.326905422375828,
		0.332686133698534,0.338492140587147,0.344322565252915,0.350176526215451,0.356053138436006,0.361951513451266,0.367870759507683,0.373809981696295,0.379768282088018,0.385744759869409,
		0.391738511478851,0.397748630743159,0.403774209014585,0.409814335308190,0.415868096439573,0.421934577162933,0.428012860309440,0.434102026925899,0.440201156413684,0.446309326667918,
		0.452425614216887,0.458549094361650,0.464678841315846,0.470813928345655,0.476953427909915,0.483096411800347,0.489241951281889,0.495389117233110,0.501536980286678,0.507684610969869,
		0.513831079845091,0.519975457650400,0.526116815439995,0.532254224724658,0.538386757612132,0.544513486947405,0.550633486452881,0.556745830868423,0.562849596091240,0.568943859315596,
		0.575027699172326,0.581100195868139,0.587160431324672,0.593207489317293,0.599240455613625,0.605258418111759,0.611260466978157,0.617245694785205,0.623213196648401,0.629162070363163,
		0.635091416541232,0.641000338746643,0.646887943631258,0.652753341069825,0.658595644294553,0.664413970029181,0.670207438622517,0.675975174181427,0.681716304703260,0.687429962207682,
		0.693115282867903,0.698771407141278,0.704397479899253,0.709992650556653,0.715556073200279,0.721086906716794,0.726584314919893,0.732047466676720,0.737475536033525,0.742867702340537,
		0.748223150376032,0.753541070469590,0.758820658624500,0.764061116639314,0.769261652228528,0.774421479142360,0.779539817285623,0.784615892835666,0.789648938359361,0.794638192929131,
		0.799582902237994,0.804482318713598,0.809335701631252,0.814142317225904,0.818901438803083,0.823612346848765,0.828274329138148,0.832886680843338,0.837448704639904,0.841959710812305,
		0.846419017358170,0.850825950091399,0.855179842744098,0.859480037067308,0.863725882930520,0.867916738419968,0.872051969935680,0.876130952287266,0.880153068788438,0.884117711350248,
		0.888024280573021,0.891872185836974,0.895660845391512,0.899389686443183,0.903058145242268,0.906665667168023,0.910211706812523,0.913695728063121,0.917117204183505,0.920475617893327,
		0.923770461446416,0.927001236707534,0.930167455227694,0.933268638318005,0.936304317122042,0.939274032686730,0.942177336031735,0.945013788217338,0.947782960410804,0.950484433951210,
		0.953117800412740,0.955682661666441,0.958178629940405,0.960605327878401,0.962962388596925,0.965249455740667,0.967466183536386,0.969612236845189,0.971687291213196,0.973691032920598,
		0.975623159029080,0.977483377427628,0.979271406876687,0.980986977050686,0.982629828578900,0.984199713084672,0.985696393222957,0.987119642716209,0.988469246388591,0.989745000198504,
		0.990946711269438,0.992074197919133,0.993127289687043,0.994105827360110,0.995009662996835,0.995838659949645,0.996592692885550,0.997271647805093,0.997875422059586,0.998403924366628,
		0.998857074823906,0.999234804921275,0.999537057551115,0.999763787016967,0.999914959040440,0.999990550766394,0.999990550766394,0.999914959040440,0.999763787016967,0.999537057551115,
		0.999234804921275,0.998857074823906,0.998403924366628,0.997875422059586,0.997271647805093,0.996592692885550,0.995838659949645,0.995009662996835,0.994105827360110,0.993127289687043,
		0.992074197919133,0.990946711269438,0.989745000198504,0.988469246388591,0.987119642716209,0.985696393222957,0.984199713084672,0.982629828578900,0.980986977050686,0.979271406876687,
		0.977483377427628,0.975623159029080,0.973691032920598,0.971687291213196,0.969612236845189,0.967466183536386,0.965249455740667,0.962962388596925,0.960605327878401,0.958178629940405,
		0.955682661666441,0.953117800412740,0.950484433951210,0.947782960410805,0.945013788217338,0.942177336031735,0.939274032686730,0.936304317122042,0.933268638318005,0.930167455227694,
		0.927001236707534,0.923770461446416,0.920475617893328,0.917117204183505,0.913695728063121,0.910211706812523,0.906665667168023,0.903058145242268,0.899389686443183,0.895660845391513,
		0.891872185836974,0.888024280573021,0.884117711350248,0.880153068788438,0.876130952287266,0.872051969935680,0.867916738419968,0.863725882930520,0.859480037067308,0.855179842744099,
		0.850825950091399,0.846419017358170,0.841959710812305,0.837448704639904,0.832886680843338,0.828274329138148,0.823612346848765,0.818901438803083,0.814142317225904,0.809335701631252,
		0.804482318713599,0.799582902237994,0.794638192929131,0.789648938359361,0.784615892835666,0.779539817285623,0.774421479142360,0.769261652228528,0.764061116639314,0.758820658624500,
		0.753541070469591,0.748223150376033,0.742867702340537,0.737475536033525,0.732047466676720,0.726584314919893,0.721086906716794,0.715556073200279,0.709992650556654,0.704397479899253,
		0.698771407141278,0.693115282867903,0.687429962207682,0.681716304703260,0.675975174181427,0.670207438622517,0.664413970029181,0.658595644294553,0.652753341069825,0.646887943631259,
		0.641000338746643,0.635091416541232,0.629162070363163,0.623213196648400,0.617245694785205,0.611260466978157,0.605258418111759,0.599240455613625,0.593207489317293,0.587160431324672,
		0.581100195868140,0.575027699172326,0.568943859315596,0.562849596091240,0.556745830868423,0.550633486452881,0.544513486947404,0.538386757612133,0.532254224724658,0.526116815439995,
		0.519975457650400,0.513831079845092,0.507684610969869,0.501536980286678,0.495389117233110,0.489241951281889,0.483096411800347,0.476953427909915,0.470813928345655,0.464678841315846,
		0.458549094361651,0.452425614216887,0.446309326667919,0.440201156413684,0.434102026925899,0.428012860309440,0.421934577162933,0.415868096439574,0.409814335308190,0.403774209014585,
		0.397748630743159,0.391738511478851,0.385744759869409,0.379768282088018,0.373809981696295,0.367870759507684,0.361951513451266,0.356053138436005,0.350176526215452,0.344322565252915,
		0.338492140587147,0.332686133698534,0.326905422375828,0.321150880583437,0.315423378329297,0.309723781533332,0.304052951896545,0.298411746770740,0.292801019028898,0.287221616936238,
		0.281674384021968,0.276160158951760,0.270679775400947,0.265234061928494,0.259823841851719,0.254449933121828,0.249113148200246,0.243814293935788,0.238554171442674,0.233333575979408,
		0.228153296828549,0.223014117177384,0.217916813999514,0.212862157937393,0.207850913185816,0.202883837376380,0.197961681462944,0.193085189608094,0.188255099070633,0.183472140094124,
		0.178737035796480,0.174050502060644,0.169413247426353,0.164825972983019,0.160289372263735,0.155804131140420,0.151370927720124,0.146990432242509,0.142663306978520,0.138390206130252,
		0.134171775732054,0.130008653552846,0.125901468999705,0.121850843022703,0.117857388021034,0.113921707750418,0.110044397231829,0.106226042661535,0.102467221322469,0.0987685014969555,
		0.0951304423807942,0.0915535939987150,0.0880384971212229,0.0845856831828409,0.0811956742017641,0.0778689827009387,0.0746061116305735,0.0714075542921033,0.0682737942636061,0.0652053053266948,
		0.0622025513948854,0.0592659864434625,0.0563960544408427,0.0535931892814527,0.0508578147201306,0.0481903443080604,0.0455911813302486,0.0430607187445523,0.0405993391222702,0.0382074145903025,
		0.0358853067748913,0.0336333667469461,0.0314519349689680,0.0293413412435765,0.0273019046636465,0.0253339335640675,0.0234377254751261,0.0216135670775249,0.0198617341590389,0.0181824915728216,
		0.0165760931973613,0.0150427818980994,0.0135827894907122,0.0121963367060628,0.0108836331568306,0.00964487730581992,0.00848025643595618,0.00738994662196968,0.00637411270377736,0.00543290826155973,
		0.00456647559254275,0.00377494568948344,0.00305843822086654,0.00241706151281168,0.00185091253269615,0.00136007687449446,0.000944628745838338,0.000604630956796859,0.000340134910380874,0.000151180594771427,
		3.77965772740962e-05,0};

//float32_t hanningwind[HANNING_SIZE] = {0,9.43076911874563e-06,3.77227207172259e-05,8.48747875361600e-05,0.000150885190854377,0.000235751440556431,0.000339470335226355,0.000462037962268236,0.000603449698054204,0.000763700208098517,
//		0.000942783447258899,0.00114069265996464,0.00135742038047137,0.00159295843314272,0.00184729793275873,0.00212042928485096,0.00241234218606456,0.00272302562454685,0.00305246788036268,0.00340065652593669,
//		0.00376757842652187,0.00415321974069544,0.00455756592088058,0.00498060171389536,0.00542231116152819,0.00588267760113986,0.00636168366629197,0.00685931128740208,0.00737554169242533,0.00791035540756274,
//		0.00846373225799557,0.00903565136864665,0.00962609116496754,0.0102350293737528,0.0108624430239796,0.0115083084476750,0.0121726012808081,0.0128552964642097,0.0135563682445171,0.0142757901751459,
//		0.0150135351172876,0.0157695752409334,0.0165438820259238,0.0173364262630248,0.0181471780550296,0.0189761068178861,0.0198231812818515,0.0206883694926708,0.0215716388127831,0.0224729559225521,
//		0.0233922868215237,0.0243295968297081,0.0252848505888881,0.0262580120639531,0.0272490445442584,0.0282579106450099,0.0292845723086745,0.0303289908064159,0.0313911267395551,0.0324709400410572,
//		0.0335683899770422,0.0346834351483225,0.0358160334919637,0.0369661422828719,0.0381337181354052,0.0393187170050104,0.0405210941898847,0.0417408043326614,0.0429778014221215,0.0442320387949294,
//		0.0455034691373926,0.0467920444872470,0.0480977162354663,0.0494204351280952,0.0507601512681078,0.0521168141172897,0.0534903724981444,0.0548807745958242,0.0562879679600843,0.0577118995072619,
//		0.0591525155222783,0.0606097616606654,0.0620835829506157,0.0635739237950560,0.0650807279737445,0.0666039386453922,0.0681434983498062,0.0696993490100581,0.0712714319346745,0.0728596878198510,
//		0.0744640567516894,0.0760844782084577,0.0777208910628734,0.0793732335844094,0.0810414434416222,0.0827254577045039,0.0844252128468556,0.0861406447486842,0.0878716886986206,0.0896182793963619,
//		0.0913803509551333,0.0931578369041747,0.0949506701912479,0.0967587831851655,0.0985821076783428,0.100420574889371,0.102274115465610,0.104142659485807,0.106026136462734,0.107924475345844,
//		0.109837604523954,0.111765451827947,0.113707944533490,0.115665009363784,0.117636572492321,0.119622559545675,0.121622895606305,0.123637505215381,0.125666312375629,0.127709240554203,
//		0.129766212685566,0.131837151174401,0.133921977898538,0.136020614211899,0.138132980947465,0.140258998420264,0.142398586430377,0.144551664265962,0.146718150706299,0.148897964024853,
//		0.151091021992361,0.153297241879930,0.155516540462157,0.157748834020272,0.159994038345295,0.162252068741212,0.164522840028169,0.166806266545687,0.169102262155893,0.171410740246769,
//		0.173731613735420,0.176064795071357,0.178410196239802,0.180767728765008,0.183137303713596,0.185518831697907,0.187912222879380,0.190317386971936,0.192734233245387,0.195162670528854,
//		0.197602607214214,0.200053951259548,0.202516610192617,0.204990491114349,0.207475500702346,0.209971545214401,0.212478530492035,0.214996361964052,0.217524944650102,0.220064183164268,
//		0.222613981718660,0.225174244127032,0.227744873808411,0.230325773790736,0.232916846714519,0.235517994836519,0.238129120033426,0.240750123805564,0.243380907280608,0.246021371217312,
//		0.248671416009252,0.251330941688587,0.253999847929824,0.256678034053612,0.259365399030529,0.262061841484900,0.264767259698622,0.267481551614996,0.270204614842579,0.272936346659051,
//		0.275676644015082,0.278425403538226,0.281182521536818,0.283947894003883,0.286721416621066,0.289502984762559,0.292292493499057,0.295089837601706,0.297894911546082,0.300707609516166,
//		0.303527825408337,0.306355452835375,0.309190385130475,0.312032515351269,0.314881736283861,0.317737940446871,0.320601020095493,0.323470867225554,0.326347373577590,0.329230430640933,
//		0.332119929657800,0.335015761627399,0.337917817310040,0.340825987231253,0.343740161685923,0.346660230742423,0.349586084246766,0.352517611826756,0.355454702896154,0.358397246658848,
//		0.361345132113036,0.364298248055408,0.367256483085347,0.370219725609126,0.373187863844121,0.376160785823026,0.379138379398078,0.382120532245286,0.385107131868672,0.388098065604509,
//		0.391093220625574,0.394092483945407,0.397095742422567,0.400102882764905,0.403113791533836,0.406128355148618,0.409146459890638,0.412167991907700,0.415192837218320,0.418220881716027,
//		0.421252011173667,0.424286111247712,0.427323067482572,0.430362765314916,0.433405090077989,0.436449927005942,0.439497161238159,0.442546677823589,0.445598361725086,0.448652097823743,
//		0.451707770923241,0.454765265754190,0.457824466978478,0.460885259193623,0.463947526937127,0.467011154690830,0.470076026885267,0.473142027904030,0.476209042088129,0.479276953740352,
//		0.482345647129634,0.485415006495420,0.488484916052031,0.491555259993034,0.494625922495611,0.497696787724924,0.500767739838490,0.503838662990547,0.506909441336427,0.509979959036923,
//		0.513050100262661,0.516119749198468,0.519188790047744,0.522257107036826,0.525324584419356,0.528391106480652,0.531456557542067,0.534520821965356,0.537583784157038,0.540645328572756,
//		0.543705339721636,0.546763702170644,0.549820300548938,0.552875019552224,0.555927743947105,0.558978358575423,0.562026748358610,0.565072798302026,0.568116393499295,0.571157419136643,
//		0.574195760497226,0.577231302965463,0.580263932031351,0.583293533294794,0.586319992469910,0.589343195389348,0.592363028008595,0.595379376410272,0.598392126808439,0.601401165552882,
//		0.604406379133402,0.607407654184098,0.610404877487640,0.613397935979547,0.616386716752442,0.619371107060320,0.622350994322798,0.625326266129360,0.628296810243601,0.631262514607457,
//		0.634223267345437,0.637178956768838,0.640129471379962,0.643074699876320,0.646014531154834,0.648948854316022,0.651877558668189,0.654800533731594,0.657717669242628,0.660628855157963,
//		0.663533981658711,0.666432939154562,0.669325618287922,0.672211909938034,0.675091705225098,0.677964895514375,0.680831372420289,0.683691027810512,0.686543753810046,0.689389442805288,
//		0.692227987448096,0.695059280659833,0.697883215635408,0.700699685847307,0.703508585049608,0.706309807281992,0.709103246873738,0.711888798447712,0.714666356924337,0.717435817525563,
//		0.720197075778817,0.722950027520942,0.725694568902128,0.728430596389831,0.731158006772678,0.733876697164358,0.736586565007504,0.739287508077566,0.741979424486662,0.744662212687422,
//		0.747335771476822,0.750000000000000,0.752654797754059,0.755300064591861,0.757935700725802,0.760561606731579,0.763177683551939,0.765783832500416,0.768379955265053,0.770965953912114,
//		0.773541730889773,0.776107189031799,0.778662231561218,0.781206762093965,0.783740684642520,0.786263903619530,0.788776323841411,0.791277850531944,0.793768389325845,0.796247846272330,
//		0.798716127838655,0.801173140913645,0.803618792811209,0.806052991273832,0.808475644476059,0.810886661027960,0.813285949978571,0.815673420819333,0.818048983487500,0.820412548369540,
//		0.822764026304515,0.825103328587442,0.827430366972642,0.829745053677070,0.832047301383620,0.834337023244426,0.836614132884136,0.838878544403169,0.841130172380956,0.843368931879162,
//		0.845594738444892,0.847807508113875,0.850007157413629,0.852193603366619,0.854366763493374,0.856526555815610,0.858672898859316,0.860805711657828,0.862924913754887,0.865030425207670,
//		0.867122166589807,0.869200058994379,0.871264024036890,0.873313983858230,0.875349861127608,0.877371579045470,0.879379061346395,0.881372232301977,0.883351016723675,0.885315339965651,
//		0.887265127927592,0.889200307057495,0.891120804354451,0.893026547371394,0.894917464217833,0.896793483562569,0.898654534636379,0.900500547234692,0.902331451720234,0.904147179025654,
//		0.905947660656133,0.907732828691964,0.909502615791117,0.911256955191777,0.912995780714867,0.914719026766536,0.916426628340643,0.918118521021203,0.919794640984819,0.921454925003090,
//		0.923099310444994,0.924727735279253,0.926340138076674,0.927936458012461,0.929516634868517,0.931080609035708,0.932628321516118,0.934159713925270,0.935674728494331,0.937173308072291,
//		0.938655396128118,0.940120936752891,0.941569874661908,0.943002155196774,0.944417724327462,0.945816528654348,0.947198515410229,0.948563632462315,0.949911828314189,0.951243052107756,
//		0.952557253625161,0.953854383290677,0.955134392172584,0.956397231985009,0.957642855089749,0.958871214498068,0.960082263872470,0.961275957528448,0.962452250436204,0.963611098222351,
//		0.964752457171584,0.965876284228334,0.966982536998386,0.968071173750483,0.969142153417897,0.970195435599979,0.971230980563686,0.972248749245076,0.973248703250784,0.974230804859466,
//		0.975195017023231,0.976141303369028,0.977069628200024,0.977979956496953,0.978872253919429,0.979746486807249,0.980602622181658,0.981440627746597,0.982260471889917,0.983062123684575,
//		0.983845552889800,0.984610729952230,0.985357626007035,0.986086212878995,0.986796463083574,0.987488349827948,0.988161847012022,0.988816929229408,0.989453571768390,0.990071750612854,
//		0.990671442443191,0.991252624637181,0.991815275270844,0.992359373119266,0.992884897657402,0.993391829060851,0.993880148206600,0.994349836673749,0.994800876744203,0.995233251403345,
//		0.995646944340671,0.996041939950411,0.996418223332115,0.996775780291217,0.997114597339568,0.997434661695948,0.997735961286542,0.998018484745405,0.998282221414882,0.998527161346014,
//		0.998753295298914,0.998960614743114,0.999149111857886,0.999318779532538,0.999469611366685,0.999601601670485,0.999714745464859,0.999809038481674,0.999884477163907,0.999941058665780,
//		0.999978780852864,0.999997642302162,0.999997642302162,0.999978780852864,0.999941058665780,0.999884477163907,0.999809038481674,0.999714745464859,0.999601601670485,0.999469611366685,
//		0.999318779532538,0.999149111857886,0.998960614743114,0.998753295298914,0.998527161346014,0.998282221414882,0.998018484745405,0.997735961286542,0.997434661695948,0.997114597339568,
//		0.996775780291217,0.996418223332115,0.996041939950411,0.995646944340671,0.995233251403345,0.994800876744203,0.994349836673749,0.993880148206600,0.993391829060851,0.992884897657402,
//		0.992359373119266,0.991815275270844,0.991252624637181,0.990671442443191,0.990071750612854,0.989453571768390,0.988816929229408,0.988161847012022,0.987488349827948,0.986796463083574,
//		0.986086212878995,0.985357626007035,0.984610729952230,0.983845552889800,0.983062123684575,0.982260471889917,0.981440627746597,0.980602622181658,0.979746486807249,0.978872253919429,
//		0.977979956496953,0.977069628200024,0.976141303369028,0.975195017023231,0.974230804859467,0.973248703250784,0.972248749245076,0.971230980563686,0.970195435599979,0.969142153417897,
//		0.968071173750483,0.966982536998386,0.965876284228334,0.964752457171584,0.963611098222351,0.962452250436204,0.961275957528448,0.960082263872471,0.958871214498068,0.957642855089749,
//		0.956397231985009,0.955134392172584,0.953854383290677,0.952557253625161,0.951243052107756,0.949911828314189,0.948563632462315,0.947198515410229,0.945816528654348,0.944417724327462,
//		0.943002155196774,0.941569874661908,0.940120936752891,0.938655396128118,0.937173308072291,0.935674728494331,0.934159713925270,0.932628321516118,0.931080609035708,0.929516634868517,
//		0.927936458012461,0.926340138076674,0.924727735279253,0.923099310444994,0.921454925003090,0.919794640984819,0.918118521021203,0.916426628340643,0.914719026766536,0.912995780714867,
//		0.911256955191777,0.909502615791117,0.907732828691964,0.905947660656133,0.904147179025654,0.902331451720234,0.900500547234692,0.898654534636379,0.896793483562569,0.894917464217833,
//		0.893026547371394,0.891120804354451,0.889200307057495,0.887265127927592,0.885315339965652,0.883351016723675,0.881372232301977,0.879379061346396,0.877371579045470,0.875349861127608,
//		0.873313983858230,0.871264024036890,0.869200058994379,0.867122166589807,0.865030425207670,0.862924913754887,0.860805711657828,0.858672898859316,0.856526555815610,0.854366763493374,
//		0.852193603366619,0.850007157413630,0.847807508113875,0.845594738444892,0.843368931879163,0.841130172380956,0.838878544403169,0.836614132884137,0.834337023244426,0.832047301383620,
//		0.829745053677070,0.827430366972643,0.825103328587442,0.822764026304515,0.820412548369540,0.818048983487500,0.815673420819333,0.813285949978571,0.810886661027960,0.808475644476059,
//		0.806052991273832,0.803618792811209,0.801173140913645,0.798716127838655,0.796247846272330,0.793768389325846,0.791277850531944,0.788776323841411,0.786263903619530,0.783740684642520,
//		0.781206762093965,0.778662231561218,0.776107189031800,0.773541730889774,0.770965953912115,0.768379955265053,0.765783832500416,0.763177683551939,0.760561606731580,0.757935700725802,
//		0.755300064591861,0.752654797754059,0.750000000000000,0.747335771476822,0.744662212687422,0.741979424486662,0.739287508077567,0.736586565007505,0.733876697164358,0.731158006772678,
//		0.728430596389831,0.725694568902128,0.722950027520942,0.720197075778818,0.717435817525564,0.714666356924337,0.711888798447712,0.709103246873738,0.706309807281992,0.703508585049608,
//		0.700699685847307,0.697883215635408,0.695059280659833,0.692227987448096,0.689389442805288,0.686543753810046,0.683691027810513,0.680831372420290,0.677964895514375,0.675091705225098,
//		0.672211909938034,0.669325618287923,0.666432939154563,0.663533981658711,0.660628855157963,0.657717669242628,0.654800533731594,0.651877558668189,0.648948854316023,0.646014531154834,
//		0.643074699876321,0.640129471379962,0.637178956768838,0.634223267345437,0.631262514607458,0.628296810243601,0.625326266129360,0.622350994322798,0.619371107060320,0.616386716752442,
//		0.613397935979546,0.610404877487641,0.607407654184098,0.604406379133402,0.601401165552882,0.598392126808439,0.595379376410272,0.592363028008594,0.589343195389348,0.586319992469910,
//		0.583293533294794,0.580263932031351,0.577231302965463,0.574195760497226,0.571157419136643,0.568116393499295,0.565072798302026,0.562026748358611,0.558978358575423,0.555927743947105,
//		0.552875019552224,0.549820300548938,0.546763702170644,0.543705339721637,0.540645328572757,0.537583784157038,0.534520821965356,0.531456557542067,0.528391106480653,0.525324584419356,
//		0.522257107036826,0.519188790047744,0.516119749198469,0.513050100262661,0.509979959036923,0.506909441336428,0.503838662990548,0.500767739838490,0.497696787724924,0.494625922495611,
//		0.491555259993034,0.488484916052031,0.485415006495420,0.482345647129635,0.479276953740352,0.476209042088129,0.473142027904030,0.470076026885267,0.467011154690830,0.463947526937127,
//		0.460885259193623,0.457824466978477,0.454765265754190,0.451707770923241,0.448652097823743,0.445598361725086,0.442546677823589,0.439497161238159,0.436449927005943,0.433405090077989,
//		0.430362765314916,0.427323067482573,0.424286111247712,0.421252011173667,0.418220881716027,0.415192837218320,0.412167991907700,0.409146459890639,0.406128355148619,0.403113791533836,
//		0.400102882764905,0.397095742422567,0.394092483945407,0.391093220625574,0.388098065604509,0.385107131868672,0.382120532245287,0.379138379398078,0.376160785823026,0.373187863844121,
//		0.370219725609127,0.367256483085347,0.364298248055408,0.361345132113036,0.358397246658848,0.355454702896154,0.352517611826756,0.349586084246767,0.346660230742424,0.343740161685923,
//		0.340825987231253,0.337917817310040,0.335015761627399,0.332119929657800,0.329230430640933,0.326347373577590,0.323470867225554,0.320601020095494,0.317737940446872,0.314881736283860,
//		0.312032515351269,0.309190385130475,0.306355452835375,0.303527825408337,0.300707609516166,0.297894911546082,0.295089837601706,0.292292493499057,0.289502984762560,0.286721416621066,
//		0.283947894003883,0.281182521536818,0.278425403538226,0.275676644015082,0.272936346659051,0.270204614842580,0.267481551614996,0.264767259698622,0.262061841484900,0.259365399030529,
//		0.256678034053612,0.253999847929825,0.251330941688587,0.248671416009252,0.246021371217312,0.243380907280608,0.240750123805564,0.238129120033426,0.235517994836519,0.232916846714519,
//		0.230325773790736,0.227744873808411,0.225174244127032,0.222613981718660,0.220064183164268,0.217524944650102,0.214996361964052,0.212478530492035,0.209971545214401,0.207475500702346,
//		0.204990491114349,0.202516610192617,0.200053951259548,0.197602607214214,0.195162670528854,0.192734233245387,0.190317386971936,0.187912222879380,0.185518831697907,0.183137303713596,
//		0.180767728765008,0.178410196239802,0.176064795071357,0.173731613735420,0.171410740246769,0.169102262155893,0.166806266545687,0.164522840028169,0.162252068741212,0.159994038345295,
//		0.157748834020272,0.155516540462157,0.153297241879930,0.151091021992362,0.148897964024853,0.146718150706299,0.144551664265962,0.142398586430378,0.140258998420264,0.138132980947465,
//		0.136020614211899,0.133921977898539,0.131837151174401,0.129766212685566,0.127709240554203,0.125666312375629,0.123637505215381,0.121622895606305,0.119622559545675,0.117636572492321,
//		0.115665009363784,0.113707944533490,0.111765451827947,0.109837604523954,0.107924475345844,0.106026136462734,0.104142659485807,0.102274115465610,0.100420574889371,0.0985821076783430,
//		0.0967587831851654,0.0949506701912478,0.0931578369041747,0.0913803509551335,0.0896182793963619,0.0878716886986207,0.0861406447486842,0.0844252128468558,0.0827254577045039,0.0810414434416224,
//		0.0793732335844096,0.0777208910628734,0.0760844782084577,0.0744640567516894,0.0728596878198511,0.0712714319346744,0.0696993490100583,0.0681434983498063,0.0666039386453924,0.0650807279737445,
//		0.0635739237950560,0.0620835829506158,0.0606097616606653,0.0591525155222784,0.0577118995072620,0.0562879679600845,0.0548807745958242,0.0534903724981444,0.0521168141172897,0.0507601512681081,
//		0.0494204351280953,0.0480977162354664,0.0467920444872471,0.0455034691373925,0.0442320387949293,0.0429778014221215,0.0417408043326616,0.0405210941898847,0.0393187170050105,0.0381337181354053,
//		0.0369661422828720,0.0358160334919637,0.0346834351483226,0.0335683899770424,0.0324709400410572,0.0313911267395551,0.0303289908064159,0.0292845723086746,0.0282579106450098,0.0272490445442585,
//		0.0262580120639532,0.0252848505888881,0.0243295968297081,0.0233922868215238,0.0224729559225522,0.0215716388127830,0.0206883694926709,0.0198231812818516,0.0189761068178863,0.0181471780550296,
//		0.0173364262630248,0.0165438820259238,0.0157695752409334,0.0150135351172876,0.0142757901751459,0.0135563682445171,0.0128552964642096,0.0121726012808081,0.0115083084476750,0.0108624430239797,
//		0.0102350293737528,0.00962609116496760,0.00903565136864670,0.00846373225799557,0.00791035540756274,0.00737554169242533,0.00685931128740214,0.00636168366629197,0.00588267760113992,0.00542231116152825,
//		0.00498060171389536,0.00455756592088052,0.00415321974069549,0.00376757842652192,0.00340065652593669,0.00305246788036268,0.00272302562454685,0.00241234218606456,0.00212042928485096,0.00184729793275873,
//		0.00159295843314272,0.00135742038047137,0.00114069265996464,0.000942783447258899,0.000763700208098517,0.000603449698054204,0.000462037962268236,0.000339470335226355,0.000235751440556431,0.000150885190854377,
//		8.48747875361600e-05,3.77227207172259e-05,9.43076911874563e-06,0};

extern SemaphoreHandle_t calculateSemaphore1;
extern SemaphoreHandle_t calculateSemaphore2;

void DMA1channel3_mem2mem_init(uint32_t *source, uint32_t *dest)
{
	//enabling clock
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

	//selecting DMA1 channel 3

    //memory to memory initialization
	DMA1_Channel3->CPAR = (uint32_t)(source); //with DIR set to 0 this is the source address
	DMA1_Channel3->CMAR = (uint32_t)dest; // with DIR set to 0 this is the destination address
	DMA1_Channel3->CNDTR = NUM_TO_COPY;
	DMA1_Channel3->CCR |= (DMA_CCR_MINC);
	DMA1_Channel3->CCR |= (DMA_CCR_PINC);
	DMA1_Channel3->CCR = ((DMA1_Channel3->CCR)&~(DMA_CCR_MSIZE))
							|(DMA_CCR_MSIZE_0);//clearing size and setting length to 16bits
	DMA1_Channel3->CCR = ((DMA1_Channel3->CCR)&~(DMA_CCR_PSIZE))
							|(DMA_CCR_PSIZE_0);
	DMA1_Channel3->CCR &= ~(DMA_CCR_DIR); //setting DIR to 0
	DMA1_Channel3->CCR |= (DMA_CCR_MEM2MEM); //enabling memory to memory
	DMA1_Channel3->CCR |= (DMA_CCR_TCIE); //enable interrupts at transfer complete

	NVIC->ISER[0] = (1<<(DMA1_Channel3_IRQn & 0x1F)); //enable interrupts for channel 3
	__enable_irq();




}

/*
 * for  DMA1channel1_ADC_init(uint32_t * dest)
 * interrupts are enabled in order to allow for synchronizing calculations with semaphores
 */

void DMA1channel1_ADC_init(uint32_t * dest)
{
	//enabling clock
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

    //peripheral to memory initialization
	DMA1_CSELR->CSELR &= ~(DMA_CSELR_C1S); //select dma channel
	DMA1_Channel1->CPAR = (uint32_t)&(ADC1->DR); //with DIR set to 0 this is the source address
	DMA1_Channel1->CMAR = (uint32_t)(dest); // with DIR set to 0 this is the destination address
	DMA1_Channel1->CNDTR = NUM_TO_COPY;  // number to copy
	DMA1_Channel1->CCR &= ~(DMA_CCR_DIR); //setting DIR to 0
	DMA1_Channel1->CCR |= (DMA_CCR_CIRC); //enable circular modele
	DMA1_Channel1->CCR |= (DMA_CCR_MINC); //increments address
	DMA1_Channel1->CCR &= ~(DMA_CCR_PINC);//keeps address the same
	DMA1_Channel1->CCR = ((DMA1_Channel1->CCR)&~(DMA_CCR_MSIZE))|(DMA_CCR_MSIZE_0);//clearing size and setting length to 16bits
	DMA1_Channel1->CCR = ((DMA1_Channel1->CCR)&~(DMA_CCR_PSIZE))|(DMA_CCR_PSIZE_0);//clearing size and setting length to 16bits

	DMA1_Channel1->CCR |= (DMA_CCR_TCIE); //enable interrupts at transfer complete
	DMA1_Channel1->CCR |= (DMA_CCR_HTIE); //enable interrupts at half transfer


	NVIC->ISER[0] = (1<<(DMA1_Channel1_IRQn & 0x1F)); //enable interrupts for channel 1

	//this mcu uses 4 bits for priority level, uses msb
//	NVIC->IP[DMA1_Channel1_IRQn] = (0x70 << ((DMA1_Channel1_IRQn%4)*8)); //modulus since the remainder determines position in register for priority
	NVIC_SetPriority(DMA1_Channel1_IRQn, 0x19);
	__enable_irq();

	DMA1_Channel1->CCR |= (DMA_CCR_EN); //activating channel

}



void DMA1_Channel3_IRQHandler()
{
	if(DMA1->ISR & DMA_ISR_TCIF3)
	{
	  	GPIOC->ODR ^= GPIO_ODR_OD1; //toggle off
		  DMA1_Channel3->CCR &= ~(DMA_CCR_EN); //disable channel 3
		  DMA1->IFCR |= (DMA_IFCR_CTCIF3); //clear interrupt flag
		  DMA1->IFCR |= (DMA_IFCR_CGIF3); //clear global interrupt flag
		  DMA1->IFCR |= (DMA_IFCR_CHTIF3); //clear half transfer flag

	}
}


void DMA1_Channel1_IRQHandler()
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if(DMA1->ISR & DMA_ISR_HTIF1)
	{
		DMA1->IFCR |= (DMA_IFCR_CHTIF1); //clear half transfer flag


		xSemaphoreGiveFromISR(calculateSemaphore1, &xHigherPriorityTaskWoken );
	}
	else if(DMA1->ISR & DMA_ISR_TCIF1)
	{

		  DMA1->IFCR |= (DMA_IFCR_CTCIF1); //clear interrupt flag
		  DMA1->IFCR |= (DMA_IFCR_CGIF1); //clear global interrupt flag
		  xSemaphoreGiveFromISR(calculateSemaphore2, &xHigherPriorityTaskWoken );

	}

}

void hanning(q15_t *window, uint16_t length) {

    for (uint16_t i = 0; i < length; i++) {
        window[i] = (q15_t)(0.5 - 0.5 * cos(2 * PI * i / (length - 1)));
    }
}

void applyhanning(q15_t *samples, uint16_t length){

    for (uint16_t i = 0; i < length; i++) {
        samples[i] = hanningwind[i]*samples[i];
    }
}
