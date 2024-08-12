SHELL := /bin/bash

run: help

BOLD ?= $(shell tput bold)
NORMAL ?= $(shell tput sgr0)

help:
	@echo "To build container run: make start"

start:
	docker-compose up -d
	docker-compose exec mycontainer /bin/bash
