/*-
 * Copyright (c) 2014, 2016 Robert N. M. Watson
 * All rights reserved.
 *
 * This software was developed by SRI International and the University of
 * Cambridge Computer Laboratory under DARPA/AFRL contract (FA8750-10-C-0237)
 * ("CTSRD"), as part of the DARPA CRASH research programme.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * A few non-faulting CHERI-related virtual-memory tests.
 */

#include <sys/cdefs.h>

#if !__has_feature(capabilities)
#error "This code requires a CHERI-aware compiler"
#endif

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/socket.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/ucontext.h>
#include <sys/wait.h>

#include <sys/caprevoke.h>
#include <sys/event.h>

#include <machine/cpuregs.h>
#include <machine/frame.h>
#include <machine/trap.h>

#include <cheri/cheri.h>
#include <cheri/cheric.h>
#include <cheri/libcheri_fd.h>
#include <cheri/libcheri_sandbox.h>

#include <cheritest-helper.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#include "cheritest.h"

/*
 * Tests to check that tags are ... or aren't ... preserved for various page
 * types.  'Anonymous' pages provided by the VM subsystem should always
 * preserve tags.  Pages from the filesystem should not -- unless they are
 * mapped MAP_PRIVATE, in which case they should, since they are effectively
 * anonymous pages.  Or so I claim.
 *
 * Most test cases only differ in the mmap flags and the file descriptor, this
 * function does all the shared checks
 */
static void
mmap_and_check_tag_stored(int fd, int protflags, int mapflags)
{
	void * __capability volatile *cp;
	void * __capability cp_value;
	int v;

	cp = CHERITEST_CHECK_SYSCALL(mmap(NULL, getpagesize(), protflags,
	     mapflags, fd, 0));
	cp_value = cheri_ptr(&v, sizeof(v));
	*cp = cp_value;
	cp_value = *cp;
	CHERITEST_VERIFY2(cheri_gettag(cp_value) != 0, "tag lost");
	CHERITEST_CHECK_SYSCALL(munmap(__DEVOLATILE(void *, cp), getpagesize()));
	if (fd != -1)
		CHERITEST_CHECK_SYSCALL(close(fd));
}

void
cheritest_vm_tag_mmap_anon(const struct cheri_test *ctp __unused)
{
	mmap_and_check_tag_stored(-1, PROT_READ | PROT_WRITE, MAP_ANON);
	cheritest_success();
}

void
cheritest_vm_tag_shm_open_anon_shared(const struct cheri_test *ctp __unused)
{
	int fd = CHERITEST_CHECK_SYSCALL(shm_open(SHM_ANON, O_RDWR, 0600));
	CHERITEST_CHECK_SYSCALL(ftruncate(fd, getpagesize()));
	mmap_and_check_tag_stored(fd, PROT_READ | PROT_WRITE, MAP_SHARED);
	cheritest_success();
}

void
cheritest_vm_tag_shm_open_anon_private(const struct cheri_test *ctp __unused)
{
	int fd = CHERITEST_CHECK_SYSCALL(shm_open(SHM_ANON, O_RDWR, 0600));
	CHERITEST_CHECK_SYSCALL(ftruncate(fd, getpagesize()));
	mmap_and_check_tag_stored(fd, PROT_READ | PROT_WRITE, MAP_PRIVATE);
	cheritest_success();
}

/*
 * Test aliasing of SHM_ANON objects
 */
void
cheritest_vm_tag_shm_open_anon_shared2x(const struct cheri_test *ctp __unused)
{
	void * __capability volatile * map2;
	void * __capability c2;
	int fd = CHERITEST_CHECK_SYSCALL(shm_open(SHM_ANON, O_RDWR, 0600));
	CHERITEST_CHECK_SYSCALL(ftruncate(fd, getpagesize()));

	map2 = CHERITEST_CHECK_SYSCALL(mmap(NULL, getpagesize(),
		PROT_READ, MAP_SHARED, fd, 0));

	/* Verify that no capability present */
	c2 = *map2;
	CHERITEST_VERIFY2(cheri_gettag(c2) == 0, "tag exists on first read");
	CHERITEST_VERIFY2(c2 == NULL, "Initial read NULL");

	mmap_and_check_tag_stored(fd, PROT_READ | PROT_WRITE, MAP_SHARED);

	/* And now verify that it is, thanks to the aliased maps */
	c2 = *map2;
	CHERITEST_VERIFY2(cheri_gettag(c2) != 0, "tag lost on second read");
	CHERITEST_VERIFY2(c2 != NULL, "Second read not NULL");

	cheritest_success();
}

