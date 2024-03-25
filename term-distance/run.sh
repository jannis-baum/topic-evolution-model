#!/bin/sh

CPUS=`getconf _NPROCESSORS_ONLN`

/usr/bin/env python3 -m gunicorn \
    --workers $CPUS \
    --timeout 300 \
    --preload \
    --bind 0.0.0.0 \
    app:app
