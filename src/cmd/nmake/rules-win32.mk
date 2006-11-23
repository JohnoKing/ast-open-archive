/*
 * win32 specific makerules
 */

.INSTALL.libast.a = .
.INSTALL.libast-g.a = .

PROTOINSTALL = | $(SED) '/^#[ 	]*include <ast_/s,<,<ast/,'

RC = rc
RCFLAGS = -x

SYSDIR = $(INSTALLROOT:D:B=sys:T=F:??$(INSTALLROOT)/sys?O)

.SOURCE.%.SCAN.rc : $$(*.SOURCE.rc) $$(*.SOURCE) $$(*.SOURCE.h:D:X=include/mfc mfc/include include)

.SCAN.rc : .SCAN
	$(@.SCAN.c)

.ATTRIBUTE.%.rc : .SCAN.rc

%.res : %.rc
	$(RC) $(RCFLAGS) -r -fo$(<:P=N) $(.INCLUDE. rc:P=N:/^/-I/) $(>:P=N)

%.def : %.sym
	{
	echo LIBRARY $(.DLL.NAME. $(%:B) $($(%:B).VERSION):B:F=%(upper)s)
	echo
	echo SECTIONS
	echo .data READ WRITE
	echo
	echo EXPORTS
	cat $(>)
	} > $(<)

/*
 * SDK package support -- how about a stable registry?
 */

.PACKAGE.SDK. : .FUNCTION
	local ( DIR PKG ) $(%)
	local D V
	D := $(DIR:F=%(lower)s)
	if "$(PACKAGE_$(PKG))"
		R := $(PACKAGE_$(PKG))/$(D)
	else
		if ! .PACKAGE.SDK.DIRS.
			.PACKAGE.SDK.DIRS. := $(sh $(CC) -V 2>/dev/null) /platformsdk
		end
		R := $(.PACKAGE.SDK.DIRS.:X=$(D)/$(PKG) $(PKG)/$(D) $(D)/win64/$(PKG) $(D):T=F)
	end
	PACKAGE_$(PKG)_$(DIR) := $(R)
	return $(R)

PACKAGE_atl_INCLUDE = $(.PACKAGE.SDK. INCLUDE atl)
PACKAGE_atl_LIB = $(.PACKAGE.SDK. LIB atl)

PACKAGE_crt_INCLUDE = $(.PACKAGE.SDK. INCLUDE crt)
PACKAGE_crt_LIB = $(.PACKAGE.SDK. LIB crt)

PACKAGE_mfc_INCLUDE = $(.PACKAGE.SDK. INCLUDE mfc)
PACKAGE_mfc_LIB = $(.PACKAGE.SDK. LIB mfc)