void
cheritest_vm_shm_open_anon_unix_surprise(const struct cheri_test *ctp __unused)
{
	int sv[2];
	int pid;

	CHERITEST_CHECK_SYSCALL(socketpair(AF_UNIX, SOCK_DGRAM, 0, sv) != 0);

	pid = fork();
	if (pid == -1)
		cheritest_failure_errx("Fork failed; errno=%d", errno);

	if (pid == 0) {
		void * __capability * map;
		void * __capability crx;
		int fd, tag;
		struct msghdr msg = { 0 };
		struct cmsghdr * cmsg;
		char cmsgbuf[CMSG_SPACE(sizeof(fd))] = { 0 } ;
		char iovbuf[16];
		struct iovec iov = { .iov_base = iovbuf, .iov_len = sizeof(iovbuf) };

		close(sv[1]);

		/* Read from socket */
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;
		msg.msg_control = cmsgbuf;
		msg.msg_controllen = sizeof(cmsgbuf);
		CHERITEST_CHECK_SYSCALL(recvmsg(sv[0], &msg, 0));

		/* Deconstruct cmsg */
		/* XXX Doesn't compile: cmsg = CMSG_FIRSTHDR(&msg); */
		cmsg = msg.msg_control;
		memmove(&fd, CMSG_DATA(cmsg), sizeof(fd));

		CHERITEST_VERIFY2(fd >= 0, "fd read OK");

		map = CHERITEST_CHECK_SYSCALL(mmap(NULL, getpagesize(),
						PROT_READ, MAP_SHARED, fd,
						0));
		crx = *map;

		CHERI_FPRINT_PTR(stderr, crx);

		tag = cheri_gettag(crx);
		CHERITEST_VERIFY2(tag == 0, "tag read");

		CHERITEST_CHECK_SYSCALL(munmap(map, getpagesize()));
		close(sv[0]);
		close(fd);

		exit(tag);
	} else {
		void * __capability * map;
		void * __capability ctx;
		int fd, res;
		struct msghdr msg = { 0 };
		struct cmsghdr * cmsg;
		char cmsgbuf[CMSG_SPACE(sizeof(fd))] = { 0 };
		char iovbuf[16] = { 0 };
		struct iovec iov = { .iov_base = iovbuf, .iov_len = sizeof(iovbuf) };

		close(sv[0]);

		fd = CHERITEST_CHECK_SYSCALL(shm_open(SHM_ANON, O_RDWR, 0600));
		CHERITEST_CHECK_SYSCALL(ftruncate(fd, getpagesize()));

		map = CHERITEST_CHECK_SYSCALL(mmap(NULL, getpagesize(),
						PROT_READ | PROT_WRITE,
						MAP_SHARED, fd, 0));

		/* Just some pointer */
		*map = &fd;
		ctx = *map;
		CHERITEST_VERIFY2(cheri_gettag(ctx) != 0, "tag written");

		CHERI_FPRINT_PTR(stderr, ctx);

		CHERITEST_CHECK_SYSCALL(munmap(map, getpagesize()));

		/* Construct control message */
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;
		msg.msg_control = cmsgbuf;
		msg.msg_controllen = sizeof(cmsgbuf);
		/* XXX cmsg = CMSG_FIRSTHDR(&msg); */
		cmsg = msg.msg_control;
		cmsg->cmsg_level = SOL_SOCKET;
		cmsg->cmsg_type = SCM_RIGHTS;
		cmsg->cmsg_len = CMSG_LEN(sizeof fd);
		memmove(CMSG_DATA(cmsg), &fd, sizeof(fd));
		msg.msg_controllen = cmsg->cmsg_len;

		/* Send! */
		CHERITEST_CHECK_SYSCALL(sendmsg(sv[1], &msg, 0));

		close(sv[1]);
		close(fd);

		waitpid(pid, &res, 0);
		if (res == 0) {
			cheritest_success();
		} else {
			cheritest_failure_errx("tag transfer succeeded");
		}
	}
}

#ifdef CHERIABI_TESTS

/*
 * We can fork processes with shared file descriptor tables, including
 * shared access to a kqueue, which can hoard capabilities for us, allowing
 * them to flow between address spaces.  It is difficult to know what to do
 * about this case, but it seems important to acknowledge.
 */
void
cheritest_vm_cap_share_fd_kqueue(const struct cheri_test *ctp __unused)
{
	int kq, pid;

	kq = CHERITEST_CHECK_SYSCALL(kqueue());
	pid = rfork(RFPROC);
	if (pid == -1)
		cheritest_failure_errx("Fork failed; errno=%d", errno);

	if (pid == 0) {
		struct kevent oke;
		/*
		 * Wait for receipt of the user event, and witness the
		 * capability received from the parent.
		 */
		oke.udata = NULL;
		CHERITEST_CHECK_SYSCALL(kevent(kq, NULL, 0, &oke, 1, NULL));
		CHERITEST_VERIFY2(oke.ident == 0x2BAD, "Bad identifier from kqueue");
		CHERITEST_VERIFY2(oke.filter == EVFILT_USER, "Bad filter from kqueue");

		CHERI_FPRINT_PTR(stderr, oke.udata);

		exit(cheri_gettag(oke.udata));
	} else {
		int res;
		struct kevent ike;
		void * __capability passme;

		/*
		 * Generate a capability to a new mapping to pass to the
		 * child, who will not have this region mapped.
		 */
		passme = CHERITEST_CHECK_SYSCALL(mmap(0, PAGE_SIZE,
				PROT_READ | PROT_WRITE, MAP_ANON, -1, 0));

		EV_SET(&ike, 0x2BAD, EVFILT_USER, EV_ADD|EV_ONESHOT,
			NOTE_FFNOP, 0, passme);
		CHERITEST_CHECK_SYSCALL(kevent(kq, &ike, 1, NULL, 0, NULL));

		EV_SET(&ike, 0x2BAD, EVFILT_USER, EV_KEEPUDATA,
			NOTE_FFNOP|NOTE_TRIGGER, 0, NULL);
		CHERITEST_CHECK_SYSCALL(kevent(kq, &ike, 1, NULL, 0, NULL));

		waitpid(pid, &res, 0);
		if (res == 0) {
			cheritest_success();
		} else {
			cheritest_failure_errx("tag transfer");
		}
	}
}

