# ---------------------------------------------------------------------------
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License version 2 as 
#  published by the Free Software Foundation.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
#  As a special exception, you may use this file as part of a free software
#  library without restriction.  Specifically, if other files instantiate
#  templates or use macros or inline functions from this file, or you compile
#  this file and link it with other files to produce an executable, this
#  file does not by itself cause the resulting executable to be covered by
#  the GNU General Public License.  This exception does not however
#  invalidate any other reasons why the executable file might be covered by
#  the GNU General Public License.
#
# ---------------------------------------------------------------------------
SHELL           = /bin/bash
CC		=  gcc
AR              =  ar
CFLAGS	        += -std=c99 -Wall -Werror -g
ARFLAGS         =  rvs
INCLUDES	= -I. -I ./utils/includes
LDFLAGS 	= -L.
OPTFLAGS	= -O3 
LIBS            = -lpthread

# aggiungere qui altri targets
TARGETS		= bcclient     \
		  bcclient_v2  \
                  bcserver     \
		  bcserver_v2  \
		  toup_server  \
		  toup_client  \
		  toup_server_select

.PHONY: all clean cleanall testtoup
.SUFFIXES: .c .h

%: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) -c -o $@ $<

all		: $(TARGETS)

testtoup: toup_server toup_server_select toup_client
	./toup_server &
	./toup_test.sh toup1.txt
	kill -9 `pidof toup_server`
	@echo "primo test terminato"
	./toup_server_select &
	./toup_test.sh toup2.txt
	kill -9 `pidof toup_server_select`
	@echo "secondo test terminato"


clean		: 
	rm -f $(TARGETS)
cleanall	: clean
	\rm -f *.o *~ *.a ./cs_sock ./toup?.txt



