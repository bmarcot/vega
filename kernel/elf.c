/*
 * kernel/elf.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <kernel/fs.h>
#include <kernel/kernel.h>
#include <kernel/scheduler.h>
#include <kernel/task.h>
#include <kernel/thread.h>

#include <uapi/elf32.h>

static int elf_check_magic(Elf32_Ehdr *ehdr)
{
	return memcmp(ehdr->e_ident, ELFMAG, SELFMAG) == 0;
}

static int elf_check_arch(Elf32_Ehdr *ehdr)
{
	return ehdr->e_machine == EM_ARM;
}

static int elf_check_mode(Elf32_Ehdr *ehdr)
{
	return ehdr->e_type == ET_EXEC;
}

static int elf_check_phentsize(Elf32_Ehdr *ehdr)
{
	return ehdr->e_phentsize == sizeof(Elf32_Phdr);
}

static int check_elf_header(Elf32_Ehdr *ehdr)
{
	if (!elf_check_magic(ehdr)) {
		pr_err("Not an ELF binary");
		return -1;
	}
	if (!elf_check_arch(ehdr)) {
		pr_err("Unknown architecture (not ARM)");
		return -1;
	}
	if (ehdr->e_ehsize != sizeof(Elf32_Ehdr)) {
		pr_err("ELF header size does not match on this archicture");
		return -1;
	}
	if (!elf_check_mode(ehdr)) {
		pr_err("ELF is not an executable binary");
		return -1;
	}

	return 0;
}

static int copy_load_segments(int fd, Elf32_Ehdr *ehdr)
{
	if (!elf_check_phentsize(ehdr))
		return -1;

	/* Find the PHDR segment, and copy it to memory. */
	Elf32_Phdr phdr;
	for (int i = 0; i < ehdr->e_phnum; i++) {
		do_lseek(fd, ehdr->e_phoff + i * ehdr->e_phentsize, SEEK_SET);
		do_read(fd, &phdr, sizeof(Elf32_Phdr));
		if (phdr.p_type == PT_PHDR)
			break;
	}
	if (phdr.p_type != PT_PHDR)
		return -1;
	Elf32_Phdr *phdrs = malloc(phdr.p_memsz);
	if (phdrs == NULL) {
		pr_warn("Cannot allocate %d bytes", phdr.p_memsz);
		return -1;
	}
	do_lseek(fd, phdr.p_offset, SEEK_SET);
	do_read(fd, phdrs, phdr.p_memsz);

	/* For each LOAD segment, allocate memory and copy bytes. */
	for (int i = 0; i < ehdr->e_phnum; i++) {
		if (phdrs[i].p_type == PT_LOAD) {
			void *mem = malloc(phdrs[i].p_memsz);
			if (mem == NULL) {
				pr_warn("Cannot allocate %d bytes", phdrs[i].p_memsz);
				return -1;
			}
			do_lseek(fd, phdrs[i].p_offset, SEEK_SET);
			int rb = do_read(fd, mem, phdrs[i].p_filesz);
			if (rb != (int)phdrs[i].p_filesz) {
				pr_err("Copied %d bytes, wanted %d", rb, phdrs[i].p_filesz);
				return -1;
			}
			phdrs[i].p_paddr = (Elf32_Addr)mem;
		}
	}

	/* Relocate the start address. */
	for (int i = 0; i < ehdr->e_phnum; i++) {
		if ((phdrs[i].p_type == PT_LOAD)
			&& (ehdr->e_entry >= phdrs[i].p_vaddr)
			&& (ehdr->e_entry < phdrs[i].p_vaddr + phdrs[i].p_memsz)) {
			ehdr->e_entry -= phdrs[i].p_vaddr;
			ehdr->e_entry += phdrs[i].p_paddr;
			break;
		}
	}

	/* We don't need to keep the PHDR segment. */
	free(phdrs);

	return 0;
}

typedef void *(*start_routine)(void *);

int elf_load_binary(const char *pathname)
{
	Elf32_Ehdr ehdr;
	int fd;
	int err;

	//FIXME: struct file* do_file_open(const char *pathname, int flags);
	//FIXME: struct file* do_file_read(struct file *file, void *buf, size_t count);
	//FIXME: int          do_file_close(struct file *file);

	fd = do_open(pathname, 0);
	if (fd < 0) {
		pr_err("error: Failed to open %s", pathname);
		return -1;
	}

	/* ELF header is at the beginning of the file. */
	do_read(fd, &ehdr, sizeof(Elf32_Ehdr));
	err = check_elf_header(&ehdr);
	if (err)
		goto out;
	err = copy_load_segments(fd, &ehdr);

out:
	do_close(fd);
	if (err)
		return err;

	/* Create a new task. */
	struct task_info *bin_task = malloc(sizeof(struct task_info));
	task_init(bin_task);
	struct thread_info *thread_main;
	thread_main = thread_create((start_routine)ehdr.e_entry, NULL,
				THREAD_PRIV_USER, 1024,	bin_task);
	sched_enqueue(thread_main);

	return 0;
}
