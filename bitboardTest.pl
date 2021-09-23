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

KungFuChess::Bitboards::setupInitialPosition();
#print KungFuChess::Bitboards::debug();

#print KungFuChess::Bitboards::pretty();
print KungFuChess::Bitboards::pretty_ai();
#my ($score, $moves) = KungFuChess::Bitboards::evaluate();
#KungFuChess::Bitboards::aiThink(0.5, 1);
my ($score, $bestMoves, $moves) = (0.0, [], []);

$| = 1;

my $go = 1;
while ($go) {
    print "enter move or white/black (for ai)\n";
    my $input = <STDIN>;
    chomp($input);

    if ($input =~ m/^[a-z][0-9][a-z][0-9]$/) {
        my ($fr_bb, $to_bb, $fr_rank, $fr_file, $to_rank, $to_file) = KungFuChess::Bitboards::parseMove($input);
        print KungFuChess::Bitboards::move($fr_bb, $to_bb);
        print "\n\n";
        print KungFuChess::Bitboards::pretty_ai();
    } elsif ($input =~ m/^freeze ([a-z][0-9])$/) {
        my $input = $1 . $1;
        my ($fr_bb, $to_bb, $fr_rank, $fr_file, $to_rank, $to_file) = KungFuChess::Bitboards::parseMove($input);
        KungFuChess::Bitboards::setFrozen($fr_bb);
        KungFuChess::Bitboards::resetAiBoards();
    } elsif ($input =~ m/^induce [a-z][0-9][a-z][0-9]$/) {
        print "(not implemented)\n";
    } elsif ($input =~ m/^debug (\d+)$/) {
        KungFuChess::Bitboards::setDebugLevel($1);
    } elsif ($input =~ m/^eval$/) {
        print KungFuChess::Bitboards::pretty_ai();
        my ($eval, $moves) = KungFuChess::Bitboards::evaluate(1);
        print "eval: $eval\n";
    } elsif ($input =~ m/^(white|black)$/) {
        my $cIn = $1;
        my $color = ($cIn =~ 'white' ? 1 : 2);
        my $suggestedMoves = KungFuChess::Bitboards::aiRecommendMoves($color);
        foreach my $move (@$suggestedMoves) {
            print "moving...";
            print KungFuChess::BBHash::getSquareFromBB($move->[0]);
            print KungFuChess::BBHash::getSquareFromBB($move->[1]);
            print "\n";
            KungFuChess::Bitboards::move($move->[0], $move->[1]);
        }
        print "\n\n";
        print KungFuChess::Bitboards::pretty_ai();
    } elsif ($input =~ m/^think (\d+) ([\d\.]+)$/) {
        my $time = $1;
        my $depth = $2;
        my $start = time();
        print KungFuChess::Bitboards::pretty_ai();
        my $moves = KungFuChess::Bitboards::aiThink($depth, $time);
        my $suggestedMoves = KungFuChess::Bitboards::aiRecommendMoves(2);

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
        ($score, $bestMoves, $moves) = 
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
        KungFuChess::Bitboards::aiThink(1, 0.5);
        ($score, $bestMoves, $moves) = 
            KungFuChess::Bitboards::aiThink(2, 5);

        print "eval $cIn\n";
        print "displayMoves\n";
        print "PONDER: $ponder\n";
        print "FILTER: $filter\n";
        KungFuChess::BBHash::displayMoves($moves, $mycolor, $score, $ponder, undef, $filter);
        KungFuChess::BBHash::displayBestMoves($bestMoves, $mycolor, $score, $ponder, undef, $filter);
    } elsif ($input =~ m/^fen (.+)$/){
        KungFuChess::Bitboards::loadFENstring($1);
    } elsif ($input =~ m/^frozen$/){
        print KungFuChess::Bitboards::prettyFrozen();
    } elsif ($input =~ m/^fen$/){
        print KungFuChess::Bitboards::getFENstring();
        print "\n";
    } elsif ($input =~ m/^clear$/) {
        print KungFuChess::Bitboards::setCurrentMoves();
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
        print "  show <white|black> (show moves available)\n";
        print "  eval\n";
        print "  <move> (i.e. e2e4)\n";
        print "  freeze <move> (i.e. e2e4)\n";
        print "  frozen (show frozen moves)\n";
        print "  debug <level>\n";
        print "  clear (clear ai moves)\n";
        print "  induce <move> (i.e. induce e2e4)\n";
        print "  white|black (make moves off recommendMoves)\n";

    }
    if ($input eq 'q') {
        $go = 0;
    }
}
