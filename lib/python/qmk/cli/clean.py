"""Clean the QMK firmware folder of build artifacts.
"""
from subprocess import DEVNULL

from qmk.commands import _find_make
from milc import cli


@cli.argument('-a', '--all', arg_only=True, action='store_true', help='Remove *.hex and *.bin files in the QMK root as well.')
@cli.subcommand('Clean the QMK firmware folder of build artifacts.')
def clean(cli):
    """Runs `make clean` (or `make distclean` if --all is passed)
    """
    make_cmd = [_find_make(), 'distclean' if cli.args.all else 'clean']

    cli.run(make_cmd, capture_output=False, stdin=DEVNULL)
