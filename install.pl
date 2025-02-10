#!/usr/bin/perl
use strict;
use warnings;
use File::Path qw(make_path);
use Cwd qw(abs_path);

# Use Figlet if available
my $figlet_output;
if (system("command -v figlet >/dev/null 2>&1") == 0) {  # Check if figlet exists
    $figlet_output = `figlet -f slant CppScript`; # Use 'slant' font, you can change it
} else {
    $figlet_output = "CppScript"; # Fallback if figlet is not installed
}

print "\n";  # Add a newline for spacing
print $figlet_output;
print "\n";  # Add a newline for spacing


# ... (Rest of the installation script from the previous response) ...

# Configuration (adjust these if needed)
my $executable_name = "cpps";
my $build_dir = "."; # Directory where your Makefile is located
my $bin_dir;

# Determine bin directory (Termux or normal)
if ($ENV{TERMUX_PREFIX}) {
    $bin_dir = "$ENV{TERMUX_PREFIX}/bin";
} else {
    $bin_dir = "/usr/local/bin"; # Or another appropriate location
}

# Create bin directory if it doesn't exist
unless (-d $bin_dir) {
    print "Creating bin directory: $bin_dir\n";
    make_path($bin_dir) or die "Failed to create bin directory: $!\n";
}

# Change to the build directory
chdir $build_dir or die "Failed to change to build directory '$build_dir': $!\n";
my $build_dir_abs = abs_path($build_dir); # Get the absolute path

print "Building in directory: $build_dir_abs\n";

# Run make
print "Running make...\n";
my $make_output = `make 2>&1`; # Capture both stdout and stderr
my $make_exit_code = $?;

if ($make_exit_code != 0) {
    print "Make failed:\n$make_output\n";
    exit $make_exit_code;
} else {
  print "Make successful.\n";
}


# Check if the executable exists
my $executable_path = "$build_dir/$executable_name";  # Path relative to build dir
unless (-e $executable_path) {
    die "Executable '$executable_name' not found in build directory '$build_dir'. Make sure it's built.\n";
}

# Install the executable (copy to bin directory)
my $target_path = "$bin_dir/$executable_name";

print "Installing '$executable_name' to: $target_path\n";

# Use 'install' command if available for better permissions handling
if (system("command -v install >/dev/null 2>&1") == 0) { # Check if install exists
    my $install_output = `install -m 0755 $executable_path $target_path 2>&1`;
    my $install_exit_code = $?;

    if ($install_exit_code != 0) {
        print "Install failed: $install_output\nTrying cp...\n"; # Fallback to cp
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


