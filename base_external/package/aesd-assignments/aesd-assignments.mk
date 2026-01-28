##############################################################
#
# AESD-ASSIGNMENTS
# modification: 27DEC2025, by Arnaud SIMO
# Works for builds root and qemu: 30DEC2025
##############################################################

AESD_ASSIGNMENTS_VERSION = 429138b #a83f102 
AESD_ASSIGNMENTS_SITE = git@github.com:cu-ecen-aeld/assignments-3-and-later-simo-2021.git
AESD_ASSIGNMENTS_SITE_METHOD = git
AESD_ASSIGNMENTS_GIT_SUBMODULES = YES

define AESD_ASSIGNMENTS_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/finder-app all
endef

define AESD_ASSIGNMENTS_INSTALL_TARGET_CMDS
	# Configuration
	$(INSTALL) -d -m 0755 $(TARGET_DIR)/etc/finder-app/conf
	$(INSTALL) -m 0644 $(@D)/finder-app/conf/* $(TARGET_DIR)/etc/finder-app/conf/

	# Binaire writer (and aesdsocket)
	$(INSTALL) -m 0755 $(@D)/finder-app/writer $(TARGET_DIR)/usr/bin/
	$(INSTALL) -m 0755 $(@D)/finder-app/writer $(TARGET_DIR)/etc/finder-app/

    	$(INSTALL) -m 0755 $(@D)/server/aesdsocket  $(TARGET_DIR)/usr/bin/
	
   	# Installer le script finder-test.sh et finder.sh (and aesdsocket-start-stop 
    	$(INSTALL) -m 0755 $(@D)/finder-app/finder-test.sh $(TARGET_DIR)/usr/bin/
    	$(INSTALL) -m 0755 $(@D)/finder-app/finder.sh $(TARGET_DIR)/usr/bin/
    	
    	$(INSTALL) -m 0755 $(@D)/server/aesdsocket-start-stop.sh  $(TARGET_DIR)/etc/init.d/S99aesdsocket
    	$(INSTALL) -m 0755 $(@D)/server/aesdsocket-start-stop  $(TARGET_DIR)/etc/init.d/S99aesdsocket_
endef

$(eval $(generic-package))