/*
 * We can rfork and share the sigaction table across parent and child, which
 * again allows for capability passing across address spaces.
 */
void
cheritest_vm_cap_share_sigaction(const struct cheri_test *ctp __unused)
{
	int pid;

	pid = rfork(RFPROC | RFSIGSHARE);
	if (pid == -1)
		cheritest_failure_errx("Fork failed; errno=%d", errno);

	if (pid == 0) {
		void * __capability passme;
		struct sigaction sa;

		bzero(&sa, sizeof(sa));

		/* This is a little abusive, but shows the point, I think */

		passme = CHERITEST_CHECK_SYSCALL(mmap(0, PAGE_SIZE,
				PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANON, -1, 0));
		sa.sa_handler = passme;
		sa.sa_flags = 0;

		CHERITEST_CHECK_SYSCALL(sigaction(SIGUSR1, &sa, NULL));
		exit(0);
	} else {
		struct sigaction sa;

		waitpid(pid, NULL, 0);

		sa.sa_handler = NULL;
		CHERITEST_CHECK_SYSCALL(sigaction(SIGUSR1, NULL, &sa));

		CHERI_FPRINT_PTR(stderr, sa.sa_handler);

		if (cheri_gettag(sa.sa_handler)) {
			cheritest_failure_errx("tag transfer");
		} else {
			cheritest_success();
		}
	}
}

#endif

void
cheritest_vm_tag_dev_zero_shared(const struct cheri_test *ctp __unused)
{
	int fd = CHERITEST_CHECK_SYSCALL(open("/dev/zero", O_RDWR));
	mmap_and_check_tag_stored(fd, PROT_READ | PROT_WRITE, MAP_SHARED);
	cheritest_success();
}

void
cheritest_vm_tag_dev_zero_private(const struct cheri_test *ctp __unused)
{
	int fd = CHERITEST_CHECK_SYSCALL(open("/dev/zero", O_RDWR));
	mmap_and_check_tag_stored(fd, PROT_READ | PROT_WRITE, MAP_PRIVATE);
	cheritest_success();
}

static int
create_tempfile()
{
	char template[] = "/tmp/cheritest.XXXXXXXX";
	int fd = CHERITEST_CHECK_SYSCALL2(mkstemp(template),
	    "mkstemp %s", template);
	CHERITEST_CHECK_SYSCALL(unlink(template));
	CHERITEST_CHECK_SYSCALL(ftruncate(fd, getpagesize()));
	return fd;
}

/*
 * This case should fault.
 */
