# $FreeBSD$

# Use the following command to build local debug version of dynamic
# linker:
# make DEBUG_FLAGS=-g WITHOUT_TESTS=yes all
# Not adapted for CHERI

WANT_CHERI=none

RTLD_ELF_DIR:=	${.PARSEDIR}

.ifdef BUILD_RTLD_DEBUG
.ifdef RTLD_DEBUG_VERBOSE
DEBUG+=-DDEBUG -DDEBUG_VERBOSE=${RTLD_DEBUG_VERBOSE}
.else
DEBUG+=-DDEBUG
.endif
.endif

.if empty(DEBUG) && !defined(RTLD_INCLUDE_DEBUG_MESSAGES)
# Avoid the overhead of all the debug checks now that we have a rtld-debug that
# can be used instead.
CFLAGS+=-DNO_LD_DEBUG
.endif

.include <src.opts.mk>
PACKAGE=	clibs
MK_PIE=		no # Always position independent using local rules
MK_SSP=		no

CONFS=		libmap.conf
PROG?=		ld-elf.so.1
.if (${PROG:M*ld-elf32*} != "")
TAGS+=		lib32
.endif
.if (${PROG:M*ld-elf64*} != "")
TAGS+=		lib64
.endif
.PATH: ${RTLD_ELF_DIR}
SRCS= \
	rtld_start.S \
	reloc.c \
	rtld.c \
	rtld_lock.c \
	rtld_malloc.c \
	rtld_printf.c \
	map_object.c \
	xmalloc.c \
	debug.c \
	libmap.c
MAN?=		rtld.1
CSTD?=		gnu99
CFLAGS+=	-Wall -DFREEBSD_ELF -DIN_RTLD -ffreestanding
CFLAGS+=	-I${SRCTOP}/lib/csu/common
.if ${MACHINE_CPU:Mcheri}
CFLAGS+=	-DCHERI
.endif
.if exists(${RTLD_ELF_DIR}/${MACHINE_ARCH})
RTLD_ARCH=	${MACHINE_ARCH}
.else
RTLD_ARCH=	${MACHINE_CPUARCH}
.endif
CFLAGS+=	-I${RTLD_ELF_DIR}/${RTLD_ARCH} -I${RTLD_ELF_DIR}

NO_WCAST_ALIGN=	yes
WARNS?=		6
INSTALLFLAGS=	-C -b
PRECIOUSPROG=
BINDIR=		/libexec
SYMLINKS=	../..${BINDIR}/${PROG} ${LIBEXECDIR}/${PROG}
MLINKS?=	rtld.1 ld-elf.so.1.1 \
		rtld.1 ld.so.1

CFLAGS+=	-fpic -DPIC $(DEBUG)
# Since we pass -shared, we shouldn't also pass -pie
MK_PIE:=	no

LDFLAGS+=	-shared -Wl,-Bsymbolic -Wl,-z,defs -nostdlib -e ${RTLD_ENTRY}
# Pull in the dependencies that we use from libc
.include "rtld-libc/Makefile.inc"
.if ${MK_TOOLCHAIN} == "no"
LDFLAGS+=	-L${LIBCDIR}
.endif

VERSION_DEF=	${LIBCSRCDIR}/Versions.def
SYMBOL_MAPS=	${RTLD_ELF_DIR}/Symbol.map
VERSION_MAP=	Version.map
LDFLAGS+=	-Wl,--version-script=${VERSION_MAP}

.if exists(${RTLD_ELF_DIR}/${RTLD_ARCH}/Symbol.map)
SYMBOL_MAPS+=	${RTLD_ELF_DIR}/${RTLD_ARCH}/Symbol.map
.endif

.sinclude "${RTLD_ELF_DIR}/${RTLD_ARCH}/Makefile.inc"
RTLD_ENTRY?=	.rtld_start

# Since moving rtld-elf to /libexec, we need to create a symlink.
# Fixup the existing binary that's there so we can symlink over it.
beforeinstall:
.if exists(${DESTDIR}/usr/libexec/${PROG}) && ${MK_STAGING} == "no"
	-chflags -h noschg ${DESTDIR}/usr/libexec/${PROG}
.endif

.PATH: ${RTLD_ELF_DIR}/${RTLD_ARCH}

.if ${.CURDIR} == ${RTLD_ELF_DIR} || defined(RTLD_CHERI_ELF_DIR)
HAS_TESTS=
SUBDIR.${MK_TESTS}+= tests
.endif

.include <bsd.prog.mk>
${PROG_FULL}:	${VERSION_MAP}
.include <bsd.symver.mk>

.if ${COMPILER_TYPE} == "gcc"
# GCC warns about redeclarations even though they have __exported
# and are therefore not identical to the ones from the system headers.
CFLAGS+=	-Wno-redundant-decls
.if ${COMPILER_VERSION} < 40300
# Silence -Wshadow false positives in ancient GCC
CFLAGS+=	-Wno-shadow
.endif
.endif
