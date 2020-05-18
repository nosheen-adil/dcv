#!/bin/bash
docker-compose -f docker/test.yml up --abort-on-container-exit
docker-compose -f docker/test.yml down --volumes