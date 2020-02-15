#!/usr/bin/perl
use strict; use warnings;

package KungFuChess::Player;
use JSON::XS;

sub new {
	my $class = shift;

	my $self = {};
	bless( $self, $class );

	if ($self->_init(@_)){
		return $self;
	} else {
		return undef;
	}
}

sub _init {
    my $self = shift;
    my ($data, $dbh) = @_;

    $self->{dbh} = $dbh;

    if (defined($data->{userId})) { 
        return $self->_loadById($data->{userId});
    } elsif (defined($data->{screenname})) { 
        return $self->_loadByScreenname($data->{screenname});
    } elsif (defined($data->{row})) { 
        return $self->_loadByRow($data->{row});
    } else {
        print "undef\n";
        return undef;
    }
}

sub getBelt {
    my $self = shift;
    my $gameType = shift;

    if (!$gameType) { $gameType = 'standard'; }

    if (! defined( $self->{'rating_' . $gameType})) { 
        return undef;
    }
    my $rating = $self->{'rating_' . $gameType};

    # provisional belt
    if ($self->getGamesPlayed($gameType) < 10) {
        return 'green';
    }

    return 'yellow' if ($rating < 1400); 
    return 'orange' if ($rating < 1600); 
    return 'red'    if ($rating < 1800); 
    return 'brown'  if ($rating < 2000); 
    return 'black'  if ($rating < 2200); 
    return 'doubleblack';
}

sub getRating {
    my $self = shift;
    my $gameType = shift;

    return $self->{'rating_' . $gameType};
}

sub getJsonMsg {
    my $self = shift;

    my $msg = {
        'player_id' => $self->{'player_id'},
        'screenname' => $self->{'screenname'},
        'rating_standard'  => $self->{'rating_standard'},
        'rating_lightning' => $self->{'rating_lightning'},
    };

    return encode_json $msg;
}

sub getBestVictory {
    my $self = shift;
    my $gameType = shift;

    my @row = $self->{dbh}->selectrow_array(
        "SELECT MAX(opponent_rating_before)
            FROM game_log
            WHERE player_id = ?
            AND game_type = ?
            AND result = 'win'
            AND rated = 1",
        {},
        $self->{player_id},
        $gameType
    );

    $self->{'best_victory' . $gameType} = $row[0];

    return $self->{'best_victory' . $gameType};
}

sub getWorstDefeat {
    my $self = shift;
    my $gameType = shift;

    my @row = $self->{dbh}->selectrow_array(
        "SELECT MIN(opponent_rating_before)
            FROM game_log
            WHERE player_id = ?
            AND game_type = ?
            AND result = 'loss'
            AND rated = 1",
        {},
        $self->{player_id},
        $gameType
    );

    $self->{'worst_defeat' . $gameType} = $row[0];

    return $self->{'worst_defeat' . $gameType};
}

sub getHighestRating {
    my $self = shift;
    my $gameType = shift;

    my @row = $self->{dbh}->selectrow_array(
        "SELECT MAX(rating_after)
            FROM game_log
            WHERE player_id = ?
            AND game_type = ?
            AND rated = 1",
        {},
        $self->{player_id},
        $gameType
    );

    $self->{'highest_rating' . $gameType} = $row[0];

    return $self->{'highest_rating' . $gameType};
}

sub getLowestRating {
    my $self = shift;
    my $gameType = shift;

    my @row = $self->{dbh}->selectrow_array(
        "SELECT MIN(rating_after)
            FROM game_log
            WHERE player_id = ?
            AND game_type = ?
            AND rated = 1",
        {},
        $self->{player_id},
        $gameType
    );

    $self->{'lowwest_rating' . $gameType} = $row[0];

    return $self->{'lowwest_rating' . $gameType};
}

sub getGamesDrawn {
    my $self = shift;
    my $gameType = shift;

    my @row = $self->{dbh}->selectrow_array(
        "SELECT COUNT(*) as games_won
            FROM game_log
            WHERE player_id = ?
            AND game_type = ?
            AND result = 'draw'
            AND rated = 1",
        {},
        $self->{player_id},
        $gameType
    );

    $self->{'games_won_' . $gameType} = $row[0];

    return $self->{'games_won_' . $gameType};
}

sub getGamesLost {
    my $self = shift;
    my $gameType = shift;

    my @row = $self->{dbh}->selectrow_array(
        "SELECT COUNT(*) as games_won
            FROM game_log
            WHERE player_id = ?
            AND game_type = ?
            AND result = 'loss'
            AND rated = 1",
        {},
        $self->{player_id},
        $gameType
    );

    $self->{'games_won_' . $gameType} = $row[0];

    return $self->{'games_won_' . $gameType};
}

sub getGamesWon {
    my $self = shift;
    my $gameType = shift;

    my @row = $self->{dbh}->selectrow_array(
        "SELECT COUNT(*) as games_won
            FROM game_log
            WHERE player_id = ?
            AND game_type = ?
            AND result = 'win'
            AND rated = 1",
        {},
        $self->{player_id},
        $gameType
    );

    $self->{'games_won_' . $gameType} = $row[0];

    return $self->{'games_won_' . $gameType};
}

sub getGamesPlayed {
    my $self = shift;
    my $gameType = shift;

    my @row = $self->{dbh}->selectrow_array(
        "SELECT COUNT(*) as games_played 
            FROM game_log
            WHERE player_id = ?
            AND game_type = ?
            AND rated = 1",
        {},
        $self->{player_id},
        $gameType
    );

    $self->{'games_played_' . $gameType} = $row[0];

    return $self->{'games_played_' . $gameType};
}

sub _loadByRow {
    my $self = shift;
    my $row = shift;
    
    if (! defined($row)) {
        return undef;
    }

    my %excludedFields = (
        'password' => 1
    );
    foreach my $key (keys %$row) {
        if (! $excludedFields{$key} ) {
            $self->{$key} = $row->{$key};
        }
    }

    return $self;
}

sub _loadById {
    my $self = shift;
    my $userId = shift;

    my $profileRows = $self->{dbh}->selectall_arrayref('
        SELECT *
        FROM players
        WHERE player_id = ?',
        { 'Slice' => {} },
        $userId
    );

    my $row = shift @$profileRows;
    return $self->_loadByRow($row);
}

### get profile stats
sub _loadByScreenname {
    my $self = shift;
    my $screenname = shift;

    print "_load $screenname\n";

    my $profileRows = $self->{dbh}->selectall_arrayref('
        SELECT *
        FROM players
        WHERE screenname = ?',
        { 'Slice' => {} },
        $screenname
    );

    my $row = shift @$profileRows;
    return $self->_loadByRow($row);
}

1;
