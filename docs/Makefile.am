DOCS = \
	afCloseFile.3.txt \
	afGetFrameSize.3.txt \
	afGetFrameCount.3.txt \
	afNewFileSetup.3.txt \
	afOpenFile.3.txt \
	afQuery.3.txt \
	afReadFrames.3.txt \
	afReadMisc.3.txt \
	afSeekFrame.3.txt \
	afSetErrorHandler.3.txt \
	afSetVirtualSampleFormat.3.txt \
	afWriteFrames.3.txt \
	sfconvert.1.txt \
	sfinfo.1.txt

EXTRA_DIST = \
	$(DOCS) \
	asciidoc.conf

DOCS_MAN = $(DOCS:%.1.txt=%.1) $(DOCS:%.3.txt=%.3)
DOCS_HTML = $(DOCS:%.txt=%.html)

DOCS_MAN_EXTRA = \
	afGetDataOffset.3 \
	afGetTrackBytes.3 \
	afQueryLong.3 \
	afQueryDouble.3 \
	afQueryPointer.3 \
	afSeekMisc.3 \
	afSetVirtualByteOrder.3 \
	afSetVirtualChannels.3 \
	afSetVirtualPCMMapping.3 \
	afTellFrame.3 \
	afWriteMisc.3

dist_man_MANS = $(DOCS_MAN) $(DOCS_MAN_EXTRA)

ASCIIDOCFLAGS = -arevnumber=$(VERSION) -amansource="Audio File Library"
A2XFLAGS = $(ASCIIDOCFLAGS) -d manpage -f manpage

%.html: %.txt
	$(ASCIIDOC) $(ASCIIDOCFLAGS) -b html4 -d manpage -f asciidoc.conf $<

%.1: %.1.txt
	$(A2X) $(A2XFLAGS) --asciidoc-opts="-f asciidoc.conf" $<

%.3: %.3.txt
	$(A2X) $(A2XFLAGS) --asciidoc-opts="-f asciidoc.conf" $<

html: $(DOCS_HTML)

CLEANFILES = *.1 *.3 *.html
