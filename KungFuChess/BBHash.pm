### hardcoded translation of BB squares to board squares

use strict; use warnings;
package KungFuChess::BBHash;
use Data::Dumper;

my $way = '4way';

our %bitboardToSquare4way = (
    '128' => 'c12',
    '64' => 'd12',
    '32' => 'e12',
    '16' => 'f12',
    '8' => 'g12',
    '4' => 'h12',
    '2' => 'i12',
    '1' => 'j12',
    '32768' => 'c11',
    '16384' => 'd11',
    '8192' => 'e11',
    '4096' => 'f11',
    '2048' => 'g11',
    '1024' => 'h11',
    '512' => 'i11',
    '256' => 'j11',
    '134217728' => 'a10',
    '67108864' => 'b10',
    '33554432' => 'c10',
    '16777216' => 'd10',
    '8388608' => 'e10',
    '4194304' => 'f10',
    '2097152' => 'g10',
    '1048576' => 'h10',
    '524288' => 'i10',
    '262144' => 'j10',
    '131072' => 'k10',
    '65536' => 'l10',
    '549755813888' => 'a9',
    '274877906944' => 'b9',
    '137438953472' => 'c9',
    '68719476736' => 'd9',
    '34359738368' => 'e9',
    '17179869184' => 'f9',
    '8589934592' => 'g9',
    '4294967296' => 'h9',
    '2147483648' => 'i9',
    '1073741824' => 'j9',
    '536870912' => 'k9',
    '268435456' => 'l9',
    '2251799813685248' => 'a8',
    '1125899906842624' => 'b8',
    '562949953421312' => 'c8',
    '281474976710656' => 'd8',
    '140737488355328' => 'e8',
    '70368744177664' => 'f8',
    '35184372088832' => 'g8',
    '17592186044416' => 'h8',
    '8796093022208' => 'i8',
    '4398046511104' => 'j8',
    '2199023255552' => 'k8',
    '1099511627776' => 'l8',
    '9223372036854775808' => 'a7',
    '4611686018427387904' => 'b7',
    '2305843009213693952' => 'c7',
    '1152921504606846976' => 'd7',
    '576460752303423488' => 'e7',
    '288230376151711744' => 'f7',
    '144115188075855872' => 'g7',
    '72057594037927936' => 'h7',
    '36028797018963968' => 'i7',
    '18014398509481984' => 'j7',
    '9007199254740992' => 'k7',
    '4503599627370496' => 'l7',
    '37778931862957161709568' => 'a6',
    '18889465931478580854784' => 'b6',
    '9444732965739290427392' => 'c6',
    '4722366482869645213696' => 'd6',
    '2361183241434822606848' => 'e6',
    '1180591620717411303424' => 'f6',
    '590295810358705651712' => 'g6',
    '295147905179352825856' => 'h6',
    '147573952589676412928' => 'i6',
    '73786976294838206464' => 'j6',
    '36893488147419103232' => 'k6',
    '18446744073709551616' => 'l6',
    '154742504910672534362390528' => 'a5',
    '77371252455336267181195264' => 'b5',
    '38685626227668133590597632' => 'c5',
    '19342813113834066795298816' => 'd5',
    '9671406556917033397649408' => 'e5',
    '4835703278458516698824704' => 'f5',
    '2417851639229258349412352' => 'g5',
    '1208925819614629174706176' => 'h5',
    '604462909807314587353088' => 'i5',
    '302231454903657293676544' => 'j5',
    '151115727451828646838272' => 'k5',
    '75557863725914323419136' => 'l5',
    '633825300114114700748351602688' => 'a4',
    '316912650057057350374175801344' => 'b4',
    '158456325028528675187087900672' => 'c4',
    '79228162514264337593543950336' => 'd4',
    '39614081257132168796771975168' => 'e4',
    '19807040628566084398385987584' => 'f4',
    '9903520314283042199192993792' => 'g4',
    '4951760157141521099596496896' => 'h4',
    '2475880078570760549798248448' => 'i4',
    '1237940039285380274899124224' => 'j4',
    '618970019642690137449562112' => 'k4',
    '309485009821345068724781056' => 'l4',
    '2596148429267413814265248164610048' => 'a3',
    '1298074214633706907132624082305024' => 'b3',
    '649037107316853453566312041152512' => 'c3',
    '324518553658426726783156020576256' => 'd3',
    '162259276829213363391578010288128' => 'e3',
    '81129638414606681695789005144064' => 'f3',
    '40564819207303340847894502572032' => 'g3',
    '20282409603651670423947251286016' => 'h3',
    '10141204801825835211973625643008' => 'i3',
    '5070602400912917605986812821504' => 'j3',
    '2535301200456458802993406410752' => 'k3',
    '1267650600228229401496703205376' => 'l3',
    '664613997892457936451903530140172288' => 'c2',
    '332306998946228968225951765070086144' => 'd2',
    '166153499473114484112975882535043072' => 'e2',
    '83076749736557242056487941267521536' => 'f2',
    '41538374868278621028243970633760768' => 'g2',
    '20769187434139310514121985316880384' => 'h2',
    '10384593717069655257060992658440192' => 'i2',
    '5192296858534827628530496329220096' => 'j2',
    '170141183460469231731687303715884105728' => 'c1',
    '85070591730234615865843651857942052864' => 'd1',
    '42535295865117307932921825928971026432' => 'e1',
    '21267647932558653966460912964485513216' => 'f1',
    '10633823966279326983230456482242756608' => 'g1',
    '5316911983139663491615228241121378304' => 'h1',
    '2658455991569831745807614120560689152' => 'i1',
    '1329227995784915872903807060280344576' => 'j1',
);

