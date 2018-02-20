# http://codeforces.com/contest/915/submission/34499296

use strict;

sub fill {
    my ($s, $h) = @_;
    for (my $dig = 0; $dig < 10; ++$dig) {
        if (@$h[$dig]) {
            for (my $i = 0; $i < @$h[$dig]; ++$i) {
                push @$s, $dig;
            }
        }
    }
}

my @a = split (/ */, <>);
pop @a;
my @b = split (/ */, <>);
pop @b;
if (scalar @a < scalar @b) {
    @a = sort @a;
    @a = reverse @a;
    print @a, "\n";
} else {
    my @h = (0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    foreach my $dig (@a) {
        @h[$dig]++;
    }
    my @ans = ();
    for (my $i = 0; $i < scalar @b; ++$i) {
        my $ready = undef;
        for (my $dig = 9; $dig >= 0 && !defined $ready; --$dig) {
            if (@h[$dig]) {
                @h[$dig]--;
                my @cur = @ans;
                push @cur, $dig;
                fill (\@cur, \@h);
                unless (join("", @cur) gt join("", @b)) {
                    push @ans, $dig;
                    $ready = 1;
                } else {
                    @h[$dig]++;
                }
            }
        }
    }
    print @ans, "\n";
}
