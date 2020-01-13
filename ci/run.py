import argparse
import pathlib
import shlex
import subprocess
import sys
import typing as t


SCRIPTS_DIR = pathlib.Path(__file__).parent
ROOT_DIR = SCRIPTS_DIR / ".."
OUTPUT_DIR = ROOT_DIR / "output"
PROFILES_PATH = SCRIPTS_DIR / "profiles"


def call(args: t.List[str], cwd: str) -> int:
    print(' '.join((shlex.quote(a) for a in args)))
    return subprocess.call(args, cwd=cwd, shell=False)


def cmd_run(args: argparse.ArgumentParser) -> int:
    print("hi")
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


def cmd_show_profiles(args):
    print(f"Here's the profiles located at {PROFILES_PATH}:")
    for p in PROFILES_PATH.iterdir():
        if p.is_file():
            print(f"    {p.name}")
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(prog='ci')
    commands = parser.add_subparsers(title='command', description='sub-command to run')
    run = commands.add_parser('run', help='Runs CI')
    run.add_argument('profile', type=pathlib.Path,
                        help='Conan profile to use.')
    run.set_defaults(func=cmd_run)
    profiles = commands.add_parser('show-profiles', help='Show profiles')
    profiles.set_defaults(func=cmd_show_profiles)
    args = parser.parse_args()
    return args.func(args)
    # if args.command == 'run':
    #     return cmd_run(args)


if __name__=="__main__":
    sys.exit(main())
