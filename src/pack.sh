#!/bin/sh

set -e

if command -v python3 &>/dev/null; then
    PYTHON=python3
else
    PYTHON=python
fi

$PYTHON build.py --macro PA --pub patchy.h --priv patchy_internal.h,patchy_component.c,patchy_document.c,patchy_element.c,patchy_helper.c,patchy_memory.c,patchy_string.c
