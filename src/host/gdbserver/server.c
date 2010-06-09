/* Main code for remote server for GDB.
   Copyright (C) 1989, 1993, 1994, 1995, 1997, 1998, 1999, 2000, 2002, 2003,
   2004, 2005, 2006, 2007, 2008, 2009 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "server.h"

#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_SIGNAL_H
#include <signal.h>
#endif
#if HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#if HAVE_MALLOC_H
#include <malloc.h>
#endif

#include <common/defs.h>

ptid_t cont_thread;
ptid_t general_thread;
ptid_t step_thread;

int server_waiting;

static int extended_protocol;
static int response_needed;
static int exit_requested;

int multi_process;
int non_stop;

static char **program_argv, **wrapper_argv;

/* Enable miscellaneous debugging output.  The name is historical - it
   was originally used to debug LinuxThreads support.  */
int debug_threads;

/* Enable debugging of h/w breakpoint/watchpoint support.  */
int debug_hw_points;

int pass_signals[TARGET_SIGNAL_LAST];

jmp_buf toplevel;

const char *gdbserver_xmltarget;

/* The PID of the originally created or attached inferior.  Used to
   send signals to the process when GDB sends us an asynchronous interrupt
   (user hitting Control-C in the client), and to wait for the child to exit
   when no longer debugging it.  */

unsigned long signal_pid;

#ifdef SIGTTOU
/* A file descriptor for the controlling terminal.  */
int terminal_fd;

/* TERMINAL_FD's original foreground group.  */
pid_t old_foreground_pgrp;

/* Hand back terminal ownership to the original foreground group.  */

static void
restore_old_foreground_pgrp (void)
{
  tcsetpgrp (terminal_fd, old_foreground_pgrp);
}
#endif

/* Set if you want to disable optional thread related packets support
   in gdbserver, for the sake of testing GDB against stubs that don't
   support them.  */
int disable_packet_vCont;
int disable_packet_Tthread;
int disable_packet_qC;
int disable_packet_qfThreadInfo;

/* Last status reported to GDB.  */
static struct target_waitstatus last_status;
static ptid_t last_ptid;

static char *own_buf;
static unsigned char *mem_buf;

/* Structure holding information relative to a single stop reply.  We
   keep a queue of these (really a singly-linked list) to push to GDB
   in non-stop mode.  */
struct vstop_notif
{
  /* Pointer to next in list.  */
  struct vstop_notif *next;

  /* Thread or process that got the event.  */
  ptid_t ptid;

  /* Event info.  */
  struct target_waitstatus status;
};

/* The pending stop replies list head.  */
static struct vstop_notif *notif_queue = NULL;

/* Put a stop reply to the stop reply queue.  */

static void
queue_stop_reply (ptid_t ptid, struct target_waitstatus *status)
{
  struct vstop_notif *new_notif;

  new_notif = malloc (sizeof (*new_notif));
  new_notif->next = NULL;
  new_notif->ptid = ptid;
  new_notif->status = *status;

  if (notif_queue)
    {
      struct vstop_notif *tail;
      for (tail = notif_queue;
	   tail && tail->next;
	   tail = tail->next)
	;
      tail->next = new_notif;
    }
  else
    notif_queue = new_notif;

  if (remote_debug)
    {
      int i = 0;
      struct vstop_notif *n;

      for (n = notif_queue; n; n = n->next)
	i++;

      fprintf (stderr, "pending stop replies: %d\n", i);
    }
}

/* Place an event in the stop reply queue, and push a notification if
   we aren't sending one yet.  */

void
push_event (ptid_t ptid, struct target_waitstatus *status)
{
  queue_stop_reply (ptid, status);

  /* If this is the first stop reply in the queue, then inform GDB
     about it, by sending a Stop notification.  */
  if (notif_queue->next == NULL)
    {
      char *p = own_buf;
      strcpy (p, "Stop:");
      p += strlen (p);
      prepare_resume_reply (p,
			    notif_queue->ptid, &notif_queue->status);
      putpkt_notif (own_buf);
    }
}

/* Get rid of the currently pending stop replies for PID.  If PID is
   -1, then apply to all processes.  */

static void
discard_queued_stop_replies (int pid)
{
  struct vstop_notif *prev = NULL, *reply, *next;

  for (reply = notif_queue; reply; reply = next)
    {
      next = reply->next;

      if (pid == -1
	  || ptid_get_pid (reply->ptid) == pid)
	{
	  if (reply == notif_queue)
	    notif_queue = next;
	  else
	    prev->next = reply->next;

	  free (reply);
	}
      else
	prev = reply;
    }
}

/* If there are more stop replies to push, push one now.  */

static void
send_next_stop_reply (char *own_buf)
{
  if (notif_queue)
    prepare_resume_reply (own_buf,
			  notif_queue->ptid,
			  &notif_queue->status);
  else
    write_ok (own_buf);
}

static int
target_running (void)
{
  return all_threads.head != NULL;
}

static int
start_inferior (char **argv)
{
  char **new_argv = argv;

  if (wrapper_argv != NULL)
    {
      int i, count = 1;

      for (i = 0; wrapper_argv[i] != NULL; i++)
	count++;
      for (i = 0; argv[i] != NULL; i++)
	count++;
      new_argv = alloca (sizeof (char *) * count);
      count = 0;
      for (i = 0; wrapper_argv[i] != NULL; i++)
	new_argv[count++] = wrapper_argv[i];
      for (i = 0; argv[i] != NULL; i++)
	new_argv[count++] = argv[i];
      new_argv[count] = NULL;
    }

#ifdef SIGTTOU
  signal (SIGTTOU, SIG_DFL);
  signal (SIGTTIN, SIG_DFL);
#endif

  signal_pid = create_inferior (new_argv[0], new_argv);

  /* FIXME: we don't actually know at this point that the create
     actually succeeded.  We won't know that until we wait.  */
  fprintf (stderr, "Process %s created; pid = %ld\n", argv[0],
	   signal_pid);
  fflush (stderr);

#ifdef SIGTTOU
  signal (SIGTTOU, SIG_IGN);
  signal (SIGTTIN, SIG_IGN);
  terminal_fd = fileno (stderr);
  old_foreground_pgrp = tcgetpgrp (terminal_fd);
  tcsetpgrp (terminal_fd, signal_pid);
  atexit (restore_old_foreground_pgrp);
#endif

  if (wrapper_argv != NULL)
    {
      struct thread_resume resume_info;
      ptid_t ptid;

      resume_info.thread = pid_to_ptid (signal_pid);
      resume_info.kind = resume_continue;
      resume_info.sig = 0;

      ptid = mywait (pid_to_ptid (signal_pid), &last_status, 0, 0);

      if (last_status.kind != TARGET_WAITKIND_STOPPED)
	return signal_pid;

      do
	{
	  (*the_target->resume) (&resume_info, 1);

 	  mywait (pid_to_ptid (signal_pid), &last_status, 0, 0);
	  if (last_status.kind != TARGET_WAITKIND_STOPPED)
	    return signal_pid;
	}
      while (last_status.value.sig != TARGET_SIGNAL_TRAP);

      return signal_pid;
    }

  /* Wait till we are at 1st instruction in program, return new pid
     (assuming success).  */
  last_ptid = mywait (pid_to_ptid (signal_pid), &last_status, 0, 0);

  return signal_pid;
}

static int
attach_inferior (int pid)
{
  /* myattach should return -1 if attaching is unsupported,
     0 if it succeeded, and call error() otherwise.  */

  if (myattach (pid) != 0)
    return -1;

  fprintf (stderr, "Attached; pid = %d\n", pid);
  fflush (stderr);

  /* FIXME - It may be that we should get the SIGNAL_PID from the
     attach function, so that it can be the main thread instead of
     whichever we were told to attach to.  */
  signal_pid = pid;

  if (!non_stop)
    {
      last_ptid = mywait (pid_to_ptid (pid), &last_status, 0, 0);

      /* GDB knows to ignore the first SIGSTOP after attaching to a running
	 process using the "attach" command, but this is different; it's
	 just using "target remote".  Pretend it's just starting up.  */
      if (last_status.kind == TARGET_WAITKIND_STOPPED
	  && last_status.value.sig == TARGET_SIGNAL_STOP)
	last_status.value.sig = TARGET_SIGNAL_TRAP;
    }

  return 0;
}

extern int remote_debug;

/* Decode a qXfer read request.  Return 0 if everything looks OK,
   or -1 otherwise.  */

static int
decode_xfer_read (char *buf, char **annex, CORE_ADDR *ofs, unsigned int *len)
{
  /* Extract and NUL-terminate the annex.  */
  *annex = buf;
  while (*buf && *buf != ':')
    buf++;
  if (*buf == '\0')
    return -1;
  *buf++ = 0;

  /* After the read marker and annex, qXfer looks like a
     traditional 'm' packet.  */
  decode_m_packet (buf, ofs, len);

  return 0;
}

