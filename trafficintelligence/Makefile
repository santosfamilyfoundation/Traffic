INSTALL_DIR = /usr/local/bin

cexe:
	@cd c && make feature-based-tracking

doc:
	doxygen

clean:
	@cd c && make clean
	@cd python && rm *.pyc

install: cexe
	@echo "========================================="
	@echo "Installing for Linux"
	@echo "========================================="
	@echo "Copying feature-based tracking executable"
	@cp bin/feature-based-tracking /usr/local/bin
	@echo "========================================="
	@echo "Copying Python scripts"
	@cp scripts/* $(INSTALL_DIR)

uninstall:
	@echo "Uninstalling for Linux"
	rm $(INSTALL_DIR)/feature-based-tracking 
	@cd scripts && ./uninstall-scripts.sh $(INSTALL_DIR) 	