our %bitboardToSquare = (
    '72057594037927936' => 'a8',
    '144115188075855872' => 'b8',
    '288230376151711744' => 'c8',
    '576460752303423488' => 'd8',
    '1152921504606846976' => 'e8',
    '2305843009213693952' => 'f8',
    '4611686018427387904' => 'g8',
    '9223372036854775808' => 'h8',
    '281474976710656' => 'a7',
    '562949953421312' => 'b7',
    '1125899906842624' => 'c7',
    '2251799813685248' => 'd7',
    '4503599627370496' => 'e7',
    '9007199254740992' => 'f7',
    '18014398509481984' => 'g7',
    '36028797018963968' => 'h7',
    '1099511627776' => 'a6',
    '2199023255552' => 'b6',
    '4398046511104' => 'c6',
    '8796093022208' => 'd6',
    '17592186044416' => 'e6',
    '35184372088832' => 'f6',
    '70368744177664' => 'g6',
    '140737488355328' => 'h6',
    '4294967296' => 'a5',
    '8589934592' => 'b5',
    '17179869184' => 'c5',
    '34359738368' => 'd5',
    '68719476736' => 'e5',
    '137438953472' => 'f5',
    '274877906944' => 'g5',
    '549755813888' => 'h5',
    '16777216' => 'a4',
    '33554432' => 'b4',
    '67108864' => 'c4',
    '134217728' => 'd4',
    '268435456' => 'e4',
    '536870912' => 'f4',
    '1073741824' => 'g4',
    '2147483648' => 'h4',
    '65536' => 'a3',
    '131072' => 'b3',
    '262144' => 'c3',
    '524288' => 'd3',
    '1048576' => 'e3',
    '2097152' => 'f3',
    '4194304' => 'g3',
    '8388608' => 'h3',
    '256' => 'a2',
    '512' => 'b2',
    '1024' => 'c2',
    '2048' => 'd2',
    '4096' => 'e2',
    '8192' => 'f2',
    '16384' => 'g2',
    '32768' => 'h2',
    '1' => 'a1',
    '2' => 'b1',
    '4' => 'c1',
    '8' => 'd1',
    '16' => 'e1',
    '32' => 'f1',
    '64' => 'g1',
    '128' => 'h1',
);
use constant ({
    ### array of a move for AI
    MOVE_FR         => 0,
    MOVE_TO         => 1,
    MOVE_SCORE      => 2,
    MOVE_DISTANCE   => 3, 
    MOVE_NEXT_MOVES => 4,
    MOVE_ATTACKS    => 5,
});

sub getSquareFromBB {
    return ($way eq '2way' ? $bitboardToSquare{$_[0]} : $bitboardToSquare4way{$_[0]});
}

sub displayBestMoves {
    my $bestMoves = shift;
    my $color = shift;
    my $score = shift;
    my $ponder = shift;
    my $indent = shift;
    my $filter = shift;

    foreach my $bmove (@{$bestMoves}[$color]) {
        foreach my $move (@$bmove) {
            my ($fr_bb, $to_bb) = split('-', $move);
            print getSquareFromBB($fr_bb);
            print getSquareFromBB($to_bb);
            print "\n";
        }
    }
}

### raw moves no color passed in
sub displayMovesArray {
    my $suggestedMoves = shift;
    foreach my $move (@$suggestedMoves) {
        print getSquareFromBB($move->[MOVE_FR]) . getSquareFromBB($move->[MOVE_TO]);
        print ", scr: " . $move->[MOVE_SCORE] . "\n";
    }

    #foreach my $move (@$moves) {
        ##print Dumper($move);
        ##exit;
        #print ref $move;
        #print " $#$move\n";
        #print "$moves->[0]->[0]\n";
        #print "$moves->[1]\n";
        #print getSquareFromBB($moves->[0]) . getSquareFromBB($moves->[1]);
        #print "\n";

    #}
}

# 0 = fr_bb
# 1 = to_bb
# 2 = piece
# 3 = piece_type
# 4 = score
# 5 = child moves
sub displayMoves {
    my $moves = shift;
    my $color = shift;
    my $score = shift;
    my $ponder = shift;
    my $indent = shift;
    my $filter = shift;
    my $depth = shift // 0;
    $ponder = $ponder ? $ponder : '';
    $indent = $indent ? $indent : '';

    foreach my $move (@{$moves->[$color]}) {
        #print $k;
        my $moveS = getSquareFromBB($move->[MOVE_FR]) . getSquareFromBB($move->[MOVE_TO]);
        if (! $filter || ($moveS =~ m/^$filter/)) {
            print $indent;
            print $moveS;
            my $mScore = $move->[MOVE_SCORE];
            if ($mScore) {
                print ($mScore > $score ? " * " : "   ") ;
                print  $mScore;
            } else {
                print " NA ";
            }
            print "\n";
            if (($moveS eq $ponder) || $ponder eq 'all') {
                displayMoves(
                    $move->[MOVE_NEXT_MOVES],
                    #$color == 2 ? ($depth % 2 ? 1 : 2) : ($depth % 2 ? 2 : 1),
                    $color == 2 ? (0 ? 1 : 2) : (0 ? 2 : 1),
                    $mScore,
                    #$depth < 10 ? 'all' : '',
                    'all',
                    ' x ' x ($depth + 2),
                    $filter,
                    $depth+1
                );
            }
            if (($move eq $ponder) || $ponder eq 'all') {
                displayMoves(
                    $move->[MOVE_NEXT_MOVES],
                    $color,
                    $mScore,
                    '',
                    '   '
                );
            }
        }
        #last;
    }
}

1;
