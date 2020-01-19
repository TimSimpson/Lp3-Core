import argparse
import os
import pathlib
import shlex
import shutil
import subprocess
import sys
import typing as t


SCRIPTS_DIR = pathlib.Path(__file__).parent
ROOT_DIR = SCRIPTS_DIR / ".."
OUTPUT_DIR = ROOT_DIR / "output"
PROFILES_PATH = SCRIPTS_DIR / "profiles"


def _add_argument_profile(parser: argparse.ArgumentParser) -> None:
    if os.environ.get("PROFILE"):
        default_profile = pathlib.Path(os.environ.get("PROFILE"))
        kwargs = {
            'default': pathlib.Path(os.environ.get("PROFILE")),
            'nargs': '?'
        }
    else:
        kwargs = {}


    parser.add_argument('profile', type=pathlib.Path,
                        help='Conan profile to use.',
                        **kwargs)

def call(args: t.List[str], cwd: str) -> int:
    print(' '.join((shlex.quote(a) for a in args)))
    return subprocess.call(args, cwd=cwd, shell=False)


def cmd_clean(args: argparse.ArgumentParser) -> int:
    profile_path = PROFILES_PATH / args.profile
    build_dir = OUTPUT_DIR / args.profile
    if not profile_path.exists():
        print(f'Conan profile not found at "{profile_path}".')
        return 1

    if not build_dir.exists():
        print(f"    ... but {build_dir.absolute()} was already gone...")
    else:
        print(f"    * * destroyed {build_dir.absolute()} * *")
        shutil.rmtree(build_dir)
    return 0


def cmd_run(args: argparse.ArgumentParser) -> int:
    profile_path = PROFILES_PATH / args.profile
    build_dir = OUTPUT_DIR / args.profile
    if not profile_path.exists():
        print(f'Conan profile not found at "{profile_path}".')
        return 1

    build_dir.mkdir(exist_ok=True)
    cmd_line = [
        'conan', 'install', str(ROOT_DIR.absolute()),
        f'-pr={profile_path.absolute()}', '--build', 'missing'
    ]
    return call(args=cmd_line, cwd=build_dir)


def cmd_list_profiles(args):
    print(f"Here's the profiles located at {PROFILES_PATH}:")
    for p in PROFILES_PATH.iterdir():
        if p.is_file():
            print(f"    {p.name}")
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(prog='ci')
    commands = parser.add_subparsers(title='command', description='sub-command to run', required=True)

    clean = commands.add_parser('clean', help='Runs CI')
    _add_argument_profile(clean)
    clean.set_defaults(func=cmd_clean)

    run = commands.add_parser('run', help='Runs CI')
    _add_argument_profile(run)
    run.set_defaults(func=cmd_run)

    list_profiles = commands.add_parser('list-profiles', help='Show profiles')
    list_profiles.set_defaults(func=cmd_list_profiles)

    args = parser.parse_args()
    return args.func(args)


if __name__=="__main__":
    sys.exit(main())