/* Write the response to a successful qXfer read.  Returns the
   length of the (binary) data stored in BUF, corresponding
   to as much of DATA/LEN as we could fit.  IS_MORE controls
   the first character of the response.  */
static int
write_qxfer_response (char *buf, const void *data, int len, int is_more)
{
  int out_len;

  if (is_more)
    buf[0] = 'm';
  else
    buf[0] = 'l';

  return remote_escape_output (data, len, (unsigned char *) buf + 1, &out_len,
			       PBUFSIZ - 2) + 1;
}

/* Handle all of the extended 'Q' packets.  */
void
handle_general_set (char *own_buf)
{
  if (strncmp ("QPassSignals:", own_buf, strlen ("QPassSignals:")) == 0)
    {
      int numsigs = (int) TARGET_SIGNAL_LAST, i;
      const char *p = own_buf + strlen ("QPassSignals:");
      CORE_ADDR cursig;

      p = decode_address_to_semicolon (&cursig, p);
      for (i = 0; i < numsigs; i++)
	{
	  if (i == cursig)
	    {
	      pass_signals[i] = 1;
	      if (*p == '\0')
		/* Keep looping, to clear the remaining signals.  */
		cursig = -1;
	      else
		p = decode_address_to_semicolon (&cursig, p);
	    }
	  else
	    pass_signals[i] = 0;
	}
      strcpy (own_buf, "OK");
      return;
    }

  if (strcmp (own_buf, "QStartNoAckMode") == 0)
    {
      if (remote_debug)
	{
	  fprintf (stderr, "[noack mode enabled]\n");
	  fflush (stderr);
	}

      noack_mode = 1;
      write_ok (own_buf);
      return;
    }

  if (strncmp (own_buf, "QNonStop:", 9) == 0)
    {
      char *mode = own_buf + 9;
      int req = -1;
      char *req_str;

      if (strcmp (mode, "0") == 0)
	req = 0;
      else if (strcmp (mode, "1") == 0)
	req = 1;
      else
	{
	  /* We don't know what this mode is, so complain to
	     GDB.  */
	  fprintf (stderr, "Unknown non-stop mode requested: %s\n",
		   own_buf);
	  write_enn (own_buf);
	  return;
	}

      req_str = req ? "non-stop" : "all-stop";
      if (start_non_stop (req) != 0)
	{
	  fprintf (stderr, "Setting %s mode failed\n", req_str);
	  write_enn (own_buf);
	  return;
	}

      non_stop = req;

      if (remote_debug)
	fprintf (stderr, "[%s mode enabled]\n", req_str);

      write_ok (own_buf);
      return;
    }

  /* Otherwise we didn't know what packet it was.  Say we didn't
     understand it.  */
  own_buf[0] = 0;
}

static const char *
get_features_xml (const char *annex)
{
  /* gdbserver_xmltarget defines what to return when looking
     for the "target.xml" file.  Its contents can either be
     verbatim XML code (prefixed with a '@') or else the name
     of the actual XML file to be used in place of "target.xml".

     This variable is set up from the auto-generated
     init_registers_... routine for the current target.  */

  if (gdbserver_xmltarget
      && strcmp (annex, "target.xml") == 0)
    {
      if (*gdbserver_xmltarget == '@')
	return gdbserver_xmltarget + 1;
      else
	annex = gdbserver_xmltarget;
    }

#ifdef USE_XML
  {
    extern const char *const xml_builtin[][2];
    int i;

    /* Look for the annex.  */
    for (i = 0; xml_builtin[i][0] != NULL; i++)
      if (strcmp (annex, xml_builtin[i][0]) == 0)
	break;

    if (xml_builtin[i][0] != NULL)
      return xml_builtin[i][1];
  }
#endif

  return NULL;
}

void
monitor_show_help (void)
{
  monitor_output ("The following monitor commands are supported:\n");
  monitor_output ("  set debug <0|1>\n");
  monitor_output ("    Enable general debugging messages\n");
  monitor_output ("  set debug-hw-points <0|1>\n");
  monitor_output ("    Enable h/w breakpoint/watchpoint debugging messages\n");
  monitor_output ("  set remote-debug <0|1>\n");
  monitor_output ("    Enable remote protocol debugging messages\n");
  monitor_output ("  exit\n");
  monitor_output ("    Quit GDBserver\n");
}

/* Subroutine of handle_search_memory to simplify it.  */

static int
handle_search_memory_1 (CORE_ADDR start_addr, CORE_ADDR search_space_len,
			gdb_byte *pattern, unsigned pattern_len,
			gdb_byte *search_buf,
			unsigned chunk_size, unsigned search_buf_size,
			CORE_ADDR *found_addrp)
{
  /* Prime the search buffer.  */

  if (read_inferior_memory (start_addr, search_buf, search_buf_size) != 0)
    {
      warning ("Unable to access target memory at 0x%lx, halting search.",
	       (long) start_addr);
      return -1;
    }

  /* Perform the search.

     The loop is kept simple by allocating [N + pattern-length - 1] bytes.
     When we've scanned N bytes we copy the trailing bytes to the start and
     read in another N bytes.  */

  while (search_space_len >= pattern_len)
    {
      gdb_byte *found_ptr;
      unsigned nr_search_bytes = (search_space_len < search_buf_size
				  ? search_space_len
				  : search_buf_size);

      found_ptr = memmem (search_buf, nr_search_bytes, pattern, pattern_len);

      if (found_ptr != NULL)
	{
	  CORE_ADDR found_addr = start_addr + (found_ptr - search_buf);
	  *found_addrp = found_addr;
	  return 1;
	}

      /* Not found in this chunk, skip to next chunk.  */

      /* Don't let search_space_len wrap here, it's unsigned.  */
      if (search_space_len >= chunk_size)
	search_space_len -= chunk_size;
      else
	search_space_len = 0;

      if (search_space_len >= pattern_len)
	{
	  unsigned keep_len = search_buf_size - chunk_size;
	  CORE_ADDR read_addr = start_addr + keep_len;
	  int nr_to_read;

	  /* Copy the trailing part of the previous iteration to the front
	     of the buffer for the next iteration.  */
	  memcpy (search_buf, search_buf + chunk_size, keep_len);

	  nr_to_read = (search_space_len - keep_len < chunk_size
			? search_space_len - keep_len
			: chunk_size);

	  if (read_inferior_memory (read_addr, search_buf + keep_len,
				    nr_to_read) != 0)
	    {
	      warning ("Unable to access target memory at 0x%lx, halting search.",
		       (long) read_addr);
	      return -1;
	    }

	  start_addr += chunk_size;
	}
    }

  /* Not found.  */

  return 0;
}

/* Handle qSearch:memory packets.  */

static void
handle_search_memory (char *own_buf, int packet_len)
{
  CORE_ADDR start_addr;
  CORE_ADDR search_space_len;
  gdb_byte *pattern;
  unsigned int pattern_len;
  /* NOTE: also defined in find.c testcase.  */
#define SEARCH_CHUNK_SIZE 16000
  const unsigned chunk_size = SEARCH_CHUNK_SIZE;
  /* Buffer to hold memory contents for searching.  */
  gdb_byte *search_buf;
  unsigned search_buf_size;
  int found;
  CORE_ADDR found_addr;
  int cmd_name_len = sizeof ("qSearch:memory:") - 1;

  pattern = malloc (packet_len);
  if (pattern == NULL)
    {
      error ("Unable to allocate memory to perform the search");
      strcpy (own_buf, "E00");
      return;
    }
  if (decode_search_memory_packet (own_buf + cmd_name_len,
				   packet_len - cmd_name_len,
				   &start_addr, &search_space_len,
				   pattern, &pattern_len) < 0)
    {
      free (pattern);
      error ("Error in parsing qSearch:memory packet");
      strcpy (own_buf, "E00");
      return;
    }

  search_buf_size = chunk_size + pattern_len - 1;

  /* No point in trying to allocate a buffer larger than the search space.  */
  if (search_space_len < search_buf_size)
    search_buf_size = search_space_len;

  search_buf = malloc (search_buf_size);
  if (search_buf == NULL)
    {
      free (pattern);
      error ("Unable to allocate memory to perform the search");
      strcpy (own_buf, "E00");
      return;
    }

  found = handle_search_memory_1 (start_addr, search_space_len,
				  pattern, pattern_len,
				  search_buf, chunk_size, search_buf_size,
				  &found_addr);

  if (found > 0)
    sprintf (own_buf, "1,%lx", (long) found_addr);
  else if (found == 0)
    strcpy (own_buf, "0");
  else
    strcpy (own_buf, "E00");

  free (search_buf);
  free (pattern);
}

