./miscwrite /tmp/misc.test || exit
./miscread /tmp/misc.test > /tmp/misc.out || exit

diff /tmp/misc.out - <<END || exit
Miscellaneous 201, 19 bytes:
1998 Michael Pruett
Miscellaneous 203, 43 bytes:
Michael Pruett's home-brew methamphetamines
END

rm -f /tmp/misc.test /tmp/misc.out
