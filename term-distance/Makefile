.PHONY: build run kill

build:
	docker build . --tag tem-distance-server

run: build
	docker run --rm --publish 8000:8000 tem-distance-server

kill:
	docker kill $(shell docker ps --filter ancestor=tem-distance-server -q)