#define require_running(BUF)			\
  if (!target_running ())			\
    {						\
      write_enn (BUF);				\
      return;					\
    }

/* Handle all of the extended 'q' packets.  */
void
handle_query (char *own_buf, int packet_len, int *new_packet_len_p)
{
  static struct inferior_list_entry *thread_ptr;

  /* Reply the current thread id.  */
  if (strcmp ("qC", own_buf) == 0 && !disable_packet_qC)
    {
      ptid_t gdb_id;
      require_running (own_buf);

      if (!ptid_equal (general_thread, null_ptid)
	  && !ptid_equal (general_thread, minus_one_ptid))
	gdb_id = general_thread;
      else
	{
	  thread_ptr = all_threads.head;
	  gdb_id = thread_to_gdb_id ((struct thread_info *)thread_ptr);
	}

      sprintf (own_buf, "QC");
      own_buf += 2;
      own_buf = write_ptid (own_buf, gdb_id);
      return;
    }

  if (strcmp ("qSymbol::", own_buf) == 0)
    {
      if (target_running () && the_target->look_up_symbols != NULL)
	(*the_target->look_up_symbols) ();

      strcpy (own_buf, "OK");
      return;
    }

  if (!disable_packet_qfThreadInfo)
    {
      if (strcmp ("qfThreadInfo", own_buf) == 0)
	{
	  ptid_t gdb_id;

	  require_running (own_buf);
	  thread_ptr = all_threads.head;

	  *own_buf++ = 'm';
	  gdb_id = thread_to_gdb_id ((struct thread_info *)thread_ptr);
	  write_ptid (own_buf, gdb_id);
	  thread_ptr = thread_ptr->next;
	  return;
	}

      if (strcmp ("qsThreadInfo", own_buf) == 0)
	{
	  ptid_t gdb_id;

	  require_running (own_buf);
	  if (thread_ptr != NULL)
	    {
	      *own_buf++ = 'm';
	      gdb_id = thread_to_gdb_id ((struct thread_info *)thread_ptr);
	      write_ptid (own_buf, gdb_id);
	      thread_ptr = thread_ptr->next;
	      return;
	    }
	  else
	    {
	      sprintf (own_buf, "l");
	      return;
	    }
	}
    }

  if (the_target->read_offsets != NULL
      && strcmp ("qOffsets", own_buf) == 0)
    {
      CORE_ADDR text, data;

      require_running (own_buf);
      if (the_target->read_offsets (&text, &data))
	sprintf (own_buf, "Text=%lX;Data=%lX;Bss=%lX",
		 (long)text, (long)data, (long)data);
      else
	write_enn (own_buf);

      return;
    }

  if (the_target->qxfer_spu != NULL
      && strncmp ("qXfer:spu:read:", own_buf, 15) == 0)
    {
      char *annex;
      int n;
      unsigned int len;
      CORE_ADDR ofs;
      unsigned char *spu_buf;

      require_running (own_buf);
      strcpy (own_buf, "E00");
      if (decode_xfer_read (own_buf + 15, &annex, &ofs, &len) < 0)
	return;
      if (len > PBUFSIZ - 2)
	len = PBUFSIZ - 2;
      spu_buf = malloc (len + 1);
      if (!spu_buf)
	return;

      n = (*the_target->qxfer_spu) (annex, spu_buf, NULL, ofs, len + 1);
      if (n < 0)
	write_enn (own_buf);
      else if (n > (int)len)
	*new_packet_len_p = write_qxfer_response (own_buf, spu_buf, len, 1);
      else
	*new_packet_len_p = write_qxfer_response (own_buf, spu_buf, n, 0);

      free (spu_buf);
      return;
    }

  if (the_target->qxfer_spu != NULL
      && strncmp ("qXfer:spu:write:", own_buf, 16) == 0)
    {
      char *annex;
      int n;
      unsigned int len;
      CORE_ADDR ofs;
      unsigned char *spu_buf;

      require_running (own_buf);
      strcpy (own_buf, "E00");
      spu_buf = malloc (packet_len - 15);
      if (!spu_buf)
	return;
      if (decode_xfer_write (own_buf + 16, packet_len - 16, &annex,
			     &ofs, &len, spu_buf) < 0)
	{
	  free (spu_buf);
	  return;
	}

      n = (*the_target->qxfer_spu)
	(annex, NULL, (unsigned const char *)spu_buf, ofs, len);
      if (n < 0)
	write_enn (own_buf);
      else
	sprintf (own_buf, "%x", n);

      free (spu_buf);
      return;
    }

  if (the_target->read_auxv != NULL
      && strncmp ("qXfer:auxv:read:", own_buf, 16) == 0)
    {
      unsigned char *data;
      int n;
      CORE_ADDR ofs;
      unsigned int len;
      char *annex;

      require_running (own_buf);

      /* Reject any annex; grab the offset and length.  */
      if (decode_xfer_read (own_buf + 16, &annex, &ofs, &len) < 0
	  || annex[0] != '\0')
	{
	  strcpy (own_buf, "E00");
	  return;
	}

      /* Read one extra byte, as an indicator of whether there is
	 more.  */
      if (len > PBUFSIZ - 2)
	len = PBUFSIZ - 2;
      data = malloc (len + 1);
      if (data == NULL)
	{
	  write_enn (own_buf);
	  return;
	}
      n = (*the_target->read_auxv) (ofs, data, len + 1);
      if (n < 0)
	write_enn (own_buf);
      else if (n > (int)len)
	*new_packet_len_p = write_qxfer_response (own_buf, data, len, 1);
      else
	*new_packet_len_p = write_qxfer_response (own_buf, data, n, 0);

      free (data);

      return;
    }

  if (strncmp ("qXfer:features:read:", own_buf, 20) == 0)
    {
      CORE_ADDR ofs;
      unsigned int len, total_len;
      const char *document;
      char *annex;

      require_running (own_buf);

      /* Grab the annex, offset, and length.  */
      if (decode_xfer_read (own_buf + 20, &annex, &ofs, &len) < 0)
	{
	  strcpy (own_buf, "E00");
	  return;
	}

      /* Now grab the correct annex.  */
      document = get_features_xml (annex);
      if (document == NULL)
	{
	  strcpy (own_buf, "E00");
	  return;
	}

      total_len = strlen (document);
      if (len > PBUFSIZ - 2)
	len = PBUFSIZ - 2;

      if (ofs > total_len)
	write_enn (own_buf);
      else if (len < total_len - ofs)
	*new_packet_len_p = write_qxfer_response (own_buf, document + ofs,
						  len, 1);
      else
	*new_packet_len_p = write_qxfer_response (own_buf, document + ofs,
						  total_len - ofs, 0);

      return;
    }

  if (strncmp ("qXfer:libraries:read:", own_buf, 21) == 0)
    {
      CORE_ADDR ofs;
      unsigned int len, total_len;
      char *document, *p;
      struct inferior_list_entry *dll_ptr;
      char *annex;

      require_running (own_buf);

      /* Reject any annex; grab the offset and length.  */
      if (decode_xfer_read (own_buf + 21, &annex, &ofs, &len) < 0
	  || annex[0] != '\0')
	{
	  strcpy (own_buf, "E00");
	  return;
	}

      /* Over-estimate the necessary memory.  Assume that every character
	 in the library name must be escaped.  */
      total_len = 64;
      for (dll_ptr = all_dlls.head; dll_ptr != NULL; dll_ptr = dll_ptr->next)
	total_len += 128 + 6 * strlen (((struct dll_info *) dll_ptr)->name);

      document = malloc (total_len);
      if (document == NULL)
	{
	  write_enn (own_buf);
	  return;
	}
      strcpy (document, "<library-list>\n");
      p = document + strlen (document);

      for (dll_ptr = all_dlls.head; dll_ptr != NULL; dll_ptr = dll_ptr->next)
	{
	  struct dll_info *dll = (struct dll_info *) dll_ptr;
	  char *name;

	  strcpy (p, "  <library name=\"");
	  p = p + strlen (p);
	  name = xml_escape_text (dll->name);
	  strcpy (p, name);
	  free (name);
	  p = p + strlen (p);
	  strcpy (p, "\"><segment address=\"");
	  p = p + strlen (p);
	  sprintf (p, "0x%lx", (long) dll->base_addr);
	  p = p + strlen (p);
	  strcpy (p, "\"/></library>\n");
	  p = p + strlen (p);
	}

      strcpy (p, "</library-list>\n");

      total_len = strlen (document);
      if (len > PBUFSIZ - 2)
	len = PBUFSIZ - 2;

      if (ofs > total_len)
	write_enn (own_buf);
      else if (len < total_len - ofs)
	*new_packet_len_p = write_qxfer_response (own_buf, document + ofs,
						  len, 1);
      else
	*new_packet_len_p = write_qxfer_response (own_buf, document + ofs,
						  total_len - ofs, 0);

      free (document);
      return;
    }

  if (the_target->qxfer_osdata != NULL
      && strncmp ("qXfer:osdata:read:", own_buf, 18) == 0)
    {
      char *annex;
      int n;
      unsigned int len;
      CORE_ADDR ofs;
      unsigned char *workbuf;

      strcpy (own_buf, "E00");
      if (decode_xfer_read (own_buf + 18, &annex, &ofs, &len) < 0)
	return;
      if (len > PBUFSIZ - 2)
	len = PBUFSIZ - 2;
      workbuf = malloc (len + 1);
      if (!workbuf)
	return;

      n = (*the_target->qxfer_osdata) (annex, workbuf, NULL, ofs, len + 1);
      if (n < 0)
	write_enn (own_buf);
      else if (n > (int)len)
	*new_packet_len_p = write_qxfer_response (own_buf, workbuf, len, 1);
      else
	*new_packet_len_p = write_qxfer_response (own_buf, workbuf, n, 0);

      free (workbuf);
      return;
    }

  if (the_target->qxfer_siginfo != NULL
      && strncmp ("qXfer:siginfo:read:", own_buf, 19) == 0)
    {
      unsigned char *data;
      int n;
      CORE_ADDR ofs;
      unsigned int len;
      char *annex;

      require_running (own_buf);

      /* Reject any annex; grab the offset and length.  */
      if (decode_xfer_read (own_buf + 19, &annex, &ofs, &len) < 0
	  || annex[0] != '\0')
	{
	  strcpy (own_buf, "E00");
	  return;
	}

      /* Read one extra byte, as an indicator of whether there is
	 more.  */
      if (len > PBUFSIZ - 2)
	len = PBUFSIZ - 2;
      data = malloc (len + 1);
      if (!data)
	return;
      n = (*the_target->qxfer_siginfo) (annex, data, NULL, ofs, len + 1);
      if (n < 0)
	write_enn (own_buf);
      else if (n > (int)len)
	*new_packet_len_p = write_qxfer_response (own_buf, data, len, 1);
      else
	*new_packet_len_p = write_qxfer_response (own_buf, data, n, 0);

      free (data);
      return;
    }

  if (the_target->qxfer_siginfo != NULL
      && strncmp ("qXfer:siginfo:write:", own_buf, 20) == 0)
    {
      char *annex;
      int n;
      unsigned int len;
      CORE_ADDR ofs;
      unsigned char *data;

      require_running (own_buf);

      strcpy (own_buf, "E00");
      data = malloc (packet_len - 19);
      if (!data)
	return;
      if (decode_xfer_write (own_buf + 20, packet_len - 20, &annex,
			     &ofs, &len, data) < 0)
	{
	  free (data);
	  return;
	}

      n = (*the_target->qxfer_siginfo)
	(annex, NULL, (unsigned const char *)data, ofs, len);
      if (n < 0)
	write_enn (own_buf);
      else
	sprintf (own_buf, "%x", n);

      free (data);
      return;
    }

  /* Protocol features query.  */
  if (strncmp ("qSupported", own_buf, 10) == 0
      && (own_buf[10] == ':' || own_buf[10] == '\0'))
    {
      char *p = &own_buf[10];

      /* Process each feature being provided by GDB.  The first
	 feature will follow a ':', and latter features will follow
	 ';'.  */
      if (*p == ':')
	for (p = strtok (p + 1, ";");
	     p != NULL;
	     p = strtok (NULL, ";"))
	  {
	    if (strcmp (p, "multiprocess+") == 0)
	      {
		/* GDB supports and wants multi-process support if
		   possible.  */
		if (target_supports_multi_process ())
		  multi_process = 1;
	      }
	  }

      sprintf (own_buf, "PacketSize=%x;QPassSignals+", PBUFSIZ - 1);

      /* We do not have any hook to indicate whether the target backend
	 supports qXfer:libraries:read, so always report it.  */
      strcat (own_buf, ";qXfer:libraries:read+");

      if (the_target->read_auxv != NULL)
	strcat (own_buf, ";qXfer:auxv:read+");

      if (the_target->qxfer_spu != NULL)
	strcat (own_buf, ";qXfer:spu:read+;qXfer:spu:write+");

      if (the_target->qxfer_siginfo != NULL)
	strcat (own_buf, ";qXfer:siginfo:read+;qXfer:siginfo:write+");

      /* We always report qXfer:features:read, as targets may
	 install XML files on a subsequent call to arch_setup.
	 If we reported to GDB on startup that we don't support
	 qXfer:feature:read at all, we will never be re-queried.  */
      strcat (own_buf, ";qXfer:features:read+");

      if (transport_is_reliable)
	strcat (own_buf, ";QStartNoAckMode+");

      if (the_target->qxfer_osdata != NULL)
	strcat (own_buf, ";qXfer:osdata:read+");

      if (target_supports_multi_process ())
	strcat (own_buf, ";multiprocess+");

      if (target_supports_non_stop ())
	strcat (own_buf, ";QNonStop+");

      return;
    }

  /* Thread-local storage support.  */
  if (the_target->get_tls_address != NULL
      && strncmp ("qGetTLSAddr:", own_buf, 12) == 0)
    {
      char *p = own_buf + 12;
      CORE_ADDR parts[2], address = 0;
      int i, err;
      ptid_t ptid = null_ptid;

      require_running (own_buf);

      for (i = 0; i < 3; i++)
	{
	  char *p2;
	  int len;

	  if (p == NULL)
	    break;

	  p2 = strchr (p, ',');
	  if (p2)
	    {
	      len = p2 - p;
	      p2++;
	    }
	  else
	    {
	      len = strlen (p);
	      p2 = NULL;
	    }

	  if (i == 0)
	    ptid = read_ptid (p, NULL);
	  else
	    decode_address (&parts[i - 1], p, len);
	  p = p2;
	}

      if (p != NULL || i < 3)
	err = 1;
      else
	{
	  struct thread_info *thread = find_thread_ptid (ptid);

	  if (thread == NULL)
	    err = 2;
	  else
	    err = the_target->get_tls_address (thread, parts[0], parts[1],
					       &address);
	}

      if (err == 0)
	{
	  sprintf (own_buf, "%llx", address);
	  return;
	}
      else if (err > 0)
	{
	  write_enn (own_buf);
	  return;
	}

      /* Otherwise, pretend we do not understand this packet.  */
    }

  /* Handle "monitor" commands.  */
  if (strncmp ("qRcmd,", own_buf, 6) == 0)
    {
      char *mon = malloc (PBUFSIZ);
      int len = strlen (own_buf + 6);

      if (mon == NULL)
	{
	  write_enn (own_buf);
	  return;
	}

      if ((len % 2) != 0 || unhexify (mon, own_buf + 6, len / 2) != len / 2)
	{
	  write_enn (own_buf);
	  free (mon);
	  return;
	}
      mon[len / 2] = '\0';

      write_ok (own_buf);

      if (strcmp (mon, "set debug 1") == 0)
	{
	  debug_threads = 1;
	  monitor_output ("Debug output enabled.\n");
	}
      else if (strcmp (mon, "set debug 0") == 0)
	{
	  debug_threads = 0;
	  monitor_output ("Debug output disabled.\n");
	}
      else if (strcmp (mon, "set debug-hw-points 1") == 0)
	{
	  debug_hw_points = 1;
	  monitor_output ("H/W point debugging output enabled.\n");
	}
      else if (strcmp (mon, "set debug-hw-points 0") == 0)
	{
	  debug_hw_points = 0;
	  monitor_output ("H/W point debugging output disabled.\n");
	}
      else if (strcmp (mon, "set remote-debug 1") == 0)
	{
	  remote_debug = 1;
	  monitor_output ("Protocol debug output enabled.\n");
	}
      else if (strcmp (mon, "set remote-debug 0") == 0)
	{
	  remote_debug = 0;
	  monitor_output ("Protocol debug output disabled.\n");
	}
      else if (strcmp (mon, "help") == 0)
	monitor_show_help ();
      else if (strcmp (mon, "exit") == 0)
	exit_requested = 1;
      else
	{
	  monitor_output ("Unknown monitor command.\n\n");
	  monitor_show_help ();
	  write_enn (own_buf);
	}

      free (mon);
      return;
    }

  if (strncmp ("qSearch:memory:", own_buf, sizeof ("qSearch:memory:") - 1) == 0)
    {
      require_running (own_buf);
      handle_search_memory (own_buf, packet_len);
      return;
    }

  if (strcmp (own_buf, "qAttached") == 0
      || strncmp (own_buf, "qAttached:", sizeof ("qAttached:") - 1) == 0)
    {
      struct process_info *process;

      if (own_buf[sizeof ("qAttached") - 1])
	{
	  int pid = strtoul (own_buf + sizeof ("qAttached:") - 1, NULL, 16);
	  process = (struct process_info *)
	    find_inferior_id (&all_processes, pid_to_ptid (pid));
	}
      else
	{
	  require_running (own_buf);
	  process = current_process ();
	}

      if (process == NULL)
	{
	  write_enn (own_buf);
	  return;
	}

      strcpy (own_buf, process->attached ? "1" : "0");
      return;
    }

  /* Otherwise we didn't know what packet it was.  Say we didn't
     understand it.  */
  own_buf[0] = 0;
}

