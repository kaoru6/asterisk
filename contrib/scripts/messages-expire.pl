#!/usr/bin/perl
#
# Script to expire voicemail after a specified number of days
# by Steve Creel <screel@turbs.com>
# 

# Directory housing the voicemail spool for asterisk
$dir = "/var/spool/asterisk/voicemail";

# Context for which the script should be running
$context = "default";

# Age (Delete files older than $age days old)
$age = 31;

# Delete all files older than $age (but named msg????.??? to be sure
# we don't delete greetings or the user's name)

system('find '.$dir.'/'.$context.' -name msg????.??? -mtime +'.$age.' -exec rm {} \; -exec echo Deleted {} \;');

# For testing - what number to we start when we renumber?
$start = "0";

# Rename to msg and a 4 digit number, 0 padded.
$fnbase = sprintf "msg%04d", $start;

# Make $dir include the context too
$dir.="/".$context;

( -d $dir ) || die "Can't read list of mailboxes ($dir): $!\n"; 
@mailboxes = `ls -A1 $dir`;
chomp(@mailboxes);

$save_fnbase = $fnbase;

foreach $mailbox (@mailboxes) {

  ( -d $dir."/".$mailbox) || die "Can't read list of folders (".$dir."/".$mailbox."): $!\n";
  @folders = `ls -A1 $dir/$mailbox`;
  chomp(@folders);

  foreach $folder (@folders) {
   if (-d $dir."/".$mailbox."/".$folder) {
    ( -d $dir."/".$mailbox."/".$folder) || die "Can't read list of messages (".$dir."/".$mailbox."/".$folder.") $!\n";
    @files = `ls -A1 $dir/$mailbox/$folder/`;

    # Sort so everything is in proper order.
    @files = sort @files;
    chomp(@files);

    # If there is still (after deleting old files earlier in the
    # script) a msg0000.txt, we don't need to shuffle anything
    # in this folder.
    if (-f $dir."/".$mailbox."/".$folder."/msg0000.txt") { next; }

    foreach $ext (("WAV", "wav", "gsm", "txt")) {
      # Reset the fnbase for each file type
      $fnbase = $save_fnbase;

      foreach $file (@files) {
	if ( $file =~ /$ext/ ) {
		chdir($dir."/".$mailbox."/".$folder."/") || die "Can't change folder: $!";
		print "Renaming: ".$dir."/".$mailbox."/".$folder."/".$file." to ".$fnbase.".".$ext."\n";
		rename($file, $fnbase.".".$ext) || die "Cannot rename: $!";
		$fnbase++;
	}
      }
    }
   }
  }
}

__END__