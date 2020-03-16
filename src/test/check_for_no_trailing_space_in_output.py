#! /usr/bin/env python
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2020 Shlomi Fish <shlomif@cpan.org>
#
# Distributed under terms of the MIT license.

import click
import re
import subprocess


@click.command()
@click.option('--exe', required='True')
@click.option('--gametype', required='True')
@click.option('--board', required='True')
def main(exe, gametype, board):
    out = subprocess.check_output(
        [exe, '--type', gametype, board]).decode('utf-8')
    assert re.search('[ \\t](?:\\n|\\Z)', out, flags=(re.M | re.S)) is None


main()
