./miscwrite /tmp/motherfuck || exit
./miscread /tmp/motherfuck > /tmp/misc.fuck || exit

diff /tmp/misc.fuck - <<END || exit
Miscellaneous 201, 19 bytes:
1998 Michael Pruett
Miscellaneous 203, 43 bytes:
Michael Pruett's home-brew methamphetamines
END
