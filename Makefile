.PHONY: clean All

All:
	@echo "----------Building project:[ epoch - Debug ]----------"
	@"$(MAKE)" -f  "epoch.mk"
clean:
	@echo "----------Cleaning project:[ epoch - Debug ]----------"
	@"$(MAKE)" -f  "epoch.mk" clean