/* Parse vCont packets.  */
void
handle_v_cont (char *own_buf)
{
  char *p, *q;
  int n = 0, i = 0;
  struct thread_resume *resume_info;
  struct thread_resume default_action = {
	  .thread = {
		  .pid = 0,
		  .lwp = 0,
		  .tid = 0,
	  },
	  .kind = resume_continue,
	  .sig = 0,
  };

  /* Count the number of semicolons in the packet.  There should be one
     for every action.  */
  p = &own_buf[5];
  while (p)
    {
      n++;
      p++;
      p = strchr (p, ';');
    }

  resume_info = malloc (n * sizeof (resume_info[0]));
  if (resume_info == NULL)
    goto err;

  p = &own_buf[5];
  while (*p)
    {
      p++;

      if (p[0] == 's' || p[0] == 'S')
	resume_info[i].kind = resume_step;
      else if (p[0] == 'c' || p[0] == 'C')
	resume_info[i].kind = resume_continue;
      else if (p[0] == 't')
	resume_info[i].kind = resume_stop;
      else
	goto err;

      if (p[0] == 'S' || p[0] == 'C')
	{
	  int sig;
	  sig = strtol (p + 1, &q, 16);
	  if (p == q)
	    goto err;
	  p = q;

	  if (!target_signal_to_host_p (sig))
	    goto err;
	  resume_info[i].sig = target_signal_to_host (sig);
	}
      else
	{
	  resume_info[i].sig = 0;
	  p = p + 1;
	}

      if (p[0] == 0)
	{
	  resume_info[i].thread = minus_one_ptid;
	  default_action = resume_info[i];

	  /* Note: we don't increment i here, we'll overwrite this entry
	     the next time through.  */
	}
      else if (p[0] == ':')
	{
	  ptid_t ptid = read_ptid (p + 1, &q);

	  if (p == q)
	    goto err;
	  p = q;
	  if (p[0] != ';' && p[0] != 0)
	    goto err;

	  resume_info[i].thread = ptid;

	  i++;
	}
    }

  if (i < n)
    resume_info[i] = default_action;

  /* Still used in occasional places in the backend.  */
  if (n == 1
      && !ptid_equal (resume_info[0].thread, minus_one_ptid)
      && resume_info[0].kind != resume_stop)
    cont_thread = resume_info[0].thread;
  else
    cont_thread = minus_one_ptid;
  set_desired_inferior (0);

  if (!non_stop)
    enable_async_io ();

  (*the_target->resume) (resume_info, n);

  free (resume_info);

  if (non_stop)
    write_ok (own_buf);
  else
    {
      last_ptid = mywait (minus_one_ptid, &last_status, 0, 1);
      prepare_resume_reply (own_buf, last_ptid, &last_status);
      disable_async_io ();
    }
  return;

err:
  write_enn (own_buf);
  free (resume_info);
  return;
}

