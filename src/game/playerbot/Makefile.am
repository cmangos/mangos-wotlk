# Copyright (C) 2005-2010 MaNGOS <http://getmangos.com/>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

## Process this file with automake to produce Makefile.in

## Sub-directories to parse

## CPP flags for includes, defines, etc.
AM_CPPFLAGS = $(MANGOS_INCLUDES) -I$(top_builddir)/src/shared -I$(srcdir) -I$(srcdir)/../../../dep/include -I$(srcdir)/../../framework -I$(srcdir)/../../shared -I$(srcdir)/../../shared/vmap -I$(srcdir)/../../realmd -DSYSCONFDIR=\"$(sysconfdir)/\"

## Build MaNGOS game library as convenience library.
# libmangosbot shared library will later be reused by world server daemon.
noinst_LIBRARIES = libmangosbot.a

#  libmangossgame library will later be reused by ...
libmangosbot_a_SOURCES = \
        config.h \
        PlayerbotMgr.cpp \
        PlayerbotMgr.h \
        PlayerbotAI.cpp \
        PlayerbotAI.h \
        PlayerbotClassAI.cpp \
        PlayerbotClassAI.h \
        PlayerbotDeathKnightAI.cpp \
        PlayerbotDeathKnightAI.h \
        PlayerbotDruidAI.cpp \
        PlayerbotDruidAI.h \
        PlayerbotHunterAI.cpp \
        PlayerbotHunterAI.h \
        PlayerbotMageAI.cpp \
        PlayerbotMageAI.h \
        PlayerbotPaladinAI.cpp \
        PlayerbotPaladinAI.h \
        PlayerbotPriestAI.cpp \
        PlayerbotPriestAI.h \
        PlayerbotRogueAI.cpp \
        PlayerbotRogueAI.h \
        PlayerbotShamanAI.cpp \
        PlayerbotShamanAI.h \
        PlayerbotWarlockAI.cpp \
        PlayerbotWarlockAI.h \
        PlayerbotWarriorAI.cpp \
        PlayerbotWarriorAI.h

## Additional files to include when running 'make dist'
#  Precompiled Headers for WIN
EXTRA_DIST = \
	config.h \
	playerbot.conf.dist

## Additional files to install
sysconf_DATA = \
	playerbot.conf.dist

install-data-hook:
	@list='$(sysconf_DATA)'
	for p in $$list; do \
		dest=`echo $$p | sed -e s/.dist//`; \
		if test -f $(DESTDIR)$(sysconfdir)/$$dest; then \
			echo "$@ will not overwrite existing $(DESTDIR)$(sysconfdir)/$$dest"; \
		else \
			echo " $(INSTALL_DATA) $$p $(DESTDIR)$(sysconfdir)/$$dest"; \
			$(INSTALL_DATA) $$p $(DESTDIR)$(sysconfdir)/$$dest; \
		fi; \
	done

clean-local:
	rm -f $(sysconf_DATA)
