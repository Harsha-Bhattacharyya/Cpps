#!/usr/bin/perl
use strict;
use warnings;
use File::Path qw(make_path);
use Cwd qw(abs_path);
use LWP::Simple qw(get);

# Figlet Banner
my $figlet_output;
if (system("command -v figlet >/dev/null 2>&1") == 0) {
    $figlet_output = `figlet -f slant CppScript`;
} else {
    $figlet_output = "CppScript";
}

print "\n";
print $figlet_output;
print "\n";

# Eigen Configuration
my $eigen_version = "3.4.0";
my $eigen_zip = "eigen-$eigen_version.zip";
my $eigen_url = "https://gitlab.com/libeigen/eigen/-/archive/refs/tags/3.4.0.zip";
my $eigen_dir = "eigen-$eigen_version";

# Download Eigen
unless (-e $eigen_zip) {
    print "Downloading Eigen $eigen_version...\n";
    my $download_status = get $eigen_url, $eigen_zip;
    if (!defined $download_status) {
        die "Failed to download Eigen: $!\n";
    }
    print "Download complete.\n";
} else {
    print "Eigen zip file already exists. Skipping Download.\n";
}

# Unzip Eigen
unless (-d $eigen_dir) {
    print "Unzipping Eigen...\n";
    my $unzip_output = `unzip $eigen_zip 2>&1`;
    my $unzip_exit_code = $?;

    if ($unzip_exit_code != 0) {
        die "Unzip failed: $unzip_output\n";
    }

    print "Unzip complete.\n";
} else {
    print "Eigen Directory already exists. Skipping Unzip.\n";
}


# Installation Configuration
my $executable_name = "cpps";
my $build_dir = ".";
my $bin_dir;

# Determine bin directory (Termux or normal)
if ($ENV{TERMUX_PREFIX}) {
    $bin_dir = "$ENV{TERMUX_PREFIX}/bin";
} else {
    $bin_dir = "/usr/local/bin";
}

# Create bin directory if it doesn't exist
unless (-d $bin_dir) {
    print "Creating bin directory: $bin_dir\n";
    make_path($bin_dir) or die "Failed to create bin directory: $!\n";
}

# Change to the build directory
chdir $build_dir or die "Failed to change to build directory '$build_dir': $!\n";
my $build_dir_abs = abs_path($build_dir);

print "Building in directory: $build_dir_abs\n";

# Run make
print "Running make...\n";
my $make_output = `make 2>&1`;
my $make_exit_code = $?;

if ($make_exit_code != 0) {
    print "Make failed:\n$make_output\n";
    exit $make_exit_code;
} else {
    print "Make successful.\n";
}

# Check if the executable exists
my $executable_path = "$build_dir/$executable_name";
unless (-e $executable_path) {
    die "Executable '$executable_name' not found in build directory '$build_dir'. Make sure it's built.\n";
}

# Install the executable
my $target_path = "$bin_dir/$executable_name";

print "Installing '$executable_name' to: $target_path\n";

# Use 'install' command if available
if (system("command -v install >/dev/null 2>&1") == 0) {
    my $install_output = `install -m 0755 $executable_path $target_path 2>&1`;
    my $install_exit_code = $?;

    if ($install_exit_code != 0) {
        print "Install failed: $install_output\nTrying cp...\n";
        my $cp_output = `cp $executable_path $target_path 2>&1`;
        my $cp_exit_code = $?;
        if ($cp_exit_code != 0) {
            die "Copy failed: $cp_output\n";
        } else {
            print "Copy successful.\n";
        }
    } else {
        print "Install successful.\n";
    }

} else { # Install command not found, use cp
    my $cp_output = `cp $executable_path $target_path 2>&1`;
    my $cp_exit_code = $?;
    if ($cp_exit_code != 0) {
        die "Copy failed: $cp_output\n";
    } else {
        print "Copy successful.\n";
    }
}

print "Installation complete.\n";

exit 0;

