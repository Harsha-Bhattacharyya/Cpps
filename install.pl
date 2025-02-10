#!/usr/bin/perl
use strict;
use warnings;
use File::Path qw(make_path remove_tree);
use File::Copy;

# Configuration
my $eigen_version = "3.4.0";
my $eigen_url = "https://gitlab.com/libeigen/eigen/-/archive/$eigen_version/eigen-$eigen_version.tar.gz";
my $project_dir = `pwd`; chomp $project_dir;
my $install_dir;

# Determine environment
my $uname_o = `uname -o`; chomp $uname_o;
my $is_termux = $uname_o =~ /Android/ ? 1 : 0;

# Set install directory
if($is_termux) {
    $install_dir = $ENV{'PREFIX'}."/bin";
} else {
    $install_dir = "/bin";
    # Check for sudo access
    if($> != 0 && !system("sudo -v >/dev/null 2>&1")) {
        die "Error: sudo access required for Linux installation\n";
    }
}

# Download and extract Eigen
print "Downloading Eigen $eigen_version...\n";
system("wget", "-O", "eigen.tar.gz", $eigen_url) == 0 or die "Download failed: $!";

print "Extracting Eigen...\n";
system("tar", "-xzf", "eigen.tar.gz") == 0 or die "Extraction failed: $!";
unlink "eigen.tar.gz";

# Move Eigen headers
my $eigen_dir = "$project_dir/eigen-$eigen_version";
my $include_dir = "$project_dir/include";
make_path($include_dir) unless -d $include_dir;

# Build project
print "Building project...\n";
system("make", "clean") == 0 or die "Make clean failed: $!";
system("make") == 0 or die "Build failed: $!";

# Install executable
print "Installing to $install_dir...\n";
if($is_termux) {
    move("cpps", "$install_dir/cpps") 
        or die "Install failed: $!";
} else {
    system("sudo", "mv", "cpps", "$install_dir/") == 0 
        or die "Install failed: $!";
}

print "Installation complete!\n";
print "Executable installed to: $install_dir/cpps\n";