void
cheritest_vm_notag_tmpfile_shared(const struct cheri_test *ctp __unused)
{
	void * __capability volatile *cp;
	void * __capability cp_value;
	int fd, v;

	fd = create_tempfile();
	cp = CHERITEST_CHECK_SYSCALL(mmap(NULL, getpagesize(),
	    PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
	cp_value = cheri_ptr(&v, sizeof(v));
	*cp = cp_value;
	/* this test fails if /tmp is on tmpfs as it will silently strip the tag */
	cheritest_failure_errx("tagged store succeeded");
}

void
cheritest_vm_tag_tmpfile_private(const struct cheri_test *ctp __unused)
{
	int fd = create_tempfile();
	mmap_and_check_tag_stored(fd, PROT_READ | PROT_WRITE, MAP_PRIVATE);
	cheritest_success();
}

void
cheritest_vm_tag_tmpfile_private_prefault(const struct cheri_test *ctp __unused)
{
	int fd = create_tempfile();
	mmap_and_check_tag_stored(fd, PROT_READ | PROT_WRITE,
	    MAP_PRIVATE | MAP_PREFAULT_READ);
	cheritest_success();
}

const char *
xfail_need_writable_tmp(const char *name __unused)
{
	static const char *reason = NULL;
	static int checked = 0;
	char template[] = "/tmp/cheritest.XXXXXXXX";
	int fd;

	if (checked)
		return (reason);

	checked = 1;
	fd = mkstemp(template);
	if (fd >= 0) {
		close(fd);
		unlink(template);
		return (NULL);
	}
	reason = "/tmp is not writable";
	return (reason);
}

const char*
xfail_need_writable_non_tmpfs_tmp(const char *name)
{
	struct statfs info;

	CHERITEST_CHECK_SYSCALL(statfs("/tmp", &info));
	if (strcmp(info.f_fstypename, "tmpfs") == 0) {
		return ("cheritest_vm_notag_tmpfile_shared needs non-tmpfs /tmp");
	}
	return (xfail_need_writable_tmp(name));
}

/*
 * Exercise copy-on-write:
 *
 * 1) Create a new anonymous shared memory object, extend to page size, map,
 * and write a tagged capability to it.
 *
 * 2) Create a second copy-on-write mapping; read back the tagged value via
 * the second mapping, and confirm that it still has a tag.
 * (cheritest_vm_cow_read)
 *
 * 3) Write an adjacent word in the second mapping, which should cause a
 * copy-on-write, then read back the capability and confirm that it still has
 * a tag.  (cheritest_vm_cow_write)
 */
void
cheritest_vm_cow_read(const struct cheri_test *ctp __unused)
{
	void * __capability volatile *cp_copy;
	void * __capability volatile *cp_real;
	void * __capability cp;
	int fd;

	/*
	 * Create anonymous shared memory object.
	 */
	fd = CHERITEST_CHECK_SYSCALL(shm_open(SHM_ANON, O_RDWR, 0600));
	CHERITEST_CHECK_SYSCALL(ftruncate(fd, getpagesize()));

	/*
	 * Create 'real' and copy-on-write mappings.
	 */
	cp_real = CHERITEST_CHECK_SYSCALL2(mmap(NULL, getpagesize(),
	    PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0), "mmap cp_real");
	cp_copy = CHERITEST_CHECK_SYSCALL2(mmap(NULL, getpagesize(),
	    PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0), "mmap cp_copy");

	/*
	 * Write out a tagged capability to 'real' mapping -- doesn't really
	 * matter what it points at.  Confirm it has a tag.
	 */
	cp = cheri_ptr(&fd, sizeof(fd));
	cp_real[0] = cp;
	cp = cp_real[0];
	CHERITEST_VERIFY2(cheri_gettag(cp) != 0, "pretest: tag missing");

	/*
	 * Read in tagged capability via copy-on-write mapping.  Confirm it
	 * has a tag.
	 */
	cp = cp_copy[0];
	CHERITEST_VERIFY2(cheri_gettag(cp) != 0, "tag missing, cp_real");

	/*
	 * Clean up.
	 */
	CHERITEST_CHECK_SYSCALL2(munmap(__DEVOLATILE(void *, cp_real),
	    getpagesize()), "munmap cp_real");
	CHERITEST_CHECK_SYSCALL2(munmap(__DEVOLATILE(void *, cp_copy),
	    getpagesize()), "munmap cp_copy");
	CHERITEST_CHECK_SYSCALL(close(fd));
	cheritest_success();
}

void
cheritest_vm_cow_write(const struct cheri_test *ctp __unused)
{
	void * __capability volatile *cp_copy;
	void * __capability volatile *cp_real;
	void * __capability cp;
	int fd;

	/*
	 * Create anonymous shared memory object.
	 */
	fd = CHERITEST_CHECK_SYSCALL(shm_open(SHM_ANON, O_RDWR, 0600));
	CHERITEST_CHECK_SYSCALL(ftruncate(fd, getpagesize()));

	/*
	 * Create 'real' and copy-on-write mappings.
	 */
	cp_real = CHERITEST_CHECK_SYSCALL2(mmap(NULL, getpagesize(),
	    PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0), "mmap cp_real");
	cp_copy = CHERITEST_CHECK_SYSCALL2(mmap(NULL, getpagesize(),
	    PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0), "mmap cp_copy");

	/*
	 * Write out a tagged capability to 'real' mapping -- doesn't really
	 * matter what it points at.  Confirm it has a tag.
	 */
	cp = cheri_ptr(&fd, sizeof(fd));
	cp_real[0] = cp;
	cp = cp_real[0];
	CHERITEST_VERIFY2(cheri_gettag(cp) != 0, "pretest: tag missing");

	/*
	 * Read in tagged capability via copy-on-write mapping.  Confirm it
	 * has a tag.
	 */
	cp = cp_copy[0];
	CHERITEST_VERIFY2(cheri_gettag(cp) != 0, "tag missing, cp_real");

	/*
	 * Diverge from cheritest_vm_cow_read(): write via the second mapping
	 * to force a copy-on-write rather than continued sharing of the page.
	 */
	cp = cheri_ptr(&fd, sizeof(fd));
	cp_copy[1] = cp;

	/*
	 * Confirm that the tag is still present on the 'real' page.
	 */
	cp = cp_real[0];
	CHERITEST_VERIFY2(cheri_gettag(cp) != 0, "tag missing after COW, cp_real");

	cp = cp_copy[0];
	CHERITEST_VERIFY2(cheri_gettag(cp) != 0, "tag missing after COW, cp_copy");

	/*
	 * Clean up.
	 */
	CHERITEST_CHECK_SYSCALL2(munmap(__DEVOLATILE(void *, cp_real),
	    getpagesize()), "munmap cp_real");
	CHERITEST_CHECK_SYSCALL2(munmap(__DEVOLATILE(void *, cp_copy),
	    getpagesize()), "munmap cp_copy");
	CHERITEST_CHECK_SYSCALL(close(fd));
	cheritest_success();
}

/*
 * Test a mapped and unmapped invocation of cloadtags
 */

void
test_cloadtags_mapped(const struct cheri_test *ctp __unused)
{
	uint64_t tags = 0;
	void * __capability * __capability c;
	void * __capability * p;

	p = CHERITEST_CHECK_SYSCALL(mmap(0, PAGE_SIZE, PROT_READ | PROT_WRITE,
						MAP_ANON, -1, 0));
	c = (__cheri_tocap void * __capability * __capability) p;

	CHERITEST_VERIFY2(cheri_gettag(c) != 0, "initial cap not constructed");

	p[1] = c;
	p[2] = c;

	asm volatile ( "cloadtags %[tags], %[ptr]" : [tags]"+r"(tags) : [ptr]"r"(c) );

	CHERITEST_VERIFY2(tags == 0x6, "incorrect result from cloadtags");

	munmap(p, PAGE_SIZE);
	cheritest_success();
}

void
test_fault_cloadtags_unmapped(const struct cheri_test *ctp __unused)
{
	uint64_t tags = 0;
	void * __capability c;
	void * p;

	p = CHERITEST_CHECK_SYSCALL(mmap(0, PAGE_SIZE, PROT_READ | PROT_WRITE,
						MAP_ANON, -1, 0));
	munmap(p, PAGE_SIZE);
	c = (__cheri_tocap void * __capability) p;
	asm volatile ( "cloadtags %[tags], %[ptr]" : [tags]"+r"(tags) : [ptr]"r"(c) );
}

/*
 * Revocation tests
 */

#ifdef CHERIABI_TESTS

static int
check_revoked(void * __capability r)
{
	return ((cheri_gettag(r) == 1)
		&& (cheri_gettype(r) == (uint64_t)(-1))
		&& (cheri_getperm(r) == 0));
}

static void
install_kqueue_cap(int kq, void * __capability revme)
{
	struct kevent ike;

	EV_SET(&ike, 0x2BAD, EVFILT_USER, EV_ADD|EV_ONESHOT|EV_DISABLE,
		NOTE_FFNOP, 0, revme);
	CHERITEST_CHECK_SYSCALL(kevent(kq, &ike, 1, NULL, 0, NULL));
	EV_SET(&ike, 0x2BAD, EVFILT_USER, EV_KEEPUDATA,
		NOTE_FFNOP|NOTE_TRIGGER, 0, NULL);
	CHERITEST_CHECK_SYSCALL(kevent(kq, &ike, 1, NULL, 0, NULL));
}

static void
check_kqueue_cap(int kq, unsigned int valid)
{
	struct kevent ike, oke = { 0 };

	EV_SET(&ike, 0x2BAD, EVFILT_USER, EV_ENABLE|EV_KEEPUDATA,
		NOTE_FFNOP, 0, NULL);
	CHERITEST_CHECK_SYSCALL(kevent(kq, &ike, 1, NULL, 0, NULL));
	CHERITEST_CHECK_SYSCALL(kevent(kq, NULL, 0, &oke, 1, NULL));
	CHERITEST_VERIFY2(oke.ident == 0x2BAD, "Bad identifier from kqueue");
	CHERITEST_VERIFY2(oke.filter == EVFILT_USER, "Bad filter from kqueue");
	CHERITEST_VERIFY2(check_revoked(oke.udata) == !valid,
				"kqueue-held cap not as expected");
}

static void
fprintf_caprevoke_stats(FILE *f, struct caprevoke_stats crst, uint32_t cycsum)
{
	fprintf(f, "revoke:"
		" einit=%" PRIu64
		" efini=%" PRIu64
		" psro=%" PRIu32
		" psrw=%" PRIu32
		" scanc=%" PRIu64
		" psk=%" PRIu32
		" pskf=%" PRIu32
		" pfro=%" PRIu32
		" pfrw=%" PRIu32
		" prty=%" PRIu32
		" caps=%" PRIu32
		" crevd=%" PRIu32
		" cnuke=%" PRIu32
		" totc=%" PRIu32 "\n",
		crst.epoch_init,
		crst.epoch_fini,
		crst.pages_scan_ro,
		crst.pages_scan_rw,
		crst.page_scan_cycles,
		crst.pages_skip,
		crst.pages_skip_fast,
		crst.pages_faulted_ro,
		crst.pages_faulted_rw,
		crst.pages_retried,
		crst.caps_found,
		crst.caps_found_revoked,
		crst.caps_cleared,
		cycsum);
}

void
test_caprevoke_lightly(const struct cheri_test *ctp __unused)
{
	void * __capability * __capability mb;
	void * __capability sh;
	const volatile struct caprevoke_info * __capability cri;
	void * __capability revme;
	struct caprevoke_stats crst;
	int kq;
	uint32_t cyc_start, cyc_end;

	kq = CHERITEST_CHECK_SYSCALL(kqueue());
	mb = CHERITEST_CHECK_SYSCALL(mmap(0, PAGE_SIZE, PROT_READ | PROT_WRITE,
					  MAP_ANON, -1, 0));
	CHERITEST_CHECK_SYSCALL(caprevoke_shadow(CAPREVOKE_SHADOW_NOVMMAP,
						 mb, &sh));

	CHERITEST_CHECK_SYSCALL(
		caprevoke_shadow(CAPREVOKE_SHADOW_INFO_STRUCT, NULL,
				 __DEQUALIFY_CAP(void * __capability *,&cri)));

	/*
	 * OK, armed with the shadow mapping... generate a capability to
	 * the 0th granule of the map, spill it to the 1st granule,
	 * stash it in the kqueue, and mark it as revoked in the shadow.
	 */
	revme = cheri_andperm(mb, ~CHERI_PERM_CHERIABI_VMMAP);
	((void * __capability *)mb)[1] = revme;
	install_kqueue_cap(kq, revme);

	((uint8_t * __capability) sh)[0] = 1;

	/*
	 * First, let's see if we can shoot down just the hoarders and not
	 * the VM.  We'll put the thing back and do a full pass momentarily.
	 */
	CHERITEST_CHECK_SYSCALL(caprevoke(CAPREVOKE_JUST_HOARDERS, 0, &crst));

	check_kqueue_cap(kq, 0);
	CHERITEST_VERIFY2(!check_revoked(mb[1]), "JUST cleared memory tag");

	/* OK, full pass */
	install_kqueue_cap(kq, revme);

	cyc_start = cheri_get_cyclecount();
	CHERITEST_CHECK_SYSCALL(caprevoke(CAPREVOKE_LAST_PASS|CAPREVOKE_IGNORE_START, 0, &crst));
	cyc_end = cheri_get_cyclecount();

	fprintf_caprevoke_stats(stderr, crst, cyc_end - cyc_start);

	CHERITEST_VERIFY2(cri->epoch_dequeue == crst.epoch_fini, "Bad shared clock");

	CHERITEST_VERIFY2(check_revoked(mb[1]), "Memory tag persists");
	check_kqueue_cap(kq, 0);

	/* Clear the revocation bit and do that again */
	((uint8_t * __capability) sh)[0] = 0;

	/*
	 * We don't derive exactly the same thing, to prevent CSE from
	 * firing.  More specifically, we adjust the offset first, taking
	 * the path through the commutation diagram that doesn't share an
	 * edge with the derivation above.
	 */
	revme = cheri_andperm(mb+1, ~CHERI_PERM_CHERIABI_VMMAP);
	CHERITEST_VERIFY2(!check_revoked(revme), "Tag clear on 2nd revme?");
	((void * __capability *)mb)[1] = revme;
	install_kqueue_cap(kq, revme);

	cyc_start = cheri_get_cyclecount();
	CHERITEST_CHECK_SYSCALL(caprevoke(CAPREVOKE_IGNORE_START, 0, &crst));
	cyc_end = cheri_get_cyclecount();

	CHERITEST_VERIFY2(crst.epoch_fini >= crst.epoch_init + 1, "Bad epoch clock state");
	fprintf_caprevoke_stats(stderr, crst, cyc_end - cyc_start);

	CHERITEST_VERIFY2(cri->epoch_dequeue == crst.epoch_fini, "Bad shared clock");

	cyc_start = cheri_get_cyclecount();
	CHERITEST_CHECK_SYSCALL(caprevoke(CAPREVOKE_LAST_PASS, crst.epoch_init, &crst));
	cyc_end = cheri_get_cyclecount();

	fprintf_caprevoke_stats(stderr, crst, cyc_end - cyc_start);

	CHERITEST_VERIFY2(cri->epoch_dequeue == crst.epoch_fini, "Bad shared clock");

	CHERITEST_VERIFY2(!check_revoked(mb[1]), "Memory tag cleared");

	check_kqueue_cap(kq, 1);

	munmap(mb, PAGE_SIZE);
	close(kq);

	cheritest_success();
}

void
test_caprevoke_capdirty(const struct cheri_test *ctp __unused)
{
	void * __capability * __capability mb;
	void * __capability sh;
	const volatile struct caprevoke_info * __capability cri;
	void * __capability revme;
	struct caprevoke_stats crst;
	uint32_t cyc_start, cyc_end;

	mb = CHERITEST_CHECK_SYSCALL(mmap(0, PAGE_SIZE, PROT_READ | PROT_WRITE,
					  MAP_ANON, -1, 0));
	CHERITEST_CHECK_SYSCALL(caprevoke_shadow(CAPREVOKE_SHADOW_NOVMMAP,
						 mb, &sh));

	CHERITEST_CHECK_SYSCALL(
		caprevoke_shadow(CAPREVOKE_SHADOW_INFO_STRUCT, NULL,
				 __DEQUALIFY_CAP(void * __capability *,&cri)));

	revme = cheri_andperm(cheri_csetbounds(mb, 0x10),
			      ~CHERI_PERM_CHERIABI_VMMAP);
	mb[0] = revme;

	/* Mark the start of the arena as subject to revocation */
	((uint8_t * __capability) sh)[0] = 1;

	cyc_start = cheri_get_cyclecount();
	CHERITEST_CHECK_SYSCALL(caprevoke(CAPREVOKE_IGNORE_START, 0, &crst));
	cyc_end = cheri_get_cyclecount();
	fprintf_caprevoke_stats(stderr, crst, cyc_end - cyc_start);

	CHERITEST_VERIFY2(cri->epoch_dequeue == crst.epoch_fini, "Bad shared clock");

	CHERI_FPRINT_PTR(stderr, revme);
	CHERI_FPRINT_PTR(stderr, mb[0]);

	/* Between revocation sweeps, derive another cap and store */
	revme = cheri_andperm(cheri_csetbounds(mb, 0x11),
			      ~CHERI_PERM_CHERIABI_VMMAP);
	mb[1] = revme;

	cyc_start = cheri_get_cyclecount();
	CHERITEST_CHECK_SYSCALL(caprevoke(CAPREVOKE_IGNORE_START, 0, &crst));
	cyc_end = cheri_get_cyclecount();
	fprintf_caprevoke_stats(stderr, crst, cyc_end - cyc_start);

	CHERITEST_VERIFY2(cri->epoch_dequeue == crst.epoch_fini, "Bad shared clock");

	CHERI_FPRINT_PTR(stderr, revme);
	CHERI_FPRINT_PTR(stderr, mb[0]);
	CHERI_FPRINT_PTR(stderr, mb[1]);

	/* Between revocation sweeps, derive another cap and store */
	revme = cheri_andperm(cheri_csetbounds(mb, 0x12),
			      ~CHERI_PERM_CHERIABI_VMMAP);
	mb[2] = revme;

	cyc_start = cheri_get_cyclecount();
	CHERITEST_CHECK_SYSCALL(caprevoke(CAPREVOKE_LAST_PASS|CAPREVOKE_IGNORE_START, 0, &crst));
	cyc_end = cheri_get_cyclecount();
	fprintf_caprevoke_stats(stderr, crst, cyc_end - cyc_start);

	CHERITEST_VERIFY2(cri->epoch_dequeue == crst.epoch_fini, "Bad shared clock");

	CHERI_FPRINT_PTR(stderr, revme);
	CHERI_FPRINT_PTR(stderr, mb[0]);
	CHERI_FPRINT_PTR(stderr, mb[1]);
	CHERI_FPRINT_PTR(stderr, mb[2]);

	CHERITEST_VERIFY2(!check_revoked(mb), "Arena revoked");
	CHERITEST_VERIFY2(check_revoked(revme), "Register tag cleared");
	CHERITEST_VERIFY2(check_revoked(mb[0]), "Memory tag 0 cleared");
	CHERITEST_VERIFY2(check_revoked(mb[1]), "Memory tag 1 cleared");
	CHERITEST_VERIFY2(check_revoked(mb[2]), "Memory tag 2 cleared");

	munmap(mb, PAGE_SIZE);

	cheritest_success();
}

/*
 * Repeatedly invoke libcheri_caprevoke logic.
 * Using a bump the pointer allocator, repeatedly grab rand()-omly sized
 * objects and fill them with capabilities to themselves, mark them for
 * revocation, revoke, and validate.
 *
 */

#include <cheri/caprevoke.h>

/* Just for debugging printouts */
#ifndef CPU_CHERI
#define CPU_CHERI
#include <machine/pte.h>
#include <machine/vmparam.h>
#undef CPU_CHERI
#else
#include <machine/pte.h>
#include <machine/vmparam.h>
#endif

static void
test_caprevoke_lib_init(
	size_t bigblock_caps,
	void * __capability * __capability * obigblock,
	void * __capability * oshadow,
	const volatile struct caprevoke_info * __capability * ocri
)
{
	void * __capability * __capability bigblock;

	bigblock = CHERITEST_CHECK_SYSCALL(
			mmap(0, bigblock_caps * sizeof(void * __capability),
				PROT_READ | PROT_WRITE,
				MAP_ANON, -1, 0));

	for (size_t ix = 0; ix < bigblock_caps; ix++) {
		/* Create self-referential VMMAP-free capabilities */

		bigblock[ix] = cheri_andperm(
				cheri_csetbounds(&bigblock[ix], 16),
				~CHERI_PERM_CHERIABI_VMMAP);
	}
	*obigblock = bigblock;

	CHERITEST_CHECK_SYSCALL(
		caprevoke_shadow(CAPREVOKE_SHADOW_NOVMMAP, bigblock, oshadow));

	CHERITEST_CHECK_SYSCALL(
		caprevoke_shadow(CAPREVOKE_SHADOW_INFO_STRUCT, NULL,
			__DEQUALIFY_CAP(void * __capability *,ocri)));
}

static void
test_caprevoke_lib_run(
	int verbose,
	int paranoia,
	size_t bigblock_caps,
	void * __capability * __capability bigblock,
	void * __capability shadow,
	const volatile struct caprevoke_info * __capability cri
)
{
	size_t bigblock_offset = 0;

	while (bigblock_offset < bigblock_caps) {
		struct caprevoke_stats crst;
		uint32_t cyc_start, cyc_end;

		size_t csz = rand() % 1024 + 1;
		csz = MIN(csz, bigblock_caps - bigblock_offset);

		if (verbose > 1) {
			fprintf(stderr, "left=%zd csz=%zd\n",
					bigblock_caps - bigblock_offset,
					csz);
		}

		void * __capability * __capability chunk =
			cheri_csetbounds(bigblock + bigblock_offset,
					 csz * sizeof(void * __capability));

		if (verbose > 1) {
			CHERI_FPRINT_PTR(stderr, chunk);
		}

		size_t chunk_offset = bigblock_offset;
		bigblock_offset += csz;

		if (verbose > 3) {
			ptrdiff_t fwo, lwo;
			caprev_shadow_nomap_offsets((vaddr_t)chunk,
				csz * sizeof(void * __capability), &fwo, &lwo);

			fprintf(stderr,
				"premrk fwo=%lx lwo=%lx fw=%p "
				"*fw=%016lx *lw=%016lx\n",
				fwo, lwo,
				cheri_setaddress(shadow,
					VM_CAPREVOKE_BM_MEM_NOMAP + fwo),
				*(uint64_t *)(cheri_setaddress(shadow,
					VM_CAPREVOKE_BM_MEM_NOMAP + fwo)),
				*(uint64_t *)(cheri_setaddress(shadow,
					VM_CAPREVOKE_BM_MEM_NOMAP + lwo)));
		}

		/* Mark the chunk for revocation */
		CHERITEST_VERIFY2(caprev_shadow_nomap_set(shadow, chunk, chunk)
				  == 0, "Shadow update collision");

		__atomic_thread_fence(__ATOMIC_RELEASE);

		if (verbose > 3) {
			ptrdiff_t fwo, lwo;
			caprev_shadow_nomap_offsets((vaddr_t)chunk,
				csz * sizeof(void * __capability), &fwo, &lwo);

			fprintf(stderr,
				"marked fwo=%lx lwo=%lx fw=%p "
				"*fw=%016lx *lw=%016lx\n",
				fwo, lwo,
				cheri_setaddress(shadow,
					VM_CAPREVOKE_BM_MEM_NOMAP + fwo),
				*(uint64_t *)(cheri_setaddress(shadow,
					VM_CAPREVOKE_BM_MEM_NOMAP + fwo)),
				*(uint64_t *)(cheri_setaddress(shadow,
					VM_CAPREVOKE_BM_MEM_NOMAP + lwo)));
		}

		cyc_start = cheri_get_cyclecount();
		CHERITEST_CHECK_SYSCALL(
			caprevoke(CAPREVOKE_LAST_PASS|CAPREVOKE_IGNORE_START,
					0, &crst));
		cyc_end = cheri_get_cyclecount();
		if (verbose > 2) {
			fprintf_caprevoke_stats(stderr, crst, cyc_end - cyc_start);
		}
		CHERITEST_VERIFY2(cri->epoch_dequeue == crst.epoch_fini, "Bad shared clock");

		/* Check the surroundings */
		if (paranoia > 1) {
			for (size_t ix = 0; ix < chunk_offset; ix++) {
				CHERITEST_VERIFY2(!check_revoked(bigblock[ix]),
						"Revoked cap incorrectly below object, at ix=%zd\n", ix);
			}
			for (size_t ix = chunk_offset + csz; ix < bigblock_caps; ix++) {
				CHERITEST_VERIFY2(!check_revoked(bigblock[ix]),
						"Revoked cap incorrectly above object, at ix=%zd\n", ix);
			}
		}

		for (size_t ix = 0; ix < csz; ix++) {
			if (paranoia > 0) {
				if (!check_revoked(chunk[ix])) {
					CHERI_FPRINT_PTR(stderr, chunk[ix]);
					cheritest_failure_errx("Unrevoked at ix=%zd after revoke\n", ix);
				}
			}

			/* Put it back */
			chunk[ix] = cheri_andperm(
					cheri_csetbounds(&chunk[ix], 16),
					~CHERI_PERM_CHERIABI_VMMAP);
		}

		caprev_shadow_nomap_clear(shadow, chunk);

		__atomic_thread_fence(__ATOMIC_RELEASE);

	}
}

void
test_caprevoke_lib(const struct cheri_test *ctp __unused)
{
		/* If debugging the revoker, some verbosity can help. 0 - 4. */
	static const int verbose = 0;

		/*
		 * Tweaking paranoia can turn this test into more of a
		 * benchmark than a correctness test.  At 0, no checks
		 * will be performed; at 1, only the revoked object is
		 * investigated, and at 2, the entire allocaton arena
		 * is tested.
		 */
	static const int paranoia = 2;

	static const size_t bigblock_caps = 4096;

	void * __capability * __capability bigblock;
	void * __capability shadow;
	const volatile struct caprevoke_info * __capability cri;

	srand(1337);

	test_caprevoke_lib_init(bigblock_caps, &bigblock, &shadow, &cri);

	if (verbose > 0) {
		CHERI_FPRINT_PTR(stderr, bigblock);
		CHERI_FPRINT_PTR(stderr, shadow);
	}

	test_caprevoke_lib_run(verbose, paranoia, bigblock_caps,
		bigblock, shadow, cri);

	munmap(bigblock, bigblock_caps * sizeof(void * __capability));

	cheritest_success();
}

void
test_caprevoke_lib_fork(const struct cheri_test *ctp __unused)
{
	static const int verbose = 0;
	static const int paranoia = 2;

	static const size_t bigblock_caps = 4096;

	void * __capability * __capability bigblock;
	void * __capability shadow;
	const volatile struct caprevoke_info * __capability cri;

	int pid;

	srand(1337);

	test_caprevoke_lib_init(bigblock_caps, &bigblock, &shadow, &cri);

	if (verbose > 0) {
		CHERI_FPRINT_PTR(stderr, bigblock);
		CHERI_FPRINT_PTR(stderr, shadow);
	}

	pid = fork();
	if (pid == 0) {
		test_caprevoke_lib_run(verbose, paranoia, bigblock_caps,
			bigblock, shadow, cri);
	} else {
		int res;

		CHERITEST_VERIFY2(pid > 0, "fork failed");
		waitpid(pid, &res, 0);
		if (res == 0) {
			cheritest_success();
		} else {
			cheritest_failure_errx("Bad child process exit");
		}
	}

	munmap(bigblock, bigblock_caps * sizeof(void * __capability));

	cheritest_success();
}

#endif
