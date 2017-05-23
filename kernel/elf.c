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

#include <lib/array.h>

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
	return (ehdr->e_type == ET_EXEC) || (ehdr->e_type == ET_DYN);
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
		pr_err("ELF is not an exec binary or a dynamic object");
		return -1;
	}

	return 0;
}

static int copy_load_segments(struct file *file, Elf32_Ehdr *ehdr)
{
	if (!elf_check_phentsize(ehdr))
		return -1;

	/* Find the PHDR segment, and copy it to memory. */
	Elf32_Phdr phdr;
	for (int i = 0; i < ehdr->e_phnum; i++) {
		do_file_lseek(file, ehdr->e_phoff + i * ehdr->e_phentsize, SEEK_SET);
		do_file_read(file, &phdr, sizeof(Elf32_Phdr));
		if (phdr.p_type == PT_PHDR)
			break;
	}
	if (phdr.p_type != PT_PHDR)
		return -1;
	Elf32_Phdr *phdr_seg = malloc(phdr.p_memsz);
	if (phdr_seg == NULL) {
		pr_warn("Cannot allocate %d bytes", phdr.p_memsz);
		return -1;
	}
	do_file_lseek(file, phdr.p_offset, SEEK_SET);
	do_file_read(file, phdr_seg, phdr.p_memsz);

	/* For each LOAD segment, allocate memory and copy bytes. */
	Elf32_Phdr *ph;
	array_for_each_element(ph, phdr_seg, ehdr->e_phnum, ehdr->e_phentsize) {
		if (ph->p_type == PT_LOAD) {
			void *mem = malloc(ph->p_memsz);
			if (mem == NULL) {
				pr_warn("Cannot allocate %d bytes", ph->p_memsz);
				return -1;
			}
			do_file_lseek(file, ph->p_offset, SEEK_SET);
			int rb = do_file_read(file, mem, ph->p_filesz);
			if (rb != (int)ph->p_filesz) {
				pr_err("Copied %d bytes, wanted %d", rb, ph->p_filesz);
				return -1;
			}
			ph->p_paddr = (Elf32_Addr)mem;
		}
	}

	/* Relocate the start address. */
	array_for_each_element(ph, phdr_seg, ehdr->e_phnum, ehdr->e_phentsize) {
		if ((ph->p_type == PT_LOAD)
			&& (ehdr->e_entry >= ph->p_vaddr)
			&& (ehdr->e_entry < ph->p_vaddr + ph->p_memsz)) {
			ehdr->e_entry = ehdr->e_entry - ph->p_vaddr + ph->p_paddr;
			break;
		}
	}

	/* We don't need to keep the PHDR segment. */
	free(phdr_seg);

	return 0;
}

typedef void *(*start_routine)(void *);

int elf_load_binary(const char *pathname)
{
	Elf32_Ehdr ehdr;
	struct file *file;
	int err;

	file = do_file_open(pathname, 0);
	if (file == NULL) {
		pr_err("error: Failed to open %s", pathname);
		return -1;
	}

	/* ELF header is at the beginning of the file. */
	do_file_read(file, &ehdr, sizeof(Elf32_Ehdr));
	err = check_elf_header(&ehdr);
	if (err)
		goto out;
	err = copy_load_segments(file, &ehdr);

out:
	do_file_close(file);
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
