#!/usr/bin/perl
use strict;
my $src=$ARGV[0];
my $trans=$ARGV[1];
my @names=`ls $src/*.html`;
my $name;
my $old_name;
foreach $name (@names) {
	chomp($name);
	$old_name = $name;
	if($trans eq "trans") {
		$name =~ s/::/--/g;
	} else {
		$name =~ s/--/::/g;
	}

	print "$old_name=>$name\n";
	system ("cp $old_name $name");
} 