/* Attach to a new program.  Return 1 if successful, 0 if failure.  */
int
handle_v_attach (char *own_buf)
{
  int pid;

  pid = strtol (own_buf + 8, NULL, 16);
  if (pid != 0 && attach_inferior (pid) == 0)
    {
      /* Don't report shared library events after attaching, even if
	 some libraries are preloaded.  GDB will always poll the
	 library list.  Avoids the "stopped by shared library event"
	 notice on the GDB side.  */
      dlls_changed = 0;

      if (non_stop)
	{
	  /* In non-stop, we don't send a resume reply.  Stop events
	     will follow up using the normal notification
	     mechanism.  */
	  write_ok (own_buf);
	}
      else
	prepare_resume_reply (own_buf, last_ptid, &last_status);

      return 1;
    }
  else
    {
      write_enn (own_buf);
      return 0;
    }
}

/* Run a new program.  Return 1 if successful, 0 if failure.  */
static int
handle_v_run (char *own_buf)
{
  char *p, *next_p, **new_argv;
  int i, new_argc;

  new_argc = 0;
  for (p = own_buf + strlen ("vRun;"); p && *p; p = strchr (p, ';'))
    {
      p++;
      new_argc++;
    }

  new_argv = calloc (new_argc + 2, sizeof (char *));
  if (new_argv == NULL)
    {
      write_enn (own_buf);
      return 0;
    }

  i = 0;
  for (p = own_buf + strlen ("vRun;"); *p; p = next_p)
    {
      next_p = strchr (p, ';');
      if (next_p == NULL)
	next_p = p + strlen (p);

      if (i == 0 && p == next_p)
	new_argv[i] = NULL;
      else
	{
	  /* FIXME: Fail request if out of memory instead of dying.  */
	  new_argv[i] = xmalloc (1 + (next_p - p) / 2);
	  unhexify (new_argv[i], p, (next_p - p) / 2);
	  new_argv[i][(next_p - p) / 2] = '\0';
	}

      if (*next_p)
	next_p++;
      i++;
    }
  new_argv[i] = NULL;

  if (new_argv[0] == NULL)
    {
      /* GDB didn't specify a program to run.  Use the program from the
	 last run with the new argument list.  */

      if (program_argv == NULL)
	{
	  /* FIXME: new_argv memory leak */
	  write_enn (own_buf);
	  return 0;
	}

      new_argv[0] = strdup (program_argv[0]);
      if (new_argv[0] == NULL)
	{
	  /* FIXME: new_argv memory leak */
	  write_enn (own_buf);
	  return 0;
	}
    }

  /* Free the old argv and install the new one.  */
  freeargv (program_argv);
  program_argv = new_argv;

  start_inferior (program_argv);
  if (last_status.kind == TARGET_WAITKIND_STOPPED)
    {
      prepare_resume_reply (own_buf, last_ptid, &last_status);

      /* In non-stop, sending a resume reply doesn't set the general
	 thread, but GDB assumes a vRun sets it (this is so GDB can
	 query which is the main thread of the new inferior.  */
      if (non_stop)
	general_thread = last_ptid;

      return 1;
    }
  else
    {
      write_enn (own_buf);
      return 0;
    }
}

/* Kill process.  Return 1 if successful, 0 if failure.  */
int
handle_v_kill (char *own_buf)
{
  int pid;
  char *p = &own_buf[6];
  if (multi_process)
    pid = strtol (p, NULL, 16);
  else
    pid = signal_pid;
  if (pid != 0 && kill_inferior (pid) == 0)
    {
      last_status.kind = TARGET_WAITKIND_SIGNALLED;
      last_status.value.sig = TARGET_SIGNAL_KILL;
      last_ptid = pid_to_ptid (pid);
      discard_queued_stop_replies (pid);
      write_ok (own_buf);
      return 1;
    }
  else
    {
      write_enn (own_buf);
      return 0;
    }
}

/* Handle a 'vStopped' packet.  */
static void
handle_v_stopped (char *own_buf)
{
  /* If we're waiting for GDB to acknowledge a pending stop reply,
     consider that done.  */
  if (notif_queue)
    {
      struct vstop_notif *head;

      if (remote_debug)
	fprintf (stderr, "vStopped: acking %s\n",
		 target_pid_to_str (notif_queue->ptid));

      head = notif_queue;
      notif_queue = notif_queue->next;
      free (head);
    }

  /* Push another stop reply, or if there are no more left, an OK.  */
  send_next_stop_reply (own_buf);
}

