
#==============================================================================+
# File name   : makefile
# Begin       : 2012-08-21
# Last Update : 2012-08-21
# Version     : 1.1.0
#
# Website     : https://github.com/fubralimited/LogPipe
#
# Description : Makefile to build LogPipe.
#
# Author: Nicola Asuni
#
# (c) Copyright:
#               Fubra Limited
#               Manor Coach House
#               Church Hill
#               Aldershot
#               Hampshire
#               GU12 4RQ
#				UK
#               http://www.fubra.com
#               support@fubra.com
#
# License:
#    Copyright (C) 2012-2012 Fubra Limited
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Affero General Public License as
#    published by the Free Software Foundation, either version 3 of the
#    License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Affero General Public License for more details.
#
#    You should have received a copy of the GNU Affero General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#    See LICENSE.TXT file for more information.
#==============================================================================+

# --- PATHS ---
PATHINSTALL=$(DESTDIR)/usr/lib64/php/modules/
PATHCONFIG=$(DESTDIR)/etc/php.d/

# --- TARGETS ---

# COMPILE THE TCP RECEIVER
all:
	phpize
	./configure --enable-logpipe
	make -f Makefile

# INSTALL ALL EXECUTABLES AND SQL FILES
install:
	mkdir -p $(PATHCONFIG)
	mkdir -p $(PATHINSTALL)
	install -m 0644 logpipe.ini $(PATHCONFIG)
	install -m 0755 ./modules/logpipe.so $(PATHINSTALL)

# REMOVE ALL INSTALLED FILES
uninstall:
	rm -f $(PATHINSTALL)logpipe.so
	rm -f $(PATHCONFIG)logpipe.ini

# REMOVE COMPILED FILE
clean:
	rm -f logpipe.so
	
#==============================================================================+
# File name   : makefile
# Begin       : 2012-08-21
# Last Update : 2012-08-21
# Version     : 1.1.0
#
# Website     : https://github.com/fubralimited/LogPipe
#
# Description : Makefile to build LogPipe.
#
# Author: Nicola Asuni
#
# (c) Copyright:
#               Fubra Limited
#               Manor Coach House
#               Church Hill
#               Aldershot
#               Hampshire
#               GU12 4RQ
#				UK
#               http://www.fubra.com
#               support@fubra.com
#
# License:
#    Copyright (C) 2012-2012 Fubra Limited
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Affero General Public License as
#    published by the Free Software Foundation, either version 3 of the
#    License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Affero General Public License for more details.
#
#    You should have received a copy of the GNU Affero General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#    See LICENSE.TXT file for more information.
#==============================================================================+

# --- PATHS ---
PATHINSTALL=$(DESTDIR)/usr/lib64/php/modules/
PATHCONFIG=$(DESTDIR)/etc/php.d/

# --- TARGETS ---

# COMPILE THE TCP RECEIVER
all:
	phpize
	./configure --enable-logpipe
	make -f Makefile

# INSTALL ALL EXECUTABLES AND SQL FILES
install:
	mkdir -p $(PATHCONFIG)
	mkdir -p $(PATHINSTALL)
	install -m 0644 logpipe.ini $(PATHCONFIG)
	install -m 0755 ./modules/logpipe.so $(PATHINSTALL)

# REMOVE ALL INSTALLED FILES
uninstall:
	rm -f $(PATHINSTALL)logpipe.so
	rm -f $(PATHCONFIG)logpipe.ini

# REMOVE COMPILED FILE
clean:
	make -f Makefile clean

# TEST
test:
	make -f Makefile test

#==============================================================================+
# END OF FILE
#==============================================================================+
