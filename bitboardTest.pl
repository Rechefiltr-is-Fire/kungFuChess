#!/usr/bin/perl
#
use strict; use warnings;

use Cwd qw( abs_path );
use File::Basename qw( dirname );
use Time::HiRes qw(utime time);
use lib dirname(abs_path($0));
use Data::Dumper;

use KungFuChess::Bitboards;
use KungFuChess::BBHash;

my $fen = shift;
my $colorIn = shift // 2;
my $frozenIn = shift;
if ($fen) {
    #print "fen: $fen\n";
    KungFuChess::Bitboards::loadFENstring($fen);
} else {
    KungFuChess::Bitboards::setupInitialPosition();
}

KungFuChess::Bitboards::initXS();
KungFuChess::Bitboards::setPosXS();

#$frozenIn = 0x1111111111111111;

if ($frozenIn) {
    #KungFuChess::Bitboards::setFrozen($frozenIn);
    #KungFuChess::Bitboards::resetAiBoards();
}
#print KungFuChess::Bitboards::setPosXS();
print KungFuChess::Bitboards::pretty();
#print KungFuChess::Bitboards::pretty_ai();
#print "XS evaluate:\n";
print KungFuChess::Bitboards::evaluateXS();
print "\n";
KungFuChess::Bitboards::xsDebug();
exit;
#print "\nmoves:\n";
#my @moves = KungFuChess::Bitboards::getMovesXS();
#print Dumper @moves;
#print "\n\n";
#print KungFuChess::Bitboards::debug();
#print KungFuChess::Bitboards::debug2();
#my ($score, $moves) = KungFuChess::Bitboards::evaluate();
#KungFuChess::Bitboards::aiThink(0.5, 1);
my ($score, $bestMoves, $moves) = (0.0, [], []);

$| = 1;

