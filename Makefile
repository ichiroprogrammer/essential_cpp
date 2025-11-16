MAKEFILE:=$(word $(words $(MAKEFILE_LIST)), $(MAKEFILE_LIST))

TARGET:=o/essential_cpp.md
TITLE=Deep C++($(VERSION))

MD_SEC_NUM:=
#MD_SEC_NUM:=--sec_num

MD_GEN:=./md_gen/export/py

# VPATH=./md:dir1:dir2
VPATH=./md:

MDS:=essential_intro.md core_lang_spec.md stdlib_and_concepts.md cpp_idioms.md essential_appendix.md

INDEX_OPT:=--exclude $(addsuffix :1,$(MDS) sample_code.md)

include make/md.mk
