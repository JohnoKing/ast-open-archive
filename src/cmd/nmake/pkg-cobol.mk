/*
 * generic cobol package setup
 * specific setup by pkg-cobol-$(COBOL:B:S=.mk)
 */

.PACKAGE.cobol.dontcare := 1

COBOL = cobc
COBOLDIALECT =
COBOLFLAGS = $(COBOLDIALECT)
COBOLLIBRARIES =

freeze COBOL

.SUFFIX.cob = .cob .COB .cbl .CBL
.SUFFIX.HEADER.cob = .cpy .CPY

.SCAN.cob : .SCAN
	I|\T COPY % |M$$(.INCLUDE.SUFFIX. cob)|
	I|\T \D COPY % |M$$(.INCLUDE.SUFFIX. cob)|

$(.SUFFIX.cob:/^/.ATTRIBUTE.%/) : .SCAN.cob

.SOURCE.%.SCAN.cob : . $$(*.SOURCE$$(.SUFFIX.HEADER.cob:O=1)) $$(*.SOURCE.cob) $$(*.SOURCE)

.PROBE.INIT : .PKG.COBOL.INIT

.PKG.COBOL.INIT : .MAKE .VIRTUAL .FORCE .AFTER
	local F
	F = pkg-cobol-$(COBOL:B:S=.mk)
	if ( F = "$(F:T=F)" )
		include + $(F)
	end
	COBOLFLAGS &= $(COBOLFLAGS:VA:V) $$(.INCLUDE. cob -I)
