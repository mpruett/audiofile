#
# Test miscellaneous data reading and writing for AIFF, AIFF-C, and WAVE
# file formats.
#

for i in aiff aifc wave
do
./miscwrite $i /tmp/misc.test || (echo "failed: miscwrite $i"; exit)
./miscread /tmp/misc.test > /tmp/misc.out || (echo "failed: miscread $i"; exit)

diff /tmp/misc.out - <<END || exit
Miscellaneous 201, 19 bytes:
1998 Michael Pruett
Miscellaneous 203, 43 bytes:
Michael Pruett's home-brew methamphetamines
END

rm -f /tmp/misc.test /tmp/misc.out
echo "passed: miscellaneous $i"
done