my $go = 1;
my $input = '';
$input = 'eval';
#$input = 'induce white c2g6';
while ($go) {

    if ($input =~ m/^[a-z][0-9][a-z][0-9]$/) {
        my ($fr_bb, $to_bb, $fr_rank, $fr_file, $to_rank, $to_file) = KungFuChess::Bitboards::parseMove($input);
        print KungFuChess::Bitboards::move($fr_bb, $to_bb);
        print "\n\n";
        print KungFuChess::Bitboards::pretty_ai();
    } elsif ($input =~ m/^setFrozen ([0-9]+)$/) {
        my $frozen = $1 + 0;
        KungFuChess::Bitboards::setFrozen($frozen);
        KungFuChess::Bitboards::resetAiBoards();
        print KungFuChess::Bitboards::prettyFrozen();
    } elsif ($input =~ m/^freeze ([a-z][0-9])$/) {
        my $input = $1 . $1;
        my ($fr_bb, $to_bb, $fr_rank, $fr_file, $to_rank, $to_file) = KungFuChess::Bitboards::parseMove($input);
        KungFuChess::Bitboards::addFrozen($fr_bb);
        KungFuChess::Bitboards::resetAiBoards();
        print KungFuChess::Bitboards::prettyFrozen();
    } elsif ($input =~ m/^unfreeze ([a-z][0-9])$/) {
        my $input = $1 . $1;
        my ($fr_bb, $to_bb, $fr_rank, $fr_file, $to_rank, $to_file) = KungFuChess::Bitboards::parseMove($input);
        KungFuChess::Bitboards::unsetFrozen($fr_bb);
        KungFuChess::Bitboards::resetAiBoards();
        print KungFuChess::Bitboards::prettyFrozen();
    } elsif ($input =~ m/^induce (white|black) ([a-z][0-9])([a-z][0-9])\s*$/) {
        my $cIn = $1;
        my $inputA = $2;
        my $inputB = $3;
        print "moves: $inputA $inputB\n";
        my $color = ($cIn =~ 'white' ? 1 : 2);
        my $fr_bb = KungFuChess::Bitboards::parseSquare($inputA);
        my $to_bb = KungFuChess::Bitboards::parseSquare($inputB);
        my ($eval, $moves, $material, $attacks) = KungFuChess::Bitboards::evaluate(2);
        my ($bestMove, $bestScore) = KungFuChess::Bitboards::recommendMoveForBB($fr_bb, $to_bb, $color, $attacks);
        print "moving...$bestMove->[1], $bestScore";
        print KungFuChess::BBHash::getSquareFromBB($bestMove->[0]);
        print KungFuChess::BBHash::getSquareFromBB($bestMove->[1]);
        print "\n";
        exit;
    } elsif ($input =~ m/^debug (\d+)$/) {
        KungFuChess::Bitboards::setDebugLevel($1);
    } elsif ($input =~ m/^eval$/) {
        #print KungFuChess::Bitboards::pretty_ai();
        my ($eval, $moves, $material, $attacks) = KungFuChess::Bitboards::evaluate(1);
        # depth time color
        print "colorIn: $colorIn\n";
        #my $m_bb = KungFuChess::Bitboards::parseSquare('g4');
        #$m_bb |= KungFuChess::Bitboards::parseSquare('h5');
        #$m_bb |= KungFuChess::Bitboards::parseSquare('f3');
        #KungFuChess::Bitboards::setMoving($m_bb);
        KungFuChess::Bitboards::aiThink(1, 9999, $colorIn);
        my $bestMove = xs::getBestMove();
        #print "best: $bestMove\n";
        #print "best: " . xs::to_bb($bestMove) . "\n";
        my $nextMove = xs::getNextBestMove();
        #print "next best: $nextMove\n";
        exit;
    } elsif ($input =~ m/^(white|black)$/) {
        my $cIn = $1;
        my $color = ($cIn =~ 'white' ? 1 : 2);
        my $suggestedMoves = KungFuChess::Bitboards::aiRecommendMoves($color, 999);
        foreach my $move (@$suggestedMoves) {
            print "moving...";
            print KungFuChess::BBHash::getSquareFromBB($move->[0]);
            print KungFuChess::BBHash::getSquareFromBB($move->[1]);
            print "\n";
            KungFuChess::Bitboards::move($move->[0], $move->[1]);
        }
        print "\n\n";
        print KungFuChess::Bitboards::pretty_ai();
    } elsif ($input =~ m/^think ([\d\.]+) (\d+)$/) {
        my $depth = $1;
        my $time = $2;
        my $start = time();
        print "think time: $time, depth: $depth\n";
        print KungFuChess::Bitboards::pretty_ai();
        my ($score, $moves, $totalMaterial, $attackedBy) =
            KungFuChess::Bitboards::aiThink($depth, $time, 2);
        my $suggestedMoves = KungFuChess::Bitboards::aiRecommendMoves(2, 999);

        print "moves\n";
        print "---- white ----\n";
        KungFuChess::BBHash::displayMoves($moves, 1, $score, undef, undef, undef);
        print "---- black ----\n";
        KungFuChess::BBHash::displayMoves($moves, 2, $score, undef, undef, undef);

        print "\n----------- suggested -------------\n";
        KungFuChess::BBHash::displayMovesArray($suggestedMoves);
        #print "eval: $eval\n";
        print "time elapsed: " . (time() - $start) . "\n";
    } elsif ($input =~ m/^ponder (white|black)\s?(\S+)?\s?(\S+?)$/) {
        my $cIn = $1;
        my $ponder = $2;
        my $filter = $3;
        my $mycolor = ($cIn =~ 'white' ? 1 : 2);
        my ($color, $move, $dir, $fr_bb, $to_bb);
        ($color, $move, $dir, $fr_bb, $to_bb) = KungFuChess::Bitboards::isLegalMove(
            KungFuChess::Bitboards::parseMove($ponder)
        );
        if ($move != 0) { # MOVE_NONE
            KungFuChess::Bitboards::do_move_ai($fr_bb, $to_bb);
        } else {
            print "  $input not legal\n";
        }
        my ($score, $moves, $totalMaterial, $attackedBy) =
            KungFuChess::Bitboards::aiThink(2, 1.5);

        if ($move != 0) { # MOVE_NONE
            KungFuChess::Bitboards::undo_move_ai($fr_bb, $to_bb);
        }

        print "displayMoves\n";
        KungFuChess::BBHash::displayMoves($moves, $mycolor, $score, $ponder, undef, $filter);
        print "---- best moves ----\n";
        KungFuChess::BBHash::displayBestMoves($bestMoves, $mycolor, $score, $ponder, undef, $filter);
    } elsif ($input =~ m/^moves (white|black)$/) {
        my $cIn = $1;
        my $color = ($cIn =~ 'white' ? 1 : 2);
        my $suggestedMoves = KungFuChess::Bitboards::aiRecommendMoves($color);
        print Dumper($suggestedMoves);
    } elsif ($input =~ m/^eval (white|black)\s?(\S+)?(?:\s(\S+?))?$/) {
        my $cIn = $1;
        my $ponder = $2;
        my $filter = $3;
        my $mycolor = ($cIn =~ 'white' ? 1 : 2);
        my ($color, $move, $dir, $fr_bb, $to_bb);

        my ($score, $moves, $totalMaterial, $attackedBy) =
            KungFuChess::Bitboards::aiThink(2, 5);

        print "eval $cIn\n";
        print "displayMoves\n";
        print "PONDER: $ponder\n";
        print "FILTER: $filter\n";
        KungFuChess::BBHash::displayMoves($moves, $mycolor, $score, $ponder, undef, $filter);
        KungFuChess::BBHash::displayBestMoves($bestMoves, $mycolor, $score, $ponder, undef, $filter);
    } elsif ($input =~ m/^fen (.+)$/){
        KungFuChess::Bitboards::loadFENstring($1);
        print KungFuChess::Bitboards::setCurrentMoves(undef);
        print KungFuChess::Bitboards::pretty_ai();
    } elsif ($input =~ m/^frozen$/){
        print KungFuChess::Bitboards::prettyFrozen();
    } elsif ($input =~ m/^fen$/){
        print KungFuChess::Bitboards::getFENstring();
        print "\n";
    } elsif ($input =~ m/^clear$/) {
        print KungFuChess::Bitboards::setCurrentMoves(undef);
    } elsif ($input =~ m/^perl (.*?)$/) {
        my $perl = $1;
        print "eval perl: $perl\n";
        eval $perl;
        print "\n";
    } elsif ($input =~ m/^show (white|black)\s?(\S+)?$/) {
        my $cIn = $1;
        my $ponder = $2;
        my $filter = undef;
        my $mycolor = ($cIn =~ 'white' ? 1 : 2);

        my $moves = KungFuChess::Bitboards::getCurrentMoves($mycolor);
        KungFuChess::BBHash::displayMoves($moves, $mycolor, $score, $ponder, undef, $filter);
    } else {
        print "command not found, commands:\n";
        print "  think <time> <depth>\n";
        print "  moves <white|black> (show recommendedMoves)\n";
        print "  show <white|black> <filter> (show moves available)\n";
        print "  ponder <white|black> <filter> (move and show moves tree)\n";
        print "  eval\n";
        print "  <move> (i.e. e2e4)\n";
        print "  setFrozen <bb> (i.e. 1024)\n";
        print "  freeze <move> (i.e. e2e4)\n";
        print "  unfreeze <move> (i.e. e2e4)\n";
        print "  frozen (show frozen moves)\n";
        print "  debug <level>\n";
        print "  clear (clear ai moves)\n";
        print "  induce <move> (i.e. induce e1e2)\n";
        print "  white|black (make moves off recommendMoves)\n";

    }
    print "enter move or white/black (for ai)\n";
    $input = <STDIN>;
    chomp($input);
    if ($input eq 'q') {
        $go = 0;
    }
}
