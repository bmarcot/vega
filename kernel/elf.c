/*
 * kernel/elf.c
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <kernel/errno-base.h> //FIXME: <uapi/...>
#include <kernel/fs.h>
#include <kernel/kernel.h>
#include <kernel/sched.h>
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

static Elf32_Phdr *ph_get_by_addr(void *phdr_seg, Elf32_Ehdr *ehdr,
				Elf32_Addr addr)
{
	Elf32_Phdr *ph;

	array_for_each_element(ph, phdr_seg, ehdr->e_phnum, ehdr->e_phentsize) {
		if ((ph->p_type == PT_LOAD) && (addr >= ph->p_vaddr)
			&& (addr < ph->p_vaddr + ph->p_memsz)) {
			return ph;
		}
	}

	return NULL;
}

#define RESOLVE_R_ARM_RELATIVE(s, ph) \
	({ (s) - (ph)->p_vaddr + (ph)->p_paddr; })

static int copy_load_segments(struct file *file, Elf32_Ehdr *ehdr)
{
	Elf32_Phdr *ph;

	if (!elf_check_phentsize(ehdr))
		return -1;

	/* Copy the PHDR segment to memory. */
	void *phdr_seg = malloc(ehdr->e_phentsize * ehdr->e_phnum);
	if (phdr_seg == NULL)
		return -ENOMEM;

	do_file_lseek(file, ehdr->e_phoff, SEEK_SET);
	do_file_read(file, phdr_seg, ehdr->e_phentsize * ehdr->e_phnum);

	/* For each LOAD segment, allocate memory and copy bytes. */
	array_for_each_element(ph, phdr_seg, ehdr->e_phnum, ehdr->e_phentsize) {
		if (ph->p_type == PT_LOAD) {
			void *mem = malloc(ph->p_memsz);
			if (mem == NULL)
				return -ENOMEM;
			do_file_lseek(file, ph->p_offset, SEEK_SET);
			do_file_read(file, mem, ph->p_filesz);
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

	/*
	 * Process the .rel.dyn section
	 */

	/* Copy the SHDR segment to memory. */
	void *shdr_seg = malloc(ehdr->e_shentsize * ehdr->e_shnum);
	if (shdr_seg == NULL)
		return -ENOMEM;
	do_file_lseek(file, ehdr->e_shoff, SEEK_SET);
	do_file_read(file, shdr_seg, ehdr->e_shentsize * ehdr->e_shnum);

	/* Find the .rel.dyn section header, it has a REL type. */
	Elf32_Shdr *sh;
	array_for_each_element(sh, shdr_seg, ehdr->e_shnum, ehdr->e_shentsize) {
		if (sh->sh_type == SHT_REL)
			break;
	}

	/* Copy the .rel.dyn section to memory */
	void *rel_seg = malloc(sh->sh_size);
	if (rel_seg == NULL)
		return -ENOMEM;
	do_file_lseek(file, sh->sh_offset, SEEK_SET);
	do_file_read(file, rel_seg, sh->sh_size);

	/* Process each relocation entry. */
	int relnum = sh->sh_size / sh->sh_entsize;
	Elf32_Rel *rel;
	array_for_each_element(rel, rel_seg, relnum, sh->sh_entsize) {
		switch (ELF32_R_TYPE(rel->r_info)) {
		case R_ARM_RELATIVE:
			ph = ph_get_by_addr(phdr_seg, ehdr, rel->r_offset);

			/* relocate the entry */
			__u32 *ent = (__u32 *)(rel->r_offset - ph->p_vaddr
					+ ph->p_paddr);

			/* relocate the value of entry */
			ph = ph_get_by_addr(phdr_seg, ehdr, *ent);
			*ent = *ent - ph->p_vaddr + ph->p_paddr;
			break;
		default:
			pr_err("Unknown relocation entry");
			return -1;
		}
	}
	free(rel_seg);

	/* We don't need to keep the PHDR segment. */
	free(phdr_seg);
	free(shdr_seg);

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
	struct thread_info *thread_main;
	thread_main = thread_create((start_routine)ehdr.e_entry, NULL,
				THREAD_PRIV_USER, 1024);
	sched_enqueue(thread_main->task);

	return 0;
}