/* Handle all of the extended 'v' packets.  */
void
handle_v_requests (char *own_buf, int packet_len, int *new_packet_len)
{
  if (!disable_packet_vCont)
    {
      if (strncmp (own_buf, "vCont;", 6) == 0)
	{
	  require_running (own_buf);
	  handle_v_cont (own_buf);
	  return;
	}

      if (strncmp (own_buf, "vCont?", 6) == 0)
	{
	  strcpy (own_buf, "vCont;c;C;s;S;t");
	  return;
	}
    }

  if (strncmp (own_buf, "vFile:", 6) == 0
      && handle_vFile (own_buf, packet_len, new_packet_len))
    return;

  if (strncmp (own_buf, "vAttach;", 8) == 0)
    {
      if (!multi_process && target_running ())
	{
	  fprintf (stderr, "Already debugging a process\n");
	  write_enn (own_buf);
	  return;
	}
      handle_v_attach (own_buf);
      return;
    }

  if (strncmp (own_buf, "vRun;", 5) == 0)
    {
      if (!multi_process && target_running ())
	{
	  fprintf (stderr, "Already debugging a process\n");
	  write_enn (own_buf);
	  return;
	}
      handle_v_run (own_buf);
      return;
    }

  if (strncmp (own_buf, "vKill;", 6) == 0)
    {
      if (!target_running ())
	{
	  fprintf (stderr, "No process to kill\n");
	  write_enn (own_buf);
	  return;
	}
      handle_v_kill (own_buf);
      return;
    }

  if (strncmp (own_buf, "vStopped", 8) == 0)
    {
      handle_v_stopped (own_buf);
      return;
    }

  /* Otherwise we didn't know what packet it was.  Say we didn't
     understand it.  */
  own_buf[0] = 0;
  return;
}

/* Resume inferior and wait for another event.  In non-stop mode,
   don't really wait here, but return immediatelly to the event
   loop.  */
void
myresume (char *own_buf, int step, int sig)
{
  struct thread_resume resume_info[2];
  int n = 0;
  int valid_cont_thread;

  set_desired_inferior (0);

  valid_cont_thread = (!ptid_equal (cont_thread, null_ptid)
			 && !ptid_equal (cont_thread, minus_one_ptid));

  if (step || sig || valid_cont_thread)
    {
      resume_info[0].thread
	= ((struct inferior_list_entry *) current_inferior)->id;
      if (step)
	resume_info[0].kind = resume_step;
      else
	resume_info[0].kind = resume_continue;
      resume_info[0].sig = sig;
      n++;
    }

  if (!valid_cont_thread)
    {
      resume_info[n].thread = minus_one_ptid;
      resume_info[n].kind = resume_continue;
      resume_info[n].sig = 0;
      n++;
    }

  if (!non_stop)
    enable_async_io ();

  (*the_target->resume) (resume_info, n);

  if (non_stop)
    write_ok (own_buf);
  else
    {
      last_ptid = mywait (minus_one_ptid, &last_status, 0, 1);
      prepare_resume_reply (own_buf, last_ptid, &last_status);
      disable_async_io ();
    }
}

/* Callback for for_each_inferior.  Make a new stop reply for each
   stopped thread.  */

static int
queue_stop_reply_callback (struct inferior_list_entry *entry, void *arg)
{
  int pid = * (int *) arg;

  if (pid == -1
      || ptid_get_pid (entry->id) == pid)
    {
      struct target_waitstatus status;

      status.kind = TARGET_WAITKIND_STOPPED;
      status.value.sig = TARGET_SIGNAL_TRAP;

      /* Pass the last stop reply back to GDB, but don't notify.  */
      queue_stop_reply (entry->id, &status);
    }

  return 0;
}

/* Status handler for the '?' packet.  */

static void
handle_status (char *own_buf)
{
  struct target_waitstatus status;
  status.kind = TARGET_WAITKIND_STOPPED;
  status.value.sig = TARGET_SIGNAL_TRAP;

  /* In non-stop mode, we must send a stop reply for each stopped
     thread.  In all-stop mode, just send one for the first stopped
     thread we find.  */

  if (non_stop)
    {
      int pid = -1;
      discard_queued_stop_replies (pid);
      find_inferior (&all_threads, queue_stop_reply_callback, &pid);

      /* The first is sent immediatly.  OK is sent if there is no
	 stopped thread, which is the same handling of the vStopped
	 packet (by design).  */
      send_next_stop_reply (own_buf);
    }
  else
    {
      if (all_threads.head)
	prepare_resume_reply (own_buf,
			      all_threads.head->id, &status);
      else
	strcpy (own_buf, "W00");
    }
}

static void
gdbserver_version (void)
{
  printf ("GNU gdbserver %s%s\n"
	  "Copyright (C) 2009 Free Software Foundation, Inc.\n"
	  "gdbserver is free software, covered by the GNU General Public License.\n"
	  "This gdbserver was configured as \"%s\"\n",
	  GDBSERVER_PKGVERSION, version, host_name);
}

static void
gdbserver_usage (FILE *stream)
{
  fprintf (stream, "Usage:\tgdbserver [OPTIONS] COMM PROG [ARGS ...]\n"
	   "\tgdbserver [OPTIONS] --attach COMM PID\n"
	   "\tgdbserver [OPTIONS] --multi COMM\n"
	   "\n"
	   "COMM may either be a tty device (for serial debugging), or \n"
	   "HOST:PORT to listen for a TCP connection.\n"
	   "\n"
	   "Options:\n"
	   "  --debug               Enable general debugging output.\n"
	   "  --remote-debug        Enable remote protocol debugging output.\n"
	   "  --version             Display version information and exit.\n"
	   "  --wrapper WRAPPER --  Run WRAPPER to start new programs.\n");
  if (REPORT_BUGS_TO[0] && stream == stdout)
    fprintf (stream, "Report bugs to \"%s\".\n", REPORT_BUGS_TO);
}

static void
gdbserver_show_disableable (FILE *stream)
{
  fprintf (stream, "Disableable packets:\n"
	   "  vCont       \tAll vCont packets\n"
	   "  qC          \tQuerying the current thread\n"
	   "  qfThreadInfo\tThread listing\n"
	   "  Tthread     \tPassing the thread specifier in the T stop reply packet\n"
	   "  threads     \tAll of the above\n");
}


#undef require_running
#define require_running(BUF)			\
  if (!target_running ())			\
    {						\
      write_enn (BUF);				\
      break;					\
    }

static int
first_thread_of (struct inferior_list_entry *entry, void *args)
{
  int pid = * (int *) args;

  if (ptid_get_pid (entry->id) == pid)
    return 1;

  return 0;
}

static void
kill_inferior_callback (struct inferior_list_entry *entry)
{
  struct process_info *process = (struct process_info *) entry;
  int pid = ptid_get_pid (process->head.id);

  kill_inferior (pid);
  discard_queued_stop_replies (pid);
}

/* Callback for for_each_inferior to detach or kill the inferior,
   depending on whether we attached to it or not.
   We inform the user whether we're detaching or killing the process
   as this is only called when gdbserver is about to exit.  */

static void
detach_or_kill_inferior_callback (struct inferior_list_entry *entry)
{
  struct process_info *process = (struct process_info *) entry;
  int pid = ptid_get_pid (process->head.id);

  if (process->attached)
    detach_inferior (pid);
  else
    kill_inferior (pid);

  discard_queued_stop_replies (pid);
}

/* for_each_inferior callback for detach_or_kill_for_exit to print
   the pids of started inferiors.  */

static void
print_started_pid (struct inferior_list_entry *entry)
{
  struct process_info *process = (struct process_info *) entry;

  if (! process->attached)
    {
      int pid = ptid_get_pid (process->head.id);
      fprintf (stderr, " %d", pid);
    }
}

/* for_each_inferior callback for detach_or_kill_for_exit to print
   the pids of attached inferiors.  */

static void
print_attached_pid (struct inferior_list_entry *entry)
{
  struct process_info *process = (struct process_info *) entry;

  if (process->attached)
    {
      int pid = ptid_get_pid (process->head.id);
      fprintf (stderr, " %d", pid);
    }
}

/* Call this when exiting gdbserver with possible inferiors that need
   to be killed or detached from.  */

static void
detach_or_kill_for_exit (void)
{
  /* First print a list of the inferiors we will be killing/detaching.
     This is to assist the user, for example, in case the inferior unexpectedly
     dies after we exit: did we screw up or did the inferior exit on its own?
     Having this info will save some head-scratching.  */

  if (have_started_inferiors_p ())
    {
      fprintf (stderr, "Killing process(es):");
      for_each_inferior (&all_processes, print_started_pid);
      fprintf (stderr, "\n");
    }
  if (have_attached_inferiors_p ())
    {
      fprintf (stderr, "Detaching process(es):");
      for_each_inferior (&all_processes, print_attached_pid);
      fprintf (stderr, "\n");
    }

  /* Now we can kill or detach the inferiors.  */

  for_each_inferior (&all_processes, detach_or_kill_inferior_callback);
}

static void
join_inferiors_callback (struct inferior_list_entry *entry)
{
  struct process_info *process = (struct process_info *) entry;

  /* If we are attached, then we can exit.  Otherwise, we need to hang
     around doing nothing, until the child is gone.  */
  if (!process->attached)
    join_inferior (ptid_get_pid (process->head.id));
}

