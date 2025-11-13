SRCS20:=

SRCS:=\
	accessor_ut.cpp class_ut.cpp copy_and_swap_ut.cpp crtp_ut.cpp enum_operator_ut.cpp future_ut.cpp guard_ut.cpp \
	light_pimpl_ut.cpp no_copy_and_swap_ut.cpp null_object_ut.cpp nvi_ut.cpp raii_ut.cpp \
    widget_ng_ut.cpp widget_ng.cpp widget_ok.cpp widget_ok_ut.cpp \
    string_holder_ut.cpp string_holder_old.cpp string_holder_new.cpp \
    lack_of_cohesion_ut.cpp spurious_wakeup_ut.cpp class_relation_ut.cpp override_overload_ut.cpp \
    most_vexing_parse_ut.cpp aaa.cpp  east_west_const.cpp ambiguous_ownership_ut.cpp unique_ptr_ownership_ut.cpp \
    shared_ptr_ownership_ut.cpp shared_ptr_cycle_ut.cpp deep_shallow_copy_ut.cpp semantics_ut.cpp \
    slice_ut.cpp rule_of_zero_ut.cpp dangling_ut.cpp spin_lock_ut.cpp \


SRCS+=$(SRCS20)

CPP_VER:= c++17
SHARED:=../../
include $(SHARED)make/env.mk

ifeq ($(CXX), g++)
# @@@ sample begin 0:0

CCFLAGS_ADD:=-fdump-lang-class
# @@@ sample end
CCFLAGS_ADD:=$(CCFLAGS_ADD) -fcoroutines
endif

include $(SHARED)make/example.mk
include $(SHARED)make/gtest.mk
