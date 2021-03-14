BIN_TARGETS := \
s6-frontend-config-preprocess

LIBEXEC_TARGETS :=

WRAP_ANY :=

ifdef WRAP_DAEMONTOOLS

ifdef WRAP_SYMLINKS

WRAP_ANY := 1

DAEMONTOOLS_TARGETS := \
envdir \
envuidgid \
fghack \
multilog \
pgrphack \
readproctitle \
setlock \
setuidgid \
softlimit \
supervise \
svc \
svok \
svscan \
svscanboot \
svstat \
tai64n \
tai64nlocal

else

DAEMONTOOLS_TARGETS :=

endif

install-bin: $(DAEMONTOOLS_TARGETS:%=$(DESTDIR)$(bindir)/%)

ifneq ($(exthome),)
global-links: $(DAEMONTOOLS_TARGETS:%=$(DESTDIR)$(sproot)/command/%)
endif

endif

ifdef WRAP_RUNIT

ifdef WRAP_SYMLINKS

WRAP_ANY := 1

RUNIT_TARGETS := \
runit \
runit-init \
runsv \
runsvchdir \
runsvdir \
svlogd \
utmpset

else

RUNIT_TARGETS :=

endif

RUNIT_SPECIAL_TARGETS := chpst sv
BIN_TARGETS += s6-frontend-alias-sv s6-frontend-alias-chpst

install-bin: $(RUNIT_TARGETS:%=$(DESTDIR)$(bindir)/%) $(RUNIT_SPECIAL_TARGETS:%=$(DESTDIR)$(bindir)/%)

ifneq ($(exthome),)
global-links: $(RUNIT_TARGETS:%=$(DESTDIR)$(sproot)/command/%) $(RUNIT_SPECIAL_TARGETS:%=$(DESTDIR)$(sproot)/command/%)
endif

$(DESTDIR)$(bindir)/chpst: $(DESTDIR)$(bindir)/s6-frontend-alias-chpst
	 exec $(INSTALL) -D -l s6-frontend-alias-chpst $@
$(DESTDIR)$(bindir)/sv: $(DESTDIR)$(bindir)/s6-frontend-alias-sv
	 exec $(INSTALL) -D -l s6-frontend-alias-sv $@

endif

ifdef WRAP_ANY

BIN_TARGETS += s6-frontend-alias

$(DAEMONTOOLS_TARGETS:%=$(DESTDIR)$(bindir)/%) $(RUNIT_TARGETS:%=$(DESTDIR)$(bindir)/%): $(DESTDIR)$(bindir)/s6-frontend-alias
	 exec $(INSTALL) -D -l s6-frontend-alias $@

endif