int
gdbserver_main (int argc, char *argv[])
{
  int bad_attach;
  int pid;
  char *arg_end, *port;
  char **next_arg = &argv[1];
  int multi_mode = 0;
  int attach = 0;
  int was_running;

  while (*next_arg != NULL && **next_arg == '-')
    {
      if (strcmp (*next_arg, "--version") == 0)
	{
	  gdbserver_version ();
	  exit (0);
	}
      else if (strcmp (*next_arg, "--help") == 0)
	{
	  gdbserver_usage (stdout);
	  exit (0);
	}
      else if (strcmp (*next_arg, "--attach") == 0)
	attach = 1;
      else if (strcmp (*next_arg, "--multi") == 0)
	multi_mode = 1;
      else if (strcmp (*next_arg, "--wrapper") == 0)
	{
	  next_arg++;

	  wrapper_argv = next_arg;
	  while (*next_arg != NULL && strcmp (*next_arg, "--") != 0)
	    next_arg++;

	  if (next_arg == wrapper_argv || *next_arg == NULL)
	    {
	      gdbserver_usage (stderr);
	      exit (1);
	    }

	  /* Consume the "--".  */
	  *next_arg = NULL;
	}
      else if (strcmp (*next_arg, "--debug") == 0)
	debug_threads = 1;
      else if (strcmp (*next_arg, "--remote-debug") == 0)
	remote_debug = 1;
      else if (strcmp (*next_arg, "--disable-packet") == 0)
	{
	  gdbserver_show_disableable (stdout);
	  exit (0);
	}
      else if (strncmp (*next_arg,
			"--disable-packet=",
			sizeof ("--disable-packet=") - 1) == 0)
	{
	  char *packets, *tok;

	  packets = *next_arg += sizeof ("--disable-packet=") - 1;
	  for (tok = strtok (packets, ",");
	       tok != NULL;
	       tok = strtok (NULL, ","))
	    {
	      if (strcmp ("vCont", tok) == 0)
		disable_packet_vCont = 1;
	      else if (strcmp ("Tthread", tok) == 0)
		disable_packet_Tthread = 1;
	      else if (strcmp ("qC", tok) == 0)
		disable_packet_qC = 1;
	      else if (strcmp ("qfThreadInfo", tok) == 0)
		disable_packet_qfThreadInfo = 1;
	      else if (strcmp ("threads", tok) == 0)
		{
		  disable_packet_vCont = 1;
		  disable_packet_Tthread = 1;
		  disable_packet_qC = 1;
		  disable_packet_qfThreadInfo = 1;
		}
	      else
		{
		  fprintf (stderr, "Don't know how to disable \"%s\".\n\n",
			   tok);
		  gdbserver_show_disableable (stderr);
		  exit (1);
		}
	    }
	}
      else
	{
	  fprintf (stderr, "Unknown argument: %s\n", *next_arg);
	  exit (1);
	}

      next_arg++;
      continue;
    }

  if (setjmp (toplevel))
    {
      fprintf (stderr, "Exiting\n");
      exit (1);
    }

  port = *next_arg;
  next_arg++;
  if (port == NULL || (!attach && !multi_mode && *next_arg == NULL))
    {
      gdbserver_usage (stderr);
      exit (1);
    }

  bad_attach = 0;
  pid = 0;

  /* --attach used to come after PORT, so allow it there for
       compatibility.  */
  if (*next_arg != NULL && strcmp (*next_arg, "--attach") == 0)
    {
      attach = 1;
      next_arg++;
    }

  if (attach
      && (*next_arg == NULL
	  || (*next_arg)[0] == '\0'
	  || (pid = strtoul (*next_arg, &arg_end, 0)) == 0
	  || *arg_end != '\0'
	  || next_arg[1] != NULL))
    bad_attach = 1;

  if (bad_attach)
    {
      gdbserver_usage (stderr);
      exit (1);
    }

  initialize_inferiors ();
  initialize_async_io ();
  initialize_low ();

  own_buf = xmalloc (PBUFSIZ + 1);
  mem_buf = xmalloc (PBUFSIZ);

  if (pid == 0 && *next_arg != NULL)
    {
      int i, n;

      n = argc - (next_arg - argv);
      program_argv = xmalloc (sizeof (char *) * (n + 1));
      for (i = 0; i < n; i++)
	program_argv[i] = xstrdup (next_arg[i]);
      program_argv[i] = NULL;

      /* Wait till we are at first instruction in program.  */
      start_inferior (program_argv);

      /* We are now (hopefully) stopped at the first instruction of
	 the target process.  This assumes that the target process was
	 successfully created.  */
    }
  else if (pid != 0)
    {
      if (attach_inferior (pid) == -1)
	error ("Attaching not supported on this target");

      /* Otherwise succeeded.  */
    }
  else
    {
      last_status.kind = TARGET_WAITKIND_EXITED;
      last_status.value.integer = 0;
      last_ptid = minus_one_ptid;
    }

  /* Don't report shared library events on the initial connection,
     even if some libraries are preloaded.  Avoids the "stopped by
     shared library event" notice on gdb side.  */
  dlls_changed = 0;

  if (setjmp (toplevel))
    {
      detach_or_kill_for_exit ();
      exit (1);
    }

  if (last_status.kind == TARGET_WAITKIND_EXITED
      || last_status.kind == TARGET_WAITKIND_SIGNALLED)
    was_running = 0;
  else
    was_running = 1;

  if (!was_running && !multi_mode)
    {
      fprintf (stderr, "No program to debug.  GDBserver exiting.\n");
      exit (1);
    }

  while (1)
    {
      noack_mode = 0;
      multi_process = 0;
      non_stop = 0;

      remote_open (port);

      if (setjmp (toplevel) != 0)
	{
	  /* An error occurred.  */
	  if (response_needed)
	    {
	      write_enn (own_buf);
	      putpkt (own_buf);
	    }
	}

      /* Wait for events.  This will return when all event sources are
	 removed from the event loop. */
      start_event_loop ();

      /* If an exit was requested (using the "monitor exit" command),
	 terminate now.  The only other way to get here is for
	 getpkt to fail; close the connection and reopen it at the
	 top of the loop.  */

      if (exit_requested)
	{
	  detach_or_kill_for_exit ();
	  exit (0);
	}
      else
	fprintf (stderr, "Remote side has terminated connection.  "
		 "GDBserver will reopen the connection.\n");
    }
}

/* Event loop callback that handles a serial event.  The first byte in
   the serial buffer gets us here.  We expect characters to arrive at
   a brisk pace, so we read the rest of the packet with a blocking
   getpkt call.  */

