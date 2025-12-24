BIN_TARGETS := \
s6 \
s6-frontend

LIBEXEC_TARGETS := \
s6-frontend-helper-kill \

EXTRA_TARGETS := \
src/s6-frontend/live_help.c \
src/s6-frontend/main_help.c \
src/s6-frontend/process_help.c \
src/s6-frontend/repository_help.c \
src/s6-frontend/set_help.c \
src/s6-frontend/system_help.c \

%_help.c: %.help.txt
	exec ./tools/help2c.sh $* < $< > $@
