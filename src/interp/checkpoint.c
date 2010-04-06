/* 
 * checkpoint.c
 * by WN @ Ap6. 06, 2010
 */

#include <config.h>
#include <common/defs.h>
#include <common/debug.h>
#include <interp/checkpoint.h>
#include <interp/signal.h>

#include <xasm/syscall.h>
#include <xasm/string.h>
#include <xasm/tls.h>

static void
build_head(struct thread_private_data * tpd,
		struct checkpoint_head * head)
{
	/* fill magic */
	memset(head->magic, '\0', CKPT_MAGIC_SZ);
	memcpy(head->magic, CKPT_MAGIC, sizeof(CKPT_MAGIC));

	/* brk */
	head->brk = INTERNAL_SYSCALL_int80(brk, 1, 0);

	/* pid and tid */
	head->pid = tpd->pid;
	head->tid = tpd->tid;
	head->tnr = tpd->tnr;

	/* thread areas */
	for (int i = 0; i < GDT_ENTRY_TLS_ENTRIES; i++) {
		int err;
		head->thread_area[i].entry_number =
			GDT_ENTRY_TLS_MIN + i;
		err = INTERNAL_SYSCALL_int80(get_thread_area,
				1, &(head->thread_area[i]));
		assert(err == 0);
	}

	/* sigactions */
	struct tls_signal * ts = &tpd->signal;
	memcpy(&(head->sigactions), &(ts->sigactions),
			sizeof(head->sigactions));

	/* sigmask */
	memcpy(&head->sigmask, &(ts->sigmask),
			sizeof(ts->sigmask));
}

void
make_checkpoint(struct pusha_regs * regs)
{
	assert(sizeof(struct checkpoint_head) < 0x2000);
	struct thread_private_data * tpd = get_tpd();
	struct tls_logger * logger = &tpd->logger;

	/* first, create the checkpoint file */
	int err;
	int fd = INTERNAL_SYSCALL_int80(open, 3,
			logger->ckpt_fn, O_WRONLY|O_CREAT|O_TRUNC, 0664);
	if (fd <= 0)
		FATAL(CKPT, "create ckeckpoint file %s failed: %d\n",
				logger->ckpt_fn, fd);

	struct checkpoint_head head;
	build_head(tpd, &head);

	/* flush head */
	err = INTERNAL_SYSCALL_int80(write, 3,
			fd, &head, sizeof(head));
	assert(err == sizeof(head));

	err = INTERNAL_SYSCALL_int80(close, 1, fd);
	assert(err == 0);
}

// vim:ts=4:sw=4