static void
process_serial_event (void)
{
  char ch;
  int i = 0;
  int signal;
  unsigned int len;
  CORE_ADDR mem_addr;
  int pid;
  unsigned char sig;
  int packet_len;
  int new_packet_len = -1;

  /* Used to decide when gdbserver should exit in
     multi-mode/remote.  */
  static int have_ran = 0;

  if (!have_ran)
    have_ran = target_running ();

  disable_async_io ();

  response_needed = 0;
  packet_len = getpkt (own_buf);
  if (packet_len <= 0)
    {
      target_async (0);
      remote_close ();
      return;
    }
  response_needed = 1;

  i = 0;
  ch = own_buf[i++];
  switch (ch)
    {
    case 'q':
      handle_query (own_buf, packet_len, &new_packet_len);
      break;
    case 'Q':
      handle_general_set (own_buf);
      break;
    case 'D':
      require_running (own_buf);

      if (multi_process)
	{
	  i++; /* skip ';' */
	  pid = strtol (&own_buf[i], NULL, 16);
	}
      else
	pid =
	  ptid_get_pid (((struct inferior_list_entry *) current_inferior)->id);

      fprintf (stderr, "Detaching from process %d\n", pid);
      if (detach_inferior (pid) != 0)
	write_enn (own_buf);
      else
	{
	  discard_queued_stop_replies (pid);
	  write_ok (own_buf);

	  if (extended_protocol)
	    {
	      /* Treat this like a normal program exit.  */
	      last_status.kind = TARGET_WAITKIND_EXITED;
	      last_status.value.integer = 0;
	      last_ptid = pid_to_ptid (pid);

	      current_inferior = NULL;
	    }
	  else
	    {
	      putpkt (own_buf);
	      remote_close ();

	      /* If we are attached, then we can exit.  Otherwise, we
		 need to hang around doing nothing, until the child is
		 gone.  */
	      for_each_inferior (&all_processes,
				 join_inferiors_callback);
	      exit (0);
	    }
	}
      break;
    case '!':
      extended_protocol = 1;
      write_ok (own_buf);
      break;
    case '?':
      handle_status (own_buf);
      break;
    case 'H':
      if (own_buf[1] == 'c' || own_buf[1] == 'g' || own_buf[1] == 's')
	{
	  ptid_t gdb_id, thread_id;
	  int pid;

	  require_running (own_buf);

	  gdb_id = read_ptid (&own_buf[2], NULL);

	  pid = ptid_get_pid (gdb_id);

	  if (ptid_equal (gdb_id, null_ptid)
	      || ptid_equal (gdb_id, minus_one_ptid))
	    thread_id = null_ptid;
	  else if (pid != 0
		   && ptid_equal (pid_to_ptid (pid),
				  gdb_id))
	    {
	      struct thread_info *thread =
		(struct thread_info *) find_inferior (&all_threads,
						      first_thread_of,
						      &pid);
	      if (!thread)
		{
		  write_enn (own_buf);
		  break;
		}

	      thread_id = ((struct inferior_list_entry *)thread)->id;
	    }
	  else
	    {
	      thread_id = gdb_id_to_thread_id (gdb_id);
	      if (ptid_equal (thread_id, null_ptid))
		{
		  write_enn (own_buf);
		  break;
		}
	    }

	  if (own_buf[1] == 'g')
	    {
	      if (ptid_equal (thread_id, null_ptid))
		{
		  /* GDB is telling us to choose any thread.  Check if
		     the currently selected thread is still valid. If
		     it is not, select the first available.  */
		  struct thread_info *thread =
		    (struct thread_info *) find_inferior_id (&all_threads,
							     general_thread);
		  if (thread == NULL)
		    thread_id = all_threads.head->id;
		}

	      general_thread = thread_id;
	      set_desired_inferior (1);
	    }
	  else if (own_buf[1] == 'c')
	    cont_thread = thread_id;
	  else if (own_buf[1] == 's')
	    step_thread = thread_id;

	  write_ok (own_buf);
	}
      else
	{
	  /* Silently ignore it so that gdb can extend the protocol
	     without compatibility headaches.  */
	  own_buf[0] = '\0';
	}
      break;
    case 'g':
      require_running (own_buf);
      set_desired_inferior (1);
      registers_to_string (own_buf);
      break;
    case 'G':
      require_running (own_buf);
      set_desired_inferior (1);
      registers_from_string (&own_buf[1]);
      write_ok (own_buf);
      break;
    case 'm':
      require_running (own_buf);
      decode_m_packet (&own_buf[1], &mem_addr, &len);
      if (read_inferior_memory (mem_addr, mem_buf, len) == 0)
	convert_int_to_ascii (mem_buf, own_buf, len);
      else
	write_enn (own_buf);
      break;
    case 'M':
      require_running (own_buf);
      decode_M_packet (&own_buf[1], &mem_addr, &len, mem_buf);
      if (write_inferior_memory (mem_addr, mem_buf, len) == 0)
	write_ok (own_buf);
      else
	write_enn (own_buf);
      break;
    case 'X':
      require_running (own_buf);
      if (decode_X_packet (&own_buf[1], packet_len - 1,
			   &mem_addr, &len, mem_buf) < 0
	  || write_inferior_memory (mem_addr, mem_buf, len) != 0)
	write_enn (own_buf);
      else
	write_ok (own_buf);
      break;
    case 'C':
      require_running (own_buf);
      convert_ascii_to_int (own_buf + 1, &sig, 1);
      if (target_signal_to_host_p (sig))
	signal = target_signal_to_host (sig);
      else
	signal = 0;
      myresume (own_buf, 0, signal);
      break;
    case 'S':
      require_running (own_buf);
      convert_ascii_to_int (own_buf + 1, &sig, 1);
      if (target_signal_to_host_p (sig))
	signal = target_signal_to_host (sig);
      else
	signal = 0;
      myresume (own_buf, 1, signal);
      break;
    case 'c':
      require_running (own_buf);
      signal = 0;
      myresume (own_buf, 0, signal);
      break;
    case 's':
      require_running (own_buf);
      signal = 0;
      myresume (own_buf, 1, signal);
      break;
    case 'Z':  /* insert_ ... */
      /* Fallthrough.  */
    case 'z':  /* remove_ ... */
      {
	char *lenptr;
	char *dataptr;
	CORE_ADDR addr = strtoul (&own_buf[3], &lenptr, 16);
	int len = strtol (lenptr + 1, &dataptr, 16);
	char type = own_buf[1];
	int res;
	const int insert = ch == 'Z';

	/* Default to unrecognized/unsupported.  */
	res = 1;
	switch (type)
	  {
	  case '0': /* software-breakpoint */
	  case '1': /* hardware-breakpoint */
	  case '2': /* write watchpoint */
	  case '3': /* read watchpoint */
	  case '4': /* access watchpoint */
	    require_running (own_buf);
	    if (insert && the_target->insert_point != NULL)
	      res = (*the_target->insert_point) (type, addr, len);
	    else if (!insert && the_target->remove_point != NULL)
	      res = (*the_target->remove_point) (type, addr, len);
	    break;
	  default:
	    break;
	  }

	if (res == 0)
	  write_ok (own_buf);
	else if (res == 1)
	  /* Unsupported.  */
	  own_buf[0] = '\0';
	else
	  write_enn (own_buf);
	break;
      }
    case 'k':
      response_needed = 0;
      if (!target_running ())
	/* The packet we received doesn't make sense - but we can't
	   reply to it, either.  */
	return;

      fprintf (stderr, "Killing all inferiors\n");
      for_each_inferior (&all_processes, kill_inferior_callback);

      /* When using the extended protocol, we wait with no program
	 running.  The traditional protocol will exit instead.  */
      if (extended_protocol)
	{
	  last_status.kind = TARGET_WAITKIND_EXITED;
	  last_status.value.sig = TARGET_SIGNAL_KILL;
	  return;
	}
      else
	{
	  exit (0);
	  break;
	}
    case 'T':
      {
	ptid_t gdb_id, thread_id;

	require_running (own_buf);

	gdb_id = read_ptid (&own_buf[1], NULL);
	thread_id = gdb_id_to_thread_id (gdb_id);
	if (ptid_equal (thread_id, null_ptid))
	  {
	    write_enn (own_buf);
	    break;
	  }

	if (mythread_alive (thread_id))
	  write_ok (own_buf);
	else
	  write_enn (own_buf);
      }
      break;
    case 'R':
      response_needed = 0;

      /* Restarting the inferior is only supported in the extended
	 protocol.  */
      if (extended_protocol)
	{
	  if (target_running ())
	    for_each_inferior (&all_processes,
			       kill_inferior_callback);
	  fprintf (stderr, "GDBserver restarting\n");

	  /* Wait till we are at 1st instruction in prog.  */
	  if (program_argv != NULL)
	    start_inferior (program_argv);
	  else
	    {
	      last_status.kind = TARGET_WAITKIND_EXITED;
	      last_status.value.sig = TARGET_SIGNAL_KILL;
	    }
	  return;
	}
      else
	{
	  /* It is a request we don't understand.  Respond with an
	     empty packet so that gdb knows that we don't support this
	     request.  */
	  own_buf[0] = '\0';
	  break;
	}
    case 'v':
      /* Extended (long) request.  */
      handle_v_requests (own_buf, packet_len, &new_packet_len);
      break;

    default:
      /* It is a request we don't understand.  Respond with an empty
	 packet so that gdb knows that we don't support this
	 request.  */
      own_buf[0] = '\0';
      break;
    }

  if (new_packet_len != -1)
    putpkt_binary (own_buf, new_packet_len);
  else
    putpkt (own_buf);

  response_needed = 0;

  if (!extended_protocol && have_ran && !target_running ())
    {
      /* In non-stop, defer exiting until GDB had a chance to query
	 the whole vStopped list (until it gets an OK).  */
      if (!notif_queue)
	{
	  fprintf (stderr, "GDBserver exiting\n");
	  remote_close ();
	  exit (0);
	}
    }
}

/* Event-loop callback for serial events.  */

void
handle_serial_event (int err ATTR_UNUSED, gdb_client_data client_data ATTR_UNUSED)
{
  if (debug_threads)
    fprintf (stderr, "handling possible serial event\n");

  /* Really handle it.  */
  process_serial_event ();

  /* Be sure to not change the selected inferior behind GDB's back.
     Important in the non-stop mode asynchronous protocol.  */
  set_desired_inferior (1);
}

/* Event-loop callback for target events.  */

void
handle_target_event (int err ATTR_UNUSED, gdb_client_data client_data ATTR_UNUSED)
{
  if (debug_threads)
    fprintf (stderr, "handling possible target event\n");

  last_ptid = mywait (minus_one_ptid, &last_status,
		      TARGET_WNOHANG, 1);

  if (last_status.kind != TARGET_WAITKIND_IGNORE)
    {
      /* Something interesting.  Tell GDB about it.  */
      push_event (last_ptid, &last_status);
    }

  /* Be sure to not change the selected inferior behind GDB's back.
     Important in the non-stop mode asynchronous protocol.  */
  set_desired_inferior (1);
}
